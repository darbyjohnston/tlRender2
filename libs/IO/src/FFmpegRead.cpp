// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include "FFmpegRead.h"

#include <ftk/Core/Format.h>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavutil/pixdesc.h>
#include <libavutil/timecode.h>

} // extern "C"

namespace tl
{
    namespace io
    {
        namespace
        {
            constexpr size_t avIOContextBufferSize = 4096;

            struct AVIOBufferData
            {
                AVIOBufferData() = default;
                AVIOBufferData(const uint8_t* p, size_t size) :
                    p(p),
                    size(size)
                {}

                const uint8_t* p = nullptr;
                size_t size = 0;
                size_t offset = 0;
            };

            int avIOBufferRead(void* opaque, uint8_t* buf, int bufSize)
            {
                AVIOBufferData* bufferData = static_cast<AVIOBufferData*>(opaque);

                const int64_t remaining = bufferData->size - bufferData->offset;
                int bufSizeClamped = ftk::clamp(
                    static_cast<int64_t>(bufSize),
                    static_cast<int64_t>(0),
                    remaining);
                if (!bufSizeClamped)
                {
                    return AVERROR_EOF;
                }

                memcpy(buf, bufferData->p + bufferData->offset, bufSizeClamped);
                bufferData->offset += bufSizeClamped;

                return bufSizeClamped;
            }

            int64_t avIOBufferSeek(void* opaque, int64_t offset, int whence)
            {
                AVIOBufferData* bufferData = static_cast<AVIOBufferData*>(opaque);

                if (whence & AVSEEK_SIZE)
                {
                    return bufferData->size;
                }

                bufferData->offset = ftk::clamp(
                    offset,
                    static_cast<int64_t>(0),
                    static_cast<int64_t>(bufferData->size));

                return offset;
            }

            struct AVFormatContextDeleter
            {
                void operator()(AVFormatContext* p) const
                {
                    if (p)
                    {
                        avformat_close_input(&p);
                    }
                }
            };
            using AVFormatContextPtr = std::unique_ptr<AVFormatContext, AVFormatContextDeleter>;

            struct AVIOContextDeleter
            {
                void operator()(AVIOContext* p) const
                {
                    if (p)
                    {
                        av_freep(&p->buffer);
                        avio_context_free(&p);
                    }
                }
            };
            using AVIOContextPtr = std::unique_ptr<AVIOContext, AVIOContextDeleter>;

            struct AVCodecContextDeleter
            {
                void operator()(AVCodecContext* p) const
                {
                    if (p)
                    {
                        avcodec_free_context(&p);
                    }
                }
            };
            using AVCodecContextPtr = std::unique_ptr<AVCodecContext, AVCodecContextDeleter>;
            
            struct AVFrameDeleter
            {
                void operator()(AVFrame* p) const { if (p) { av_frame_free(&p); } }
            };
            using AVFramePtr = std::unique_ptr<AVFrame, AVFrameDeleter>;

            struct SwsContextDeleter
            {
                void operator()(SwsContext* p) const { if (p) { sws_freeContext(p); } }
            };
            using SwsContextPtr = std::unique_ptr<SwsContext, SwsContextDeleter>;

            core::AudioType toAudioType(AVSampleFormat value)
            {
                core::AudioType out = core::AudioType::None;
                switch (value)
                {
                case AV_SAMPLE_FMT_S16:  out = core::AudioType::S16; break;
                case AV_SAMPLE_FMT_S32:  out = core::AudioType::S32; break;
                case AV_SAMPLE_FMT_FLT:  out = core::AudioType::F32; break;
                case AV_SAMPLE_FMT_DBL:  out = core::AudioType::F64; break;
                case AV_SAMPLE_FMT_S16P: out = core::AudioType::S16; break;
                case AV_SAMPLE_FMT_S32P: out = core::AudioType::S32; break;
                case AV_SAMPLE_FMT_FLTP: out = core::AudioType::F32; break;
                case AV_SAMPLE_FMT_DBLP: out = core::AudioType::F64; break;
                default: break;
                }
                return out;
            }
        }

        std::string avErrorLabel(int r)
        {
            char buf[ftk::cStringSize];
            av_strerror(r, buf, ftk::cStringSize);
            return std::string(buf);
        }

        AVRational avSwap(AVRational value)
        {
            return AVRational({ value.den, value.num });
        }

        struct FFmpegRead::Private
        {
            AVFormatContextPtr avFormatContext;
            AVIOBufferData avIOBufferData;
            AVIOContextPtr avIOContext;
            int avVideoStream = -1;
            int avAudioStream = -1;
            std::map<int, AVCodecContextPtr> avCodecContext;
            AVPixelFormat avInputPixelFormat = AV_PIX_FMT_NONE;
            AVPixelFormat avOutputPixelFormat = AV_PIX_FMT_NONE;
            ReadInfo info;
        };

        FFmpegRead::FFmpegRead(
            const ftk::Path& path,
            const std::vector<ftk::MemFile>& mem,
            const ReadOptions& options,
            const std::shared_ptr<ftk::LogSystem>& logSystem) :
            IRead(path, mem, options, logSystem),
            _p(new Private)
        {
            FTK_P();
            
            // Open the file.
            const std::string fileName = path.get();
            if (!mem.empty())
            {
                p.avFormatContext.reset(avformat_alloc_context());
                if (!p.avFormatContext)
                {
                    throw std::runtime_error(ftk::Format("Cannot allocate format context: \"{0}\"").arg(fileName));
                }

                p.avIOBufferData = AVIOBufferData(mem[0].p, mem[0].size);
                uint8_t* avIOContextBuffer = static_cast<uint8_t*>(av_malloc(avIOContextBufferSize));
                if (!avIOContextBuffer)
                {
                    throw std::runtime_error(ftk::Format("Cannot allocate I/O buffer: \"{0}\"").arg(fileName));
                }
                p.avIOContext.reset(avio_alloc_context(
                    avIOContextBuffer,
                    avIOContextBufferSize,
                    0,
                    &p.avIOBufferData,
                    &avIOBufferRead,
                    nullptr,
                    &avIOBufferSeek));
                if (!p.avIOContext)
                {
                    av_free(avIOContextBuffer);
                    throw std::runtime_error(ftk::Format("Cannot allocate I/O context: \"{0}\"").arg(fileName));
                }

                p.avFormatContext->pb = p.avIOContext.get();
            }

            // avformat_open_input takes AVFormatContext** and may replace the
            // pointer. Release ours, pass the raw pointer, then reset to
            // whatever it ends up with (success or failure).
            AVFormatContext* rawFormatContext = p.avFormatContext.release();
            int r = avformat_open_input(
                &rawFormatContext,
                !rawFormatContext ? fileName.c_str() : nullptr,
                nullptr,
                nullptr);
            p.avFormatContext.reset(rawFormatContext);
            if (r < 0)
            {
                throw std::runtime_error(ftk::Format("{0}: \"{1}\"").arg(avErrorLabel(r)).arg(fileName));
            }

            // Find stream information. We check for a default stream first,
            // or use the first stream.
            r = avformat_find_stream_info(p.avFormatContext.get(), nullptr);
            if (r < 0)
            {
                throw std::runtime_error(ftk::Format("{0}: \"{1}\"").arg(avErrorLabel(r)).arg(fileName));
            }
            for (unsigned int i = 0; i < p.avFormatContext->nb_streams; ++i)
            {
                if (-1 == p.avVideoStream &&
                    AVMEDIA_TYPE_VIDEO == p.avFormatContext->streams[i]->codecpar->codec_type &&
                    p.avFormatContext->streams[i]->disposition & AV_DISPOSITION_DEFAULT)
                {
                    p.avVideoStream = i;
                }
                if (-1 == p.avAudioStream &&
                    AVMEDIA_TYPE_AUDIO == p.avFormatContext->streams[i]->codecpar->codec_type &&
                    p.avFormatContext->streams[i]->disposition & AV_DISPOSITION_DEFAULT)
                {
                    p.avAudioStream = i;
                }
            }
            for (unsigned int i = 0; i < p.avFormatContext->nb_streams; ++i)
            {
                if (-1 == p.avVideoStream &&
                    AVMEDIA_TYPE_VIDEO == p.avFormatContext->streams[i]->codecpar->codec_type)
                {
                    p.avVideoStream = i;
                }
                if (-1 == p.avAudioStream &&
                    AVMEDIA_TYPE_AUDIO == p.avFormatContext->streams[i]->codecpar->codec_type)
                {
                    p.avAudioStream = i;
                }
            }
            
            // Get metadata and look for timecode in the same pass.
            std::string timecode;
            std::optional<int> timeReference;
            AVDictionaryEntry* tag = nullptr;
            while ((tag = av_dict_get(p.avFormatContext->metadata, "", tag, AV_DICT_IGNORE_SUFFIX)))
            {
                p.info.tags[tag->key] = tag->value;
                if (ftk::compare(
                    tag->key,
                    "timecode",
                    ftk::CaseCompare::Insensitive))
                {
                    timecode = tag->value;
                }
                else if (ftk::compare(
                    tag->key,
                    "time_reference",
                    ftk::CaseCompare::Insensitive))
                {
                    timeReference = std::atoi(tag->value);
                }
            }
            if (timecode.empty())
            {
                // Check the data stream for timecode.
                int avDataStream = -1;
                for (unsigned int i = 0; i < p.avFormatContext->nb_streams; ++i)
                {
                    if (AVMEDIA_TYPE_DATA == p.avFormatContext->streams[i]->codecpar->codec_type &&
                        p.avFormatContext->streams[i]->disposition & AV_DISPOSITION_DEFAULT)
                    {
                        avDataStream = i;
                        break;
                    }
                }
                if (-1 == avDataStream)
                {
                    for (unsigned int i = 0; i < p.avFormatContext->nb_streams; ++i)
                    {
                        if (AVMEDIA_TYPE_DATA == p.avFormatContext->streams[i]->codecpar->codec_type)
                        {
                            avDataStream = i;
                            break;
                        }
                    }
                }
                if (avDataStream != -1)
                {
                    AVDictionaryEntry* tag = nullptr;
                    while ((tag = av_dict_get(
                        p.avFormatContext->streams[avDataStream]->metadata,
                        "",
                        tag,
                        AV_DICT_IGNORE_SUFFIX)))
                    {
                        if (ftk::compare(
                            tag->key,
                            "timecode",
                            ftk::CaseCompare::Insensitive))
                        {
                            timecode = tag->value;
                            break;
                        }
                    }
                }
            }

            // Get video information.
            std::optional<AVRational> avVideoSpeed;
            if (p.avVideoStream != -1)
            {
                auto avVideoStream = p.avFormatContext->streams[p.avVideoStream];
                auto avVideoCodecParameters = avVideoStream->codecpar;
                auto avVideoCodec = avcodec_find_decoder(avVideoCodecParameters->codec_id);
                if (!avVideoCodec)
                {
                    throw std::runtime_error(ftk::Format("No video codec found: \"{0}\"").arg(fileName));
                }

                AVCodecContextPtr avVideoCodecContext(avcodec_alloc_context3(avVideoCodec));
                if (!avVideoCodecContext)
                {
                    throw std::runtime_error(ftk::Format("Cannot allocate context: \"{0}\"").arg(fileName));
                }
                r = avcodec_parameters_to_context(avVideoCodecContext.get(), avVideoCodecParameters);
                if (r < 0)
                {
                    throw std::runtime_error(ftk::Format("{0}: \"{1}\"").arg(avErrorLabel(r)).arg(fileName));
                }
                avVideoCodecContext->thread_count = 0;
                avVideoCodecContext->thread_type = FF_THREAD_FRAME;
                r = avcodec_open2(avVideoCodecContext.get(), avVideoCodec, nullptr);
                if (r < 0)
                {
                    throw std::runtime_error(ftk::Format("{0}: \"{1}\"").arg(avErrorLabel(r)).arg(fileName));
                }

                // Get image information.
                ftk::ImageInfo imageInfo;
                imageInfo.size.w = avVideoCodecParameters->width;
                imageInfo.size.h = avVideoCodecParameters->height;
                if (avVideoCodecParameters->sample_aspect_ratio.den > 0 &&
                    avVideoCodecParameters->sample_aspect_ratio.num > 0)
                {
                    imageInfo.pixelAspectRatio = av_q2d(avVideoCodecParameters->sample_aspect_ratio);
                }
                imageInfo.layout.mirror.y = true;

                p.avInputPixelFormat = static_cast<AVPixelFormat>(avVideoCodecParameters->format);
                switch (p.avInputPixelFormat)
                {
                case AV_PIX_FMT_RGB24:
                    p.avOutputPixelFormat = p.avInputPixelFormat;
                    imageInfo.type = ftk::ImageType::RGB_U8;
                    break;
                case AV_PIX_FMT_GRAY8:
                    p.avOutputPixelFormat = p.avInputPixelFormat;
                    imageInfo.type = ftk::ImageType::L_U8;
                    break;
                case AV_PIX_FMT_RGBA:
                    p.avOutputPixelFormat = p.avInputPixelFormat;
                    imageInfo.type = ftk::ImageType::RGBA_U8;
                    break;
                case AV_PIX_FMT_YUV420P:
                case AV_PIX_FMT_YUV422P:
                    p.avOutputPixelFormat = AV_PIX_FMT_RGB24;
                    imageInfo.type = ftk::ImageType::RGB_U8;
                    break;
                case AV_PIX_FMT_YUV420P10BE:
                case AV_PIX_FMT_YUV420P10LE:
                case AV_PIX_FMT_YUV420P12BE:
                case AV_PIX_FMT_YUV420P12LE:
                case AV_PIX_FMT_YUV420P16BE:
                case AV_PIX_FMT_YUV420P16LE:
                case AV_PIX_FMT_YUV422P10BE:
                case AV_PIX_FMT_YUV422P10LE:
                case AV_PIX_FMT_YUV422P12BE:
                case AV_PIX_FMT_YUV422P12LE:
                case AV_PIX_FMT_YUV422P16BE:
                case AV_PIX_FMT_YUV422P16LE:
                case AV_PIX_FMT_YUV444P10BE:
                case AV_PIX_FMT_YUV444P10LE:
                case AV_PIX_FMT_YUV444P12BE:
                case AV_PIX_FMT_YUV444P12LE:
                case AV_PIX_FMT_YUV444P16BE:
                case AV_PIX_FMT_YUV444P16LE:
                    p.avOutputPixelFormat = AV_PIX_FMT_RGB48;
                    imageInfo.type = ftk::ImageType::RGB_U16;
                    break;
                case AV_PIX_FMT_YUVA420P:
                case AV_PIX_FMT_YUVA422P:
                case AV_PIX_FMT_YUVA444P:
                    p.avOutputPixelFormat = AV_PIX_FMT_RGBA;
                    imageInfo.type = ftk::ImageType::RGBA_U8;
                    break;
                case AV_PIX_FMT_YUVA444P10BE:
                case AV_PIX_FMT_YUVA444P10LE:
                case AV_PIX_FMT_YUVA444P12BE:
                case AV_PIX_FMT_YUVA444P12LE:
                case AV_PIX_FMT_YUVA444P16BE:
                case AV_PIX_FMT_YUVA444P16LE:
                    p.avOutputPixelFormat = AV_PIX_FMT_RGBA64;
                    imageInfo.type = ftk::ImageType::RGBA_U16;
                    break;
                default:
                    p.avOutputPixelFormat = AV_PIX_FMT_RGB24;
                    imageInfo.type = ftk::ImageType::RGB_U8;
                    _logSystem->print(
                        "tl::io::FFmpegRead",
                        ftk::Format("Unsupported pixel format, falling back to RGB24: \"{0}\"")
                        .arg(av_get_pix_fmt_name(p.avInputPixelFormat)));
                    break;
                }
                if (avVideoCodecContext->color_range != AVCOL_RANGE_JPEG)
                {
                    imageInfo.videoLevels = ftk::VideoLevels::LegalRange;
                }
                p.info.video.push_back(imageInfo);

                // Get the duration.
                int64_t frameCount = 0;
                if (avVideoStream->nb_frames > 0)
                {
                    frameCount = avVideoStream->nb_frames;
                }
                else if (avVideoStream->duration != AV_NOPTS_VALUE)
                {
                    frameCount = av_rescale_q(
                        avVideoStream->duration,
                        avVideoStream->time_base,
                        avSwap(avVideoStream->r_frame_rate));
                }
                else if (p.avFormatContext->duration != AV_NOPTS_VALUE)
                {
                    frameCount = av_rescale_q(
                        p.avFormatContext->duration,
                        av_get_time_base_q(),
                        avSwap(avVideoStream->r_frame_rate));
                }
                avVideoSpeed = av_guess_frame_rate(
                    p.avFormatContext.get(),
                    avVideoStream,
                    nullptr);
                const core::MediaRate rate{ avVideoSpeed->num, avVideoSpeed->den };
                p.info.videoDuration = core::MediaDuration{ frameCount, rate };

                // Get the start time.
                if (!timecode.empty())
                {
                    AVTimecode avTimecode;
                    if (0 == av_timecode_init_from_string(
                        &avTimecode,
                        *avVideoSpeed,
                        timecode.c_str(),
                        nullptr))
                    {
                        const core::MediaRate rate{ avTimecode.rate.num, avTimecode.rate.den };
                        p.info.videoStart = core::MediaTime{ avTimecode.start, rate };
                    }
                }
                
                // Add metadata.
                p.info.tags["NativePixelFormat"] = av_get_pix_fmt_name(p.avInputPixelFormat);
                
                p.avCodecContext.emplace(p.avVideoStream, std::move(avVideoCodecContext));
            }

            // Get audio information.
            if (p.avAudioStream != -1)
            {
                auto avAudioStream = p.avFormatContext->streams[p.avAudioStream];
                auto avAudioCodecParameters = avAudioStream->codecpar;
                auto avAudioCodec = avcodec_find_decoder(avAudioCodecParameters->codec_id);
                if (!avAudioCodec)
                {
                    throw std::runtime_error(ftk::Format("No audio codec found: \"{0}\"").arg(fileName));
                }

                AVCodecContextPtr avAudioCodecContext(avcodec_alloc_context3(avAudioCodec));
                if (!avAudioCodecContext)
                {
                    throw std::runtime_error(ftk::Format("Cannot allocate context: \"{0}\"").arg(fileName));
                }
                r = avcodec_parameters_to_context(avAudioCodecContext.get(), avAudioCodecParameters);
                if (r < 0)
                {
                    throw std::runtime_error(ftk::Format("{0}: \"{1}\"").arg(avErrorLabel(r)).arg(fileName));
                }
                avAudioCodecContext->thread_count = 0;
                avAudioCodecContext->thread_type = FF_THREAD_FRAME;
                r = avcodec_open2(avAudioCodecContext.get(), avAudioCodec, nullptr);
                if (r < 0)
                {
                    throw std::runtime_error(ftk::Format("{0}: \"{1}\"").arg(avErrorLabel(r)).arg(fileName));
                }

                core::AudioInfo audioInfo;
                audioInfo.channelCount = avAudioCodecParameters->ch_layout.nb_channels;
                audioInfo.type = toAudioType(static_cast<AVSampleFormat>(avAudioCodecParameters->format));
                if (core::AudioType::None == audioInfo.type)
                {
                    throw std::runtime_error(ftk::Format("Unsupported audio format: \"{0}\": {1}").
                        arg(fileName).
                        arg(av_get_sample_fmt_name(static_cast<AVSampleFormat>(avAudioCodecParameters->format))));
                }
                audioInfo.sampleRate = avAudioCodecParameters->sample_rate;
                p.info.audio.push_back(audioInfo);

                // Get the duration.
                int64_t sampleCount = 0;
                if (avAudioStream->duration != AV_NOPTS_VALUE)
                {
                    AVRational r;
                    r.num = 1;
                    r.den = audioInfo.sampleRate;
                    sampleCount = av_rescale_q(
                        avAudioStream->duration,
                        avAudioStream->time_base,
                        r);
                }
                else if (p.avFormatContext->duration != AV_NOPTS_VALUE)
                {
                    AVRational r;
                    r.num = 1;
                    r.den = audioInfo.sampleRate;
                    sampleCount = av_rescale_q(
                        p.avFormatContext->duration,
                        av_get_time_base_q(),
                        r);
                }
                const core::MediaRate audioRate{ audioInfo.sampleRate, 1 };
                p.info.audioDuration = core::MediaDuration{ sampleCount, audioRate };

                // Get the start time.
                if (!timecode.empty() && avVideoSpeed)
                {
                    AVTimecode avTimecode;
                    if (0 == av_timecode_init_from_string(
                        &avTimecode,
                        *avVideoSpeed,
                        timecode.c_str(),
                        nullptr))
                    {
                        const int64_t audioSampleStart = av_rescale_q(
                            avTimecode.start,
                            avSwap(avTimecode.rate),
                            AVRational{ 1, audioInfo.sampleRate });
                        p.info.audioStart = core::MediaTime{ audioSampleStart, audioRate };
                    }
                }
                else if (timeReference)
                {
                    p.info.audioStart = core::MediaTime{ *timeReference, audioRate };
                }

                p.avCodecContext.emplace(p.avAudioStream, std::move(avAudioCodecContext));
            }
        }

        std::shared_ptr<FFmpegRead> FFmpegRead::create(
            const ftk::Path& path,
            const ReadOptions& options,
            const std::shared_ptr<ftk::LogSystem>& logSystem)
        {
            return std::shared_ptr<FFmpegRead>(new FFmpegRead(path, {}, options, logSystem));
        }

        std::shared_ptr<FFmpegRead> FFmpegRead::create(
            const ftk::Path& path,
            const std::vector<ftk::MemFile>& mem,
            const ReadOptions& options,
            const std::shared_ptr<ftk::LogSystem>& logSystem)
        {
            return std::shared_ptr<FFmpegRead>(new FFmpegRead(path, mem, options, logSystem));
        }

        ReadInfo FFmpegRead::getInfo()
        {
            return _p->info;
        }

        std::shared_ptr<ftk::Image> FFmpegRead::getVideo(
            const core::MediaTime& time,
            const ReadOptions& options)
        {
            return nullptr;
        }

        std::shared_ptr<core::Audio> FFmpegRead::getAudio(
            const core::MediaTime& time,
            size_t sampleCount,
            const ReadOptions& options)
        {
            return nullptr;
        }

        struct FFmpegReadPlugin::Private
        {
            std::vector<AVCodecID> codecIds;
            std::vector<std::string> codecNames;

            //! \todo What is a better way to access the log system from the
            //! FFmpeg callback?
            static std::weak_ptr<ftk::LogSystem> logSystemWeak;
            static void logCallback(void*, int, const char*, va_list);
        };

        std::weak_ptr<ftk::LogSystem> FFmpegReadPlugin::Private::logSystemWeak;

        void FFmpegReadPlugin::Private::logCallback(void*, int level, const char* fmt, va_list vl)
        {
            switch (level)
            {
            case AV_LOG_PANIC:
            case AV_LOG_FATAL:
            case AV_LOG_ERROR:
            case AV_LOG_WARNING:
            case AV_LOG_INFO:
                if (auto logSystem = logSystemWeak.lock())
                {
                    char buf[ftk::cStringSize];
                    vsnprintf(buf, ftk::cStringSize, fmt, vl);
                    std::string s(buf);
                    ftk::removeTrailingNewlines(s);
                    logSystem->print("tl::io::FFmpegReadPlugin", s);
                }
                break;
            case AV_LOG_VERBOSE:
            default: break;
            }
        }

        void FFmpegReadPlugin::_init(
            const std::shared_ptr<ftk::LogSystem>& logSystem)
        {
            FTK_P();

            Private::logSystemWeak = logSystem;
            //av_log_set_level(AV_LOG_QUIET);
            av_log_set_level(AV_LOG_VERBOSE);
            av_log_set_callback(Private::logCallback);

            // Get codecs.
            const AVCodec* avCodec = nullptr;
            void* avCodecIterate = nullptr;
            while ((avCodec = av_codec_iterate(&avCodecIterate)))
            {
                if ((AVMEDIA_TYPE_VIDEO == avCodec->type || AVMEDIA_TYPE_AUDIO == avCodec->type) &&
                    av_codec_is_decoder(avCodec))
                {
                    p.codecIds.push_back(avCodec->id);
                    p.codecNames.push_back(avCodec->name);
                }
            }

            // Get formats.
            std::map<std::string, FileType> exts;
            const AVInputFormat* avInputFormat = nullptr;
            void* avInputFormatIterate = nullptr;
            std::vector<std::string> formatLog;
            while ((avInputFormat = av_demuxer_iterate(&avInputFormatIterate)))
            {
                if (avInputFormat->extensions)
                {
                    for (auto ext : ftk::split(avInputFormat->extensions, ','))
                    {
                        if (!ext.empty() && ext[0] != '.')
                        {
                            ext.insert(0, ".");
                        }
                        exts[ext] = FileType::Media;
                    }
                    formatLog.push_back(ftk::Format("{0} ({1})").arg(avInputFormat->name).arg(avInputFormat->extensions));
                }
            }
            //! \bug Why aren't these in the list of input formats?
            exts[".mxf"] = FileType::Media;
            exts[".wav"] = FileType::Media;

            IReadPlugin::_init("FFmpeg", exts, logSystem);

            logSystem->print(
                "tl::io::FFmpegReadPlugin",
                ftk::Format(
                    "\n"
                    "    * Codecs: {0}\n"
                    "    * Formats: {1}").
                arg(ftk::join(p.codecNames, ", ")).
                arg(ftk::join(formatLog, ", ")));
        }

        FFmpegReadPlugin::FFmpegReadPlugin() :
            _p(new Private)
        {}

        FFmpegReadPlugin::~FFmpegReadPlugin()
        {}
            
        std::shared_ptr<FFmpegReadPlugin> FFmpegReadPlugin::create(
            const std::shared_ptr<ftk::LogSystem>& logSystem)
        {
            auto out = std::shared_ptr<FFmpegReadPlugin>(new FFmpegReadPlugin);
            out->_init(logSystem);
            return out;
        }

        bool FFmpegReadPlugin::canRead(
            const ftk::Path& path,
            const ReadOptions& options)
        {
            const auto i = _exts.find(ftk::toLower(path.getExt()));
            return i != _exts.end();
        }

        std::shared_ptr<IRead> FFmpegReadPlugin::read(
            const ftk::Path& path,
            const ReadOptions& options)
        {
            return FFmpegRead::create(path, options, _logSystem);
        }

        std::shared_ptr<IRead> FFmpegReadPlugin::read(
            const ftk::Path& path,
            const std::vector<ftk::MemFile>& mem,
            const ReadOptions& options)
        {
            return FFmpegRead::create(path, mem, options, _logSystem);
        }
    }
}

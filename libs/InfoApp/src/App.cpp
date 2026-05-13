// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/InfoApp/App.h>

#include <ftk/Core/Context.h>
#include <ftk/Core/Format.h>

namespace tl
{
    namespace info_app
    {
        void App::_init(
            const std::shared_ptr<ftk::Context>& context,
            std::vector<std::string>& argv)
        {
            _cmdLine.inputs = ftk::CmdLineListArg<std::string>::create(
                "inputs",
                "Input timeline and media files.");

            _cmdLine.brief = ftk::CmdLineFlag::create(
                { "-brief" },
                "Print brief information.");

            IApp::_init(
                context,
                argv,
                "tlinfo",
                "Print information about timelines and media.",
                {
                    _cmdLine.inputs
                },
                {
                    _cmdLine.brief
                });
        }

        App::App()
        {}

        App::~App()
        {}

        std::shared_ptr<App> App::create(
            const std::shared_ptr<ftk::Context>& context,
            std::vector<std::string>& argv)
        {
            auto out = std::shared_ptr<App>(new App);
            out->_init(context, argv);
            return out;
        }

        void App::run()
        {
            auto readSystem = _context->getSystem<io::ReadSystem>();
            for (const auto& input : _cmdLine.inputs->getList())
            {
                _printTimeline(readSystem, ftk::Path(input));
            }
        }

        bool App::_printMedia(
            const std::shared_ptr<io::ReadSystem>& readSystem,
            const timeline::Media& media,
            int indent)
        {
            const auto& exts = readSystem->getExts();
            const auto i = exts.find(ftk::toLower(media.path.getExt()));
            if (i == exts.end())
            {
                return false;
            }
            try
            {
                auto read = readSystem->read(media.path, media.mem);
                if (!read)
                {
                    throw std::runtime_error("Unknown file: " + media.path.get());
                }
                const auto info = read->getInfo();

                _printIndented(media.path.get() + ":", indent);
                indent += 2;
                for (size_t i = 0; i < info.video.size(); ++i)
                {
                    const auto& video = info.video[i];
                    _printIndented(ftk::Format("video {0}:").arg(i), indent);
                    indent += 2;
                    _printIndented(ftk::Format("size: {0}x{1}").
                        arg(video.size.w).
                        arg(video.size.h),
                        indent);
                    _printIndented(ftk::Format("type: {0}").arg(video.type), indent);
                }
                if (!info.video.empty())
                {
                    if (info.videoStart)
                    {
                        _printIndented(ftk::Format("start: {0}").
                            arg(core::to_string(*info.videoStart)),
                            indent);
                    }
                    _printIndented(ftk::Format("duration: {0}").
                        arg(core::to_string(info.videoDuration)),
                        indent);
                    indent -= 2;
                }
                for (size_t i = 0; i < info.audio.size(); ++i)
                {
                    const auto& audio = info.audio[i];
                    _printIndented(ftk::Format("audio {0}:").arg(i), indent);
                    indent += 2;
                    _printIndented(ftk::Format("channels: {0}").arg(audio.channelCount), indent);
                    _printIndented(ftk::Format("type: {0}").arg(audio.type), indent);
                    _printIndented(ftk::Format("sample rate: {0}").arg(audio.sampleRate), indent);
                }
                if (!info.audio.empty())
                {
                    if (info.audioStart)
                    {
                        _printIndented(ftk::Format("start: {0}").
                            arg(core::to_string(*info.audioStart)),
                            indent);
                    }
                    _printIndented(ftk::Format("duration: {0}").
                        arg(core::to_string(info.audioDuration)),
                        indent);
                    indent -= 2;
                }
                if (!_cmdLine.brief->found())
                {
                    for (const auto& tag : info.tags)
                    {
                        _printIndented(ftk::Format("  tag \"{0}\": \"{1}\"").
                            arg(tag.first).
                            arg(tag.second),
                            indent);
                    }
                }
            }
            catch (const std::exception& e)
            {
                _printError(e.what());
            }
            return true;
        }

        void App::_printTimeline(
            const std::shared_ptr<io::ReadSystem>& readSystem,
            const ftk::Path& path)
        {
            try
            {
                auto timeline = timeline::Timeline::create(_context, path);

                _printIndented(path.get() + ":", 0);
                _printIndented(ftk::Format("rate: {0}").
                    arg(core::to_string(timeline->getRate())),
                    2);
                const auto& videoInfo = timeline->getVideoInfo();
                _printIndented(ftk::Format("video: {0}x{1} {2}").
                    arg(videoInfo.size.w).
                    arg(videoInfo.size.h).
                    arg(videoInfo.type),
                    2);
                const auto& audioInfo = timeline->getAudioInfo();
                _printIndented(ftk::Format("audio: {0}ch {1} {2}kHz").
                    arg(audioInfo.channelCount).
                    arg(audioInfo.type).
                    arg(audioInfo.sampleRate),
                    2);

                auto stack = timeline->getStack();
                _printIndented(ftk::Format("stack \"{0}\" ({1}, {2}):").
                    arg(stack->name).
                    arg(core::to_string(stack->startTime)).
                    arg(core::to_string(stack->duration)),
                    2);

                for (const auto& stackChild : stack->children)
                {
                    if (auto track = std::dynamic_pointer_cast<timeline::Track>(stackChild))
                    {
                        _printTrack(readSystem, track, 4);
                    }
                }
            }
            catch (const std::exception& e)
            {
                _printError(e.what());
            }
        }

        void App::_printTrack(
            const std::shared_ptr<io::ReadSystem>& readSystem,
            const std::shared_ptr<timeline::Track>& track,
            int indent)
        {
            _printIndented(ftk::Format("track \"{0}\" ({1}, {2}):").
                arg(track->name).
                arg(core::to_string(track->startTime)).
                arg(core::to_string(track->duration)),
                indent);
            indent += 2;

            for (const auto& trackChild : track->children)
            {
                if (auto clip = std::dynamic_pointer_cast<timeline::Clip>(trackChild))
                {
                    _printClip(readSystem, clip, indent);
                }
            }
        }

        void App::_printClip(
            const std::shared_ptr<io::ReadSystem>& readSystem,
            const std::shared_ptr<timeline::Clip>& clip,
            int indent)
        {
            _printIndented(ftk::Format("clip \"{0}\" ({1}, {2}):").
                arg(clip->name).
                arg(core::to_string(clip->startTime)).
                arg(core::to_string(clip->duration)),
                indent);
            indent += 2;
            _printIndented(ftk::Format("active media reference: \"{0}\"").
                arg(clip->activeMediaReference),
                indent);

            for (const auto& mediaRef : clip->mediaReferences)
            {
                _printIndented(ftk::Format("media reference \"{0}\":").
                    arg(mediaRef.first),
                    indent);
                indent += 2;
                if (mediaRef.second->availableRangeStart.has_value())
                {
                    _printIndented(ftk::Format("start: {0}").
                        arg(core::to_string(mediaRef.second->availableRangeStart.value())),
                        indent);
                }
                if (mediaRef.second->availableRangeDuration.has_value())
                {
                    _printIndented(ftk::Format("duration: {0}").
                        arg(core::to_string(mediaRef.second->availableRangeDuration.value())),
                        indent);
                }
                if (!_printMedia(readSystem, *mediaRef.second->media, indent))
                {
                    _printError("Unknown file: " + mediaRef.second->media->path.get());
                }
                indent -= 2;
            }
        }

        void App::_printIndented(const std::string& s, int indent)
        {
            _print(std::string(indent, ' ') + s);
        }
    }
}

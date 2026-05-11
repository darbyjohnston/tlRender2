// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include "OIIORead.h"

#include <OpenImageIO/filesystem.h>
#include <OpenImageIO/imagebufalgo.h>

namespace tl
{
    namespace io
    {
        namespace
        {
            constexpr core::MediaRate defaultRate{ 24, 1 };
            constexpr core::MediaRate defaultStillRate{ 1, 1 };

            void oiioDiscardError()
            {
                //! \bug If we don't get the error it will be automatically
                //! printed to stderr?
                OIIO::geterror();
            }

            ftk::ImageType fromOIIO(const OIIO::ImageSpec& oiio)
            {
                ftk::ImageType out = ftk::ImageType::None;
                if (1 == oiio.nchannels)
                {
                    switch (oiio.format.basetype)
                    {
                    case OIIO::TypeDesc::UINT8:  out = ftk::ImageType::L_U8;  break;
                    case OIIO::TypeDesc::UINT16: out = ftk::ImageType::L_U16; break;
                    case OIIO::TypeDesc::UINT32: out = ftk::ImageType::L_U32; break;
                    case OIIO::TypeDesc::HALF:   out = ftk::ImageType::L_F16; break;
                    case OIIO::TypeDesc::FLOAT:  out = ftk::ImageType::L_F32; break;
                    default: break;
                    }
                }
                else if (2 == oiio.nchannels)
                {
                    switch (oiio.format.basetype)
                    {
                    case OIIO::TypeDesc::UINT8:  out = ftk::ImageType::LA_U8;  break;
                    case OIIO::TypeDesc::UINT16: out = ftk::ImageType::LA_U16; break;
                    case OIIO::TypeDesc::UINT32: out = ftk::ImageType::LA_U32; break;
                    case OIIO::TypeDesc::HALF:   out = ftk::ImageType::LA_F16; break;
                    case OIIO::TypeDesc::FLOAT:  out = ftk::ImageType::LA_F32; break;
                    default: break;
                    }
                }
                else if (3 == oiio.nchannels)
                {
                    switch (oiio.format.basetype)
                    {
                    case OIIO::TypeDesc::UINT8:  out = ftk::ImageType::RGB_U8;  break;
                    case OIIO::TypeDesc::UINT16: out = ftk::ImageType::RGB_U16; break;
                    case OIIO::TypeDesc::UINT32: out = ftk::ImageType::RGB_U32; break;
                    case OIIO::TypeDesc::HALF:   out = ftk::ImageType::RGB_F16; break;
                    case OIIO::TypeDesc::FLOAT:  out = ftk::ImageType::RGB_F32; break;
                    default: break;
                    }
                }
                else if (oiio.nchannels >= 4)
                {
                    switch (oiio.format.basetype)
                    {
                    case OIIO::TypeDesc::UINT8:  out = ftk::ImageType::RGBA_U8;  break;
                    case OIIO::TypeDesc::UINT16: out = ftk::ImageType::RGBA_U16; break;
                    case OIIO::TypeDesc::UINT32: out = ftk::ImageType::RGBA_U32; break;
                    case OIIO::TypeDesc::HALF:   out = ftk::ImageType::RGBA_F16; break;
                    case OIIO::TypeDesc::FLOAT:  out = ftk::ImageType::RGBA_F32; break;
                    default: break;
                    }
                }
                return out;
            }
        }

        OIIORead::OIIORead(
            const ftk::Path& path,
            const std::vector<ftk::MemFile>& mem,
            const ReadOptions& options,
            const std::shared_ptr<ftk::LogSystem>& logSystem) :
            IRead(path, mem, options, logSystem)
        {}

        OIIORead::~OIIORead()
        {}

        std::shared_ptr<OIIORead> OIIORead::create(
            const ftk::Path& path,
            const ReadOptions& options,
            const std::shared_ptr<ftk::LogSystem>& logSystem)
        {
            return std::shared_ptr<OIIORead>(new OIIORead(path, {}, options, logSystem));
        }

        std::shared_ptr<OIIORead> OIIORead::create(
            const ftk::Path& path,
            const std::vector<ftk::MemFile>& mem,
            const ReadOptions& options,
            const std::shared_ptr<ftk::LogSystem>& logSystem)
        {
            return std::shared_ptr<OIIORead>(new OIIORead(path, mem, options, logSystem));
        }

        ReadInfo OIIORead::getInfo()
        {
            // Open the file.
            const std::string fileName =
                _path.getFrames().has_value() ?
                _path.getFrame(_path.getFrames()->min(), true) :
                _path.getFileName(true);
            std::unique_ptr<OIIO::Filesystem::IOMemReader> oiioMemReader;
            if (!_mem.empty())
            {
                oiioMemReader.reset(new OIIO::Filesystem::IOMemReader(
                    _mem.front().p,
                    _mem.front().size));
            }
            const auto oiioInput = OIIO::ImageInput::open(
                fileName,
                nullptr,
                oiioMemReader.get());
            if (!oiioInput)
            {
                oiioDiscardError();
                std::stringstream ss;
                ss << "Cannot open file: " << fileName;
                throw std::runtime_error(ss.str());
            }
            
            // Get information.
            ReadInfo out;
            auto oiioSpec = oiioInput->spec();
            for (const auto& i : oiioSpec.extra_attribs)
            {
                out.tags[std::string(i.name())] = i.get_string();
            }
            for (int sub = 0; oiioInput->seek_subimage(sub, 0); ++sub)
            {
                oiioSpec = oiioInput->spec();
                const ftk::ImageType imageType = fromOIIO(oiioSpec);
                if (ftk::ImageType::None == imageType)
                {
                    std::stringstream ss;
                    ss << "Unsupported file: " << fileName;
                    throw std::runtime_error(ss.str());
                }
                ftk::ImageInfo imageInfo(oiioSpec.width, oiioSpec.height, imageType);
                if (const auto param = oiioSpec.find_attribute("oiio:subimagename"))
                {
                    imageInfo.name = param->get_string();
                }
                else
                {
                    imageInfo.name = "";
                    for (int j = 0; j < oiioSpec.nchannels; ++j)
                    {
                        imageInfo.name += oiioSpec.channelnames[j];
                    }
                }
                imageInfo.layout.mirror.y = true;
                out.video.push_back(imageInfo);
            }
            if (_path.getFrames().has_value() && _path.getFrames()->size() > 1)
            {
                out.videoStart = core::MediaTime();
                out.videoStart->frames = _path.getFrames()->min();
                out.videoStart->rate = defaultRate;
                out.videoDuration.frames = _path.getFrames()->size();
                out.videoDuration.rate = defaultRate;
            }
            else
            {
                out.videoDuration.frames = 1;
                out.videoDuration.rate = defaultStillRate;
            }
            return out;
        }

        std::shared_ptr<ftk::Image> OIIORead::getVideo(
            const core::MediaTime& time,
            const ReadOptions& options)
        {
            return nullptr;
        }

        std::shared_ptr<core::Audio> OIIORead::getAudio(
            const core::MediaTime& time,
            size_t sampleCount,
            const ReadOptions& options)
        {
            return nullptr;
        }

        void OIIOReadPlugin::_init(
            const std::shared_ptr<ftk::LogSystem>& logSystem)
        {
            std::map<std::string, FileType> exts;
            for (const auto& i : OIIO::get_extension_map())
            {
                // Filter out FFmpeg extensions.
                if (i.first != "ffmpeg")
                {
                    for (const auto& ext : i.second)
                    {
                        exts["." + ext] = FileType::Seq;
                    }
                }
            }
            IReadPlugin::_init("OIIO", exts, logSystem);
        }

        OIIOReadPlugin::~OIIOReadPlugin()
        {}
            
        std::shared_ptr<OIIOReadPlugin> OIIOReadPlugin::create(
            const std::shared_ptr<ftk::LogSystem>& logSystem)
        {
            auto out = std::shared_ptr<OIIOReadPlugin>(new OIIOReadPlugin);
            out->_init(logSystem);
            return out;
        }

        bool OIIOReadPlugin::canRead(
            const ftk::Path& path,
            const ReadOptions& options)
        {
            const auto i = _exts.find(ftk::toLower(path.getExt()));
            return i != _exts.end();
        }

        std::shared_ptr<IRead> OIIOReadPlugin::read(
            const ftk::Path& path,
            const ReadOptions& options)
        {
            return OIIORead::create(path, options, _logSystem);
        }

        std::shared_ptr<IRead> OIIOReadPlugin::read(
            const ftk::Path& path,
            const std::vector<ftk::MemFile>& mem,
            const ReadOptions& options)
        {
            return OIIORead::create(path, mem, options, _logSystem);
        }
    }
}

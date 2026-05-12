// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include "OIIOWrite.h"

#include <OpenImageIO/filesystem.h>
#include <OpenImageIO/imagebufalgo.h>

namespace tl
{
    using namespace core;

    namespace io
    {
        namespace
        {
            void oiioDiscardError()
            {
                //! \bug If we don't get the error it will be automatically
                //! printed to stderr?
                OIIO::geterror();
            }

            OIIO::TypeDesc toOIIO(ftk::ImageType value)
            {
                OIIO::TypeDesc out;
                switch (value)
                {
                case ftk::ImageType::L_U8:
                case ftk::ImageType::LA_U8:
                case ftk::ImageType::RGB_U8:
                case ftk::ImageType::RGBA_U8:
                    out = OIIO::TypeDesc::UINT8;
                    break;
                case ftk::ImageType::L_U16:
                case ftk::ImageType::LA_U16:
                case ftk::ImageType::RGB_U16:
                case ftk::ImageType::RGBA_U16:
                    out = OIIO::TypeDesc::UINT16;
                    break;
                case ftk::ImageType::L_U32:
                case ftk::ImageType::LA_U32:
                case ftk::ImageType::RGB_U32:
                case ftk::ImageType::RGBA_U32:
                    out = OIIO::TypeDesc::UINT32;
                    break;
                case ftk::ImageType::L_F16:
                case ftk::ImageType::LA_F16:
                case ftk::ImageType::RGB_F16:
                case ftk::ImageType::RGBA_F16:
                    out = OIIO::TypeDesc::HALF;
                    break;
                case ftk::ImageType::L_F32:
                case ftk::ImageType::LA_F32:
                case ftk::ImageType::RGB_F32:
                case ftk::ImageType::RGBA_F32:
                    out = OIIO::TypeDesc::FLOAT;
                    break;
                default: break;
                }
                return out;
            }
        }

        OIIOWrite::OIIOWrite(
            const ftk::Path& path,
            const WriteOptions& options,
            const std::shared_ptr<ftk::LogSystem>& logSystem) :
            IWrite(path, options, logSystem)
        {}

        OIIOWrite::~OIIOWrite()
        {}

        std::shared_ptr<OIIOWrite> OIIOWrite::create(
            const ftk::Path& path,
            const WriteOptions& options,
            const std::shared_ptr<ftk::LogSystem>& logSystem)
        {
            return std::shared_ptr<OIIOWrite>(new OIIOWrite(path, options, logSystem));
        }

        void OIIOWrite::writeVideo(
            const core::MediaTime& time,
            const std::shared_ptr<ftk::Image>& image,
            const WriteOptions& options)
        {
            // Open the file.
            const std::string fileName =
                _path.getFrames().has_value() ?
                _path.getFrame(time.frames, true) :
                _path.get();
            auto oiioOutput = OIIO::ImageOutput::create(fileName);
            if (!oiioOutput)
            {
                throw std::runtime_error(OIIO::geterror());
            }
            const std::string format = oiioOutput->format_name();

            const auto& info = image->getInfo();
            OIIO::ImageSpec oiioSpec(
                info.size.w,
                info.size.h,
                ftk::getChannelCount(info.type),
                toOIIO(info.type));
            for (const auto& tag : image->getTags())
            {
                oiioSpec.attribute(tag.first, tag.second);
            }
            /*if ("openexr" == format)
            {
                auto i = options.find("OpenEXR/Compression");
                if (i != options.end())
                {
                    std::string compression = i->second;
                    if ("dwaa" == compression || "dwab" == compression)
                    {
                        i = options.find("OpenEXR/DWACompressionLevel");
                        if (i != options.end())
                        {
                            compression += ":" + i->second;
                        }
                    }
                    oiioSpec.attribute("compression", compression);
                }
            }*/
            if (!oiioOutput->open(fileName, oiioSpec))
            {
                throw std::runtime_error(OIIO::geterror());
            }

            // Write the image.
            const size_t scanlineByteCount = oiioSpec.scanline_bytes();
            if (!oiioOutput->write_image(
                oiioSpec.format,
                info.layout.mirror.y ?
                    image->getData() :
                    image->getData() + (info.size.h - 1) * scanlineByteCount,
                OIIO::AutoStride,
                info.layout.mirror.y ? scanlineByteCount : -scanlineByteCount,
                OIIO::AutoStride))
            {
                throw std::runtime_error(OIIO::geterror());
            }
        }

        void OIIOWrite::writeAudio(
            const core::MediaTime&,
            const std::shared_ptr<core::Audio>&,
            const WriteOptions&)
        {}

        void OIIOWritePlugin::_init(
            const std::shared_ptr<ftk::LogSystem>& logSystem)
        {
            std::vector<std::string> exts;
            for (const auto& i : OIIO::get_extension_map())
            {
                // Filter out FFmpeg extensions.
                if (i.first != "ffmpeg")
                {
                    for (const auto& ext : i.second)
                    {
                        exts.push_back("." + ext);
                    }
                }
            }
            IWritePlugin::_init("OIIO", exts, logSystem);
        }

        OIIOWritePlugin::~OIIOWritePlugin()
        {}
            
        std::shared_ptr<OIIOWritePlugin> OIIOWritePlugin::create(
            const std::shared_ptr<ftk::LogSystem>& logSystem)
        {
            auto out = std::shared_ptr<OIIOWritePlugin>(new OIIOWritePlugin);
            out->_init(logSystem);
            return out;
        }

        bool OIIOWritePlugin::canWrite(
            const ftk::Path& path,
            const WriteOptions& options)
        {
            const auto i = std::find(_exts.begin(), _exts.end(), ftk::toLower(path.getExt()));
            return i != _exts.end();
        }

        std::shared_ptr<IWrite> OIIOWritePlugin::write(
            const ftk::Path& path,
            const WriteOptions& options)
        {
            return OIIOWrite::create(path, options, _logSystem);
        }
    }
}

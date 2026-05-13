// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include "OIIOUtil.h"

#include <ftk/Core/Format.h>

#include <OpenImageIO/imagebufalgo.h>

namespace tl
{
    namespace render
    {
        OIIO::TypeDesc toOIIO(ftk::ImageType value)
        {
            switch (value)
            {
            case ftk::ImageType::L_U8:
            case ftk::ImageType::LA_U8:
            case ftk::ImageType::RGB_U8:
            case ftk::ImageType::RGBA_U8:    return OIIO::TypeDesc::UINT8;
            case ftk::ImageType::L_U16:
            case ftk::ImageType::LA_U16:
            case ftk::ImageType::RGB_U16:
            case ftk::ImageType::RGBA_U16:   return OIIO::TypeDesc::UINT16;
            case ftk::ImageType::L_U32:
            case ftk::ImageType::LA_U32:
            case ftk::ImageType::RGB_U32:
            case ftk::ImageType::RGBA_U32:   return OIIO::TypeDesc::UINT32;
            case ftk::ImageType::L_F16:
            case ftk::ImageType::LA_F16:
            case ftk::ImageType::RGB_F16:
            case ftk::ImageType::RGBA_F16:   return OIIO::TypeDesc::HALF;
            case ftk::ImageType::L_F32:
            case ftk::ImageType::LA_F32:
            case ftk::ImageType::RGB_F32:
            case ftk::ImageType::RGBA_F32:   return OIIO::TypeDesc::FLOAT;
            default: break;
            }
            return OIIO::TypeDesc();
        }

        ftk::ImageType fromOIIO(const OIIO::ImageSpec& spec)
        {
            const int n = spec.nchannels;
            const auto bt = spec.format.basetype;
            if (1 == n)
            {
                switch (bt)
                {
                case OIIO::TypeDesc::UINT8:  return ftk::ImageType::L_U8;
                case OIIO::TypeDesc::UINT16: return ftk::ImageType::L_U16;
                case OIIO::TypeDesc::UINT32: return ftk::ImageType::L_U32;
                case OIIO::TypeDesc::HALF:   return ftk::ImageType::L_F16;
                case OIIO::TypeDesc::FLOAT:  return ftk::ImageType::L_F32;
                default: break;
                }
            }
            else if (2 == n)
            {
                switch (bt)
                {
                case OIIO::TypeDesc::UINT8:  return ftk::ImageType::LA_U8;
                case OIIO::TypeDesc::UINT16: return ftk::ImageType::LA_U16;
                case OIIO::TypeDesc::UINT32: return ftk::ImageType::LA_U32;
                case OIIO::TypeDesc::HALF:   return ftk::ImageType::LA_F16;
                case OIIO::TypeDesc::FLOAT:  return ftk::ImageType::LA_F32;
                default: break;
                }
            }
            else if (3 == n)
            {
                switch (bt)
                {
                case OIIO::TypeDesc::UINT8:  return ftk::ImageType::RGB_U8;
                case OIIO::TypeDesc::UINT16: return ftk::ImageType::RGB_U16;
                case OIIO::TypeDesc::UINT32: return ftk::ImageType::RGB_U32;
                case OIIO::TypeDesc::HALF:   return ftk::ImageType::RGB_F16;
                case OIIO::TypeDesc::FLOAT:  return ftk::ImageType::RGB_F32;
                default: break;
                }
            }
            else if (n >= 4)
            {
                switch (bt)
                {
                case OIIO::TypeDesc::UINT8:  return ftk::ImageType::RGBA_U8;
                case OIIO::TypeDesc::UINT16: return ftk::ImageType::RGBA_U16;
                case OIIO::TypeDesc::UINT32: return ftk::ImageType::RGBA_U32;
                case OIIO::TypeDesc::HALF:   return ftk::ImageType::RGBA_F16;
                case OIIO::TypeDesc::FLOAT:  return ftk::ImageType::RGBA_F32;
                default: break;
                }
            }
            return ftk::ImageType::None;
        }

        OIIO::ImageBuf wrap(const ftk::Image& image)
        {
            const auto& info = image.getInfo();
            const int channels = ftk::getChannelCount(info.type);
            OIIO::ImageSpec spec(
                info.size.w,
                info.size.h,
                channels,
                toOIIO(info.type));
            const size_t scanlineBytes = spec.scanline_bytes();
            uint8_t* data = const_cast<uint8_t*>(image.getData());
            if (info.layout.mirror.y)
            {
                return OIIO::ImageBuf(
                    spec,
                    data + (info.size.h - 1) * scanlineBytes,
                    OIIO::AutoStride,
                    -static_cast<OIIO::stride_t>(scanlineBytes),
                    OIIO::AutoStride);
            }
            return OIIO::ImageBuf(spec, data);
        }

        std::shared_ptr<ftk::Image> materialize(const OIIO::ImageBuf& buf)
        {
            const auto& spec = buf.spec();
            const ftk::ImageType type = fromOIIO(spec);
            if (type == ftk::ImageType::None)
            {
                throw std::runtime_error(ftk::Format(
                    "Unsupported image format: {0} channels of {1}").
                    arg(spec.nchannels).
                    arg(spec.format.c_str()));
            }
            const ftk::ImageInfo info(spec.width, spec.height, type);
            auto out = ftk::Image::create(info);
            if (!buf.get_pixels(
                OIIO::ROI::All(),
                spec.format,
                out->getData()))
            {
                throw std::runtime_error(OIIO::geterror());
            }
            return out;
        }
        
        OIIO::ImageBuf addAlpha(const OIIO::ImageBuf& input)
        {
            const int n = input.spec().nchannels;
            if (n == 1)
            {
                // L → LA
                const int order[] = { 0, -1 };
                const float values[] = { 0, 1.0f };
                const std::string names[] = { "", "A" };
                return OIIO::ImageBufAlgo::channels(input, 2, order, values, names);
            }
            if (n == 3)
            {
                // RGB → RGBA
                const int order[] = { 0, 1, 2, -1 };
                const float values[] = { 0, 0, 0, 1.0f };
                const std::string names[] = { "", "", "", "A" };
                return OIIO::ImageBufAlgo::channels(input, 4, order, values, names);
            }
            return input;
        }
    }
}


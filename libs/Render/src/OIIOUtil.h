// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <ftk/Core/Image.h>

#include <OpenImageIO/imagebuf.h>

namespace tl
{
    namespace render
    {
        //! Convert to OIIO.
        OIIO::TypeDesc toOIIO(ftk::ImageType);
        
        //! Convert from OIIO.
        ftk::ImageType fromOIIO(const OIIO::ImageSpec&);

        //! Wrap an ftk::Image as an OIIO::ImageBuf without copying pixels.
        //! The returned ImageBuf borrows the ftk::Image's memory; the
        //! caller must keep the ftk::Image alive for the ImageBuf's
        //! lifetime. If the ftk::Image is stored bottom-up
        //! (info.layout.mirror.y), the wrap uses a negative row stride so
        //! the resulting ImageBuf is logically top-down.
        OIIO::ImageBuf wrap(const ftk::Image&);

        //! Materialize an OIIO::ImageBuf into a new ftk::Image. Always
        //! produces a top-down image (mirror.y = false). Copies pixel
        //! data. Throws if the ImageBuf's spec doesn't map to an
        //! ftk::ImageType.
        std::shared_ptr<ftk::Image> materialize(const OIIO::ImageBuf&);
        
        //! Convert L-LA and RGB->RGBA.
        OIIO::ImageBuf addAlpha(const OIIO::ImageBuf&);
    }
}

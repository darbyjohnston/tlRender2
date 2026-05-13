// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/Render/RenderUtil.h>

#include "OIIOUtil.h"

#include <OpenImageIO/imagebufalgo.h>

namespace tl
{
    namespace render
    {
        bool compare(
            const std::shared_ptr<ftk::Image>& a,
            const std::shared_ptr<ftk::Image>& b)
        {
            if (a->getInfo() != b->getInfo()) return false;
            OIIO::ImageBuf bufA = wrap(*a);
            OIIO::ImageBuf bufB = wrap(*b);
            auto cmp = OIIO::ImageBufAlgo::compare(bufA, bufB, 0.0f, 0.0f);
            return cmp.nfail == 0;
        }
    }
}

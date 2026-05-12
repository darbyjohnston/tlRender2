// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/Render/Render.h>

namespace tl
{
    namespace render
    {
        VideoRenderer::VideoRenderer(const std::shared_ptr<ftk::Context>& context)
        {}

        std::shared_ptr<VideoRenderer> VideoRenderer::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            auto out = std::shared_ptr<VideoRenderer>(new VideoRenderer(context));
            return out;
        }
        
        std::shared_ptr<ftk::Image> VideoRenderer::render(const timeline::VideoGraph& graph)
        {
            return nullptr;
        }
    }
}

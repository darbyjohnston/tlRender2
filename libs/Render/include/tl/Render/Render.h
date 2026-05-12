// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <tl/Timeline/VideoGraph.h>

#include <ftk/Core/Context.h>
#include <ftk/Core/Image.h>

namespace tl
{
    namespace render
    {
        class TL_API_TYPE VideoRenderer : public std::enable_shared_from_this<VideoRenderer>
        {
            FTK_NON_COPYABLE(VideoRenderer);

        protected:
            VideoRenderer(const std::shared_ptr<ftk::Context>&);

        public:
            //! Create a new renderer.
            static std::shared_ptr<VideoRenderer> create(
                const std::shared_ptr<ftk::Context>&);
            
            //! Render a graph to an image.
            std::shared_ptr<ftk::Image> render(const timeline::VideoGraph& graph);
        };
    }
}

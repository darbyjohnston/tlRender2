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
        //! Video renderer.
        //!
        //! A graph evaluator: walks a VideoGraph and produces an image
        //! according to the graph's structure. Output size and type are
        //! determined by the graph (typically by a SolidColorVideo canvas
        //! at the bottom of a Composite stack). Holds a reader cache
        //! across calls; intended to be reused across frames.
        class TL_API_TYPE VideoRenderer
        {
            FTK_NON_COPYABLE(VideoRenderer);

        protected:
            VideoRenderer(const std::shared_ptr<ftk::Context>&);

        public:
            ~VideoRenderer();

            //! Create a new renderer.
            static std::shared_ptr<VideoRenderer> create(
                const std::shared_ptr<ftk::Context>&);

            //! Render a graph to an image.
            std::shared_ptr<ftk::Image> render(const timeline::VideoGraph&);

        private:
            FTK_PRIVATE();
        };
    }
}

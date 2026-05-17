// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <RenderTest/RenderTest.h>

#include <tl/Render/Render.h>
#include <tl/Render/RenderUtil.h>
#include <tl/Timeline/Timeline.h>

#include <ftk/Core/Assert.h>
#include <ftk/Core/Format.h>

namespace tl
{
    namespace render_test
    {
        RenderTest::RenderTest(
            const std::shared_ptr<ftk::Context>& context) :
            ITest(context, "tl::render_test::RenderTest")
        {}

        RenderTest::~RenderTest()
        {}

        std::shared_ptr<RenderTest> RenderTest::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            return std::shared_ptr<RenderTest>(new RenderTest(context));
        }

        void RenderTest::run()
        {
            _render();
        }

        void RenderTest::_render()
        {
            auto context = _context.lock();
            const ftk::Path path(TL_TEST_DATA_DIR, "Overlay.otio");
            auto timeline = timeline::Timeline::create(context, path);
            auto renderer = render::VideoRenderer::create(context);

            const ftk::Path path2(TL_TEST_DATA_DIR, "Overlay.otioz");
            auto timeline2 = timeline::Timeline::create(context, path2);
            auto renderer2 = render::VideoRenderer::create(context);

            for (core::Time t = timeline->getStartTime();
                t < timeline->getStartTime() + timeline->getDuration();
                ++t.frames)
            {
                _print(ftk::Format("render frame: {0}").arg(core::to_string(t)));
                auto graph = timeline->getVideo(t);
                FTK_ASSERT(graph);
                auto graph2 = timeline2->getVideo(t);
                FTK_ASSERT(graph2);
                auto image = renderer->render(*graph);
                FTK_ASSERT(image);
                auto image2 = renderer2->render(*graph2);
                FTK_ASSERT(image2);
                FTK_ASSERT(image->getInfo() == image2->getInfo());
                FTK_ASSERT(render::compare(image, image2));
            }
        }
    }
}

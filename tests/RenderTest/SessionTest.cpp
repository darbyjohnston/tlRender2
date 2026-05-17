// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <RenderTest/SessionTest.h>

#include <tl/Render/RenderUtil.h>
#include <tl/Render/Session.h>

#include <ftk/Core/Assert.h>
#include <ftk/Core/Format.h>

namespace tl
{
    namespace render_test
    {
        SessionTest::SessionTest(
            const std::shared_ptr<ftk::Context>& context) :
            ITest(context, "tl::render_test::SessionTest")
        {}

        SessionTest::~SessionTest()
        {}

        std::shared_ptr<SessionTest> SessionTest::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            return std::shared_ptr<SessionTest>(new SessionTest(context));
        }

        void SessionTest::run()
        {
            _render();
        }

        void SessionTest::_render()
        {
            auto context = _context.lock();
            const ftk::Path path(TL_TEST_DATA_DIR, "Overlay.otio");
            auto session = render::Session::create(context, path);

            const ftk::Path path2(TL_TEST_DATA_DIR, "Overlay.otioz");
            auto session2 = render::Session::create(context, path2);

            auto timeline = session->getTimeline();
            for (core::Time t = timeline->getStartTime();
                t < timeline->getStartTime() + timeline->getDuration();
                ++t.frames)
            {
                _print(ftk::Format("render frame: {0}").arg(core::to_string(t)));
                auto image = session->render(t).get();
                FTK_ASSERT(image);
                auto image2 = session2->render(t).get();
                FTK_ASSERT(image2);
                FTK_ASSERT(image->getInfo() == image2->getInfo());
                FTK_ASSERT(render::compare(image, image2));
            }
        }
    }
}

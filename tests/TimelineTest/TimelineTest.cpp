// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <TimelineTest/TimelineTest.h>

#include <tl/Timeline/Timeline.h>

#include <ftk/Core/Assert.h>

namespace tl
{
    namespace timeline_test
    {
        TimelineTest::TimelineTest(
            const std::shared_ptr<ftk::Context>& context) :
            ITest(context, "tl::timeline_test::TimelineTest")
        {}

        TimelineTest::~TimelineTest()
        {}

        std::shared_ptr<TimelineTest> TimelineTest::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            return std::shared_ptr<TimelineTest>(new TimelineTest(context));
        }

        void TimelineTest::run()
        {
            _create();
        }

        void TimelineTest::_create()
        {
            auto context = _context.lock();
            {
                const ftk::Path path(TL_TEST_DATA_DIR, "SingleClip.otio");
                auto timeline = timeline::Timeline::create(context, path);
                FTK_ASSERT(path == timeline->getPath());
                FTK_ASSERT(core::MediaRate(24) == timeline->getRate());
                FTK_ASSERT(core::Time() == timeline->getStartTime());
                FTK_ASSERT(timeline->getDuration() > core::Duration());

                const auto& stack = timeline->getStack();
                FTK_ASSERT(!stack->children.empty());
                for (const auto& stackIt : stack->children)
                {
                    auto track = std::dynamic_pointer_cast<timeline::Track>(stackIt);
                    FTK_ASSERT(track);
                    FTK_ASSERT(!stack->children.empty());
                    for (const auto& trackIt : track->children)
                    {
                        auto clip = std::dynamic_pointer_cast<timeline::Clip>(trackIt);
                        FTK_ASSERT(clip);
                        FTK_ASSERT(1 == clip->mediaReferences.size());
                    }
                }
            }
        }
    }
}

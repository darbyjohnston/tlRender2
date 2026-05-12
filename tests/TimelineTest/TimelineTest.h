// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <ftk/TestLib/ITest.h>

namespace tl
{
    namespace timeline_test
    {
        class TimelineTest : public ftk::test::ITest
        {
        protected:
            TimelineTest(const std::shared_ptr<ftk::Context>&);

        public:
            virtual ~TimelineTest();

            static std::shared_ptr<TimelineTest> create(
                const std::shared_ptr<ftk::Context>&);

            void run() override;

        private:
            void _create();
            void _getVideo();
        };
    }
}

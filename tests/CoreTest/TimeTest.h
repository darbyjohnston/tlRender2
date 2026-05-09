// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <ftk/TestLib/ITest.h>

namespace tl
{
    namespace core_test
    {
        class TimeTest : public ftk::test::ITest
        {
        protected:
            TimeTest(const std::shared_ptr<ftk::Context>&);

        public:
            virtual ~TimeTest();

            static std::shared_ptr<TimeTest> create(
                const std::shared_ptr<ftk::Context>&);

            void run() override;

        private:
            void _members();
            void _arithmetic();
            void _comparison();
            void _mediaRate();
            void _mediaTime();
            void _rescale();
            void _serialize();
        };
    }
}

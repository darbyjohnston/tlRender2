// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <ftk/TestLib/ITest.h>

namespace tl
{
    namespace core_test
    {
        class AudioTest : public ftk::test::ITest
        {
        protected:
            AudioTest(const std::shared_ptr<ftk::Context>&);

        public:
            virtual ~AudioTest();

            static std::shared_ptr<AudioTest> create(
                const std::shared_ptr<ftk::Context>&);

            void run() override;

        private:
            void _types();
            void _info();
            void _audio();
            void _objectCount();
            void _combine();
            void _reverse();
            void _convert();
            void _mix();
            void _move();
        };
    }
}

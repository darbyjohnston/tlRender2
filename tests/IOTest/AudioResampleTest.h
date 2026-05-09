// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <ftk/TestLib/ITest.h>

namespace tl
{
    namespace io_test
    {
        class AudioResampleTest : public ftk::test::ITest
        {
        protected:
            AudioResampleTest(const std::shared_ptr<ftk::Context>&);

        public:
            virtual ~AudioResampleTest();

            static std::shared_ptr<AudioResampleTest> create(
                const std::shared_ptr<ftk::Context>&);

            void run() override;

        private:
            void _create();
            void _process();
        };
    }
}

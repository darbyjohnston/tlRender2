// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <ftk/TestLib/ITest.h>

namespace tl
{
    namespace player_app_test
    {
        class FilesModelTest : public ftk::test::ITest
        {
        protected:
            FilesModelTest(const std::shared_ptr<ftk::Context>&);

        public:
            virtual ~FilesModelTest();

            static std::shared_ptr<FilesModelTest> create(
                const std::shared_ptr<ftk::Context>&);

            void run() override;
        };
    }
}

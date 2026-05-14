// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <ftk/TestLib/ITest.h>

namespace tl
{
    namespace render_test
    {
        class SessionTest : public ftk::test::ITest
        {
        protected:
            SessionTest(const std::shared_ptr<ftk::Context>&);

        public:
            virtual ~SessionTest();

            static std::shared_ptr<SessionTest> create(
                const std::shared_ptr<ftk::Context>&);

            void run() override;

        private:
            void _render();
        };
    }
}

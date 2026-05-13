// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <ftk/TestLib/ITest.h>

namespace tl
{
    namespace render_test
    {
        class RenderTest : public ftk::test::ITest
        {
        protected:
            RenderTest(const std::shared_ptr<ftk::Context>&);

        public:
            virtual ~RenderTest();

            static std::shared_ptr<RenderTest> create(
                const std::shared_ptr<ftk::Context>&);

            void run() override;

        private:
            void _render();
        };
    }
}

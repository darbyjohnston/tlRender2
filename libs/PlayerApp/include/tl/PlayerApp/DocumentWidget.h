// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <tl/Render/Session.h>

#include <ftk/UI/IWidget.h>

namespace tl
{
    namespace player_app
    {
        class App;

        //! Document widget.
        class TL_API_TYPE DocumentWidget : public ftk::IWidget
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<render::Session>&);

            DocumentWidget();

        public:
            virtual ~DocumentWidget();

            //! Create a new window.
            static std::shared_ptr<DocumentWidget> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<render::Session>&);

            ftk::Size2I getSizeHint() const override;
            void setGeometry(const ftk::Box2I&) override;

        private:
            FTK_PRIVATE();
        };
    }
}

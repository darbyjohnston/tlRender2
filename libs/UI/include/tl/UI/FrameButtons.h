// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <tl/Timeline/Player.h>

#include <ftk/UI/IWidget.h>

namespace tl
{
    namespace ui
    {
        //! Frame buttons.
        class TL_API_TYPE FrameButtons : public ftk::IWidget
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<ftk::IWidget>& parent = nullptr);

            FrameButtons();

        public:
            virtual ~FrameButtons();

            //! Create a new widget.
            static std::shared_ptr<FrameButtons> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            //! Set the callback.
            void setCallback(const std::function<void(timeline::FrameAction)>&);
            
            ftk::Size2I getSizeHint() const override;
            void setGeometry(const ftk::Box2I&) override;

        private:
            FTK_PRIVATE();
        };
    }
}

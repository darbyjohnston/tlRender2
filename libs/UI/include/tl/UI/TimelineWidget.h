// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <tl/UI/TimeUnitsModel.h>
#include <tl/Timeline/Player.h>

#include <ftk/UI/IMouseWidget.h>

namespace tl
{
    namespace ui
    {
        //! Timeline widget.
        class TL_API_TYPE TimelineWidget : public ftk::IMouseWidget
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<TimeUnitsModel>&,
                const std::shared_ptr<timeline::Player>&,
                const std::shared_ptr<ftk::IWidget>& parent = nullptr);

            TimelineWidget();

        public:
            virtual ~TimelineWidget();

            //! Create a new widget.
            static std::shared_ptr<TimelineWidget> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<TimeUnitsModel>&,
                const std::shared_ptr<timeline::Player>&,
                const std::shared_ptr<IWidget>& parent = nullptr);
            
            ftk::Size2I getSizeHint() const override;
            void setGeometry(const ftk::Box2I&) override;

            private:
            FTK_PRIVATE();
        };
    }
}

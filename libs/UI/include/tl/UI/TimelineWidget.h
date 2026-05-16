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
            void styleEvent(const ftk::StyleEvent&) override;
            void sizeHintEvent(const ftk::SizeHintEvent&) override;
            void clipEvent(const ftk::Box2I&, bool) override;
            void drawEvent(const ftk::Box2I&, const ftk::DrawEvent&) override;
            void mouseEnterEvent(ftk::MouseEnterEvent&) override;
            void mouseLeaveEvent() override;
            void mouseMoveEvent(ftk::MouseMoveEvent&) override;
            void mousePressEvent(ftk::MouseClickEvent&) override;
            void mouseReleaseEvent(ftk::MouseClickEvent&) override;
            void scrollEvent(ftk::ScrollEvent&) override;
            void keyPressEvent(ftk::KeyEvent&) override;
            void keyReleaseEvent(ftk::KeyEvent&) override;

        private:
            int _timeToPos(const core::Time&) const;
            core::Time _posToTime(int) const;

            FTK_PRIVATE();
        };
    }
}

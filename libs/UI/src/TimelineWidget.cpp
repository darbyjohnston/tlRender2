// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/UI/TimelineWidget.h>

#include <ftk/UI/DrawUtil.h>

namespace tl
{
    using namespace core;

    namespace ui
    {
        struct TimelineWidget::Private
        {
            std::shared_ptr<TimeUnitsModel> timeUnitsModel;
            std::shared_ptr<timeline::Player> player;
            std::string timeText;
            ftk::FontType font = ftk::FontType::Regular;
            int fontSize = ftk::FontInfo().size;

            std::shared_ptr<ftk::Observer<Time>> startTimeObserver;
            std::shared_ptr<ftk::Observer<Duration>> durationObserver;
            std::shared_ptr<ftk::Observer<Time>> timeObserver;

            struct SizeData
            {
                bool init = true;
                int margin = 0;
                int spacing = 0;
                int border = 0;
                ftk::FontInfo fontInfo;
                ftk::FontMetrics fontMetrics;
                ftk::Size2I timeTextSize;
                ftk::Size2I sizeHint;
            };
            SizeData size;

            struct DrawData
            {
                std::vector<std::shared_ptr<ftk::Glyph> > timeGlyphs;
            };
            std::optional<DrawData> draw;
        };

        void TimelineWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<TimeUnitsModel>& timeUnitsModel,
            const std::shared_ptr<timeline::Player>& player,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IMouseWidget::_init(context, "tl::ui::TimelineWidget", parent);
            FTK_P();

            setHStretch(ftk::Stretch::Expanding);
            _setMouseHoverEnabled(true);
            _setMousePressEnabled(true);

            p.timeUnitsModel = timeUnitsModel;
            p.player = player;
    
            auto timeline = player->getTimeline();
            p.startTimeObserver = ftk::Observer<Time>::create(
                timeline->observeStartTime(),
                [this](const Time&)
                {
                    FTK_P();
                    p.size.init = true;
                    setSizeUpdate();
                    setDrawUpdate();
                });

            p.durationObserver = ftk::Observer<Duration>::create(
                timeline->observeDuration(),
                [this](const Duration&)
                {
                    FTK_P();
                    p.size.init = true;
                    setSizeUpdate();
                    setDrawUpdate();
                });

            p.timeObserver = ftk::Observer<Time>::create(
                player->observeTime(),
                [this](const Time& time)
                {
                    FTK_P();
                    p.timeText = p.timeUnitsModel->getLabel(
                        time,
                        p.player->getRate());
                    p.size.init = true;
                    setSizeUpdate();
                    setDrawUpdate();
                });
        }

        TimelineWidget::TimelineWidget() :
            _p(new Private)
        {}

        TimelineWidget::~TimelineWidget()
        {}

        std::shared_ptr<TimelineWidget> TimelineWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<TimeUnitsModel>& timeUnitsModel,
            const std::shared_ptr<timeline::Player>& player,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<TimelineWidget>(new TimelineWidget);
            out->_init(context, timeUnitsModel, player, parent);
            return out;
        }

        ftk::Size2I TimelineWidget::getSizeHint() const
        {
            return _p->size.sizeHint;
        }

        void TimelineWidget::setGeometry(const ftk::Box2I& value)
        {
            if (value != getGeometry())
            {
                _p->draw.reset();
            }
            IMouseWidget::setGeometry(value);
        }

        void TimelineWidget::styleEvent(const ftk::StyleEvent& event)
        {
            FTK_P();
            if (event.hasChanges())
            {
                p.size.init = true;
                p.draw.reset();
            }
        }

        void TimelineWidget::sizeHintEvent(const ftk::SizeHintEvent& event)
        {
            FTK_P();
            if (p.size.init)
            {
                p.size.init = false;
                p.size.margin = event.style->getSizeRole(ftk::SizeRole::MarginInside, event.displayScale);
                p.size.spacing = event.style->getSizeRole(ftk::SizeRole::SpacingSmall, event.displayScale);
                p.size.border = event.style->getSizeRole(ftk::SizeRole::Border, event.displayScale);
                p.size.fontInfo = event.style->getFont(p.font, p.fontSize, event.displayScale);
                p.size.fontMetrics = event.fontSystem->getMetrics(p.size.fontInfo);
                p.size.timeTextSize = event.fontSystem->getSize(p.timeText, p.size.fontInfo);

                p.size.sizeHint = ftk::margin(p.size.timeTextSize, p.size.margin);

                p.draw.reset();
            }
        }

        void TimelineWidget::clipEvent(const ftk::Box2I& clipRect, bool clipped)
        {
            IMouseWidget::clipEvent(clipRect, clipped);
            FTK_P();
            if (clipped)
            {
                p.draw.reset();
            }
        }

        void TimelineWidget::drawEvent(const ftk::Box2I& drawRect, const ftk::DrawEvent& event)
        {
            IMouseWidget::drawEvent(drawRect, event);
            FTK_P();

            if (!p.draw.has_value())
            {
                p.draw = Private::DrawData();
                if (!p.timeText.empty())
                {
                    p.draw->timeGlyphs = event.fontSystem->getGlyphs(p.timeText, p.size.fontInfo);
                }
            }

            const ftk::Box2I& g = getGeometry();
            event.render->drawRect(
                g,
                event.style->getColorRole(ftk::ColorRole::Base));

            const ftk::Box2I g2 = ftk::margin(g, -p.size.margin);
            ftk::V2I pos(_timeToPos(p.player->getTime()), g2.min.y);
            event.render->drawRect(
                ftk::Box2I(pos.x, pos.y, p.size.border * 2, g2.h()),
                event.style->getColorRole(ftk::ColorRole::Red));

            if (pos.x + p.size.border * 2 + p.size.spacing + p.size.timeTextSize.w < g2.max.x)
            {
                pos.x += p.size.border * 2 + p.size.spacing;
            }
            else
            {
                pos.x -= (p.size.spacing + p.size.timeTextSize.w);
            }
            event.render->drawText(
                p.draw->timeGlyphs,
                p.size.fontMetrics,
                pos,
                event.style->getColorRole(ftk::ColorRole::Text));
        }

        void TimelineWidget::mouseEnterEvent(ftk::MouseEnterEvent& event)
        {
            IMouseWidget::mouseEnterEvent(event);
            setDrawUpdate();
        }

        void TimelineWidget::mouseLeaveEvent()
        {
            IMouseWidget::mouseLeaveEvent();
            setDrawUpdate();
        }

        void TimelineWidget::mouseMoveEvent(ftk::MouseMoveEvent& event)
        {
            IMouseWidget::mouseMoveEvent(event);
            FTK_P();
            if (_isMousePressed())
            {
                p.player->setTime(_posToTime(_getMousePos().x));
            }
        }

        void TimelineWidget::mousePressEvent(ftk::MouseClickEvent& event)
        {
            IMouseWidget::mousePressEvent(event);
            FTK_P();
            p.player->setTime(_posToTime(_getMousePos().x));
            setDrawUpdate();
        }

        void TimelineWidget::mouseReleaseEvent(ftk::MouseClickEvent& event)
        {
            IMouseWidget::mouseReleaseEvent(event);
            FTK_P();
            p.player->setTime(_posToTime(_getMousePos().x));
            setDrawUpdate();
        }

        void TimelineWidget::scrollEvent(ftk::ScrollEvent& event)
        {
            FTK_P();
            //event.accept = true;
        }

        void TimelineWidget::keyPressEvent(ftk::KeyEvent& event)
        {
            FTK_P();
            if (isEnabled() && 0 == event.modifiers)
            {
                switch (event.key)
                {
                case ftk::Key::Plus:
                    event.accept = true;
                    break;
                case ftk::Key::Equals:
                    event.accept = true;
                    break;
                case ftk::Key::Backspace:
                    event.accept = true;
                    break;
                default: break;
                }
            }
            if (!event.accept)
            {
                IMouseWidget::keyPressEvent(event);
            }
        }

        void TimelineWidget::keyReleaseEvent(ftk::KeyEvent& event)
        {
            IMouseWidget::keyReleaseEvent(event);
            event.accept = true;
        }

        int TimelineWidget::_timeToPos(const core::Time& time) const
        {
            FTK_P();
            const Time& start = p.player->getTimeline()->getStartTime();
            const Duration& duration = p.player->getTimeline()->getDuration();
            const ftk::Box2I& g = ftk::margin(getGeometry(), -p.size.margin);
            const float v = duration.frames ?
                ((time.frames - start.frames) / static_cast<float>(duration.frames)) :
                0.F;
            return g.min.x + v * (g.w() - 1);
        }

        core::Time TimelineWidget::_posToTime(int pos) const
        {
            FTK_P();
            const Time& start = p.player->getTimeline()->getStartTime();
            const Duration& duration = p.player->getTimeline()->getDuration();
            const ftk::Box2I& g = ftk::margin(getGeometry(), -p.size.margin);
            const int w = g.w();
            const float v = w > 1 ?
                ((pos - g.min.x) / static_cast<float>(w - 1)) :
                0.F;
            return core::Time{ static_cast<Frame>(start.frames + v * (duration.frames - 1)) };
        }
    }
}

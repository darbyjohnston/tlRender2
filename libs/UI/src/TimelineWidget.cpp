// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/UI/TimelineWidget.h>

#include <ftk/UI/IntSlider.h>
#include <ftk/UI/RowLayout.h>

namespace tl
{
    using namespace core;

    namespace ui
    {
        struct TimelineWidget::Private
        {
            std::shared_ptr<TimeUnitsModel> timeUnitsModel;
            std::shared_ptr<timeline::Player> player;

            std::shared_ptr<ftk::IntSlider> slider;
            std::shared_ptr<ftk::HorizontalLayout> layout;

            std::shared_ptr<ftk::Observer<Time>> startTimeObserver;
            std::shared_ptr<ftk::Observer<Duration>> durationObserver;
            std::shared_ptr<ftk::Observer<Time>> timeObserver;
        };

        void TimelineWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<TimeUnitsModel>& timeUnitsModel,
            const std::shared_ptr<timeline::Player>& player,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IMouseWidget::_init(context, "tl::ui::TimelineWidget", parent);
            FTK_P();

            p.timeUnitsModel = timeUnitsModel;
            p.player = player;

            p.slider = ftk::IntSlider::create(context);

            p.layout = ftk::HorizontalLayout::create(context, shared_from_this());
            p.layout->setMarginRole(ftk::SizeRole::MarginSmall);
            p.slider->setParent(p.layout);

            p.slider->setCallback(
                [this](int value)
                {
                    _p->player->setTime(Time{ value });
                });
    
            auto timeline = player->getTimeline();
            p.startTimeObserver = ftk::Observer<Time>::create(
                timeline->observeStartTime(),
                [this](const Time& value)
                {
                    FTK_P();
                    auto timeline = p.player->getTimeline();
                    p.slider->setRange(value.frames, timeline->getDuration().frames);
                });

            p.durationObserver = ftk::Observer<Duration>::create(
                timeline->observeDuration(),
                [this](const Duration& value)
                {
                    FTK_P();
                    auto timeline = p.player->getTimeline();
                    p.slider->setRange(timeline->getStartTime().frames, value.frames);
                });

            p.timeObserver = ftk::Observer<Time>::create(
                player->observeTime(),
                [this](const Time& value)
                {
                    _p->slider->setValue(value.frames);
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
            return _p->layout->getSizeHint();
        }

        void TimelineWidget::setGeometry(const ftk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }
    }
}

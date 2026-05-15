// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/PlayerApp/BottomToolBar.h>

#include <tl/PlayerApp/App.h>
#include <tl/UI/DurationLabel.h>
#include <tl/UI/FrameButtons.h>
#include <tl/UI/PlaybackButtons.h>
#include <tl/UI/TimeEdit.h>
#include <tl/UI/TimeUnitsWidget.h>

#include <ftk/UI/ComboBox.h>
#include <ftk/UI/RowLayout.h>

namespace tl
{
    using namespace core;

    namespace player_app
    {
        struct BottomToolBar::Private
        {
            std::shared_ptr<render::Session> session;

            std::shared_ptr<ui::PlaybackButtons> playbackButtons;
            std::shared_ptr<ui::FrameButtons> frameButtons;
            std::shared_ptr<ui::TimeEdit> timeEdit;
            std::shared_ptr<ui::DurationLabel> durationLabel;
            std::shared_ptr<ftk::ComboBox> rateComboBox;
            std::shared_ptr<ui::TimeUnitsWidget> timeUnitsWidget;
            std::shared_ptr<ftk::HorizontalLayout> layout;

            std::shared_ptr<ftk::Observer<MediaRate>> rateObserver;
            std::shared_ptr<ftk::Observer<Duration>> durationObserver;
            std::shared_ptr<ftk::Observer<Time>> timeObserver;
        };

        void BottomToolBar::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<render::Session>& session)
        {
            ftk::IWidget::_init(context, "tl::player_app::BottomToolBar", nullptr);
            FTK_P();

            p.session = session;

            p.playbackButtons = ui::PlaybackButtons::create(context);

            p.frameButtons = ui::FrameButtons::create(context);

            auto timeUnitsModel = app->getTimeUnitsModel();
            p.timeEdit = ui::TimeEdit::create(context, timeUnitsModel);
            p.timeEdit->setTooltip("Current time");

            p.durationLabel = ui::DurationLabel::create(context, timeUnitsModel);
            p.durationLabel->setTooltip("Timeline duration");

            p.rateComboBox = ftk::ComboBox::create(context, getCommonRateLabels());
            p.rateComboBox->setTooltip("Playback rate");

            p.timeUnitsWidget = ui::TimeUnitsWidget::create(context, timeUnitsModel);
            p.timeUnitsWidget->setTooltip("Time units");

            p.layout = ftk::HorizontalLayout::create(context, shared_from_this());
            p.playbackButtons->setParent(p.layout);
            p.frameButtons->setParent(p.layout);
            p.timeEdit->setParent(p.layout);
            p.durationLabel->setParent(p.layout);
            p.rateComboBox->setParent(p.layout);
            p.timeUnitsWidget->setParent(p.layout);

            p.rateComboBox->setIndexCallback(
                [this](int value)
                {
                    FTK_P();
                    p.session->getTimeline()->setRate(
                        getCommonRate(static_cast<CommonRate>(value)));
                });

            auto player = session->getPlayer();
            p.rateObserver = ftk::Observer<MediaRate>::create(
                player->observeRate(),
                [this](const MediaRate& value)
                {
                    FTK_P();
                    int index = -1;
                    const auto enums = getCommonRateEnums();
                    for (size_t i = 0; i < enums.size(); ++i)
                    {
                        if (value == getCommonRate(enums[i]))
                        {
                            index = static_cast<int>(i);
                            break;
                        }
                    }
                    p.rateComboBox->setCurrentIndex(index);
                    p.timeEdit->setRate(value);
                    p.durationLabel->setRate(value);
                });

            p.durationObserver = ftk::Observer<Duration>::create(
                player->getTimeline()->observeDuration(),
                [this](const Duration& value)
                {
                    FTK_P();
                    p.durationLabel->setValue(value);
                });

            p.timeObserver = ftk::Observer<Time>::create(
                player->observeTime(),
                [this](const Time& value)
                {
                    FTK_P();
                    p.timeEdit->setValue(value);
                });
        }

        BottomToolBar::BottomToolBar() :
            _p(new Private)
        {}

        BottomToolBar::~BottomToolBar()
        {}

        std::shared_ptr<BottomToolBar> BottomToolBar::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<render::Session>& session)
        {
            auto out = std::shared_ptr<BottomToolBar>(new BottomToolBar);
            out->_init(context, app, session);
            return out;
        }

        ftk::Size2I BottomToolBar::getSizeHint() const
        {
            return _p->layout->getSizeHint();
        }

        void BottomToolBar::setGeometry(const ftk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }
    }
}

// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/UI/PlaybackButtons.h>

#include <ftk/UI/ButtonGroup.h>
#include <ftk/UI/ToolButton.h>
#include <ftk/UI/RowLayout.h>

namespace tl
{
    using namespace core;

    namespace ui
    {
        struct PlaybackButtons::Private
        {
            timeline::Playback playback = timeline::Playback::Stop;
            std::function<void(timeline::Playback)> callback;

            std::vector<timeline::Playback> playbackActions;
            std::shared_ptr<ftk::ButtonGroup> buttonGroup;
            std::map<timeline::Playback, std::shared_ptr<ftk::ToolButton>> buttons;
            std::shared_ptr<ftk::HorizontalLayout> layout;
        };

        void PlaybackButtons::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IWidget::_init(context, "tl::ui::PlaybackButtons", parent);
            FTK_P();

            p.playbackActions.push_back(timeline::Playback::Reverse);
            p.playbackActions.push_back(timeline::Playback::Stop);
            p.playbackActions.push_back(timeline::Playback::Forward);

            const std::map<timeline::Playback, std::string> icons =
            {
                { timeline::Playback::Stop, "PlaybackStop" },
                { timeline::Playback::Forward, "PlaybackForward" },
                { timeline::Playback::Reverse, "PlaybackReverse" }
            };
            const std::map<timeline::Playback, std::string> tooltips =
            {
                { timeline::Playback::Stop, "Stop playback" },
                { timeline::Playback::Forward, "Start forward playback" },
                { timeline::Playback::Reverse, "Start reverse playback" }
            };

            p.buttonGroup = ftk::ButtonGroup::create(context, ftk::ButtonGroupType::Radio);
            p.layout = ftk::HorizontalLayout::create(context, shared_from_this());
            p.layout->setSpacingRole(ftk::SizeRole::SpacingTool);
            for (auto action : p.playbackActions)
            {
                auto button = ftk::ToolButton::create(context, p.layout);
                button->setIcon(icons.find(action)->second);
                button->setTooltip(tooltips.find(action)->second);
                p.buttonGroup->addButton(button);
                p.buttons[action] = button;
            }

            p.buttonGroup->setCheckedCallback(
                [this](int index, bool value)
                {
                    FTK_P();
                    if (value &&
                        p.callback &&
                        index >= 0 &&
                        index < p.playbackActions.size())
                    {
                        p.callback(p.playbackActions[index]);
                    }
                });
        }

        PlaybackButtons::PlaybackButtons() :
            _p(new Private)
        {}

        PlaybackButtons::~PlaybackButtons()
        {}

        std::shared_ptr<PlaybackButtons> PlaybackButtons::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<PlaybackButtons>(new PlaybackButtons);
            out->_init(context, parent);
            return out;
        }

        void PlaybackButtons::setCallback(const std::function<void(timeline::Playback)>& value)
        {
            _p->callback = value;
        }

        ftk::Size2I PlaybackButtons::getSizeHint() const
        {
            return _p->layout->getSizeHint();
        }

        void PlaybackButtons::setGeometry(const ftk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }
    }
}

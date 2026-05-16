// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/UI/FrameButtons.h>

#include <ftk/UI/ButtonGroup.h>
#include <ftk/UI/ToolButton.h>
#include <ftk/UI/RowLayout.h>

namespace tl
{
    using namespace core;

    namespace ui
    {
        struct FrameButtons::Private
        {
            std::function<void(timeline::FrameAction)> callback;

            std::vector<timeline::FrameAction> frameActions;
            std::shared_ptr<ftk::ButtonGroup> buttonGroup;
            std::map<timeline::FrameAction, std::shared_ptr<ftk::ToolButton>> buttons;
            std::shared_ptr<ftk::HorizontalLayout> layout;
        };

        void FrameButtons::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IWidget::_init(context, "tl::ui::FrameButtons", parent);
            FTK_P();

            p.frameActions.push_back(timeline::FrameAction::Start);
            p.frameActions.push_back(timeline::FrameAction::Prev);
            p.frameActions.push_back(timeline::FrameAction::Next);
            p.frameActions.push_back(timeline::FrameAction::End);
            const std::map<timeline::FrameAction, std::string> icons =
            {
                { timeline::FrameAction::Next, "FrameNext" },
                { timeline::FrameAction::Prev, "FramePrev" },
                { timeline::FrameAction::Start, "FrameStart" },
                { timeline::FrameAction::End, "FrameEnd" }
            };
            const std::map<timeline::FrameAction, std::string> tooltips =
            {
                { timeline::FrameAction::Next, "Go to the next frame" },
                { timeline::FrameAction::Prev, "Go to the previous frame" },
                { timeline::FrameAction::Start, "Go to the start frame" },
                { timeline::FrameAction::End, "Go to the end frame" }
            };

            p.buttonGroup = ftk::ButtonGroup::create(context, ftk::ButtonGroupType::Click);
            p.layout = ftk::HorizontalLayout::create(context, shared_from_this());
            p.layout->setSpacingRole(ftk::SizeRole::SpacingTool);
            for (auto action : p.frameActions)
            {
                auto button = ftk::ToolButton::create(context, p.layout);
                button->setIcon(icons.find(action)->second);
                button->setTooltip(tooltips.find(action)->second);
                p.buttonGroup->addButton(button);
                p.buttons[action] = button;
            }

            p.buttonGroup->setClickedCallback(
                [this](int index)
                {
                    FTK_P();
                    if (p.callback &&
                        index >= 0 &&
                        index < p.frameActions.size())
                    {
                        p.callback(p.frameActions[index]);
                    }
                });
        }

        FrameButtons::FrameButtons() :
            _p(new Private)
        {}

        FrameButtons::~FrameButtons()
        {}

        std::shared_ptr<FrameButtons> FrameButtons::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<FrameButtons>(new FrameButtons);
            out->_init(context, parent);
            return out;
        }

        void FrameButtons::setCallback(const std::function<void(timeline::FrameAction)>& value)
        {
            _p->callback = value;
        }

        ftk::Size2I FrameButtons::getSizeHint() const
        {
            return _p->layout->getSizeHint();
        }

        void FrameButtons::setGeometry(const ftk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }
    }
}

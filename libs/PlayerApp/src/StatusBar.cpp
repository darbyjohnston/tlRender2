// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/PlayerApp/StatusBar.h>

#include <tl/PlayerApp/App.h>

#include <ftk/UI/Label.h>
#include <ftk/UI/RowLayout.h>
#include <ftk/Core/Timer.h>

namespace tl
{
    using namespace core;

    namespace player_app
    {
        struct StatusBar::Private
        {
            std::shared_ptr<ftk::Label> statusLabel;
            std::shared_ptr<ftk::HorizontalLayout> layout;

            std::shared_ptr<ftk::Timer> messagesTimer;

            std::shared_ptr<ftk::ListObserver<ftk::LogItem>> logObserver;
        };

        void StatusBar::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<App>& app)
        {
            ftk::IWidget::_init(context, "tl::player_app::StatusBar", nullptr);
            FTK_P();

            p.statusLabel = ftk::Label::create(context);
            p.statusLabel->setMarginRole(ftk::SizeRole::MarginInside);

            p.layout = ftk::HorizontalLayout::create(context, shared_from_this());
            p.layout->setSpacingRole(ftk::SizeRole::None);
            p.statusLabel->setParent(p.layout);

            p.messagesTimer = ftk::Timer::create(context);

            p.logObserver = ftk::ListObserver<ftk::LogItem>::create(
                context->getSystem<ftk::LogSystem>()->observeLogItems(),
                [this](const std::vector<ftk::LogItem>& value)
                {
                    FTK_P();
                    std::string text;
                    for (const auto& i : value)
                    {
                        if (ftk::LogType::Error == i.type)
                        {
                            text = i.message;
                        }
                    }
                    p.statusLabel->setText(text);
                    p.statusLabel->setTooltip(text);
                    if (!text.empty())
                    {
                        p.messagesTimer->start(
                            std::chrono::seconds(5),
                            [this]
                            {
                                FTK_P();
                                p.statusLabel->setText(std::string());
                                p.statusLabel->setTooltip(std::string());
                            });
                    }
                });
        }

        StatusBar::StatusBar() :
            _p(new Private)
        {}

        StatusBar::~StatusBar()
        {}

        std::shared_ptr<StatusBar> StatusBar::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<App>& app)
        {
            auto out = std::shared_ptr<StatusBar>(new StatusBar);
            out->_init(context, app);
            return out;
        }

        ftk::Size2I StatusBar::getSizeHint() const
        {
            return _p->layout->getSizeHint();
        }

        void StatusBar::setGeometry(const ftk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }
    }
}

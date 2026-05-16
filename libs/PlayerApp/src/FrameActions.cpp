// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/PlayerApp/FrameActions.h>

#include <tl/PlayerApp/App.h>

namespace tl
{
    namespace player_app
    {
        struct FrameActions::Private
        {
            std::map<std::string, std::shared_ptr<ftk::Action> > actions;

            std::shared_ptr<ftk::Observer<std::shared_ptr<render::Session>>> sessionObserver;
        };

        void FrameActions::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<App>& app)
        {
            FTK_P();
            std::weak_ptr<App> appWeak(std::dynamic_pointer_cast<App>(app));
            p.actions["Start"] = ftk::Action::create(
                "Start",
                "FrameStart",
                ftk::Key::Up,
                [appWeak]
                {
                    if (auto session = appWeak.lock()->getSession())
                    {
                        session->getPlayer()->frameAction(timeline::FrameAction::Start);
                    }
                });
            p.actions["Start"]->setTooltip("Goto the start frame");

            p.actions["End"] = ftk::Action::create(
                "End",
                "FrameEnd",
                ftk::Key::Down,
                [appWeak]
                {
                    if (auto session = appWeak.lock()->getSession())
                    {
                        session->getPlayer()->frameAction(timeline::FrameAction::End);
                    }
                });
            p.actions["End"]->setTooltip("Goto the end frame");

            p.actions["Prev"] = ftk::Action::create(
                "Prev",
                "FramePrev",
                ftk::Key::Left,
                [appWeak]
                {
                    if (auto session = appWeak.lock()->getSession())
                    {
                        session->getPlayer()->frameAction(timeline::FrameAction::Prev);
                    }
                });
            p.actions["Prev"]->setTooltip("Goto the previous frame");

            p.actions["Next"] = ftk::Action::create(
                "Next",
                "FrameNext",
                ftk::Key::Right,
                [appWeak]
                {
                    if (auto session = appWeak.lock()->getSession())
                    {
                        session->getPlayer()->frameAction(timeline::FrameAction::Next);
                    }
                });
            p.actions["Next"]->setTooltip("Goto the next frame");

            p.sessionObserver = ftk::Observer<std::shared_ptr<render::Session> >::create(
                app->observeSession(),
                [this](const std::shared_ptr<render::Session>& session)
                {
                    FTK_P();
                    p.actions["Start"]->setEnabled(session.get());
                    p.actions["End"]->setEnabled(session.get());
                    p.actions["Prev"]->setEnabled(session.get());
                    p.actions["Next"]->setEnabled(session.get());
                });
        }

        FrameActions::FrameActions() :
            _p(new Private)
        {}

        FrameActions::~FrameActions()
        {}

        std::shared_ptr<FrameActions> FrameActions::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<App>& app)
        {
            auto out = std::shared_ptr<FrameActions>(new FrameActions);
            out->_init(context, app);
            return out;
        }

        const std::map<std::string, std::shared_ptr<ftk::Action> >& FrameActions::getActions() const
        {
            return _p->actions;
        }
    }
}

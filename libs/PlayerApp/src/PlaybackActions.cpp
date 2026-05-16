// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/PlayerApp/PlaybackActions.h>

#include <tl/PlayerApp/App.h>

namespace tl
{
    namespace player_app
    {
        struct PlaybackActions::Private
        {
            std::map<std::string, std::shared_ptr<ftk::Action> > actions;

            std::shared_ptr<ftk::Observer<std::shared_ptr<render::Session>>> sessionObserver;
            std::shared_ptr<ftk::Observer<timeline::Playback>> playbackObserver;
        };

        void PlaybackActions::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<App>& app)
        {
            FTK_P();
            std::weak_ptr<App> appWeak(std::dynamic_pointer_cast<App>(app));
            p.actions["Stop"] = ftk::Action::create(
                "Stop",
                "PlaybackStop",
                ftk::Key::K,
                [appWeak]
                {
                    if (auto session = appWeak.lock()->getSession())
                    {
                        session->getPlayer()->setPlayback(timeline::Playback::Stop);
                    }
                });
            p.actions["Stop"]->setTooltip("Stop playback");

            p.actions["Forward"] = ftk::Action::create(
                "Forward",
                "PlaybackForward",
                ftk::Key::L,
                [appWeak]
                {
                    if (auto session = appWeak.lock()->getSession())
                    {
                        session->getPlayer()->setPlayback(timeline::Playback::Forward);
                    }
                });
            p.actions["Forward"]->setTooltip("Start forward playback");

            p.actions["Reverse"] = ftk::Action::create(
                "Reverse",
                "PlaybackReverse",
                ftk::Key::J,
                [appWeak]
                {
                    if (auto session = appWeak.lock()->getSession())
                    {
                        session->getPlayer()->setPlayback(timeline::Playback::Reverse);
                    }
                });
            p.actions["Reverse"]->setTooltip("Start reverse playback");

            p.actions["TogglePlayback"] = ftk::Action::create(
                "Toggle Playback",
                ftk::Key::Space,
                [appWeak]
                {
                    if (auto session = appWeak.lock()->getSession())
                    {
                        session->getPlayer()->togglePlayback();
                    }
                });

            /*p.actions["SetInPoint"] = ftk::Action::create(
                "Set In Point",
                ftk::Key::I,
                [appWeak]
                {
                    if (auto session = appWeak.lock()->getSession())
                    {
                        session->getPlayer()->setInPoint();
                    }
                });
            p.actions["SetInPoint"]->setTooltip("Set the playback in point");

            p.actions["ResetInPoint"] = ftk::Action::create(
                "Reset In Point",
                ftk::KeyShortcut(ftk::Key::I, static_cast<int>(ftk::KeyModifier::Shift)),
                [appWeak]
                {
                    if (auto session = appWeak.lock()->getSession())
                    {
                        session->getPlayer()->resetInPoint();
                    }
                });
            p.actions["ResetInPoint"]->setTooltip("Reset the playback in point");

            p.actions["SetOutPoint"] = ftk::Action::create(
                "Set Out Point",
                ftk::Key::O,
                [appWeak]
                {
                    if (auto session = appWeak.lock()->getSession())
                    {
                        session->getPlayer()->setOutPoint();
                    }
                });
            p.actions["SetOutPoint"]->setTooltip("Set the playback out point");

            p.actions["ResetOutPoint"] = ftk::Action::create(
                "Reset Out Point",
                ftk::KeyShortcut(ftk::Key::O, static_cast<int>(ftk::KeyModifier::Shift)),
                [appWeak]
                {
                    if (auto session = appWeak.lock()->getSession())
                    {
                        session->getPlayer()->resetOutPoint();
                    }
                });
            p.actions["ResetOutPoint"]->setTooltip("Reset the playback out point");*/

            p.sessionObserver = ftk::Observer<std::shared_ptr<render::Session> >::create(
                app->observeSession(),
                [this](const std::shared_ptr<render::Session>& session)
                {
                    FTK_P();
                    if (session)
                    {
                        p.playbackObserver = ftk::Observer<timeline::Playback>::create(
                            session->getPlayer()->observePlayback(),
                            [this](timeline::Playback value)
                            {
                                FTK_P();
                                p.actions["Stop"]->setChecked(timeline::Playback::Stop == value);
                                p.actions["Forward"]->setChecked(timeline::Playback::Forward == value);
                                p.actions["Reverse"]->setChecked(timeline::Playback::Reverse == value);
                            });
                    }
                    else
                    {
                        p.playbackObserver.reset();

                        p.actions["Stop"]->setChecked(true);
                        p.actions["Forward"]->setChecked(false);
                        p.actions["Reverse"]->setChecked(false);
                    }

                    p.actions["Stop"]->setEnabled(session.get());
                    p.actions["Forward"]->setEnabled(session.get());
                    p.actions["Reverse"]->setEnabled(session.get());
                    p.actions["TogglePlayback"]->setEnabled(session.get());
                    /*p.actions["SetInPoint"]->setEnabled(session.get());
                    p.actions["ResetInPoint"]->setEnabled(session.get());
                    p.actions["SetOutPoint"]->setEnabled(session.get());
                    p.actions["ResetOutPoint"]->setEnabled(session.get());*/
                });
        }

        PlaybackActions::PlaybackActions() :
            _p(new Private)
        {}

        PlaybackActions::~PlaybackActions()
        {}

        std::shared_ptr<PlaybackActions> PlaybackActions::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<App>& app)
        {
            auto out = std::shared_ptr<PlaybackActions>(new PlaybackActions);
            out->_init(context, app);
            return out;
        }

        const std::map<std::string, std::shared_ptr<ftk::Action> >& PlaybackActions::getActions() const
        {
            return _p->actions;
        }
    }
}

// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/PlayerApp/WindowActions.h>

#include <tl/PlayerApp/App.h>
#include <tl/PlayerApp/MainWindow.h>

namespace tl
{
    namespace player_app
    {
        struct WindowActions::Private
        {
            std::map<std::string, std::shared_ptr<ftk::Action> > actions;

            std::shared_ptr<ftk::Observer<bool>> fullScreenObserver;
        };

        void WindowActions::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<MainWindow>& mainWindow)
        {
            FTK_P();
            auto windowWeak = std::weak_ptr<MainWindow>(mainWindow);
            p.actions["FullScreen"] = ftk::Action::create(
                "Full Screen",
                "FullScreen",
                ftk::KeyShortcut(ftk::Key::U, static_cast<int>(ftk::commandKeyModifier)),
                [windowWeak](bool value)
                {
                    if (auto window = windowWeak.lock())
                    {
                        window->setFullScreen(value);
                    }
                });
            p.actions["FullScreen"]->setTooltip("Toggle full screen mode");

            p.fullScreenObserver = ftk::Observer<bool>::create(
                mainWindow->observeFullScreen(),
                [this](bool value)
                {
                    FTK_P();
                    p.actions["FullScreen"]->setChecked(value);
                });
        }

        WindowActions::WindowActions() :
            _p(new Private)
        {}

        WindowActions::~WindowActions()
        {}

        std::shared_ptr<WindowActions> WindowActions::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<MainWindow>& mainWindow)
        {
            auto out = std::shared_ptr<WindowActions>(new WindowActions);
            out->_init(context, app, mainWindow);
            return out;
        }

        const std::map<std::string, std::shared_ptr<ftk::Action> >& WindowActions::getActions() const
        {
            return _p->actions;
        }
    }
}

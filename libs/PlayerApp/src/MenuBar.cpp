// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/PlayerApp/MenuBar.h>

#include <tl/PlayerApp/App.h>
#include <tl/PlayerApp/FileActions.h>
#include <tl/PlayerApp/FrameActions.h>
#include <tl/PlayerApp/PlaybackActions.h>
#include <tl/PlayerApp/WindowActions.h>

namespace tl
{
    using namespace core;

    namespace player_app
    {
        struct MenuBar::Private
        {};

        void MenuBar::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<FileActions>& fileActions,
            const std::shared_ptr<PlaybackActions>& playbackActions,
            const std::shared_ptr<FrameActions>& frameActions,
            const std::shared_ptr<WindowActions>& windowActions)
        {
            ftk::MenuBar::_init(context, nullptr);

            auto fileMenu = addMenu("File");
            auto actions = fileActions->getActions();
            fileMenu->addAction(actions["Open"]);
            fileMenu->addAction(actions["Close"]);
            fileMenu->addDivider();
            fileMenu->addAction(actions["Exit"]);

            auto playbackMenu = addMenu("Playback");
            actions = playbackActions->getActions();
            playbackMenu->addAction(actions["Stop"]);
            playbackMenu->addAction(actions["Forward"]);
            playbackMenu->addAction(actions["Reverse"]);
            playbackMenu->addAction(actions["TogglePlayback"]);

            auto frameMenu = addMenu("Frame");
            actions = frameActions->getActions();
            frameMenu->addAction(actions["Start"]);
            frameMenu->addAction(actions["End"]);
            frameMenu->addAction(actions["Prev"]);
            frameMenu->addAction(actions["Next"]);

            auto windowMenu = addMenu("Window");
            actions = windowActions->getActions();
            windowMenu->addAction(actions["FullScreen"]);
        }

        MenuBar::MenuBar() :
            _p(new Private)
        {}

        MenuBar::~MenuBar()
        {}

        std::shared_ptr<MenuBar> MenuBar::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<FileActions>& fileActions,
            const std::shared_ptr<PlaybackActions>& playbackActions,
            const std::shared_ptr<FrameActions>& frameActions,
            const std::shared_ptr<WindowActions>& windowActions)
        {
            auto out = std::shared_ptr<MenuBar>(new MenuBar);
            out->_init(
                context,
                app,
                fileActions,
                playbackActions,
                frameActions,
                windowActions);
            return out;
        }
    }
}

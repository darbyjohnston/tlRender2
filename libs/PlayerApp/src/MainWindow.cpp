// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/PlayerApp/MainWindow.h>

#include <tl/PlayerApp/App.h>
#include <tl/PlayerApp/DocumentWidget.h>
#include <tl/PlayerApp/FileActions.h>
#include <tl/PlayerApp/PlaybackActions.h>
#include <tl/PlayerApp/StatusBar.h>

#include <ftk/UI/Action.h>
#include <ftk/UI/Divider.h>
#include <ftk/UI/Menu.h>
#include <ftk/UI/MenuBar.h>
#include <ftk/UI/RowLayout.h>

namespace tl
{
    namespace player_app
    {
        struct MainWindow::Private
        {
            std::weak_ptr<App> app;
            std::shared_ptr<FileActions> fileActions;
            std::shared_ptr<PlaybackActions> playbackActions;
            std::shared_ptr<DocumentWidget> documentWidget;
            std::shared_ptr<StatusBar> statusBar;
            std::shared_ptr<ftk::VerticalLayout> layout;
            std::shared_ptr<ftk::VerticalLayout> widgetLayout;
        };

        void MainWindow::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<App>& app)
        {
            ftk::MainWindow::_init(context, app, ftk::Size2I(1280, 720));
            FTK_P();

            p.app = app;

            p.fileActions = FileActions::create(context, app);
            p.playbackActions = PlaybackActions::create(context, app);

            auto menuBar = getMenuBar();
            menuBar->clear();
            auto fileMenu = menuBar->addMenu("File");
            auto actions = p.fileActions->getActions();
            fileMenu->addAction(actions["Open"]);
            fileMenu->addAction(actions["Close"]);
            fileMenu->addDivider();
            fileMenu->addAction(actions["Exit"]);
            auto playbackMenu = menuBar->addMenu("Playback");
            actions = p.playbackActions->getActions();
            playbackMenu->addAction(actions["Stop"]);
            playbackMenu->addAction(actions["Forward"]);
            playbackMenu->addAction(actions["Reverse"]);
            playbackMenu->addAction(actions["TogglePlayback"]);
            playbackMenu->addDivider();
            playbackMenu->addAction(actions["Start"]);
            playbackMenu->addAction(actions["Prev"]);
            playbackMenu->addAction(actions["Next"]);
            playbackMenu->addAction(actions["End"]);

            p.statusBar = StatusBar::create(context, app);

            p.layout = ftk::VerticalLayout::create(context);
            p.layout->setSpacingRole(ftk::SizeRole::None);
            p.widgetLayout = ftk::VerticalLayout::create(context, p.layout);
            p.widgetLayout->setVStretch(ftk::Stretch::Expanding);
            p.widgetLayout->setSpacingRole(ftk::SizeRole::None);
            ftk::Divider::create(context, ftk::Orientation::Vertical, p.layout);
            p.statusBar->setParent(p.layout);
            setWidget(p.layout);
        }

        MainWindow::MainWindow() :
            _p(new Private)
        {}

        MainWindow::~MainWindow()
        {}

        std::shared_ptr<MainWindow> MainWindow::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<App>& app)
        {
            auto out = std::shared_ptr<MainWindow>(new MainWindow);
            out->_init(context, app);
            return out;
        }

        void MainWindow::setSession(const std::shared_ptr<render::Session>& session)
        {
            FTK_P();
            if (p.documentWidget)
            {
                p.documentWidget->setParent(nullptr);
                p.documentWidget.reset();
            }
            p.documentWidget = DocumentWidget::create(
                getContext(),
                p.app.lock(),
                session,
                p.widgetLayout);
        }
    }
}

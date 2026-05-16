// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/PlayerApp/MainWindow.h>

#include <tl/PlayerApp/App.h>
#include <tl/PlayerApp/BottomToolBar.h>
#include <tl/PlayerApp/DocumentWidget.h>
#include <tl/PlayerApp/FileActions.h>
#include <tl/PlayerApp/FrameActions.h>
#include <tl/PlayerApp/MenuBar.h>
#include <tl/PlayerApp/PlaybackActions.h>
#include <tl/PlayerApp/StatusBar.h>
#include <tl/PlayerApp/WindowActions.h>

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
            std::shared_ptr<FrameActions> frameActions;
            std::shared_ptr<WindowActions> windowActions;
            std::shared_ptr<DocumentWidget> documentWidget;
            std::shared_ptr<BottomToolBar> bottomToolBar;
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
            p.frameActions = FrameActions::create(context, app);
            p.windowActions = WindowActions::create(
                context,
                app,
                std::dynamic_pointer_cast<MainWindow>(shared_from_this()));

            setMenuBar(MenuBar::create(
                context,
                app,
                p.fileActions,
                p.playbackActions,
                p.frameActions,
                p.windowActions));

            p.bottomToolBar = BottomToolBar::create(
                context,
                app,
                p.playbackActions,
                p.frameActions);

            p.statusBar = StatusBar::create(context, app);

            p.layout = ftk::VerticalLayout::create(context);
            p.layout->setSpacingRole(ftk::SizeRole::None);
            p.widgetLayout = ftk::VerticalLayout::create(context, p.layout);
            p.widgetLayout->setVStretch(ftk::Stretch::Expanding);
            p.widgetLayout->setSpacingRole(ftk::SizeRole::None);
            ftk::Divider::create(context, ftk::Orientation::Vertical, p.layout);
            p.bottomToolBar->setParent(p.layout);
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
            if (session)
            {
                p.documentWidget = DocumentWidget::create(
                    getContext(),
                    p.app.lock(),
                    session,
                    p.widgetLayout);
            }
        }
    }
}

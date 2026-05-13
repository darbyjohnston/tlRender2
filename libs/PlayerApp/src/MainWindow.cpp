// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/PlayerApp/MainWindow.h>

#include <tl/PlayerApp/App.h>
#include <tl/UI/Viewport.h>
#include <tl/UI/TimelineWidget.h>

#include <ftk/UI/RowLayout.h>

namespace tl
{
    namespace player_app
    {
        struct MainWindow::Private
        {
            std::shared_ptr<ui::Viewport> viewport;
            std::shared_ptr<ui::TimelineWidget> timelineWidget;
            std::shared_ptr<ftk::VerticalLayout> layout;
        };

        void MainWindow::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<App>& app)
        {
            ftk::MainWindow::_init(context, app, ftk::Size2I(1280, 720));
            FTK_P();
            
            p.viewport = ui::Viewport::create(context);
            p.viewport->setVStretch(ftk::Stretch::Expanding);

            p.timelineWidget = ui::TimelineWidget::create(
                context,
                app->getTimeUnitsModel());

            p.layout = ftk::VerticalLayout::create(context);
            p.viewport->setParent(p.layout);
            p.timelineWidget->setParent(p.layout);
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
    }
}

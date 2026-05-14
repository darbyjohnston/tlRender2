// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/PlayerApp/MainWindow.h>

#include <tl/PlayerApp/App.h>
#include <tl/PlayerApp/DocumentWidget.h>

#include <ftk/UI/Action.h>
#include <ftk/UI/FileBrowser.h>
#include <ftk/UI/Menu.h>
#include <ftk/UI/MenuBar.h>

namespace tl
{
    namespace player_app
    {
        struct MainWindow::Private
        {
            std::weak_ptr<App> app;
        };

        void MainWindow::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<App>& app)
        {
            ftk::MainWindow::_init(context, app, ftk::Size2I(1280, 720));
            FTK_P();

            p.app = app;

            auto menuBar = getMenuBar();
            auto fileMenu = menuBar->getMenu("File");
            fileMenu->clear();
            std::weak_ptr<App> appWeak(app);
            std::weak_ptr<MainWindow> windowWeak(
                std::dynamic_pointer_cast<MainWindow>(shared_from_this()));
            fileMenu->addAction(ftk::Action::create(                
                "Open",
                "FileOpen",
                ftk::KeyShortcut(ftk::Key::O, ftk::commandKeyModifier),
                [this, appWeak, windowWeak]
                {
                    auto fileBrowserSystem = appWeak.lock()->getContext()->getSystem<ftk::FileBrowserSystem>();
                    fileBrowserSystem->open(
                        windowWeak.lock(),
                        [appWeak](const ftk::Path& value)
                        {
                            appWeak.lock()->open(std::filesystem::u8path(value.get()));
                        });
                }));
            fileMenu->addDivider();
            fileMenu->addAction(ftk::Action::create(
                "Quit",
                ftk::KeyShortcut(ftk::Key::Q, ftk::commandKeyModifier),
                [appWeak]
                {
                    appWeak.lock()->exit();
                }));
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

        void MainWindow::setPlayer(const std::shared_ptr<timeline::Player>& player)
        {
            FTK_P();
            setWidget(DocumentWidget::create(getContext(), p.app.lock(), player));
        }
    }
}

// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/PlayerApp/App.h>

#include <tl/PlayerApp/MainWindow.h>

#include <ftk/Core/Context.h>
#include <ftk/Core/Format.h>

namespace tl
{
    namespace player_app
    {
        struct App::Private
        {
            CmdLine cmdLine;
            std::shared_ptr<ui::TimeUnitsModel> timeUnitsModel;
            std::shared_ptr<timeline::Timeline> timeline;
            std::shared_ptr<timeline::Player> player;
            std::shared_ptr<MainWindow> mainWindow;
        };

        void App::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::vector<std::string>& args)
        {
            FTK_P();

            p.cmdLine.input = ftk::CmdLineArg<std::string>::create(
                "input",
                "Input timeline or media file.");

            ftk::App::_init(
                context,
                args,
                "tlplayer",
                "Playback timelines and media.",
                {
                    p.cmdLine.input
                });
            
            p.timeUnitsModel = ui::TimeUnitsModel::create(context);
        }

        App::App() :
            _p(new Private)
        {}

        App::~App()
        {}

        std::shared_ptr<App> App::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::vector<std::string>& args)
        {
            auto out = std::shared_ptr<App>(new App);
            out->_init(context, args);
            return out;
        }
        
        const std::shared_ptr<ui::TimeUnitsModel>& App::getTimeUnitsModel() const
        {
            return _p->timeUnitsModel;
        }

        void App::run()
        {
            FTK_P();

            ftk::Path path(p.cmdLine.input->getValue());
            p.timeline = timeline::Timeline::create(_context, path);
            
            p.player = timeline::Player::create(_context, p.timeline);

            p.mainWindow = MainWindow::create(
                _context,
                std::dynamic_pointer_cast<App>(shared_from_this()));

            ftk::App::run();
        }
            
        const std::shared_ptr<timeline::Timeline>& App::getTimeline() const
        {
            return _p->timeline;
        }

        const std::shared_ptr<timeline::Player>& App::getPlayer() const
        {
            return _p->player;
        }
    }
}

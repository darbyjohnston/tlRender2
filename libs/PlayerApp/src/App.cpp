// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/PlayerApp/App.h>

#include <tl/PlayerApp/MainWindow.h>
#include <tl/Render/Render.h>

#include <ftk/UI/DialogSystem.h>
#include <ftk/Core/Context.h>
#include <ftk/Core/Format.h>

namespace tl
{
    using namespace core;

    namespace player_app
    {
        struct App::Private
        {
            CmdLine cmdLine;
            std::shared_ptr<ui::TimeUnitsModel> timeUnitsModel;
            std::shared_ptr<timeline::Timeline> timeline;
            std::shared_ptr<timeline::Player> player;
            std::shared_ptr<render::VideoRenderer> renderer;
            std::shared_ptr<ftk::Observable<std::shared_ptr<ftk::Image>>> videoFrame;
            std::shared_ptr<MainWindow> mainWindow;

            std::shared_ptr<ftk::Observer<Time>> timeObserver;
        };

        void App::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::vector<std::string>& args)
        {
            FTK_P();

            p.cmdLine.input = ftk::CmdLineArg<std::string>::create(
                "input",
                "Input timeline or media file.",
                true);

            ftk::App::_init(
                context,
                args,
                "tlplayer",
                "Playback timelines and media.",
                {
                    p.cmdLine.input
                });
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
            
            p.timeUnitsModel = ui::TimeUnitsModel::create(_context);

            p.renderer = render::VideoRenderer::create(_context);
            p.videoFrame = ftk::Observable<std::shared_ptr<ftk::Image>>::create();

            p.mainWindow = MainWindow::create(
                _context,
                std::dynamic_pointer_cast<App>(shared_from_this()));
            
            ftk::App::run();
        }

        void App::open(const ftk::Path& value)
        {
            FTK_P();
            try
            {
                auto timeline = timeline::Timeline::create(_context, value);            
                auto player = timeline::Player::create(_context, timeline);

                p.timeline = timeline;
                p.player = player;
                p.mainWindow->setPlayer(player);

                p.timeObserver = ftk::Observer<Time>::create(
                    player->observeTime(),
                    [this](const Time& value)
                    {
                        _render(value);
                    });
            }
            catch (const std::exception& e)
            {
                _context->getSystem<ftk::DialogSystem>()->message(
                    "ERROR",
                    e.what(),
                    p.mainWindow);
            }
        }

        const std::shared_ptr<timeline::Timeline>& App::getTimeline() const
        {
            return _p->timeline;
        }

        const std::shared_ptr<timeline::Player>& App::getPlayer() const
        {
            return _p->player;
        }

        std::shared_ptr<ftk::IObservable<std::shared_ptr<ftk::Image>>> App::observeVideoFrame() const
        {
            return _p->videoFrame;
        }

        void App::_render(const Time& time)
        {
            FTK_P();
            if (auto graph = p.timeline->getVideo(time))
            {
                p.videoFrame->setIfChanged(p.renderer->render(*graph));
            }
        }
    }
}

// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/PlayerApp/App.h>

#include <tl/PlayerApp/MainWindow.h>
#include <tl/Render/Session.h>

#include <ftk/UI/DialogSystem.h>
#include <ftk/UI/FileBrowser.h>
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
            std::shared_ptr<ftk::Observable<std::shared_ptr<render::Session>>> session;
            std::shared_ptr<ftk::Observable<std::shared_ptr<ftk::Image>>> videoFrame;
            std::future<std::shared_ptr<ftk::Image> > videoFrameRequest;
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

        void App::open()
        {
            FTK_P();
            auto fileBrowserSystem = _context->getSystem<ftk::FileBrowserSystem>();
            std::weak_ptr<App> appWeak(std::dynamic_pointer_cast<App>(shared_from_this()));
            fileBrowserSystem->open(
                p.mainWindow,
                [appWeak](const ftk::Path& value)
                {
                    appWeak.lock()->open(value);
                });
        }

        void App::open(const ftk::Path& path)
        {
            FTK_P();
            try
            {
                auto session = render::Session::create(_context, path);

                p.mainWindow->setSession(session);

                p.timeObserver = ftk::Observer<Time>::create(
                    session->getPlayer()->observeTime(),
                    [this](const Time& value)
                    {
                        FTK_P();
                        if (auto session = p.session->get())
                        {
                            p.videoFrameRequest = session->render(value);
                        }
                    });
                
                p.session->setIfChanged(session);
            }
            catch (const std::exception& e)
            {
                _context->getSystem<ftk::DialogSystem>()->message(
                    "ERROR",
                    e.what(),
                    p.mainWindow);
            }
        }

        void App::close()
        {
            FTK_P();
            p.session->setIfChanged(nullptr);
            p.mainWindow->setSession(nullptr);
            p.timeObserver.reset();
        }

        const std::shared_ptr<render::Session>& App::getSession() const
        {
            return _p->session->get();
        }

        std::shared_ptr<ftk::IObservable<std::shared_ptr<render::Session>>> App::observeSession() const
        {
            return _p->session;
        }

        std::shared_ptr<ftk::IObservable<std::shared_ptr<ftk::Image>>> App::observeVideoFrame() const
        {
            return _p->videoFrame;
        }

        void App::run()
        {
            FTK_P();
            
            p.timeUnitsModel = ui::TimeUnitsModel::create(_context);

            p.session = ftk::Observable<std::shared_ptr<render::Session>>::create();

            p.videoFrame = ftk::Observable<std::shared_ptr<ftk::Image>>::create();

            p.mainWindow = MainWindow::create(
                _context,
                std::dynamic_pointer_cast<App>(shared_from_this()));
            
            ftk::App::run();
        }

        void App::tick()
        {
            ftk::App::tick();
            FTK_P();
            if (p.videoFrameRequest.valid() &&
                p.videoFrameRequest.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
            {
                p.videoFrame->setIfChanged(p.videoFrameRequest.get());
            }
        }
    }
}

// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/Render/Session.h>

#include "Inbox.h"

#include <tl/Render/Render.h>
#include <tl/Timeline/Player.h>
#include <tl/Timeline/Timeline.h>

#include <thread>

namespace tl
{
    using namespace core;

    namespace render
    {
        struct Session::Private
        {
            std::shared_ptr<ftk::LogSystem> logSystem;
            std::shared_ptr<timeline::Timeline> timeline;
            std::shared_ptr<timeline::Player> player;
            std::shared_ptr<render::VideoRenderer> renderer;
            std::unique_ptr<IInbox> inbox;
            std::thread worker;

            void workerLoop();
        };

        void Session::_init(
            const std::shared_ptr<ftk::Context>& context,
            const ftk::Path& path,
            RequestPolicy policy)
        {
            FTK_P();
            p.logSystem = context->getSystem<ftk::LogSystem>();
            p.timeline = timeline::Timeline::create(context, path);            
            p.player = timeline::Player::create(context, p.timeline);
            p.renderer = render::VideoRenderer::create(context);

            switch (policy)
            {
            case RequestPolicy::LatestWins:
                p.inbox = createLatestWinsInbox();
                break;
            case RequestPolicy::All:
                p.inbox = createAllInbox();
                break;
            }

            p.worker = std::thread([this] { _p->workerLoop(); });
        }

        Session::Session() :
            _p(new Private)
        {}

        Session::~Session()
        {
            FTK_P();
            if (p.inbox)
            {
                p.inbox->shutdown();
            }
            if (p.worker.joinable())
            {
                p.worker.join();
            }
        }

        std::shared_ptr<Session> Session::create(
            const std::shared_ptr<ftk::Context>& context,
            const ftk::Path& path,
            RequestPolicy policy)
        {
            auto out = std::shared_ptr<Session>(new Session);
            out->_init(context, path, policy);
            return out;
        }

        const std::shared_ptr<timeline::Timeline>& Session::getTimeline() const
        {
            return _p->timeline;
        }

        const std::shared_ptr<timeline::Player>& Session::getPlayer() const
        {
            return _p->player;
        }

        std::future<std::shared_ptr<ftk::Image>> Session::render(const Time& time)
        {
            FTK_P();
            Request req;
            req.time = time;
            req.graph = p.timeline->getVideo(time);
            auto future = req.promise.get_future();
            p.inbox->submit(std::move(req));
            return future;
        }

        std::shared_ptr<timeline::VideoGraph> Session::getGraph(const core::Time& time)
        {
            return _p->timeline->getVideo(time);            
        }

        void Session::Private::workerLoop()
        {
            while (auto req = inbox->wait_and_pop())
            {
                std::shared_ptr<ftk::Image> result;
                try
                {
                    if (req->graph)
                    {
                        result = renderer->render(*req->graph);
                    }
                }
                catch (const std::exception& e)
                {
                    logSystem->print(
                        "tl::render::Session",
                        std::string("Render error: ") + e.what(),
                        ftk::LogType::Error);
                }
                req->promise.set_value(std::move(result));
            }
        }
    }
}

// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/Render/Session.h>

#include <tl/Render/Render.h>
#include <tl/Timeline/Player.h>
#include <tl/Timeline/Timeline.h>

namespace tl
{
    using namespace core;

    namespace render
    {
        struct Session::Private
        {
            std::shared_ptr<timeline::Timeline> timeline;
            std::shared_ptr<timeline::Player> player;
            std::shared_ptr<render::VideoRenderer> renderer;
        };

        void Session::_init(
            const std::shared_ptr<ftk::Context>& context,
            const ftk::Path& path)
        {
            FTK_P();
            p.timeline = timeline::Timeline::create(context, path);            
            p.player = timeline::Player::create(context, p.timeline);
            p.renderer = render::VideoRenderer::create(context);
        }

        Session::Session() :
            _p(new Private)
        {}

        Session::~Session()
        {}

        std::shared_ptr<Session> Session::create(
            const std::shared_ptr<ftk::Context>& context,
            const ftk::Path& path)
        {
            auto out = std::shared_ptr<Session>(new Session);
            out->_init(context, path);
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
            std::promise<std::shared_ptr<ftk::Image>> out;
            std::shared_ptr<ftk::Image> videoFrame;
            auto graph = p.timeline->getVideo(time);
            out.set_value(p.renderer->render(*graph));
            return out.get_future();
        }

        std::shared_ptr<timeline::VideoGraph> Session::getGraph(const core::Time& time)
        {
            return _p->timeline->getVideo(time);            
        }
    }
}

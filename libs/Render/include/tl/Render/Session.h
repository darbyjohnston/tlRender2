// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <tl/Timeline/Player.h>
#include <tl/Timeline/VideoGraph.h>

#include <ftk/Core/Context.h>
#include <ftk/Core/Image.h>

#include <future>

namespace tl
{
    namespace render
    {
        //! Policy for how the Session handles render requests.
        enum class TL_API_TYPE RequestPolicy
        {
            //! Single-slot inbox: a new request replaces any pending one.
            //! Superseded requests resolve with nullptr. For interactive
            //! playback where only the most recent frame matters.
            LatestWins,

            //! FIFO inbox: every request is rendered in submission order.
            //! For batch rendering where every frame's output is consumed.
            All,
        };

        //! Render session.
        class TL_API_TYPE Session
        {
            FTK_NON_COPYABLE(Session);

        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const ftk::Path&,
                RequestPolicy);

            Session();

        public:
            ~Session();

            //! Create a new render session.
            static std::shared_ptr<Session> create(
                const std::shared_ptr<ftk::Context>&,
                const ftk::Path&,
                RequestPolicy = RequestPolicy::LatestWins);

            //! Get the timeline.
            const std::shared_ptr<timeline::Timeline>& getTimeline() const;

            //! Get the player.
            const std::shared_ptr<timeline::Player>& getPlayer() const;

            //! Render a frame.
            std::future<std::shared_ptr<ftk::Image>> render(const core::Time&);

            //! Get the render graph for a given frame.
            std::shared_ptr<timeline::VideoGraph> getGraph(const core::Time&);

        private:
            FTK_PRIVATE();
        };
    }
}

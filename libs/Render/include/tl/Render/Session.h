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
        //! Render session.
        class TL_API_TYPE Session
        {
            FTK_NON_COPYABLE(Session);

        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const ftk::Path&);

            Session();

        public:
            ~Session();

            //! Create a new render session.
            static std::shared_ptr<Session> create(
                const std::shared_ptr<ftk::Context>&,
                const ftk::Path&);

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

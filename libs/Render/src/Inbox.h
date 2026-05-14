// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <tl/Timeline/VideoGraph.h>

#include <ftk/Core/Image.h>

#include <future>
#include <memory>
#include <optional>

namespace tl
{
    namespace render
    {
        //! A render request submitted to a Session's worker thread.
        struct Request
        {
            core::Time time;
            std::shared_ptr<timeline::VideoGraph> graph;
            std::promise<std::shared_ptr<ftk::Image>> promise;
        };

        //! Interface for the worker's request queue.
        //!
        //! Two implementations:
        //!  - LatestWinsInbox: single-slot, superseded requests resolve to
        //!    nullptr. Intended for interactive playback / scrubbing where
        //!    only the most recent frame matters.
        //!  - AllInbox: FIFO queue, every request is rendered. Intended for
        //!    batch rendering where each frame's output is consumed.
        class IInbox
        {
        public:
            virtual ~IInbox() = default;

            //! Submit a request. Thread-safe; called from the submitter.
            virtual void submit(Request&&) = 0;

            //! Block until a request is available, then return it. Returns
            //! nullopt when shutdown has been signaled and no more requests
            //! will be delivered. Thread-safe; called from the worker.
            virtual std::optional<Request> wait_and_pop() = 0;

            //! Signal shutdown. Any pending request's promise is resolved
            //! with nullptr. Subsequent wait_and_pop calls return nullopt
            //! (after draining for AllInbox).
            virtual void shutdown() = 0;
        };

        std::unique_ptr<IInbox> createLatestWinsInbox();
        std::unique_ptr<IInbox> createAllInbox();
    }
}


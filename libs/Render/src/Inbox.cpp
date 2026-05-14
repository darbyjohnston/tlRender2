// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include "Inbox.h"

#include <condition_variable>
#include <mutex>
#include <queue>

namespace tl
{
    namespace render
    {
        namespace
        {
            //! Single-slot inbox; new submissions replace pending ones.
            class LatestWinsInbox : public IInbox
            {
            public:
                void submit(Request&& request) override
                {
                    std::unique_lock<std::mutex> lock(_mutex);
                    if (_shutdown)
                    {
                        request.promise.set_value(nullptr);
                        return;
                    }
                    if (_pending)
                    {
                        _pending->promise.set_value(nullptr);
                    }
                    _pending = std::move(request);
                    _cv.notify_one();
                }

                std::optional<Request> wait_and_pop() override
                {
                    std::unique_lock<std::mutex> lock(_mutex);
                    _cv.wait(lock, [this] { return _pending.has_value() || _shutdown; });
                    if (_pending)
                    {
                        auto out = std::move(*_pending);
                        _pending.reset();
                        return out;
                    }
                    return std::nullopt;
                }

                void shutdown() override
                {
                    std::unique_lock<std::mutex> lock(_mutex);
                    _shutdown = true;
                    if (_pending)
                    {
                        _pending->promise.set_value(nullptr);
                        _pending.reset();
                    }
                    _cv.notify_all();
                }

            private:
                std::mutex _mutex;
                std::condition_variable _cv;
                std::optional<Request> _pending;
                bool _shutdown = false;
            };

            //! FIFO inbox; every submitted request is rendered.
            class AllInbox : public IInbox
            {
            public:
                void submit(Request&& request) override
                {
                    std::unique_lock<std::mutex> lock(_mutex);
                    if (_shutdown)
                    {
                        request.promise.set_value(nullptr);
                        return;
                    }
                    _queue.push(std::move(request));
                    _cv.notify_one();
                }

                std::optional<Request> wait_and_pop() override
                {
                    std::unique_lock<std::mutex> lock(_mutex);
                    _cv.wait(lock, [this] { return !_queue.empty() || _shutdown; });
                    if (!_queue.empty())
                    {
                        auto out = std::move(_queue.front());
                        _queue.pop();
                        return out;
                    }
                    return std::nullopt;
                }

                void shutdown() override
                {
                    std::unique_lock<std::mutex> lock(_mutex);
                    _shutdown = true;
                    _cv.notify_all();
                    // Queued requests are NOT resolved with nullptr here;
                    // the worker drains them and resolves each with its
                    // rendered value before exiting.
                }

            private:
                std::mutex _mutex;
                std::condition_variable _cv;
                std::queue<Request> _queue;
                bool _shutdown = false;
            };
        }

        std::unique_ptr<IInbox> createLatestWinsInbox()
        {
            return std::make_unique<LatestWinsInbox>();
        }

        std::unique_ptr<IInbox> createAllInbox()
        {
            return std::make_unique<AllInbox>();
        }
    }
}

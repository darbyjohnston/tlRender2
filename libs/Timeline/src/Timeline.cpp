// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/Timeline/Timeline.h>

namespace tl
{
    namespace timeline
    {
        struct Timeline::Private
        {
            std::shared_ptr<ftk::Observable<core::Time> > startTime;
            std::shared_ptr<ftk::Observable<core::Duration> > duration;
        };

        void Timeline::_init(const std::string&)
        {
            FTK_P();
            p.startTime = ftk::Observable<core::Time>::create();
            p.duration = ftk::Observable<core::Duration>::create();
        }

        Timeline::Timeline() :
            _p(new Private)
        {}

        Timeline::~Timeline()
        {}

        std::shared_ptr<Timeline> Timeline::create()
        {
            auto out = std::shared_ptr<Timeline>(new Timeline);
            out->_init(std::string());
            return out;
        }

        std::shared_ptr<Timeline> Timeline::create(const std::string& fileName)
        {
            auto out = std::shared_ptr<Timeline>(new Timeline);
            out->_init(fileName);
            return out;
        }
        
        const core::Time& Timeline::getStartTime() const
        {
            return _p->startTime->get();
        }

        std::shared_ptr<ftk::IObservable<core::Time> > Timeline::observeStartTime() const
        {
            return _p->startTime;
        }

        const core::Duration& Timeline::getDuration() const
        {
            return _p->duration->get();
        }

        std::shared_ptr<ftk::IObservable<core::Duration> > Timeline::observeDuration() const
        {
            return _p->duration;
        }

        std::shared_ptr<IVideoNode> Timeline::getVideo(const core::Time&)
        {
            return nullptr;
        }

        std::future<std::shared_ptr<core::Audio> > Timeline::getAudio(int64_t seconds)
        {
            return std::future<std::shared_ptr<core::Audio> >();
        }
    }
}

// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/Timeline/Player.h>

namespace tl
{
    using namespace core;

    namespace timeline
    {
        struct Player::Private
        {
            std::shared_ptr<Timeline> timeline;
            std::shared_ptr<ftk::Observable<Time> > startTime;
            std::shared_ptr<ftk::Observable<Duration> > duration;
            std::shared_ptr<ftk::Observable<Time> > time;

            std::shared_ptr<ftk::Observer<Time>> startTimeObserver;
            std::shared_ptr<ftk::Observer<Duration>> durationObserver;
        };

        void Player::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<Timeline>& timeline)
        {
            FTK_P();
            p.timeline = timeline;
            p.startTime = ftk::Observable<Time>::create();
            p.duration = ftk::Observable<Duration>::create();
            p.time = ftk::Observable<Time>::create();

            p.startTimeObserver = ftk::Observer<Time>::create(
                timeline->observeStartTime(),
                [this](const Time& value)
                {
                    _p->startTime->setIfChanged(value);
                });

            p.durationObserver = ftk::Observer<Duration>::create(
                timeline->observeDuration(),
                [this](const Duration& value)
                {
                    _p->duration->setIfChanged(value);
                });
        }

        Player::Player() :
            _p(new Private)
        {}

        Player::~Player()
        {}

        std::shared_ptr<Player> Player::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<Timeline>& timeline)
        {
            auto out = std::shared_ptr<Player>(new Player);
            out->_init(context, timeline);
            return out;
        }

        const core::MediaRate& Player::getRate() const
        {
            return _p->timeline->getRate();
        }

        const core::Time& Player::getStartTime() const
        {
            return _p->startTime->get();
        }

        std::shared_ptr<ftk::IObservable<core::Time> > Player::observeStartTime() const
        {
            return _p->startTime;
        }

        const core::Duration& Player::getDuration() const
        {
            return _p->duration->get();
        }

        std::shared_ptr<ftk::IObservable<core::Duration> > Player::observeDuration() const
        {
            return _p->duration;
        }
        
        const Time& Player::getTime() const
        {
            return _p->time->get();
        }

        std::shared_ptr<ftk::IObservable<Time> > Player::observeTime() const
        {
            return _p->time;
        }
        
        void Player::setTime(const Time& value)
        {
            FTK_P();
            if (p.time->setIfChanged(value))
            {
            
            }
        }
    }
}

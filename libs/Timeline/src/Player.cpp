// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/Timeline/Player.h>

namespace tl
{
    using namespace core;

    namespace timeline
    {
        TL_ENUM_IMPL(
            Playback,
            "Stop",
            "Forward",
            "Reverse");

        TL_ENUM_IMPL(
            FrameAction,
            "Next",
            "Next X10",
            "Next X100",
            "Prev X10",
            "Prev X100",
            "Prev",
            "Start",
            "End");

        struct Player::Private
        {
            std::shared_ptr<Timeline> timeline;
            std::shared_ptr<ftk::Observable<MediaRate>> rate;
            std::shared_ptr<ftk::Observable<Time>> time;
            std::shared_ptr<ftk::Observable<Playback>> playback;
        };

        void Player::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<Timeline>& timeline)
        {
            FTK_P();
            p.timeline = timeline;
            p.rate = ftk::Observable<MediaRate>::create(timeline->getRate());
            p.time = ftk::Observable<Time>::create();
            p.playback = ftk::Observable<Playback>::create();
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

        const std::shared_ptr<Timeline>& Player::getTimeline() const
        {
            return _p->timeline;
        }

        const MediaRate& Player::getRate() const
        {
            return _p->timeline->getRate();
        }

        std::shared_ptr<ftk::IObservable<MediaRate> > Player::observeRate() const
        {
            return _p->rate;
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
        
        timeline::Playback Player::getPlayback() const
        {
            return _p->playback->get();
        }

        std::shared_ptr<ftk::IObservable<timeline::Playback> > Player::observePlayback() const
        {
            return _p->playback;
        }
        
        void Player::setPlayback(timeline::Playback value)
        {
            FTK_P();
            if (p.playback->setIfChanged(value))
            {
            
            }
        }

        void Player::frameAction(timeline::FrameAction value)
        {
            FTK_P();
            switch (value)
            {
            case timeline::FrameAction::Next:
                setTime(getTime() + Duration{ 1 });
                break;
            case timeline::FrameAction::Next_X10:
                setTime(getTime() + Duration{ 10 });
                break;
            case timeline::FrameAction::Next_X100:
                setTime(getTime() + Duration{ 100 });
                break;
            case timeline::FrameAction::Prev:
                setTime(getTime() - Duration{ 1 });
                break;
            case timeline::FrameAction::Prev_X10:
                setTime(getTime() - Duration{ 10 });
                break;
            case timeline::FrameAction::Prev_X100:
                setTime(getTime() - Duration{ 100 });
                break;
            case timeline::FrameAction::Start:
                setTime(p.timeline->getStartTime());
                break;
            case timeline::FrameAction::End:
                setTime(p.timeline->getStartTime() +
                    p.timeline->getDuration() - Duration{ 1 });
                break;
            default: break;
            }
        }
    }
}

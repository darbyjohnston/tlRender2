// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/Timeline/Player.h>

namespace tl
{
    namespace timeline
    {
        struct Player::Private
        {
            std::shared_ptr<Timeline> timeline;
            std::shared_ptr<ftk::Observable<core::Time> > time;
        };

        void Player::_init(const std::shared_ptr<Timeline>& timeline)
        {
            FTK_P();
            p.timeline = timeline;
            p.time = ftk::Observable<core::Time>::create();
        }

        Player::Player() :
            _p(new Private)
        {}

        Player::~Player()
        {}

        std::shared_ptr<Player> Player::create(const std::shared_ptr<Timeline>& timeline)
        {
            auto out = std::shared_ptr<Player>(new Player);
            out->_init(timeline);
            return out;
        }
        
        const core::Time& Player::getTime() const
        {
            return _p->time->get();
        }

        std::shared_ptr<ftk::IObservable<core::Time> > Player::observeTime() const
        {
            return _p->time;
        }
    }
}

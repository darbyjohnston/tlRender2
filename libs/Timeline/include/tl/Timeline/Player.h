// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender2 project.

#pragma once

#include <tl/Timeline/Timeline.h>

namespace tl
{
    namespace timeline
    {
        //! Timeline player.
        class TL_API_TYPE Player
        {
            FTK_NON_COPYABLE(Player);

        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<Timeline>&);

            Player();

        public:
            ~Player();

            //! Create a new timeline player.
            static std::shared_ptr<Player> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<Timeline>&);
            
            //! Get the timeline rate.
            const core::MediaRate& getRate() const;

            //! Get the timeline start time.
            const core::Time& getStartTime() const;

            //! Observe the timeline start time.
            std::shared_ptr<ftk::IObservable<core::Time> > observeStartTime() const;

            //! Get the timeline duration.
            const core::Duration& getDuration() const;

            //! Observe the timeline duration.
            std::shared_ptr<ftk::IObservable<core::Duration> > observeDuration() const;
            
            //! Get the current time.
            const core::Time& getTime() const;

            //! Observe the current time.
            std::shared_ptr<ftk::IObservable<core::Time> > observeTime() const;
            
            //! Set the current time.
            void setTime(const core::Time&);

        private:
            FTK_PRIVATE();
        };
    }
}

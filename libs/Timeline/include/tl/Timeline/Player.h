// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender2 project.

#pragma once

#include <tl/Timeline/Timeline.h>

namespace tl
{
    namespace timeline
    {
        //! Playback.
        enum class TL_API_TYPE Playback
        {
            Stop,
            Forward,
            Reverse,

            Count,
            First = Stop
        };
        TL_ENUM(Playback);

        //! Frame actions.
        enum class TL_API_TYPE FrameAction
        {
            Next,
            Next_X10,
            Next_X100,
            Prev,
            Prev_X10,
            Prev_X100,
            Start,
            End,

            Count,
            First = Next
        };
        TL_ENUM(FrameAction);
    
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

            //! Get the timeline.
            const std::shared_ptr<Timeline>& getTimeline() const;

            //! \name Time
            ///@{
            
            const core::MediaRate& getRate() const;
            std::shared_ptr<ftk::IObservable<core::MediaRate> > observeRate() const;
            void setRate(const core::MediaRate&);
            
            const core::Time& getTime() const;
            std::shared_ptr<ftk::IObservable<core::Time> > observeTime() const;
            void setTime(const core::Time&);

            ///@}

            //! \name Playback
            ///@{

            timeline::Playback getPlayback() const;
            std::shared_ptr<ftk::IObservable<timeline::Playback> > observePlayback() const;
            void setPlayback(timeline::Playback);
            void togglePlayback();

            ///@}

            //! \name Frame Actions
            ///@{

            void frameAction(timeline::FrameAction);

            ///@}

        private:
            FTK_PRIVATE();
        };
    }
}

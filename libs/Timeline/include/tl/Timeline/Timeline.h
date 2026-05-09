// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <tl/Timeline/VideoGraph.h>
#include <tl/Core/Audio.h>
#include <tl/Core/Time.h>

#include <ftk/Core/Observable.h>

namespace tl
{
    namespace timeline
    {
        //! Timeline.
        class TL_API_TYPE Timeline : public std::enable_shared_from_this<Timeline>
        {
            FTK_NON_COPYABLE(Timeline);

        protected:
            void _init(const std::string&);

            Timeline();

        public:
            ~Timeline();

            //! Create an empty timeline.
            static std::shared_ptr<Timeline> create();

            //! Create a new timeline from an .otio, .otioz, or .otiod file.
            static std::shared_ptr<Timeline> create(const std::string&);
            
            //! Get the timeline start time.
            const core::Time& getStartTime() const;

            //! Observe the timeline start time.
            std::shared_ptr<ftk::IObservable<core::Time> > observeStartTime() const;

            //! Get the timeline duration.
            const core::Duration& getDuration() const;

            //! Observe the timeline duration.
            std::shared_ptr<ftk::IObservable<core::Duration> > observeDuration() const;

            //! Get video for the given time.
            std::shared_ptr<IVideoNode> getVideo(const core::Time&);

            //! Get audio for the given seconds.
            std::future<std::shared_ptr<core::Audio> > getAudio(int64_t seconds);

        private:
            FTK_PRIVATE();
        };
    }
}

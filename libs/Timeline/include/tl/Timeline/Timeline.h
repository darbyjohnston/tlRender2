// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <tl/Timeline/VideoGraph.h>
#include <tl/Core/Audio.h>
#include <tl/Core/Time.h>

#include <ftk/Core/Context.h>
#include <ftk/Core/Observable.h>
#include <ftk/Core/Path.h>
#include <ftk/Core/FileIO.h>

namespace tl
{
    namespace timeline
    {
        //! Media.
        struct Media
        {
            ftk::Path path;
            std::vector<ftk::MemFile> mem;
        };

        //! Media reference.
        struct MediaReference
        {
            std::shared_ptr<Media> media;
            std::optional<core::MediaTime> availableRangeStart;
            std::optional<core::MediaDuration> availableRangeDuration;
        };

        //! Base class for items in the timeline.
        struct IItem : public std::enable_shared_from_this<IItem>
        {
            virtual ~IItem() = 0;

            std::string name;
            core::Time startTime;
            core::Duration duration;
        };

        //! Clip.
        struct Clip : public IItem
        {
            std::map<std::string, std::shared_ptr<MediaReference>> mediaReferences;
            std::string activeMediaReference;
        };

        //! Track.
        struct Track : public IItem
        {
            std::vector<std::shared_ptr<IItem>> children;
        };

        //! Stack.
        struct Stack : public IItem
        {
            std::vector<std::shared_ptr<IItem>> children;
        };

        //! Timeline.
        class TL_API_TYPE Timeline : public std::enable_shared_from_this<Timeline>
        {
            FTK_NON_COPYABLE(Timeline);

        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const ftk::Path&);

            Timeline();

        public:
            ~Timeline();

            //! Create an empty timeline.
            static std::shared_ptr<Timeline> create(
                const std::shared_ptr<ftk::Context>&);

            //! Create a new timeline from an .otio, .otioz, or .otiod file.
            static std::shared_ptr<Timeline> create(
                const std::shared_ptr<ftk::Context>&,
                const ftk::Path&);

            //! Get the path the timeline was loaded from, or an empty path if
            //! the timeline was created empty.
            const ftk::Path& getPath() const;
            
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

            //! Get the timeline stack.
            const std::shared_ptr<Stack>& getStack() const;

            //! Get video for the given time.
            std::shared_ptr<IVideoNode> getVideo(const core::Time&);

            //! Get audio for the given seconds.
            std::future<std::shared_ptr<core::Audio> > getAudio(int64_t seconds);

        private:
            FTK_PRIVATE();
        };
    }
}

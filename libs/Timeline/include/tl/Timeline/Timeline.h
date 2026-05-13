// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <tl/Timeline/VideoGraph.h>
#include <tl/Core/Audio.h>

#include <ftk/Core/Context.h>
#include <ftk/Core/Image.h>
#include <ftk/Core/Observable.h>

namespace tl
{
    namespace timeline
    {
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
            std::string activeMediaReference = defaultMediaReference;
        };

        //! Track types.
        enum class TL_API_TYPE TrackType
        {
            Unknown,
            Video,
            Audio,

            Count,
            First = Unknown
        };
        TL_ENUM(TrackType);

        //! Track.
        struct Track : public IItem
        {
            TrackType type = TrackType::Unknown;
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

            //! Create a new timeline from a timeline file or media file.
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
            
            //! Get video information.
            const std::pair<ftk::Size2I, ftk::ImageType>& getVideoInfo() const;

            //! Get audio information.
            const core::AudioInfo& getAudioInfo() const;

            //! Get the timeline stack.
            const std::shared_ptr<Stack>& getStack() const;

            //! Get the video graph for the given time.
            std::shared_ptr<VideoGraph> getVideo(const core::Time&);

            //! Get audio for the given seconds.
            std::shared_ptr<core::Audio> getAudio(int64_t seconds);

        private:
            FTK_PRIVATE();
        };
    }
}

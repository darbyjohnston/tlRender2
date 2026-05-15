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

        //! Video information.
        struct VideoInfo
        {
            ftk::Size2I    size = ftk::Size2I(1920, 1080);
            ftk::ImageType type = ftk::ImageType::RGBA_U8;
            
            bool operator == (const VideoInfo&) const;
            bool operator != (const VideoInfo&) const;
        };
        
        //! Get default audio information.
        TL_API core::AudioInfo defaultAudioInfo();

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

            //! Get the timeline stack.
            const std::shared_ptr<Stack>& getStack() const;

            //! \name Time
            ///@{
            
            const core::MediaRate& getRate() const;
            std::shared_ptr<ftk::IObservable<core::MediaRate> > observeRate() const;
            void setRate(const core::MediaRate&);

            const core::Time& getStartTime() const;
            std::shared_ptr<ftk::IObservable<core::Time> > observeStartTime() const;

            const core::Duration& getDuration() const;
            std::shared_ptr<ftk::IObservable<core::Duration> > observeDuration() const;
            
            ///@}

            //! \name Video
            ///@{

            const VideoInfo& getVideoInfo() const;
            std::shared_ptr<VideoGraph> getVideo(const core::Time&);

            ///@}

            //! \name Audio
            ///@{

            const core::AudioInfo& getAudioInfo() const;
            std::shared_ptr<core::Audio> getAudio(int64_t seconds);

            ///@}

        private:
            FTK_PRIVATE();
        };
    }
}

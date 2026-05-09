// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <tl/Core/Audio.h>
#include <tl/Core/Time.h>

#include <ftk/Core/Image.h>

namespace tl
{
    namespace io
    {
        //! Read information.
        struct TL_API_TYPE ReadInfo
        {
            std::vector<ftk::ImageInfo> video;
            core::MediaTime             videoTime;
            core::MediaDuration         videoDuration;
            
            core::AudioInfo             audio;
            core::MediaTime             audioTime;
            core::MediaDuration         audioDuration;
        };

        //! Read video options.
        struct TL_API_TYPE ReadVideoOptions
        {
            int layer = 0;
        };

        //! Read audio options.
        struct TL_API_TYPE ReadAudioOptions
        {
            int layer = 0;
        };

        //! Base class for I/O plugins.
        class TL_API_TYPE IReadPlugin : public std::enable_shared_from_this<IReadPlugin>
        {
        protected:
            void _init(const std::string&);

            IReadPlugin() = default;

        public:
            virtual ~IReadPlugin() = 0;

            //! Get information.
            virtual ReadInfo getInfo() = 0;

            //! Get video frames.
            virtual std::shared_ptr<ftk::Image> getVideo(
                const core::MediaTime&,
                const ReadVideoOptions& = ReadVideoOptions()) = 0;

            //! Get audio data.
            virtual std::shared_ptr<core::Audio> getAudio(
                const core::MediaTime&,
                size_t sampleCount,
                const ReadAudioOptions& = ReadAudioOptions()) = 0;

        protected:
            std::string _fileName;
        };
    }
}

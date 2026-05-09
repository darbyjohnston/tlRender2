// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <tl/IO/Plugin.h>

namespace tl
{
    namespace io
    {
        class EXRRead : public IReadPlugin
        {
            FTK_NON_COPYABLE(EXRRead);

        protected:
            void _init(const std::string&);

            EXRRead();

        public:
            virtual ~EXRRead();

            ReadInfo getInfo() override;
            std::shared_ptr<ftk::Image> getVideo(
                const core::MediaTime&,
                const ReadVideoOptions& = ReadVideoOptions()) override;
            std::shared_ptr<core::Audio> getAudio(
                const core::MediaTime&,
                size_t sampleCount,
                const ReadAudioOptions& = ReadAudioOptions()) override;

        private:
            FTK_PRIVATE();
        };
    }
}

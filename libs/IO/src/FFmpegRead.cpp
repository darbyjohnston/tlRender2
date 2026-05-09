// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include "FFmpegRead.h"

namespace tl
{
    namespace io
    {
        struct FFmpegRead::Private
        {
        };

        void FFmpegRead::_init(const std::string& fileName)
        {
            IReadPlugin::_init(fileName);
        }

        FFmpegRead::FFmpegRead() :
            _p(new Private)
        {}
        
        FFmpegRead::~FFmpegRead()
        {}

        ReadInfo FFmpegRead::getInfo()
        {
            return ReadInfo();
        }

        std::shared_ptr<ftk::Image> FFmpegRead::getVideo(
            const core::MediaTime&,
            const ReadVideoOptions&)
        {
            return nullptr;
        }

        std::shared_ptr<core::Audio> FFmpegRead::getAudio(
            const core::MediaTime&,
            size_t sampleCount,
            const ReadAudioOptions&)
        {
            return nullptr;
        }
    }
}

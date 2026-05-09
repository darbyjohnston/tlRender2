// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include "EXRRead.h"

namespace tl
{
    namespace io
    {
        struct EXRRead::Private
        {
        };

        void EXRRead::_init(const std::string& fileName)
        {
            IReadPlugin::_init(fileName);
        }

        EXRRead::EXRRead() :
            _p(new Private)
        {}
        
        EXRRead::~EXRRead()
        {}

        ReadInfo EXRRead::getInfo()
        {
            return ReadInfo();
        }

        std::shared_ptr<ftk::Image> EXRRead::getVideo(
            const core::MediaTime&,
            const ReadVideoOptions&)
        {
            return nullptr;
        }

        std::shared_ptr<core::Audio> EXRRead::getAudio(
            const core::MediaTime&,
            size_t sampleCount,
            const ReadAudioOptions&)
        {
            return nullptr;
        }
    }
}

// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include "OIIORead.h"

namespace tl
{
    namespace io
    {
        struct OIIORead::Private
        {
        };

        void OIIORead::_init(const std::string& fileName)
        {
            IReadPlugin::_init(fileName);
        }

        OIIORead::OIIORead() :
            _p(new Private)
        {}
        
        OIIORead::~OIIORead()
        {}

        ReadInfo OIIORead::getInfo()
        {
            return ReadInfo();
        }

        std::shared_ptr<ftk::Image> OIIORead::getVideo(
            const core::MediaTime&,
            const ReadVideoOptions&)
        {
            return nullptr;
        }

        std::shared_ptr<core::Audio> OIIORead::getAudio(
            const core::MediaTime&,
            size_t sampleCount,
            const ReadAudioOptions&)
        {
            return nullptr;
        }
    }
}

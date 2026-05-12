// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/Timeline/Init.h>

#include <tl/IO/IOSystem.h>
#include <tl/Core/Audio.h>

#include <ftk/Core/DiagSystem.h>

namespace tl
{
    using namespace core;

    namespace timeline
    {
        void init(const std::shared_ptr<ftk::Context>& context)
        {
            auto diagSystem = context->getSystem<ftk::DiagSystem>();
            diagSystem->addSampler(
                "tl Memory/Audio: {0}MB",
                [] { return tl::Audio::getTotalByteCount() / ftk::megabyte; });
            diagSystem->addSampler(
                "tl Objects/Audio: {0}",
                [] { return tl::Audio::getObjectCount(); });

            if (!context->getSystem<io::ReadSystem>())
            {
                context->addSystem(io::ReadSystem::create(context));
            }
            if (!context->getSystem<io::WriteSystem>())
            {
                context->addSystem(io::WriteSystem::create(context));
            }
        }
    }
}

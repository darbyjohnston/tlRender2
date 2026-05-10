// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include "EXRRead.h"

namespace tl
{
    namespace io
    {
        struct EXRReadPlugin::Private
        {
        };

        EXRReadPlugin::EXRReadPlugin() :
            _p(new Private)
        {}
        
        EXRReadPlugin::~EXRReadPlugin()
        {}

        bool EXRReadPlugin::canRead(
            const ftk::Path& path,
            const ReadOptions& options)
        {
            return false;
        }

        std::shared_ptr<IRead> EXRReadPlugin::read(
            const ftk::Path& path,
            const ReadOptions& options)
        {
            return nullptr;
        }
    }
}

// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include "FFmpegRead.h"

namespace tl
{
    namespace io
    {
        struct FFmpegReadPlugin::Private
        {
        };

        FFmpegReadPlugin::FFmpegReadPlugin() :
            _p(new Private)
        {}
        
        FFmpegReadPlugin::~FFmpegReadPlugin()
        {}
    }
}

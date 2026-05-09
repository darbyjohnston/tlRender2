// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/IO/Plugin.h>

namespace tl
{
    namespace io
    {
        void IReadPlugin::_init(const std::string& fileName)
        {
            _fileName = fileName;
        }

        IReadPlugin::~IReadPlugin()
        {}
    }
}

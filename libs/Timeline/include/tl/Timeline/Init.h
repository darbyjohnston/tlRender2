// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <tl/Core/Export.h>

#include <ftk/Core/Context.h>

namespace tl
{
    namespace timeline
    {
        // Initialize the library.
        TL_API void init(const std::shared_ptr<ftk::Context>&);
    }
}

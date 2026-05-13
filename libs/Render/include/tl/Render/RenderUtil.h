// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <tl/Core/Export.h>

#include <ftk/Core/Image.h>

namespace tl
{
    namespace render
    {
        TL_API bool compare(
            const std::shared_ptr<ftk::Image>&,
            const std::shared_ptr<ftk::Image>&);
    }
}

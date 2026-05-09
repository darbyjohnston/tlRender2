// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/Core/Time.h>

namespace tl
{
    namespace core
    {
        Time operator + (Time t, Duration d)
        {
            return { t.frames + d.frames };
        }

        Duration operator - (Time a, Time b)
        {
            return { a.frames - b.frames };
        }

        Time timeFromOTIO(const OTIO_NS::RationalTime& rt, double projectRate)
        {
            return { static_cast<int64_t>(rt.rescaled_to(projectRate).value()) };
        }

        OTIO_NS::RationalTime timeToOTIO(Time t, double projectRate)
        {
            return OTIO_NS::RationalTime(t.frames, projectRate);
        }
    }
}


// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <tl/Core/Time.h>

#include <opentime/rationalTime.h>

namespace tl
{
    namespace timeline
    {
        core::Time timeFromOTIO(
            const opentime::OPENTIME_VERSION_NS::RationalTime&,
            double projectRate);
        core::Duration durationFromOTIO(
            const opentime::OPENTIME_VERSION_NS::RationalTime&,
            double projectRate);

        opentime::OPENTIME_VERSION_NS::RationalTime timeToOTIO(
            core::Time,
            double projectRate);
        opentime::OPENTIME_VERSION_NS::RationalTime durationToOTIO(
            core::Duration,
            double projectRate);

        core::MediaTime mediaTimeFromOTIO(
            const opentime::OPENTIME_VERSION_NS::RationalTime&);
        core::MediaDuration mediaDurationFromOTIO(
            const opentime::OPENTIME_VERSION_NS::RationalTime&);

        opentime::OPENTIME_VERSION_NS::RationalTime mediaTimeToOTIO(
            const core::MediaTime&);
        opentime::OPENTIME_VERSION_NS::RationalTime mediaDurationToOTIO(
            const core::MediaDuration&);
    }
}

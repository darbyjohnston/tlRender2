// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <tl/Core/Util.h>

#include <opentimelineio/version.h>
#include <opentime/rationalTime.h>

namespace tl
{
    namespace core
    {
        //! Time.
        struct Time
        {
            int64_t frames = 0;
        };

        //! Duration.
        struct Duration
        {
            int64_t frames = 0;
        };
        
        //! Media rate.
        struct MediaRate
        {
            int num = 1;
            int den = 24;
        };

        //! Media time.
        struct MediaTime
        {
            int64_t   value = 0;
            MediaRate rate;
        };

        //! Media duration.
        struct MediaDuration
        {
            int64_t   value = 0;
            MediaRate rate;
        };

        Time operator + (Time t, Duration d);
        Duration operator - (Time a, Time b);

        Time timeFromOTIO(const OTIO_NS::RationalTime&, double projectRate);

        OTIO_NS::RationalTime timeToOTIO(Time, double projectRate);
    }
}

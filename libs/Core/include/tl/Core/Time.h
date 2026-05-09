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
        struct TL_API_TYPE Time
        {
            int64_t frames = 0;
        };

        //! Duration.
        struct TL_API_TYPE Duration
        {
            int64_t frames = 0;
        };
        
        //! Media rate.
        struct TL_API_TYPE MediaRate
        {
            int num = 1;
            int den = 24;
        };

        //! Media time.
        struct TL_API_TYPE MediaTime
        {
            int64_t   value = 0;
            MediaRate rate;
        };

        //! Media duration.
        struct TL_API_TYPE MediaDuration
        {
            int64_t   value = 0;
            MediaRate rate;
        };

        TL_API Time operator + (Time t, Duration d);
        TL_API Duration operator - (Time a, Time b);

        TL_API Time timeFromOTIO(const OTIO_NS::RationalTime&, double projectRate);

        TL_API OTIO_NS::RationalTime timeToOTIO(Time, double projectRate);
    }
}

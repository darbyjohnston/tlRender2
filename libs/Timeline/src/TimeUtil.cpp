// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include "TimeUtil.h"

using namespace opentime::OPENTIME_VERSION_NS;

namespace tl
{
    using namespace core;

    namespace timeline
    {
        Time timeFromOTIO(const RationalTime& rt, double projectRate)
        {
            return { static_cast<int64_t>(rt.rescaled_to(projectRate).value()) };
        }

        Duration durationFromOTIO(const RationalTime& rt, double projectRate)
        {
            return { static_cast<int64_t>(rt.rescaled_to(projectRate).value()) };
        }

        RationalTime timeToOTIO(Time t, double projectRate)
        {
            return RationalTime(static_cast<double>(t.frames), projectRate);
        }

        RationalTime durationToOTIO(Duration d, double projectRate)
        {
            return RationalTime(static_cast<double>(d.frames), projectRate);
        }

        core::MediaRate mediaRateFromOTIO(double rate)
        {
            // OTIO stores rate as a single double; recovering an exact
            // integer num/den isn't always possible. We special-case the
            // common NTSC-family rates that lose precision in double form
            // and otherwise fall back to (round(rate), 1), which is exact
            // for integer-valued rates (24, 25, 30, 48000, 44100, etc.)
            // but lossy for unusual rationals not in the table below.
            MediaRate r{ static_cast<int>(std::nearbyint(rate)), 1 };
            if      (std::abs(rate - 24000.0 / 1001.0) < 1e-6) r = getCommonRate(CommonRate::_23_976);
            else if (std::abs(rate - 30000.0 / 1001.0) < 1e-6) r = getCommonRate(CommonRate::_29_97);
            else if (std::abs(rate - 60000.0 / 1001.0) < 1e-6) r = getCommonRate(CommonRate::_59_94);
            return r;
        }

        MediaTime mediaTimeFromOTIO(const RationalTime& rt)
        {
            return MediaTime{ static_cast<int64_t>(rt.value()), mediaRateFromOTIO(rt.rate()) };
        }

        MediaDuration mediaDurationFromOTIO(const RationalTime& rt)
        {
            const auto t = mediaTimeFromOTIO(rt);
            return MediaDuration{ t.frames, t.rate };
        }

        RationalTime mediaTimeToOTIO(const MediaTime& t)
        {
            return RationalTime(static_cast<double>(t.frames), t.rate.toDouble());
        }

        RationalTime mediaDurationToOTIO(const MediaDuration& d)
        {
            return RationalTime(static_cast<double>(d.frames), d.rate.toDouble());
        }
    }
}


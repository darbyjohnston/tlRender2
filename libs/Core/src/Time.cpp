// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/Core/Time.h>

#include <ftk/Core/Format.h>

#include <cmath>
#include <ostream>

namespace tl
{
    namespace core
    {
        MediaRate mediaRate24()     { return { 24,    1    }; }
        MediaRate mediaRate23_976() { return { 24000, 1001 }; }
        MediaRate mediaRate25()     { return { 25,    1    }; }
        MediaRate mediaRate30()     { return { 30,    1    }; }
        MediaRate mediaRate29_97()  { return { 30000, 1001 }; }
        MediaRate mediaRate48()     { return { 48,    1    }; }
        MediaRate mediaRate50()     { return { 50,    1    }; }
        MediaRate mediaRate60()     { return { 60,    1    }; }
        MediaRate mediaRate59_94()  { return { 60000, 1001 }; }

        bool operator == (const MediaTime& a, const MediaTime& b)
        {
            return a.value == b.value && a.rate == b.rate;
        }

        bool operator != (const MediaTime& a, const MediaTime& b)
        {
            return !(a == b);
        }

        bool operator == (const MediaDuration& a, const MediaDuration& b)
        {
            return a.value == b.value && a.rate == b.rate;
        }

        bool operator != (const MediaDuration& a, const MediaDuration& b)
        {
            return !(a == b);
        }

        namespace
        {
            // Round-half-to-even ("banker's rounding") to limit cumulative
            // bias when rescaling repeatedly.
            int64_t roundToInt64(double v)
            {
                const double r = std::nearbyint(v);
                return static_cast<int64_t>(r);
            }
        }

        MediaTime rescale(const MediaTime& in, const MediaRate& target)
        {
            MediaTime out;
            out.rate = target;
            if (in.rate == target)
            {
                out.value = in.value;
            }
            else if (in.rate.isValid() && target.isValid())
            {
                // value_target = value_in * (target.num / target.den) /
                //                           (in.rate.num / in.rate.den)
                //              = value_in * target.num * in.rate.den /
                //                          (target.den * in.rate.num)
                const double v =
                    static_cast<double>(in.value) *
                    static_cast<double>(target.num) *
                    static_cast<double>(in.rate.den) /
                    (static_cast<double>(target.den) *
                     static_cast<double>(in.rate.num));
                out.value = roundToInt64(v);
            }
            return out;
        }

        MediaDuration rescale(const MediaDuration& in, const MediaRate& target)
        {
            MediaDuration out;
            out.rate = target;
            if (in.rate == target)
            {
                out.value = in.value;
            }
            else if (in.rate.isValid() && target.isValid())
            {
                const double v =
                    static_cast<double>(in.value) *
                    static_cast<double>(target.num) *
                    static_cast<double>(in.rate.den) /
                    (static_cast<double>(target.den) *
                     static_cast<double>(in.rate.num));
                out.value = roundToInt64(v);
            }
            return out;
        }

        std::string to_string(Time t)
        {
            return ftk::Format("{0}f").arg(t.frames).str();
        }

        std::string to_string(Duration d)
        {
            return ftk::Format("{0}f").arg(d.frames).str();
        }

        std::string to_string(const MediaRate& r)
        {
            return ftk::Format("{0}/{1}").arg(r.num).arg(r.den).str();
        }

        std::string to_string(const MediaTime& t)
        {
            return ftk::Format("{0}@{1}/{2}").
                arg(t.value).arg(t.rate.num).arg(t.rate.den).str();
        }

        std::string to_string(const MediaDuration& d)
        {
            return ftk::Format("{0}@{1}/{2}").
                arg(d.value).arg(d.rate.num).arg(d.rate.den).str();
        }

        std::ostream& operator << (std::ostream& os, Time t)
        {
            return os << to_string(t);
        }

        std::ostream& operator << (std::ostream& os, Duration d)
        {
            return os << to_string(d);
        }

        std::ostream& operator << (std::ostream& os, const MediaRate& r)
        {
            return os << to_string(r);
        }

        std::ostream& operator << (std::ostream& os, const MediaTime& t)
        {
            return os << to_string(t);
        }

        std::ostream& operator << (std::ostream& os, const MediaDuration& d)
        {
            return os << to_string(d);
        }

        Time timeFromOTIO(const OTIO_NS::RationalTime& rt, double projectRate)
        {
            return { static_cast<int64_t>(rt.rescaled_to(projectRate).value()) };
        }

        Duration durationFromOTIO(const OTIO_NS::RationalTime& rt, double projectRate)
        {
            return { static_cast<int64_t>(rt.rescaled_to(projectRate).value()) };
        }

        OTIO_NS::RationalTime timeToOTIO(Time t, double projectRate)
        {
            return OTIO_NS::RationalTime(
                static_cast<double>(t.frames),
                projectRate);
        }

        OTIO_NS::RationalTime durationToOTIO(Duration d, double projectRate)
        {
            return OTIO_NS::RationalTime(
                static_cast<double>(d.frames),
                projectRate);
        }

        MediaTime mediaTimeFromOTIO(const OTIO_NS::RationalTime& rt)
        {
            // OTIO stores rate as a single double; recovering an exact
            // integer num/den isn't always possible. We special-case the
            // common NTSC-family rates that lose precision in double form
            // and otherwise fall back to (round(rate), 1), which is exact
            // for integer-valued rates (24, 25, 30, 48000, 44100, etc.)
            // but lossy for unusual rationals not in the table below.
            const double rate = rt.rate();
            MediaRate r{ static_cast<int>(std::nearbyint(rate)), 1 };
            if      (std::abs(rate - 24000.0 / 1001.0) < 1e-6) r = mediaRate23_976();
            else if (std::abs(rate - 30000.0 / 1001.0) < 1e-6) r = mediaRate29_97();
            else if (std::abs(rate - 60000.0 / 1001.0) < 1e-6) r = mediaRate59_94();
            return MediaTime{ static_cast<int64_t>(rt.value()), r };
        }

        MediaDuration mediaDurationFromOTIO(const OTIO_NS::RationalTime& rt)
        {
            const auto t = mediaTimeFromOTIO(rt);
            return MediaDuration{ t.value, t.rate };
        }

        OTIO_NS::RationalTime mediaTimeToOTIO(const MediaTime& t)
        {
            return OTIO_NS::RationalTime(
                static_cast<double>(t.value),
                t.rate.toDouble());
        }

        OTIO_NS::RationalTime mediaDurationToOTIO(const MediaDuration& d)
        {
            return OTIO_NS::RationalTime(
                static_cast<double>(d.value),
                d.rate.toDouble());
        }
    }
}

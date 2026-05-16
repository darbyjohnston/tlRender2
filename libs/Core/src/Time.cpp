// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/Core/Time.h>

#include <ftk/Core/Error.h>
#include <ftk/Core/Format.h>
#include <ftk/Core/String.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <ostream>
#include <sstream>

namespace tl
{
    namespace core
    {
        TL_ENUM_IMPL(
            CommonRate,
            "24",
            "23.976",
            "25",
            "30",
            "29.97",
            "48",
            "50",
            "60",
            "59.94");

        MediaRate getCommonRate(CommonRate value)
        {
            const std::array<MediaRate, static_cast<size_t>(CommonRate::Count)> data =
            {
                MediaRate(24),
                MediaRate(24000, 1001),
                MediaRate(25),
                MediaRate(30),
                MediaRate(30000, 1001),
                MediaRate(48),
                MediaRate(50),
                MediaRate(60),
                MediaRate(60000, 1001)
            };
            return data[static_cast<size_t>(value)];
        }

        namespace
        {
            // Round-half-to-even ("banker's rounding") to limit cumulative
            // bias when rescaling repeatedly.
            Frame roundToInt64(double v)
            {
                const double r = std::nearbyint(v);
                return static_cast<Frame>(r);
            }
        }

        MediaTime rescale(const MediaTime& in, const MediaRate& target)
        {
            MediaTime out;
            out.rate = target;
            if (in.rate == target)
            {
                out.frames = in.frames;
            }
            else if (in.rate.isValid() && target.isValid())
            {
                // value_target = value_in * (target.num / target.den) /
                //                           (in.rate.num / in.rate.den)
                //              = value_in * target.num * in.rate.den /
                //                          (target.den * in.rate.num)
                const double v =
                    static_cast<double>(in.frames) *
                    static_cast<double>(target.num) *
                    static_cast<double>(in.rate.den) /
                    (static_cast<double>(target.den) *
                     static_cast<double>(in.rate.num));
                out.frames = roundToInt64(v);
            }
            return out;
        }

        MediaDuration rescale(const MediaDuration& in, const MediaRate& target)
        {
            MediaDuration out;
            out.rate = target;
            if (in.rate == target)
            {
                out.frames = in.frames;
            }
            else if (in.rate.isValid() && target.isValid())
            {
                const double v =
                    static_cast<double>(in.frames) *
                    static_cast<double>(target.num) *
                    static_cast<double>(in.rate.den) /
                    (static_cast<double>(target.den) *
                     static_cast<double>(in.rate.num));
                out.frames = roundToInt64(v);
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
                arg(t.frames).arg(t.rate.num).arg(t.rate.den).str();
        }

        std::string to_string(const MediaDuration& d)
        {
            return ftk::Format("{0}@{1}/{2}").
                arg(d.frames).arg(d.rate.num).arg(d.rate.den).str();
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
    }
}

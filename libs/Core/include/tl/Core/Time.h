// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <tl/Core/Util.h>

#include <iosfwd>
#include <string>

namespace tl
{
    namespace core
    {
        // Frame.
        typedef int64_t Frame;

        //! Time, in project frames.
        struct TL_API_TYPE Time
        {
            Frame frames = 0;
        };

        //! Duration, in project frames.
        struct TL_API_TYPE Duration
        {
            Frame frames = 0;
        };

        //! Media rate as a rational number (num/den frames per second).
        struct TL_API_TYPE MediaRate
        {
            MediaRate() = default;
            constexpr MediaRate(int num, int dev = 1);

            int num = 0;
            int den = 0;

            //! Get the rate as a double (num / den).
            constexpr double toDouble() const;

            //! Is the rate valid (positive numerator and denominator)?
            constexpr bool isValid() const;

            constexpr bool operator == (const MediaRate&) const;
            constexpr bool operator != (const MediaRate&) const;
        };

        //! Common media rates.
        TL_API MediaRate mediaRate24();
        TL_API MediaRate mediaRate23_976();
        TL_API MediaRate mediaRate25();
        TL_API MediaRate mediaRate30();
        TL_API MediaRate mediaRate29_97();
        TL_API MediaRate mediaRate48();
        TL_API MediaRate mediaRate50();
        TL_API MediaRate mediaRate60();
        TL_API MediaRate mediaRate59_94();

        //! Media time, in frames at a given rate.
        struct TL_API_TYPE MediaTime
        {
            Frame     frames = 0;
            MediaRate rate;

            //! Get the time in seconds.
            constexpr double toSeconds() const;
        };

        //! Media duration, in frames at a given rate.
        struct TL_API_TYPE MediaDuration
        {
            Frame     frames = 0;
            MediaRate rate;

            //! Get the duration in seconds.
            constexpr double toSeconds() const;
        };

        //! \name Time / Duration operations
        ///@{

        constexpr Time operator + (const Time& t, const Duration& d);
        constexpr Time operator + (const Duration& d, Time t);
        constexpr Time operator - (const Time& t, const Duration& d);

        constexpr Duration operator - (const Time& a, const Time& b);
        constexpr Duration operator + (const Duration& a, const Duration& b);
        constexpr Duration operator - (const Duration& a, const Duration& b);
        constexpr Duration operator - (const Duration& d);
        constexpr Duration operator * (const Duration& d, Frame n);
        constexpr Duration operator * (Frame n, const Duration& d);

        constexpr Time& operator += (Time& t, const Duration& d);
        constexpr Time& operator -= (Time& t, const Duration& d);

        constexpr Duration& operator += (Duration& a, const Duration& b);
        constexpr Duration& operator -= (Duration& a, const Duration& b);

        constexpr bool operator == (const Time&, const Time&);
        constexpr bool operator != (const Time&, const Time&);
        constexpr bool operator <  (const Time&, const Time&);
        constexpr bool operator <= (const Time&, const Time&);
        constexpr bool operator >  (const Time&, const Time&);
        constexpr bool operator >= (const Time&, const Time&);

        constexpr bool operator == (const Duration&, const Duration&);
        constexpr bool operator != (const Duration&, const Duration&);
        constexpr bool operator <  (const Duration&, const Duration&);
        constexpr bool operator <= (const Duration&, const Duration&);
        constexpr bool operator >  (const Duration&, const Duration&);
        constexpr bool operator >= (const Duration&, const Duration&);

        ///@}

        //! \name MediaTime / MediaDuration operations
        //!
        //! Comparison requires matching rates; for cross-rate work, use
        //! the rescale helpers first.
        ///@{

        constexpr bool operator == (const MediaTime&, const MediaTime&);
        constexpr bool operator != (const MediaTime&, const MediaTime&);
        constexpr bool operator == (const MediaDuration&, const MediaDuration&);
        constexpr bool operator != (const MediaDuration&, const MediaDuration&);

        //! Rescale a media time to a different rate.
        TL_API MediaTime rescale(const MediaTime&, const MediaRate&);

        //! Rescale a media duration to a different rate.
        TL_API MediaDuration rescale(const MediaDuration&, const MediaRate&);

        //! Reinterpret a project time as a media time at the given rate.
        //! Caller is responsible for ensuring this is meaningful (typically
        //! when the project rate equals the media rate).
        constexpr MediaTime mediaTime(const Time&, const MediaRate&);

        ///@}

        //! \name String conversion
        ///@{

        TL_API std::string to_string(Time);
        TL_API std::string to_string(Duration);
        TL_API std::string to_string(const MediaRate&);
        TL_API std::string to_string(const MediaTime&);
        TL_API std::string to_string(const MediaDuration&);

        TL_API std::ostream& operator << (std::ostream&, Time);
        TL_API std::ostream& operator << (std::ostream&, Duration);
        TL_API std::ostream& operator << (std::ostream&, const MediaRate&);
        TL_API std::ostream& operator << (std::ostream&, const MediaTime&);
        TL_API std::ostream& operator << (std::ostream&, const MediaDuration&);

        ///@}
    }
}

#include <tl/Core/TimeInline.h>

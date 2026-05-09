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
        //! Time, in project frames.
        struct TL_API_TYPE Time
        {
            int64_t frames = 0;
        };

        //! Duration, in project frames.
        struct TL_API_TYPE Duration
        {
            int64_t frames = 0;
        };

        //! Media rate as a rational number (num/den frames per second).
        struct TL_API_TYPE MediaRate
        {
            int num = 1;
            int den = 24;

            //! Get the rate as a double (num / den).
            double toDouble() const;

            //! Is the rate valid (positive numerator and denominator)?
            bool isValid() const;

            bool operator == (const MediaRate&) const;
            bool operator != (const MediaRate&) const;
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

        //! Media time, in samples at a given rate.
        struct TL_API_TYPE MediaTime
        {
            int64_t   value = 0;
            MediaRate rate;

            //! Get the time in seconds.
            double toSeconds() const;
        };

        //! Media duration, in samples at a given rate.
        struct TL_API_TYPE MediaDuration
        {
            int64_t   value = 0;
            MediaRate rate;

            //! Get the duration in seconds.
            double toSeconds() const;
        };

        // Inline arithmetic and comparison operators on Time and Duration
        // are defined in TimeInline.h.

        //! \name MediaTime / MediaDuration operations
        //!
        //! Comparison requires matching rates; for cross-rate work, use
        //! the rescale helpers first.
        ///@{

        TL_API bool operator == (const MediaTime&, const MediaTime&);
        TL_API bool operator != (const MediaTime&, const MediaTime&);
        TL_API bool operator == (const MediaDuration&, const MediaDuration&);
        TL_API bool operator != (const MediaDuration&, const MediaDuration&);

        //! Rescale a media time to a different rate.
        TL_API MediaTime rescale(const MediaTime&, const MediaRate&);

        //! Rescale a media duration to a different rate.
        TL_API MediaDuration rescale(const MediaDuration&, const MediaRate&);

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

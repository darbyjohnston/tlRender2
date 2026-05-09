// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

namespace tl
{
    namespace core
    {
        inline double MediaRate::toDouble() const
        {
            return den != 0 ?
                static_cast<double>(num) / static_cast<double>(den) :
                0.0;
        }

        inline bool MediaRate::isValid() const
        {
            return num > 0 && den > 0;
        }

        inline bool MediaRate::operator == (const MediaRate& other) const
        {
            return num == other.num && den == other.den;
        }

        inline bool MediaRate::operator != (const MediaRate& other) const
        {
            return !(*this == other);
        }

        inline double MediaTime::toSeconds() const
        {
            return rate.num != 0 ?
                static_cast<double>(value) * rate.den / rate.num :
                0.0;
        }

        inline double MediaDuration::toSeconds() const
        {
            return rate.num != 0 ?
                static_cast<double>(value) * rate.den / rate.num :
                0.0;
        }

        inline Time operator + (Time t, Duration d)
        {
            return { t.frames + d.frames };
        }

        inline Time operator + (Duration d, Time t)
        {
            return { t.frames + d.frames };
        }

        inline Time operator - (Time t, Duration d)
        {
            return { t.frames - d.frames };
        }

        inline Duration operator - (Time a, Time b)
        {
            return { a.frames - b.frames };
        }

        inline Duration operator + (Duration a, Duration b)
        {
            return { a.frames + b.frames };
        }

        inline Duration operator - (Duration a, Duration b)
        {
            return { a.frames - b.frames };
        }

        inline Duration operator - (Duration d)
        {
            return { -d.frames };
        }

        inline Duration operator * (Duration d, int64_t n)
        {
            return { d.frames * n };
        }

        inline Duration operator * (int64_t n, Duration d)
        {
            return { d.frames * n };
        }

        inline Time& operator += (Time& t, Duration d)
        {
            t.frames += d.frames;
            return t;
        }

        inline Time& operator -= (Time& t, Duration d)
        {
            t.frames -= d.frames;
            return t;
        }

        inline Duration& operator += (Duration& a, Duration b)
        {
            a.frames += b.frames;
            return a;
        }

        inline Duration& operator -= (Duration& a, Duration b)
        {
            a.frames -= b.frames;
            return a;
        }

        inline bool operator == (Time a, Time b) { return a.frames == b.frames; }
        inline bool operator != (Time a, Time b) { return a.frames != b.frames; }
        inline bool operator <  (Time a, Time b) { return a.frames <  b.frames; }
        inline bool operator <= (Time a, Time b) { return a.frames <= b.frames; }
        inline bool operator >  (Time a, Time b) { return a.frames >  b.frames; }
        inline bool operator >= (Time a, Time b) { return a.frames >= b.frames; }

        inline bool operator == (Duration a, Duration b) { return a.frames == b.frames; }
        inline bool operator != (Duration a, Duration b) { return a.frames != b.frames; }
        inline bool operator <  (Duration a, Duration b) { return a.frames <  b.frames; }
        inline bool operator <= (Duration a, Duration b) { return a.frames <= b.frames; }
        inline bool operator >  (Duration a, Duration b) { return a.frames >  b.frames; }
        inline bool operator >= (Duration a, Duration b) { return a.frames >= b.frames; }
    }
}


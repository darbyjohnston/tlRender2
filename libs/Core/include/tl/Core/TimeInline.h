// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

namespace tl
{
    namespace core
    {
        constexpr MediaRate::MediaRate(int num, int den) :
            num(num),
            den(den)
        {}

        constexpr double MediaRate::toDouble() const
        {
            return den != 0 ?
                static_cast<double>(num) / static_cast<double>(den) :
                0.0;
        }

        constexpr bool MediaRate::isValid() const
        {
            return num > 0 && den > 0;
        }

        constexpr bool MediaRate::operator == (const MediaRate& other) const
        {
            return num == other.num && den == other.den;
        }

        constexpr bool MediaRate::operator != (const MediaRate& other) const
        {
            return !(*this == other);
        }

        constexpr double MediaTime::toSeconds() const
        {
            return rate.num != 0 ?
                static_cast<double>(frames) * rate.den / rate.num :
                0.0;
        }

        constexpr double MediaDuration::toSeconds() const
        {
            return rate.num != 0 ?
                static_cast<double>(frames) * rate.den / rate.num :
                0.0;
        }

        constexpr Time operator + (const Time& t, const Duration& d)
        {
            return { t.frames + d.frames };
        }

        constexpr Time operator + (const Duration& d, const Time& t)
        {
            return { t.frames + d.frames };
        }

        constexpr Time operator - (const Time& t, const Duration& d)
        {
            return { t.frames - d.frames };
        }

        constexpr Duration operator - (const Time& a, const Time& b)
        {
            return { a.frames - b.frames };
        }

        constexpr Duration operator + (const Duration& a, const Duration& b)
        {
            return { a.frames + b.frames };
        }

        constexpr Duration operator - (const Duration& a, const Duration& b)
        {
            return { a.frames - b.frames };
        }

        constexpr Duration operator - (const Duration& d)
        {
            return { -d.frames };
        }

        constexpr Duration operator * (const Duration& d, Frame n)
        {
            return { d.frames * n };
        }

        constexpr Duration operator * (Frame n, const Duration& d)
        {
            return { d.frames * n };
        }

        constexpr Time& operator += (Time& t, const Duration& d)
        {
            t.frames += d.frames;
            return t;
        }

        constexpr Time& operator -= (Time& t, const Duration& d)
        {
            t.frames -= d.frames;
            return t;
        }

        constexpr Duration& operator += (Duration& a, const Duration& b)
        {
            a.frames += b.frames;
            return a;
        }

        constexpr Duration& operator -= (Duration& a, const Duration& b)
        {
            a.frames -= b.frames;
            return a;
        }

        constexpr bool operator == (const Time& a, const Time& b) { return a.frames == b.frames; }
        constexpr bool operator != (const Time& a, const Time& b) { return a.frames != b.frames; }
        constexpr bool operator <  (const Time& a, const Time& b) { return a.frames <  b.frames; }
        constexpr bool operator <= (const Time& a, const Time& b) { return a.frames <= b.frames; }
        constexpr bool operator >  (const Time& a, const Time& b) { return a.frames >  b.frames; }
        constexpr bool operator >= (const Time& a, const Time& b) { return a.frames >= b.frames; }

        constexpr bool operator == (const Duration& a, const Duration& b) { return a.frames == b.frames; }
        constexpr bool operator != (const Duration& a, const Duration& b) { return a.frames != b.frames; }
        constexpr bool operator <  (const Duration& a, const Duration& b) { return a.frames <  b.frames; }
        constexpr bool operator <= (const Duration& a, const Duration& b) { return a.frames <= b.frames; }
        constexpr bool operator >  (const Duration& a, const Duration& b) { return a.frames >  b.frames; }
        constexpr bool operator >= (const Duration& a, const Duration& b) { return a.frames >= b.frames; }

        constexpr bool operator == (const MediaTime& a, const MediaTime& b)
        {
            return a.frames == b.frames && a.rate == b.rate;
        }

        constexpr bool operator != (const MediaTime& a, const MediaTime& b)
        {
            return !(a == b);
        }

        constexpr bool operator == (const MediaDuration& a, const MediaDuration& b)
        {
            return a.frames == b.frames && a.rate == b.rate;
        }

        constexpr bool operator != (const MediaDuration& a, const MediaDuration& b)
        {
            return !(a == b);
        }
    }
}


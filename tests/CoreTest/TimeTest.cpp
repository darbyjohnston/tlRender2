// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <CoreTest/TimeTest.h>

#include <tl/Core/Time.h>

#include <ftk/Core/Assert.h>

#include <cmath>
#include <sstream>

namespace tl
{
    using namespace core;

    namespace core_test
    {
        TimeTest::TimeTest(const std::shared_ptr<ftk::Context>& context) :
            ITest(context, "tl::core_test::TimeTest")
        {}

        TimeTest::~TimeTest()
        {}

        std::shared_ptr<TimeTest> TimeTest::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            return std::shared_ptr<TimeTest>(new TimeTest(context));
        }

        void TimeTest::run()
        {
            _members();
            _arithmetic();
            _comparison();
            _mediaRate();
            _mediaTime();
            _rescale();
            _serialize();
        }

        void TimeTest::_members()
        {
            {
                const Time t;
                FTK_ASSERT(0 == t.frames);
            }
            {
                const Time t{ 42 };
                FTK_ASSERT(42 == t.frames);
            }
            {
                const Duration d;
                FTK_ASSERT(0 == d.frames);
            }
            {
                const Duration d{ -7 };
                FTK_ASSERT(-7 == d.frames);
            }
        }

        void TimeTest::_arithmetic()
        {
            // Time + Duration = Time (and Duration + Time)
            {
                const Time t{ 100 };
                const Duration d{ 24 };
                FTK_ASSERT((t + d).frames == 124);
                FTK_ASSERT((d + t).frames == 124);
            }
            // Time - Duration = Time
            {
                const Time t{ 100 };
                const Duration d{ 24 };
                FTK_ASSERT((t - d).frames == 76);
            }
            // Time - Time = Duration
            {
                const Time a{ 100 };
                const Time b{ 60 };
                FTK_ASSERT((a - b).frames == 40);
                FTK_ASSERT((b - a).frames == -40);
            }
            // Duration arithmetic
            {
                const Duration a{ 5 };
                const Duration b{ 3 };
                FTK_ASSERT((a + b).frames == 8);
                FTK_ASSERT((a - b).frames == 2);
                FTK_ASSERT((-a).frames == -5);
                FTK_ASSERT((a * 4).frames == 20);
                FTK_ASSERT((4 * a).frames == 20);
            }
            // Compound assignment
            {
                Time t{ 10 };
                t += Duration{ 5 };
                FTK_ASSERT(t.frames == 15);
                t -= Duration{ 3 };
                FTK_ASSERT(t.frames == 12);
            }
            {
                Duration d{ 10 };
                d += Duration{ 5 };
                FTK_ASSERT(d.frames == 15);
                d -= Duration{ 3 };
                FTK_ASSERT(d.frames == 12);
            }
        }

        void TimeTest::_comparison()
        {
            {
                const Time a{ 10 };
                const Time b{ 10 };
                const Time c{ 20 };
                FTK_ASSERT(a == b);
                FTK_ASSERT(!(a != b));
                FTK_ASSERT(a != c);
                FTK_ASSERT(a < c);
                FTK_ASSERT(a <= c);
                FTK_ASSERT(a <= b);
                FTK_ASSERT(c > a);
                FTK_ASSERT(c >= a);
                FTK_ASSERT(b >= a);
            }
            {
                const Duration a{ 10 };
                const Duration b{ 10 };
                const Duration c{ 20 };
                FTK_ASSERT(a == b);
                FTK_ASSERT(a != c);
                FTK_ASSERT(a < c);
                FTK_ASSERT(c > a);
            }
        }

        void TimeTest::_mediaRate()
        {
            // Default-constructed.
            {
                const MediaRate r;
                FTK_ASSERT(r.num == 0);
                FTK_ASSERT(r.den == 0);
                FTK_ASSERT(!r.isValid());
            }
            // Equality and inequality.
            {
                const MediaRate a{ 24, 1 };
                const MediaRate b{ 24, 1 };
                const MediaRate c{ 30, 1 };
                FTK_ASSERT(a == b);
                FTK_ASSERT(a != c);
            }
            // toDouble for integer rates.
            {
                FTK_ASSERT(getCommonRate(CommonRate::_24).toDouble() == 24.0);
                FTK_ASSERT(getCommonRate(CommonRate::_25).toDouble() == 25.0);
                FTK_ASSERT(getCommonRate(CommonRate::_30).toDouble() == 30.0);
                FTK_ASSERT(getCommonRate(CommonRate::_48).toDouble() == 48.0);
                FTK_ASSERT(getCommonRate(CommonRate::_50).toDouble() == 50.0);
                FTK_ASSERT(getCommonRate(CommonRate::_60).toDouble() == 60.0);
            }
            // toDouble for NTSC rates (non-integer).
            {
                const double r = getCommonRate(CommonRate::_23_976).toDouble();
                FTK_ASSERT(std::abs(r - 24000.0 / 1001.0) < 1e-9);
            }
            {
                const double r = getCommonRate(CommonRate::_29_97).toDouble();
                FTK_ASSERT(std::abs(r - 30000.0 / 1001.0) < 1e-9);
            }
            {
                const double r = getCommonRate(CommonRate::_59_94).toDouble();
                FTK_ASSERT(std::abs(r - 60000.0 / 1001.0) < 1e-9);
            }
            // Invalid rates.
            {
                FTK_ASSERT(!MediaRate({ 0, 1 }).isValid());
                FTK_ASSERT(!MediaRate({ 24, 0 }).isValid());
                FTK_ASSERT(!MediaRate({ -24, 1 }).isValid());
                // toDouble on degenerate rate returns 0 rather than NaN/inf.
                FTK_ASSERT(MediaRate({ 24, 0 }).toDouble() == 0.0);
            }
        }

        void TimeTest::_mediaTime()
        {
            // Default construction.
            {
                const MediaTime t;
                FTK_ASSERT(t.frames == 0);
                FTK_ASSERT(t.toSeconds() == 0.0);
            }
            // toSeconds at 24fps: 48 frames = 2 seconds.
            {
                const MediaTime t{ 48, getCommonRate(CommonRate::_24) };
                FTK_ASSERT(t.toSeconds() == 2.0);
            }
            // toSeconds at 23.976 (24000/1001): 24000 samples = 1001s.
            {
                const MediaTime t{ 24000, getCommonRate(CommonRate::_23_976) };
                FTK_ASSERT(t.toSeconds() == 1001.0);
            }
            // MediaDuration parallels MediaTime.
            {
                const MediaDuration d{ 90, getCommonRate(CommonRate::_30) };
                FTK_ASSERT(d.toSeconds() == 3.0);
            }
            // Equality requires matching rate.
            {
                const MediaTime a{ 24, getCommonRate(CommonRate::_24) };
                const MediaTime b{ 24, getCommonRate(CommonRate::_24) };
                const MediaTime c{ 24, getCommonRate(CommonRate::_30) };
                FTK_ASSERT(a == b);
                FTK_ASSERT(a != c);
            }
        }

        void TimeTest::_rescale()
        {
            // Same rate: identity.
            {
                const MediaTime in{ 100, getCommonRate(CommonRate::_24) };
                const auto out = rescale(in, getCommonRate(CommonRate::_24));
                FTK_ASSERT(out == in);
            }
            // 24 -> 48: doubles.
            {
                const MediaTime in{ 50, getCommonRate(CommonRate::_24) };
                const auto out = rescale(in, getCommonRate(CommonRate::_48));
                FTK_ASSERT(out.frames == 100);
                FTK_ASSERT(out.rate == getCommonRate(CommonRate::_48));
            }
            // 48 -> 24: halves.
            {
                const MediaTime in{ 100, getCommonRate(CommonRate::_48) };
                const auto out = rescale(in, getCommonRate(CommonRate::_24));
                FTK_ASSERT(out.frames == 50);
            }
            // MediaDuration rescale parallels MediaTime.
            {
                const MediaDuration in{ 30, getCommonRate(CommonRate::_30) };
                const auto out = rescale(in, getCommonRate(CommonRate::_60));
                FTK_ASSERT(out.frames == 60);
            }
            // Cross-NTSC: 24 frames at 23.976 -> 24000 samples at... no, that's
            // not a clean number. Test that the round-trip is at least close.
            {
                const MediaTime in{ 240, getCommonRate(CommonRate::_23_976) };
                const auto roundTripped = rescale(
                    rescale(in, getCommonRate(CommonRate::_30)),
                    getCommonRate(CommonRate::_23_976));
                // Allow a 1-frame slop from rounding.
                FTK_ASSERT(std::abs(roundTripped.frames - in.frames) <= 1);
            }
            // Invalid rate fallback: target rate stored, value left zero.
            {
                const MediaTime in{ 100, getCommonRate(CommonRate::_24) };
                const auto out = rescale(in, MediaRate{ 0, 0 });
                FTK_ASSERT(out.rate.num == 0);
                FTK_ASSERT(out.rate.den == 0);
            }
        }

        void TimeTest::_serialize()
        {
            // to_string round-trip is *not* required (no parser), but we
            // exercise the formatter on each type to make sure it doesn't
            // crash and produces non-empty output.
            {
                const std::string s = to_string(Time{ 42 });
                FTK_ASSERT(!s.empty());
            }
            {
                const std::string s = to_string(Duration{ -7 });
                FTK_ASSERT(!s.empty());
            }
            {
                const std::string s = to_string(getCommonRate(CommonRate::_23_976));
                FTK_ASSERT(!s.empty());
            }
            {
                const std::string s = to_string(
                    MediaTime{ 100, getCommonRate(CommonRate::_24) });
                FTK_ASSERT(!s.empty());
            }
            {
                const std::string s = to_string(
                    MediaDuration{ 100, getCommonRate(CommonRate::_24) });
                FTK_ASSERT(!s.empty());
            }
            // Stream operators should match to_string output.
            {
                const Time t{ 42 };
                std::stringstream ss;
                ss << t;
                FTK_ASSERT(ss.str() == to_string(t));
            }
            {
                const MediaTime t{ 100, getCommonRate(CommonRate::_24) };
                std::stringstream ss;
                ss << t;
                FTK_ASSERT(ss.str() == to_string(t));
            }
        }
    }
}

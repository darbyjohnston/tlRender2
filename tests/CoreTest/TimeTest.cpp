// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <CoreTest/TimeTest.h>

#include <tl/Core/Time.h>

#include <ftk/Core/Assert.h>

#include <cmath>
#include <sstream>

namespace tl
{
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
                const core::Time t;
                FTK_ASSERT(0 == t.frames);
            }
            {
                const core::Time t{ 42 };
                FTK_ASSERT(42 == t.frames);
            }
            {
                const core::Duration d;
                FTK_ASSERT(0 == d.frames);
            }
            {
                const core::Duration d{ -7 };
                FTK_ASSERT(-7 == d.frames);
            }
        }

        void TimeTest::_arithmetic()
        {
            // Time + Duration = Time (and Duration + Time)
            {
                const core::Time t{ 100 };
                const core::Duration d{ 24 };
                FTK_ASSERT((t + d).frames == 124);
                FTK_ASSERT((d + t).frames == 124);
            }
            // Time - Duration = Time
            {
                const core::Time t{ 100 };
                const core::Duration d{ 24 };
                FTK_ASSERT((t - d).frames == 76);
            }
            // Time - Time = Duration
            {
                const core::Time a{ 100 };
                const core::Time b{ 60 };
                FTK_ASSERT((a - b).frames == 40);
                FTK_ASSERT((b - a).frames == -40);
            }
            // Duration arithmetic
            {
                const core::Duration a{ 5 };
                const core::Duration b{ 3 };
                FTK_ASSERT((a + b).frames == 8);
                FTK_ASSERT((a - b).frames == 2);
                FTK_ASSERT((-a).frames == -5);
                FTK_ASSERT((a * 4).frames == 20);
                FTK_ASSERT((4 * a).frames == 20);
            }
            // Compound assignment
            {
                core::Time t{ 10 };
                t += core::Duration{ 5 };
                FTK_ASSERT(t.frames == 15);
                t -= core::Duration{ 3 };
                FTK_ASSERT(t.frames == 12);
            }
            {
                core::Duration d{ 10 };
                d += core::Duration{ 5 };
                FTK_ASSERT(d.frames == 15);
                d -= core::Duration{ 3 };
                FTK_ASSERT(d.frames == 12);
            }
        }

        void TimeTest::_comparison()
        {
            {
                const core::Time a{ 10 };
                const core::Time b{ 10 };
                const core::Time c{ 20 };
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
                const core::Duration a{ 10 };
                const core::Duration b{ 10 };
                const core::Duration c{ 20 };
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
                const core::MediaRate r;
                FTK_ASSERT(r.num == 1);
                FTK_ASSERT(r.den == 24);
                FTK_ASSERT(r.isValid());
            }
            // Equality and inequality.
            {
                const core::MediaRate a{ 24, 1 };
                const core::MediaRate b{ 24, 1 };
                const core::MediaRate c{ 30, 1 };
                FTK_ASSERT(a == b);
                FTK_ASSERT(a != c);
            }
            // toDouble for integer rates.
            {
                FTK_ASSERT(core::mediaRate24().toDouble() == 24.0);
                FTK_ASSERT(core::mediaRate25().toDouble() == 25.0);
                FTK_ASSERT(core::mediaRate30().toDouble() == 30.0);
                FTK_ASSERT(core::mediaRate48().toDouble() == 48.0);
                FTK_ASSERT(core::mediaRate50().toDouble() == 50.0);
                FTK_ASSERT(core::mediaRate60().toDouble() == 60.0);
            }
            // toDouble for NTSC rates (non-integer).
            {
                const double r = core::mediaRate23_976().toDouble();
                FTK_ASSERT(std::abs(r - 24000.0 / 1001.0) < 1e-9);
            }
            {
                const double r = core::mediaRate29_97().toDouble();
                FTK_ASSERT(std::abs(r - 30000.0 / 1001.0) < 1e-9);
            }
            {
                const double r = core::mediaRate59_94().toDouble();
                FTK_ASSERT(std::abs(r - 60000.0 / 1001.0) < 1e-9);
            }
            // Invalid rates.
            {
                FTK_ASSERT(!core::MediaRate({ 0, 1 }).isValid());
                FTK_ASSERT(!core::MediaRate({ 24, 0 }).isValid());
                FTK_ASSERT(!core::MediaRate({ -24, 1 }).isValid());
                // toDouble on degenerate rate returns 0 rather than NaN/inf.
                FTK_ASSERT(core::MediaRate({ 24, 0 }).toDouble() == 0.0);
            }
        }

        void TimeTest::_mediaTime()
        {
            // Default construction.
            {
                const core::MediaTime t;
                FTK_ASSERT(t.frames == 0);
                FTK_ASSERT(t.toSeconds() == 0.0);
            }
            // toSeconds at 24fps: 48 frames = 2 seconds.
            {
                const core::MediaTime t{ 48, core::mediaRate24() };
                FTK_ASSERT(t.toSeconds() == 2.0);
            }
            // toSeconds at 23.976 (24000/1001): 24000 samples = 1001s.
            {
                const core::MediaTime t{ 24000, core::mediaRate23_976() };
                FTK_ASSERT(t.toSeconds() == 1001.0);
            }
            // MediaDuration parallels MediaTime.
            {
                const core::MediaDuration d{ 90, core::mediaRate30() };
                FTK_ASSERT(d.toSeconds() == 3.0);
            }
            // Equality requires matching rate.
            {
                const core::MediaTime a{ 24, core::mediaRate24() };
                const core::MediaTime b{ 24, core::mediaRate24() };
                const core::MediaTime c{ 24, core::mediaRate30() };
                FTK_ASSERT(a == b);
                FTK_ASSERT(a != c);
            }
        }

        void TimeTest::_rescale()
        {
            // Same rate: identity.
            {
                const core::MediaTime in{ 100, core::mediaRate24() };
                const auto out = core::rescale(in, core::mediaRate24());
                FTK_ASSERT(out == in);
            }
            // 24 -> 48: doubles.
            {
                const core::MediaTime in{ 50, core::mediaRate24() };
                const auto out = core::rescale(in, core::mediaRate48());
                FTK_ASSERT(out.frames == 100);
                FTK_ASSERT(out.rate == core::mediaRate48());
            }
            // 48 -> 24: halves.
            {
                const core::MediaTime in{ 100, core::mediaRate48() };
                const auto out = core::rescale(in, core::mediaRate24());
                FTK_ASSERT(out.frames == 50);
            }
            // MediaDuration rescale parallels MediaTime.
            {
                const core::MediaDuration in{ 30, core::mediaRate30() };
                const auto out = core::rescale(in, core::mediaRate60());
                FTK_ASSERT(out.frames == 60);
            }
            // Cross-NTSC: 24 frames at 23.976 -> 24000 samples at... no, that's
            // not a clean number. Test that the round-trip is at least close.
            {
                const core::MediaTime in{ 240, core::mediaRate23_976() };
                const auto roundTripped = core::rescale(
                    core::rescale(in, core::mediaRate30()),
                    core::mediaRate23_976());
                // Allow a 1-frame slop from rounding.
                FTK_ASSERT(std::abs(roundTripped.frames - in.frames) <= 1);
            }
            // Invalid rate fallback: target rate stored, value left zero.
            {
                const core::MediaTime in{ 100, core::mediaRate24() };
                const auto out = core::rescale(in, core::MediaRate{ 0, 0 });
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
                const std::string s = core::to_string(core::Time{ 42 });
                FTK_ASSERT(!s.empty());
            }
            {
                const std::string s = core::to_string(core::Duration{ -7 });
                FTK_ASSERT(!s.empty());
            }
            {
                const std::string s = core::to_string(core::mediaRate23_976());
                FTK_ASSERT(!s.empty());
            }
            {
                const std::string s = core::to_string(
                    core::MediaTime{ 100, core::mediaRate24() });
                FTK_ASSERT(!s.empty());
            }
            {
                const std::string s = core::to_string(
                    core::MediaDuration{ 100, core::mediaRate24() });
                FTK_ASSERT(!s.empty());
            }
            // Stream operators should match to_string output.
            {
                const core::Time t{ 42 };
                std::stringstream ss;
                ss << t;
                FTK_ASSERT(ss.str() == core::to_string(t));
            }
            {
                const core::MediaTime t{ 100, core::mediaRate24() };
                std::stringstream ss;
                ss << t;
                FTK_ASSERT(ss.str() == core::to_string(t));
            }
        }
    }
}

// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/Timeline/Timeline.h>

#include <opentimelineio/timeline.h>

namespace tl
{
    using namespace core;

    namespace timeline
    {
        namespace
        {
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

        struct Timeline::Private
        {
            std::shared_ptr<ftk::Observable<Time> > startTime;
            std::shared_ptr<ftk::Observable<Duration> > duration;
        };

        void Timeline::_init(const std::string&)
        {
            FTK_P();
            p.startTime = ftk::Observable<Time>::create();
            p.duration = ftk::Observable<Duration>::create();
        }

        Timeline::Timeline() :
            _p(new Private)
        {}

        Timeline::~Timeline()
        {}

        std::shared_ptr<Timeline> Timeline::create()
        {
            auto out = std::shared_ptr<Timeline>(new Timeline);
            out->_init(std::string());
            return out;
        }

        std::shared_ptr<Timeline> Timeline::create(const std::string& fileName)
        {
            auto out = std::shared_ptr<Timeline>(new Timeline);
            out->_init(fileName);
            return out;
        }
        
        const Time& Timeline::getStartTime() const
        {
            return _p->startTime->get();
        }

        std::shared_ptr<ftk::IObservable<Time> > Timeline::observeStartTime() const
        {
            return _p->startTime;
        }

        const Duration& Timeline::getDuration() const
        {
            return _p->duration->get();
        }

        std::shared_ptr<ftk::IObservable<Duration> > Timeline::observeDuration() const
        {
            return _p->duration;
        }

        std::shared_ptr<IVideoNode> Timeline::getVideo(const Time&)
        {
            return nullptr;
        }

        std::future<std::shared_ptr<Audio> > Timeline::getAudio(int64_t seconds)
        {
            return std::future<std::shared_ptr<Audio> >();
        }
    }
}

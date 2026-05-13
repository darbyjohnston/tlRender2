// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/UI/TimeUnitsModel.h>

#include <ftk/Core/Error.h>
#include <ftk/Core/Format.h>
#include <ftk/Core/String.h>

#include <opentime/rationalTime.h>

#include <cstdlib>

namespace otio = opentime::OPENTIME_VERSION_NS;

namespace tl
{
    using namespace core;

    namespace ui
    {
        TL_ENUM_IMPL(
            TimeUnits,
            "Frames",
            "Seconds",
            "Timecode");

        std::string timeToText(
            const Time& time,
            const core::MediaRate& rate,
            TimeUnits units)
        {
            std::string out;
            switch (units)
            {
            case TimeUnits::Frames:
                out = ftk::Format("{0}").
                    arg(rate.isValid() ? time.frames : 0);
                break;
            case TimeUnits::Seconds:
                out = ftk::Format("{0}").
                    arg(rate.isValid() ? mediaTime(time, rate).toSeconds() : 0.0, 2);
                break;
            case TimeUnits::Timecode:
            {
                if (rate.isValid())
                {
                    const otio::RationalTime otioTime(
                        time.frames,
                        rate.toDouble());
                    out = otioTime.to_timecode();
                }
                if (out.empty())
                {
                    out = "--:--:--:--";
                }
                break;
            }
            default: break;
            }
            return out;
        }

        Time textToTime(
            const std::string& text,
            const core::MediaRate& rate,
            TimeUnits units)
        {
            Time out;
            switch (units)
            {
            case TimeUnits::Frames:
            {
                const int value = std::atoi(text.c_str());
                const auto otioTime = otio::RationalTime::from_frames(value, rate.toDouble());
                out.frames = otioTime.value();
                break;
            }
            case TimeUnits::Seconds:
            {
                const double value = std::atof(text.c_str());
                const auto otioTime = otio::RationalTime::from_seconds(value).rescaled_to(rate.toDouble());
                out.frames = otioTime.value();
                break;
            }
            case TimeUnits::Timecode:
            {
                const auto otioTime = otio::RationalTime::from_timecode(text, rate.toDouble());
                out.frames = otioTime.value();
                break;
            }
            default: break;
            }
            return out;
        }

        std::string formatString(TimeUnits units)
        {
            std::string out;
            switch (units)
            {
            case TimeUnits::Frames:
                out = "000000";
                break;
            case TimeUnits::Seconds:
                out = "000000.00";
                break;
            case TimeUnits::Timecode:
                out = "00:00:00;00";
                break;
            default: break;
            }
            return out;
        }

        std::string validator(TimeUnits units)
        {
            std::string out;
            switch (units)
            {
            case TimeUnits::Frames:
                out = "[0-9]*";
                break;
            case TimeUnits::Seconds:
                out = "[0-9]*\\.[0-9]+|[0-9]+";
                break;
            case TimeUnits::Timecode:
                out = "[0-9][0-9]:[0-9][0-9]:[0-9][0-9]:[0-9][0-9]";
                break;
            default: break;
            }
            return out;
        }

        struct TimeUnitsModel::Private
        {
            std::shared_ptr<ftk::Observable<TimeUnits> > timeUnits;
        };

        void TimeUnitsModel::_init(const std::shared_ptr<ftk::Context>& context)
        {
            FTK_P();
            p.timeUnits = ftk::Observable<TimeUnits>::create(TimeUnits::Timecode);
        }

        TimeUnitsModel::TimeUnitsModel() :
            _p(new Private)
        {}

        TimeUnitsModel::~TimeUnitsModel()
        {}

        std::shared_ptr<TimeUnitsModel> TimeUnitsModel::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            auto out = std::shared_ptr<TimeUnitsModel>(new TimeUnitsModel);
            out->_init(context);
            return out;
        }

        TimeUnits TimeUnitsModel::getTimeUnits() const
        {
            return _p->timeUnits->get();
        }

        std::shared_ptr<ftk::IObservable<TimeUnits> > TimeUnitsModel::observeTimeUnits() const
        {
            return _p->timeUnits;
        }

        void TimeUnitsModel::setTimeUnits(TimeUnits value)
        {
            _p->timeUnits->setIfChanged(value);
        }

        std::string TimeUnitsModel::getLabel(const Time& value, const MediaRate& rate) const
        {
            return timeToText(value, rate, _p->timeUnits->get());
        }
    }
}


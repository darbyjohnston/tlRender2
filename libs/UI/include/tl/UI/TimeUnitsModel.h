// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <tl/Core/Time.h>
#include <tl/Core/Util.h>

#include <ftk/Core/Context.h>
#include <ftk/Core/Observable.h>

namespace tl
{
    namespace ui
    {
        //! Time units.
        enum class TL_API_TYPE TimeUnits
        {
            Frames,
            Seconds,
            Timecode,

            Count,
            First = Frames
        };
        TL_ENUM(TimeUnits);

        //! Convert a time value to text.
        TL_API std::string timeToText(
            const core::Time&,
            const core::MediaRate&,
            TimeUnits);

        //! Convert text to a time value.
        TL_API core::Time textToTime(
            const std::string&     text,
            const core::MediaRate& rate,
            TimeUnits              units);

        //! Get a time units format string.
        TL_API std::string formatString(TimeUnits);

        //! Get a time units validator regular expression.
        TL_API std::string validator(TimeUnits);

        //! Time units model.
        class TL_API_TYPE TimeUnitsModel : public std::enable_shared_from_this<TimeUnitsModel>
        {
            FTK_NON_COPYABLE(TimeUnitsModel);

        protected:
            void _init(const std::shared_ptr<ftk::Context>&);

            TimeUnitsModel();

        public:
            virtual ~TimeUnitsModel();

            //! Create a new model.
            static std::shared_ptr<TimeUnitsModel> create(
                const std::shared_ptr<ftk::Context>&);

            //! Get the time units.
            TimeUnits getTimeUnits() const;

            //! Observe the time units.
            std::shared_ptr<ftk::IObservable<TimeUnits> > observeTimeUnits() const;
                
            //! Set the time units.
            void setTimeUnits(TimeUnits);

            //! Get a time label in the current time units.
            std::string getLabel(const core::Time&, const core::MediaRate&) const;

        private:
            FTK_PRIVATE();
        };
    }
}


// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <tl/UI/TimeUnitsModel.h>
#include <tl/Core/Time.h>

#include <ftk/UI/IWidget.h>

namespace tl
{
    namespace ui
    {
        //! Time label.
        class TL_API_TYPE TimeLabel : public ftk::IWidget
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<TimeUnitsModel>&,
                const std::shared_ptr<ftk::IWidget>& parent = nullptr);

            TimeLabel();

        public:
            virtual ~TimeLabel();

            //! Create a new widget.
            static std::shared_ptr<TimeLabel> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<TimeUnitsModel>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            //! Get the time units model.
            const std::shared_ptr<TimeUnitsModel>& getTimeUnitsModel() const;

            //! Get the time rate.
            const core::MediaRate& getRate() const;

            //! Set the time rate.
            void setRate(const core::MediaRate&);

            //! Get the time value.
            const core::Time& getValue() const;

            //! Set the time value.
            void setValue(const core::Time&);

            //! Set the font.
            void setFont(ftk::FontType);

            ftk::Size2I getSizeHint() const override;
            void setGeometry(const ftk::Box2I&) override;

        private:
            void _textUpdate();

            FTK_PRIVATE();
        };
    }
}

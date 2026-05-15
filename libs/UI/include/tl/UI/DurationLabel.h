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
        //! Duration label.
        class TL_API_TYPE DurationLabel : public ftk::IWidget
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<TimeUnitsModel>&,
                const std::shared_ptr<ftk::IWidget>& parent = nullptr);

            DurationLabel();

        public:
            virtual ~DurationLabel();

            //! Create a new widget.
            static std::shared_ptr<DurationLabel> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<TimeUnitsModel>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            //! Get the time units model.
            const std::shared_ptr<TimeUnitsModel>& getTimeUnitsModel() const;

            //! Get the duration rate.
            const core::MediaRate& getRate() const;

            //! Set the duration rate.
            void setRate(const core::MediaRate&);

            //! Get the duration value.
            const core::Duration& getValue() const;

            //! Set the duration value.
            void setValue(const core::Duration&);

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

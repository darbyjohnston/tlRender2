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
        //! Time edit.
        class TL_API_TYPE TimeEdit : public ftk::IWidget
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<TimeUnitsModel>&,
                const std::shared_ptr<ftk::IWidget>& parent = nullptr);

            TimeEdit();

        public:
            virtual ~TimeEdit();

            //! Create a new widget.
            static std::shared_ptr<TimeEdit> create(
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

            //! Set the time value callback.
            void setCallback(const std::function<void(const core::Time&)>&);

            //! Select all.
            void selectAll();

            //! Set the font.
            void setFont(ftk::FontType);

            ftk::Size2I getSizeHint() const override;
            void setGeometry(const ftk::Box2I&) override;
            void takeKeyFocus() override;
            void keyPressEvent(ftk::KeyEvent&) override;
            void keyReleaseEvent(ftk::KeyEvent&) override;

        private:
            void _commitValue(const std::string&);
            void _commitValue(const core::Time&);
            void _textUpdate();

            FTK_PRIVATE();
        };
    }
}

// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/UI/DurationEdit.h>

#include <ftk/UI/IncButtons.h>
#include <ftk/UI/LineEdit.h>
#include <ftk/UI/LineEditModel.h>
#include <ftk/UI/RowLayout.h>

namespace tl
{
    using namespace core;

    namespace ui
    {
        struct DurationEdit::Private
        {
            std::shared_ptr<TimeUnitsModel> timeUnitsModel;
            MediaRate rate;
            Duration value;
            std::function<void(const Duration&)> callback;
            
            std::shared_ptr<ftk::LineEdit> lineEdit;
            std::shared_ptr<ftk::IncButtons> incButtons;
            std::shared_ptr<ftk::HorizontalLayout> layout;

            std::shared_ptr<ftk::Observer<TimeUnits>> timeUnitsObserver;
        };

        void DurationEdit::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<TimeUnitsModel>& timeUnitsModel,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IWidget::_init(context, "tl::ui::DurationEdit", parent);
            FTK_P();

            p.timeUnitsModel = timeUnitsModel;
            
            p.lineEdit = ftk::LineEdit::create(context);
            p.lineEdit->getModel()->setRegex("[0-9\\-\\.\\,\\:]+");
            p.lineEdit->setFont(ftk::FontType::Mono);
            p.lineEdit->setHStretch(ftk::Stretch::Expanding);

            p.incButtons = ftk::IncButtons::create(context);

            p.layout = ftk::HorizontalLayout::create(context, shared_from_this());
            p.layout->setSpacingRole(ftk::SizeRole::SpacingTool);
            p.lineEdit->setParent(p.layout);
            p.incButtons->setParent(p.layout);

            p.lineEdit->setCallback(
                [this](const std::string& value)
                {
                    _commitValue(value);
                });
            p.lineEdit->setFocusCallback(
                [this](bool value)
                {
                    if (!value)
                    {
                        _textUpdate();
                    }
                });

            p.incButtons->setIncCallback(
                [this]
                {
                    _commitValue(_p->value + Duration{ 1 });
                });
            p.incButtons->setDecCallback(
                [this]
                {
                    _commitValue(_p->value - Duration{ 1 });
                });

            p.timeUnitsObserver = ftk::Observer<TimeUnits>::create(
                p.timeUnitsModel->observeTimeUnits(),
                [this](TimeUnits)
                {
                    _textUpdate();
                });
        }

        DurationEdit::DurationEdit() :
            _p(new Private)
        {}

        DurationEdit::~DurationEdit()
        {}

        std::shared_ptr<DurationEdit> DurationEdit::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<TimeUnitsModel>& timeUnitsModel,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<DurationEdit>(new DurationEdit);
            out->_init(context, timeUnitsModel, parent);
            return out;
        }

        const std::shared_ptr<TimeUnitsModel>& DurationEdit::getTimeUnitsModel() const
        {
            return _p->timeUnitsModel;
        }

        const core::MediaRate& DurationEdit::getRate() const
        {
            return _p->rate;
        }

        void DurationEdit::setRate(const core::MediaRate& value)
        {
            FTK_P();
            if (value == p.rate)
                return;
            p.rate = value;
            _textUpdate();
        }

        const Duration& DurationEdit::getValue() const
        {
            return _p->value;
        }

        void DurationEdit::setValue(const Duration& value)
        {
            FTK_P();
            if (value == p.value)
                return;
            p.value = value;
            _textUpdate();
        }

        void DurationEdit::setCallback(const std::function<void(const Duration&)>& value)
        {
            _p->callback = value;
        }

        void DurationEdit::selectAll()
        {
            _p->lineEdit->selectAll();
        }

        void DurationEdit::setFont(ftk::FontType value)
        {
            _p->lineEdit->setFont(value);
        }
        
        ftk::Size2I DurationEdit::getSizeHint() const
        {
            return _p->layout->getSizeHint();
        }

        void DurationEdit::setGeometry(const ftk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }

        void DurationEdit::takeKeyFocus()
        {
            _p->lineEdit->takeKeyFocus();
        }

        void DurationEdit::keyPressEvent(ftk::KeyEvent& event)
        {
            FTK_P();
            if (isEnabled() && 0 == event.modifiers)
            {
                switch (event.key)
                {
                case ftk::Key::Up:
                    event.accept = true;
                    _commitValue(p.value + Duration{ 1 });
                    break;
                case ftk::Key::Down:
                    event.accept = true;
                    _commitValue(p.value - Duration{ 1 });
                    break;
                case ftk::Key::PageUp:
                    event.accept = true;
                    //! \bug How do we add one second?
                    _commitValue(p.value + Duration{ static_cast<Frame>(p.rate.toDouble()) });
                    break;
                case ftk::Key::PageDown:
                    event.accept = true;
                    //! \bug How do we subtract one second?
                    _commitValue(p.value - Duration{ static_cast<Frame>(p.rate.toDouble()) });
                    break;
                default: break;
                }
            }
        }

        void DurationEdit::keyReleaseEvent(ftk::KeyEvent& event)
        {
            event.accept = true;
        }

        void DurationEdit::_commitValue(const std::string& value)
        {
            FTK_P();
            const auto tmp = textToFrames(
                value,
                p.rate,
                p.timeUnitsModel->getTimeUnits());
            if (tmp.has_value())
            {
                p.value.frames = tmp.value();
            }
            _textUpdate();
            if (tmp.has_value() && p.callback)
            {
                p.callback(p.value);
            }
        }

        void DurationEdit::_commitValue(const Duration& value)
        {
            FTK_P();
            p.value = value;
            _textUpdate();
            if (p.callback)
            {
                p.callback(p.value);
            }
        }

        void DurationEdit::_textUpdate()
        {
            FTK_P();
            std::string text;
            std::string format;
            if (p.timeUnitsModel)
            {
                const TimeUnits timeUnits = p.timeUnitsModel->getTimeUnits();
                text = framesToText(p.value.frames, p.rate, timeUnits);
                format = formatString(timeUnits);
            }
            p.lineEdit->setText(text);
            p.lineEdit->setFormat(format);
        }
    }
}

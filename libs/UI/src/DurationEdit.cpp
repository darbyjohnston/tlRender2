// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/UI/DurationLabel.h>

#include <ftk/UI/Label.h>

namespace tl
{
    using namespace core;

    namespace ui
    {
        struct DurationLabel::Private
        {
            std::shared_ptr<TimeUnitsModel> timeUnitsModel;
            MediaRate rate;
            Duration value;
            
            std::shared_ptr<ftk::Label> label;

            std::shared_ptr<ftk::Observer<TimeUnits>> timeUnitsObserver;
        };

        void DurationLabel::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<TimeUnitsModel>& timeUnitsModel,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IWidget::_init(context, "tl::ui::DurationLabel", parent);
            FTK_P();

            p.timeUnitsModel = timeUnitsModel;
            
            p.label = ftk::Label::create(context, shared_from_this());
            p.label->setFont(ftk::FontType::Mono);

            p.timeUnitsObserver = ftk::Observer<TimeUnits>::create(
                p.timeUnitsModel->observeTimeUnits(),
                [this](TimeUnits)
                {
                    _textUpdate();
                });
        }

        DurationLabel::DurationLabel() :
            _p(new Private)
        {}

        DurationLabel::~DurationLabel()
        {}

        std::shared_ptr<DurationLabel> DurationLabel::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<TimeUnitsModel>& timeUnitsModel,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<DurationLabel>(new DurationLabel);
            out->_init(context, timeUnitsModel, parent);
            return out;
        }

        const std::shared_ptr<TimeUnitsModel>& DurationLabel::getTimeUnitsModel() const
        {
            return _p->timeUnitsModel;
        }

        const Duration& DurationLabel::getValue() const
        {
            return _p->value;
        }

        void DurationLabel::setValue(const Duration& value)
        {
            FTK_P();
            if (value == p.value)
                return;
            p.value = value;
            _textUpdate();
        }

        void DurationLabel::setFont(ftk::FontType value)
        {
            _p->label->setFont(value);
        }
        
        ftk::Size2I DurationLabel::getSizeHint() const
        {
            return _p->label->getSizeHint();
        }

        void DurationLabel::_textUpdate()
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
            p.label->setText(text);
        }
    }
}

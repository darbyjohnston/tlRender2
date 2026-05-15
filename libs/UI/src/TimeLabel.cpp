// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/UI/TimeLabel.h>

#include <ftk/UI/Label.h>

namespace tl
{
    using namespace core;

    namespace ui
    {
        struct TimeLabel::Private
        {
            std::shared_ptr<TimeUnitsModel> timeUnitsModel;
            MediaRate rate;
            Time value;
            
            std::shared_ptr<ftk::Label> label;

            std::shared_ptr<ftk::Observer<TimeUnits>> timeUnitsObserver;
        };

        void TimeLabel::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<TimeUnitsModel>& timeUnitsModel,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            IWidget::_init(context, "tl::ui::TimeLabel", parent);
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

        TimeLabel::TimeLabel() :
            _p(new Private)
        {}

        TimeLabel::~TimeLabel()
        {}

        std::shared_ptr<TimeLabel> TimeLabel::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<TimeUnitsModel>& timeUnitsModel,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<TimeLabel>(new TimeLabel);
            out->_init(context, timeUnitsModel, parent);
            return out;
        }

        const std::shared_ptr<TimeUnitsModel>& TimeLabel::getTimeUnitsModel() const
        {
            return _p->timeUnitsModel;
        }

        const Time& TimeLabel::getValue() const
        {
            return _p->value;
        }

        void TimeLabel::setValue(const Time& value)
        {
            FTK_P();
            if (value == p.value)
                return;
            p.value = value;
            _textUpdate();
        }

        void TimeLabel::setFont(ftk::FontType value)
        {
            _p->label->setFont(value);
        }
        
        ftk::Size2I TimeLabel::getSizeHint() const
        {
            return _p->label->getSizeHint();
        }

        void TimeLabel::_textUpdate()
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

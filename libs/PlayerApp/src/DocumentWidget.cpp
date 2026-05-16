// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/PlayerApp/DocumentWidget.h>

#include <tl/PlayerApp/App.h>
#include <tl/UI/Viewport.h>
#include <tl/UI/TimelineWidget.h>

#include <ftk/UI/Divider.h>
#include <ftk/UI/RowLayout.h>

namespace tl
{
    namespace player_app
    {
        struct DocumentWidget::Private
        {
            std::shared_ptr<render::Session> session;

            std::shared_ptr<ui::Viewport> viewport;
            std::shared_ptr<ui::TimelineWidget> timelineWidget;
            std::shared_ptr<ftk::VerticalLayout> layout;

            std::shared_ptr<ftk::Observer<std::shared_ptr<ftk::Image>>> videoFrameObserver;
        };

        void DocumentWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<render::Session>& session,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            ftk::IWidget::_init(context, "tl::player_app::DocumentWidget", parent);
            FTK_P();

            setVStretch(ftk::Stretch::Expanding);

            p.session = session;

            p.viewport = ui::Viewport::create(context);
            p.viewport->setVStretch(ftk::Stretch::Expanding);

            p.timelineWidget = ui::TimelineWidget::create(
                context,
                app->getTimeUnitsModel(),
                session->getPlayer());

            p.layout = ftk::VerticalLayout::create(context, shared_from_this());
            p.layout->setSpacingRole(ftk::SizeRole::None);
            p.viewport->setParent(p.layout);
            p.timelineWidget->setParent(p.layout);

            p.videoFrameObserver = ftk::Observer<std::shared_ptr<ftk::Image>>::create(
                app->observeVideoFrame(),
                [this](const std::shared_ptr<ftk::Image>& value)
                {
                    _p->viewport->setVideoFrame(value);
                });
        }

        DocumentWidget::DocumentWidget() :
            _p(new Private)
        {}

        DocumentWidget::~DocumentWidget()
        {}

        std::shared_ptr<DocumentWidget> DocumentWidget::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<render::Session>& session,
            const std::shared_ptr<ftk::IWidget>& parent)
        {
            auto out = std::shared_ptr<DocumentWidget>(new DocumentWidget);
            out->_init(context, app, session, parent);
            return out;
        }

        ftk::Size2I DocumentWidget::getSizeHint() const
        {
            return _p->layout->getSizeHint();
        }

        void DocumentWidget::setGeometry(const ftk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }
    }
}

// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/PlayerApp/DocumentWidget.h>

#include <tl/PlayerApp/BottomToolBar.h>

#include <tl/PlayerApp/App.h>
#include <tl/UI/Viewport.h>
#include <tl/UI/TimelineWidget.h>

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
            std::shared_ptr<BottomToolBar> bottomToolBar;
            std::shared_ptr<ftk::VerticalLayout> layout;

            std::shared_ptr<ftk::Observer<std::shared_ptr<ftk::Image>>> videoFrameObserver;
        };

        void DocumentWidget::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<render::Session>& session)
        {
            ftk::IWidget::_init(context, "tl::player_app::DocumentWidget", nullptr);
            FTK_P();

            p.session = session;

            p.viewport = ui::Viewport::create(context);
            p.viewport->setVStretch(ftk::Stretch::Expanding);

            p.timelineWidget = ui::TimelineWidget::create(
                context,
                app->getTimeUnitsModel(),
                session->getPlayer());

            p.bottomToolBar = BottomToolBar::create(context, app, session);

            p.layout = ftk::VerticalLayout::create(context, shared_from_this());
            p.layout->setSpacingRole(ftk::SizeRole::None);
            p.viewport->setParent(p.layout);
            p.timelineWidget->setParent(p.layout);
            p.bottomToolBar->setParent(p.layout);

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
            const std::shared_ptr<render::Session>& session)
        {
            auto out = std::shared_ptr<DocumentWidget>(new DocumentWidget);
            out->_init(context, app, session);
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

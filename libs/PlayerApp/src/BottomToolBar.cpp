// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/PlayerApp/BottomToolBar.h>

#include <tl/PlayerApp/App.h>
#include <tl/UI/PlaybackButtons.h>
#include <tl/UI/FrameButtons.h>

#include <ftk/UI/RowLayout.h>

namespace tl
{
    namespace player_app
    {
        struct BottomToolBar::Private
        {
            std::shared_ptr<render::Session> session;

            std::shared_ptr<ui::PlaybackButtons> playbackButtons;
            std::shared_ptr<ui::FrameButtons> frameButtons;
            std::shared_ptr<ftk::HorizontalLayout> layout;
        };

        void BottomToolBar::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<render::Session>& session)
        {
            ftk::IWidget::_init(context, "tl::player_app::BottomToolBar", nullptr);
            FTK_P();

            p.session = session;

            p.playbackButtons = ui::PlaybackButtons::create(context);

            p.frameButtons = ui::FrameButtons::create(context);

            p.layout = ftk::HorizontalLayout::create(context, shared_from_this());
            p.layout->setSpacingRole(ftk::SizeRole::SpacingTool);
            p.playbackButtons->setParent(p.layout);
            p.frameButtons->setParent(p.layout);
        }

        BottomToolBar::BottomToolBar() :
            _p(new Private)
        {}

        BottomToolBar::~BottomToolBar()
        {}

        std::shared_ptr<BottomToolBar> BottomToolBar::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<App>& app,
            const std::shared_ptr<render::Session>& session)
        {
            auto out = std::shared_ptr<BottomToolBar>(new BottomToolBar);
            out->_init(context, app, session);
            return out;
        }

        ftk::Size2I BottomToolBar::getSizeHint() const
        {
            return _p->layout->getSizeHint();
        }

        void BottomToolBar::setGeometry(const ftk::Box2I& value)
        {
            IWidget::setGeometry(value);
            _p->layout->setGeometry(value);
        }
    }
}

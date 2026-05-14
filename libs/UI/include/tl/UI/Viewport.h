// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <tl/Core/Export.h>

#include <ftk/UI/IWidget.h>
#include <ftk/GL/Texture.h>

namespace tl
{
    namespace ui
    {
        //! Timeline viewport.
        class TL_API_TYPE Viewport : public ftk::IWidget
        {
            FTK_NON_COPYABLE(Viewport);

        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<IWidget>& parent);

            Viewport();

        public:
            virtual ~Viewport();

            //! Create a new widget.
            static std::shared_ptr<Viewport> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            //! \name Image Options
            ///@{

            const ftk::ImageOptions& getImageOptions() const;
            std::shared_ptr<ftk::IObservable<ftk::ImageOptions> > observeImageOptions() const;
            void setImageOptions(const ftk::ImageOptions&);

            ///@}

            //! \name Color Buffer Type
            ///@{

            ftk::gl::TextureType getColorBuffer() const;
            std::shared_ptr<ftk::IObservable<ftk::gl::TextureType> > observeColorBuffer() const;
            void setColorBuffer(ftk::gl::TextureType);

            ///@}

            //! \name Video Frame
            ///@{

            const std::shared_ptr<ftk::Image>& getVideoFrame() const;
            virtual void setVideoFrame(const std::shared_ptr<ftk::Image>&);

            ///@}

            //! \name View
            ///@{

            //! Get the view position.
            const ftk::V2I& getViewPos() const;

            //! Observe the view position.
            std::shared_ptr<ftk::IObservable<ftk::V2I> > observeViewPos() const;

            //! Get the view zoom.
            double getZoom() const;

            //! Observe the view zoom.
            std::shared_ptr<ftk::IObservable<double> > observeZoom() const;

            //! Get the view position and zoom.
            std::pair<ftk::V2I, double> getViewPosAndZoom() const;

            //! Observe the view position and zoom.
            std::shared_ptr<ftk::IObservable<std::pair<ftk::V2I, double> > > observeViewPosAndZoom() const;

            //! Set the view position and zoom.
            void setViewPosAndZoom(const ftk::V2I&, double);

            //! Set the view zoom.
            void setZoom(double, const ftk::V2I& focus = ftk::V2I());

            //! Get the view zoom range.
            const ftk::RangeD& getZoomRange() const;

            //! Set the view zoom range.
            void setZoomRange(const ftk::RangeD&);

            //! Get whether the view is framed automatically.
            bool hasFrameView() const;

            //! Observe whether the view is framed automatically.
            std::shared_ptr<ftk::IObservable<bool> > observeFrameView() const;

            //! Observe when the view is framed.
            std::shared_ptr<ftk::IObservable<bool> > observeFramed() const;

            //! Set whether the view is framed automatically.
            void setFrameView(bool);

            //! Reset the view zoom to 1:1.
            void resetZoom();

            //! Zoom the view in.
            void zoomIn();

            //! Zoom the view out.
            void zoomOut();

            ///@}

            //! \name Color Sample
            ///@{

            //! Sample a color from the viewport.
            ftk::Color4F getColorSample(const ftk::V2I&);

            ///@}

            //! \name Input
            ///@{

            //! Get whether input is enabled.
            bool isInputEnabled() const;

            //! Set whether input is enabled.
            void setInputEnabled(bool);

            //! Set the pan binding.
            void setPanBinding(ftk::MouseButton, ftk::KeyModifier);

            //! Set the mouse wheel scale.
            void setMouseWheelScale(float);

            ///@}

            ftk::Size2I getSizeHint() const override;
            void setGeometry(const ftk::Box2I&) override;
            void sizeHintEvent(const ftk::SizeHintEvent&) override;
            void drawEvent(const ftk::Box2I&, const ftk::DrawEvent&) override;
            void mouseEnterEvent(ftk::MouseEnterEvent&) override;
            void mouseLeaveEvent() override;
            void mouseMoveEvent(ftk::MouseMoveEvent&) override;
            void mousePressEvent(ftk::MouseClickEvent&) override;
            void mouseReleaseEvent(ftk::MouseClickEvent&) override;
            void scrollEvent(ftk::ScrollEvent&) override;
            void keyPressEvent(ftk::KeyEvent&) override;
            void keyReleaseEvent(ftk::KeyEvent&) override;

        protected:
            bool _isMouseInside() const;
            const ftk::V2I& _getMousePressPos() const;

        private:
            ftk::Size2I _getRenderSize() const;
            ftk::V2I _getViewportCenter() const;
            void _frameView();

            FTK_PRIVATE();
        };
    }
}

// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/UI/Viewport.h>

#include <ftk/UI/DrawUtil.h>
#include <ftk/GL/GL.h>
#include <ftk/GL/OffscreenBuffer.h>
#include <ftk/GL/Util.h>
#include <ftk/Core/Context.h>
#include <ftk/Core/LogSystem.h>
#include <ftk/Core/RenderUtil.h>

#include <cmath>

namespace tl
{
    namespace ui
    {
        struct Viewport::Private
        {
            std::shared_ptr<ftk::Observable<ftk::ImageOptions> > imageOptions;
            std::shared_ptr<ftk::Observable<ftk::gl::TextureType> > colorBuffer;
            std::shared_ptr<timeline::Player> player;
            std::shared_ptr<ftk::Image> videoFrame;
            std::shared_ptr<ftk::Observable<ftk::V2I> > viewPos;
            std::shared_ptr<ftk::Observable<double> > zoom;
            ftk::RangeD zoomRange = ftk::RangeD(0.01, 100.0);
            std::shared_ptr<ftk::Observable<std::pair<ftk::V2I, double> > > viewPosZoom;
            std::shared_ptr<ftk::Observable<bool> > frameView;
            std::shared_ptr<ftk::Observable<bool> > framed;
            bool inputEnabled = true;
            std::pair<ftk::MouseButton, ftk::KeyModifier> panBinding =
                std::make_pair(ftk::MouseButton::Middle, ftk::KeyModifier::None);
            float mouseWheelScale = 1.1F;

            bool doRender = false;
            std::shared_ptr<ftk::gl::OffscreenBuffer> buffer;
            std::shared_ptr<ftk::gl::OffscreenBuffer> bgBuffer;
            std::shared_ptr<ftk::gl::OffscreenBuffer> fgBuffer;

            struct SizeData
            {
                float displayScale = 1.F;
                int sizeHint = 0;
            };
            SizeData size;

            enum class MouseMode
            {
                None,
                View
            };
            struct MouseData
            {
                bool inside = false;
                ftk::V2I pos;
                ftk::V2I press;
                MouseMode mode = MouseMode::None;
                ftk::V2I viewPos;
            };
            MouseData mouse;

            std::shared_ptr<ftk::Observer<std::shared_ptr<ftk::Image>>> videoFrameObserver;
        };

        void Viewport::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<IWidget>& parent)
        {
            IWidget::_init(context, "tl::ui::Viewport", parent);
            FTK_P();

            setHStretch(ftk::Stretch::Expanding);
            setVStretch(ftk::Stretch::Expanding);

            p.imageOptions = ftk::Observable<ftk::ImageOptions>::create();
            p.colorBuffer = ftk::Observable<ftk::gl::TextureType>::create(
                ftk::gl::TextureType::RGBA_F32);
            p.viewPos = ftk::Observable<ftk::V2I>::create();
            p.zoom = ftk::Observable<double>::create(1.0);
            p.viewPosZoom = ftk::Observable<std::pair<ftk::V2I, double> >::create(
                std::make_pair(ftk::V2I(), 1.0));
            p.frameView = ftk::Observable<bool>::create(true);
            p.framed = ftk::Observable<bool>::create(false);
        }

        Viewport::Viewport() :
            _p(new Private)
        {}

        Viewport::~Viewport()
        {}

        std::shared_ptr<Viewport> Viewport::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::shared_ptr<IWidget>& parent)
        {
            auto out = std::shared_ptr<Viewport>(new Viewport);
            out->_init(context, parent);
            return out;
        }

        const ftk::ImageOptions& Viewport::getImageOptions() const
        {
            return _p->imageOptions->get();
        }

        std::shared_ptr<ftk::IObservable<ftk::ImageOptions> > Viewport::observeImageOptions() const
        {
            return _p->imageOptions;
        }

        void Viewport::setImageOptions(const ftk::ImageOptions& value)
        {
            FTK_P();
            if (p.imageOptions->setIfChanged(value))
            {
                p.doRender = true;
                setDrawUpdate();
            }
        }

        ftk::gl::TextureType Viewport::getColorBuffer() const
        {
            return _p->colorBuffer->get();
        }

        std::shared_ptr<ftk::IObservable<ftk::gl::TextureType> > Viewport::observeColorBuffer() const
        {
            return _p->colorBuffer;
        }

        void Viewport::setColorBuffer(ftk::gl::TextureType value)
        {
            FTK_P();
            if (p.colorBuffer->setIfChanged(value))
            {
                p.doRender = true;
                setDrawUpdate();
            }
        }

        const std::shared_ptr<timeline::Player>& Viewport::getPlayer() const
        {
            return _p->player;
        }

        void Viewport::setPlayer(const std::shared_ptr<timeline::Player>& value)
        {
            FTK_P();

            p.videoFrameObserver.reset();

            p.player = value;

            if (p.player)
            {
                /*p.videoFrameObserver = ftk::ListObserver<VideoFrame>::create(
                    p.player->observeCurrentVideo(),
                    [this](const std::vector<VideoFrame>& value)
                    {
                        FTK_P();
                        p.videoFrame = value;

                        if (p.fpsData.has_value())
                        {
                            p.fpsData->frameCount = p.fpsData->frameCount + 1;
                            const auto now = std::chrono::steady_clock::now();
                            const std::chrono::duration<double> diff = now - p.fpsData->timer;
                            if (diff.count() > 1.0)
                            {
                                const double fps = p.fpsData->frameCount / diff.count();
                                //std::cout << "FPS: " << fps << std::endl;
                                p.fps->setIfChanged(fps);
                                p.fpsData->timer = now;
                                p.fpsData->frameCount = 0;
                            }
                        }

                        p.doRender = true;
                        setDrawUpdate();
                    });*/
            }
            else if (p.videoFrame)
            {
                p.videoFrame.reset();
                p.doRender = true;
                setDrawUpdate();
            }
        }

        const ftk::V2I& Viewport::getViewPos() const
        {
            return _p->viewPos->get();
        }

        std::shared_ptr<ftk::IObservable<ftk::V2I> > Viewport::observeViewPos() const
        {
            return _p->viewPos;
        }

        double Viewport::getZoom() const
        {
            return _p->zoom->get();
        }

        std::shared_ptr<ftk::IObservable<double> > Viewport::observeZoom() const
        {
            return _p->zoom;
        }

        std::pair<ftk::V2I, double> Viewport::getViewPosAndZoom() const
        {
            return _p->viewPosZoom->get();
        }

        std::shared_ptr<ftk::IObservable<std::pair<ftk::V2I, double> > > Viewport::observeViewPosAndZoom() const
        {
            return _p->viewPosZoom;
        }

        void Viewport::setViewPosAndZoom(const ftk::V2I& pos, double zoom)
        {
            FTK_P();
            const double zoomClamped = ftk::clamp(zoom, p.zoomRange.min(), p.zoomRange.max());
            const std::pair<ftk::V2I, double> pair(pos, zoomClamped);
            if (pair != p.viewPosZoom->get())
            {
                setFrameView(false);
            }
            if (p.viewPosZoom->setIfChanged(pair))
            {
                p.viewPos->setIfChanged(pos);
                p.zoom->setIfChanged(zoomClamped);
                p.doRender = true;
                setDrawUpdate();
            }
        }

        void Viewport::setZoom(double zoom, const ftk::V2I& focus)
        {
            FTK_P();
            ftk::V2I pos;
            const ftk::V2I& viewPos = p.viewPos->get();
            const double zoomPrev = p.zoom->get();
            const double zoomClamped = ftk::clamp(zoom, p.zoomRange.min(), p.zoomRange.max());
            pos.x = focus.x + (viewPos.x - focus.x) * (zoomClamped / zoomPrev);
            pos.y = focus.y + (viewPos.y - focus.y) * (zoomClamped / zoomPrev);
            setViewPosAndZoom(pos, zoomClamped);
        }

        bool Viewport::hasFrameView() const
        {
            return _p->frameView->get();
        }

        std::shared_ptr<ftk::IObservable<bool> > Viewport::observeFrameView() const
        {
            return _p->frameView;
        }

        std::shared_ptr<ftk::IObservable<bool> > Viewport::observeFramed() const
        {
            return _p->framed;
        }

        void Viewport::setFrameView(bool value)
        {
            FTK_P();
            if (p.frameView->setIfChanged(value))
            {
                if (value)
                {
                    p.framed->setAlways(true);
                }
                p.doRender = true;
                setDrawUpdate();
            }
        }

        void Viewport::resetZoom()
        {
            FTK_P();
            setZoom(1.F, _getViewportCenter());
        }

        void Viewport::zoomIn()
        {
            FTK_P();
            setZoom(
                p.zoom->get() * 2.0,
                p.mouse.inside ? p.mouse.pos : _getViewportCenter());
        }

        void Viewport::zoomOut()
        {
            FTK_P();
            setZoom(p.zoom->get() / 2.0, _getViewportCenter());
        }

        const ftk::RangeD& Viewport::getZoomRange() const
        {
            return _p->zoomRange;
        }

        void Viewport::setZoomRange(const ftk::RangeD& value)
        {
            FTK_P();
            p.zoomRange = value;
            setZoom(p.zoom->get());
        }

        ftk::Color4F Viewport::getColorSample(const ftk::V2I& value)
        {
            FTK_P();
            ftk::Color4F out;
            if (p.buffer)
            {
                const ftk::Box2I& g = getGeometry();
                std::vector<float> sample(4);
                ftk::gl::OffscreenBufferBinding binding(p.buffer);
                glPixelStorei(GL_PACK_ALIGNMENT, 1);
#if defined(FTK_API_GL_4_1)
                glClampColor(GL_CLAMP_READ_COLOR, GL_FALSE);
#endif // FTK_API_GL_4_1
                glReadPixels(
                    value.x,
                    g.h() - 1 - value.y,
                    1,
                    1,
                    GL_RGBA,
                    GL_FLOAT,
                    sample.data());
                out.r = std::isnan(sample[0]) || std::isinf(sample[0]) ? 0.F : sample[0];
                out.g = std::isnan(sample[1]) || std::isinf(sample[1]) ? 0.F : sample[1];
                out.b = std::isnan(sample[2]) || std::isinf(sample[2]) ? 0.F : sample[2];
                out.a = std::isnan(sample[3]) || std::isinf(sample[3]) ? 0.F : sample[3];
            }
            return out;
        }

        bool Viewport::isInputEnabled() const
        {
            return _p->inputEnabled;
        }

        void Viewport::setInputEnabled(bool value)
        {
            _p->inputEnabled = value;
        }

        void Viewport::setPanBinding(ftk::MouseButton button, ftk::KeyModifier modifier)
        {
            _p->panBinding = std::make_pair(button, modifier);
        }

        void Viewport::setMouseWheelScale(float value)
        {
            _p->mouseWheelScale = value;
        }
        
        ftk::Size2I Viewport::getSizeHint() const
        {
            FTK_P();
            return ftk::Size2I(p.size.sizeHint, p.size.sizeHint);
        }

        void Viewport::setGeometry(const ftk::Box2I& value)
        {
            const bool changed = value != getGeometry();
            IWidget::setGeometry(value);
            FTK_P();
            if (changed)
            {
                p.doRender = true;
            }
        }

        void Viewport::sizeHintEvent(const ftk::SizeHintEvent& event)
        {
            FTK_P();
            p.size.displayScale = event.displayScale;
            p.size.sizeHint = event.style->getSizeRole(ftk::SizeRole::ScrollArea, event.displayScale);
        }

        void Viewport::drawEvent(const ftk::Box2I& drawRect, const ftk::DrawEvent& event)
        {
            IWidget::drawEvent(drawRect, event);
            FTK_P();

            if (p.frameView->get())
            {
                _frameView();
            }

            const ftk::Box2I& g = getGeometry();
            event.render->drawRect(g, ftk::Color4F(0.F, 0.F, 0.F));

            if (p.doRender)
            {
                p.doRender = false;
                try
                {
                    // Create the background and foreground buffers.
                    const ftk::Size2I size = g.size();
                    ftk::gl::OffscreenBufferOptions offscreenBufferOptions;
                    offscreenBufferOptions.colorFilters.minify = ftk::ImageFilter::Nearest;
                    offscreenBufferOptions.colorFilters.magnify = ftk::ImageFilter::Nearest;
                    if (ftk::gl::doCreate(
                        p.bgBuffer,
                        size,
                        ftk::gl::TextureType::RGBA_U8,
                        offscreenBufferOptions))
                    {
                        p.bgBuffer = ftk::gl::OffscreenBuffer::create(
                            size,
                            ftk::gl::TextureType::RGBA_U8,
                            offscreenBufferOptions);
                    }
                    if (ftk::gl::doCreate(
                        p.fgBuffer,
                        size,
                        ftk::gl::TextureType::RGBA_U8,
                        offscreenBufferOptions))
                    {
                        p.fgBuffer = ftk::gl::OffscreenBuffer::create(
                            size,
                            ftk::gl::TextureType::RGBA_U8,
                            offscreenBufferOptions);
                    }

                    // Create the main buffer.
                    offscreenBufferOptions.colorFilters.minify = ftk::ImageFilter::Linear;
                    offscreenBufferOptions.colorFilters.magnify = ftk::ImageFilter::Linear;
#if defined(FTK_API_GL_4_1)
                    offscreenBufferOptions.depth = ftk::gl::OffscreenDepth::_24;
                    offscreenBufferOptions.stencil = ftk::gl::OffscreenStencil::_8;
#elif defined(FTK_API_GLES_2)
                    offscreenBufferOptions.stencil = ftk::gl::OffscreenStencil::_8;
#endif // FTK_API_GL_4_1
                    if (ftk::gl::doCreate(
                        p.buffer,
                        size,
                        p.colorBuffer->get(),
                        offscreenBufferOptions))
                    {
                        p.buffer = ftk::gl::OffscreenBuffer::create(
                            size,
                            p.colorBuffer->get(),
                            offscreenBufferOptions);
                    }

                    // Setup the transforms.
                    const auto pm = ftk::ortho(
                        0.F,
                        static_cast<float>(g.w()),
                        static_cast<float>(g.h()),
                        0.F,
                        -1.F,
                        1.F);
                    const ftk::V2I& viewPos = p.viewPos->get();
                    const double zoom = p.zoom->get();
                    const ftk::M44F vm =
                        ftk::translate(ftk::V3F(viewPos.x, viewPos.y, 0.F)) *
                        ftk::scale(ftk::V3F(zoom, zoom, 1.F));

                    // Setup the state.
                    const ftk::ViewportState viewportState(event.render);
                    const ftk::ClipRectEnabledState clipRectEnabledState(event.render);
                    const ftk::ClipRectState clipRectState(event.render);
                    const ftk::TransformState transformState(event.render);
                    const ftk::RenderSizeState renderSizeState(event.render);
                    event.render->setRenderSize(size);
                    event.render->setViewport(ftk::Box2I(0, 0, g.w(), g.h()));
                    event.render->setClipRectEnabled(false);

                    // Draw the main buffer.
                    if (p.buffer && p.videoFrame)
                    {
                        ftk::gl::OffscreenBufferBinding binding(p.buffer);
                        event.render->clearViewport(ftk::Color4F(0.F, 0.F, 0.F, 0.F));
                        event.render->setTransform(pm * vm);
                        event.render->drawImage(
                            p.videoFrame,
                            ftk::Box2I(0, 0, p.videoFrame->getWidth(), p.videoFrame->getHeight()),
                            ftk::Color4F(1.F, 1.F, 1.F),
                            p.imageOptions->get());
                    }

                    // Draw the background buffer.
                    if (p.bgBuffer)
                    {
                        ftk::gl::OffscreenBufferBinding binding(p.bgBuffer);
                        event.render->clearViewport(ftk::Color4F(0.F, 0.F, 0.F, 0.F));
                        event.render->setTransform(pm);
                    }

                    // Draw the foreground buffer.
                    if (p.fgBuffer)
                    {
                        ftk::gl::OffscreenBufferBinding binding(p.fgBuffer);
                        event.render->clearViewport(ftk::Color4F(0.F, 0.F, 0.F, 0.F));
                        event.render->setTransform(pm);
                    }
                }
                catch (const std::exception& e)
                {
                    if (auto context = getContext())
                    {
                        context->log("tl::ui::Viewport", e.what(), ftk::LogType::Error);
                    }
                }
            }

            if (p.bgBuffer)
            {
                event.render->drawTexture(p.bgBuffer->getColorID(), g, true);
            }
            if (p.buffer)
            {
                ftk::AlphaBlend alphaBlend = ftk::AlphaBlend::Straight;
                if (p.imageOptions->get().alphaBlend != ftk::AlphaBlend::None)
                {
                    alphaBlend = p.imageOptions->get().alphaBlend;
                }
                event.render->drawTexture(
                    p.buffer->getColorID(),
                    g,
                    true,
                    ftk::Color4F(1.F, 1.F, 1.F),
                    alphaBlend);
            }
            if (p.fgBuffer)
            {
                event.render->drawTexture(p.fgBuffer->getColorID(), g, true);
            }
        }

        void Viewport::mouseEnterEvent(ftk::MouseEnterEvent& event)
        {
            FTK_P();
            if (p.inputEnabled)
            {
                event.accept = true;
                p.mouse.inside = true;
                p.mouse.pos = event.pos - getGeometry().min;
            }
        }

        void Viewport::mouseLeaveEvent()
        {
            FTK_P();
            p.mouse.inside = false;
        }

        void Viewport::mouseMoveEvent(ftk::MouseMoveEvent& event)
        {
            FTK_P();
            if (p.inputEnabled)
            {
                event.accept = true;

                const ftk::Box2I& g = getGeometry();
                p.mouse.pos = event.pos - g.min;

                switch (p.mouse.mode)
                {
                case Private::MouseMode::View:
                {
                    const ftk::V2I viewPos = p.mouse.viewPos + (p.mouse.pos - p.mouse.press);
                    const double zoom = p.zoom->get();
                    const std::pair<ftk::V2I, double> pair(viewPos, zoom);
                    if (pair != p.viewPosZoom->get())
                    {
                        setFrameView(false);
                    }
                    if (p.viewPosZoom->setIfChanged(std::make_pair(viewPos, zoom)))
                    {
                        p.viewPos->setIfChanged(viewPos);
                        p.zoom->setIfChanged(zoom);
                        p.doRender = true;
                        setDrawUpdate();
                    }
                    break;
                }
                default: break;
                }
            }
        }

        void Viewport::mousePressEvent(ftk::MouseClickEvent& event)
        {
            FTK_P();
            if (p.inputEnabled)
            {
                event.accept = true;
                takeKeyFocus();

                const ftk::Box2I& g = getGeometry();
                p.mouse.pos = event.pos - g.min;
                p.mouse.press = p.mouse.pos;

                if (p.panBinding.first == event.button &&
                    ftk::checkKeyModifier(p.panBinding.second, event.modifiers))
                {
                    p.mouse.mode = Private::MouseMode::View;
                    p.mouse.viewPos = p.viewPos->get();
                }
                else
                {
                    p.mouse.mode = Private::MouseMode::None;
                }
            }
        }

        void Viewport::mouseReleaseEvent(ftk::MouseClickEvent& event)
        {
            FTK_P();
            event.accept = true;
            p.mouse.mode = Private::MouseMode::None;
        }

        void Viewport::scrollEvent(ftk::ScrollEvent& event)
        {
            FTK_P();
            if (p.inputEnabled)
            {
                if (static_cast<int>(ftk::KeyModifier::None) == event.modifiers)
                {
                    event.accept = true;

                    const ftk::Box2I& g = getGeometry();
                    p.mouse.pos = event.pos - g.min;

                    const double zoom = p.zoom->get();
                    const double newZoom =
                        event.value.y > 0 ?
                        zoom * p.mouseWheelScale :
                        zoom / p.mouseWheelScale;
                    setZoom(newZoom, p.mouse.pos);
                }
            }
        }

        void Viewport::keyPressEvent(ftk::KeyEvent& event)
        {
            FTK_P();
            if (p.inputEnabled)
            {
                const ftk::Box2I& g = getGeometry();
                p.mouse.pos = event.pos - g.min;

                if (0 == event.modifiers)
                {
                    switch (event.key)
                    {
                    case ftk::Key::_0:
                        event.accept = true;
                        setZoom(1.0, p.mouse.pos);
                        break;

                    case ftk::Key::Equals:
                        event.accept = true;
                        setZoom(p.zoom->get() * 2.0, p.mouse.pos);
                        break;

                    case ftk::Key::Minus:
                        event.accept = true;
                        setZoom(p.zoom->get() / 2.0, p.mouse.pos);
                        break;

                    case ftk::Key::Backspace:
                        event.accept = true;
                        setFrameView(true);
                        break;

                    default: break;
                    }
                }
            }
        }

        void Viewport::keyReleaseEvent(ftk::KeyEvent& event)
        {
            event.accept = true;
        }

        bool Viewport::_isMouseInside() const
        {
            return _p->mouse.inside;
        }

        const ftk::V2I& Viewport::_getMousePressPos() const
        {
            return _p->mouse.press;
        }

        ftk::Size2I Viewport::_getRenderSize() const
        {
            FTK_P();
            return p.videoFrame ? p.videoFrame->getSize() : ftk::Size2I();
        }

        ftk::V2I Viewport::_getViewportCenter() const
        {
            const ftk::Box2I& g = getGeometry();
            return ftk::V2I(g.w() / 2, g.h() / 2);
        }

        void Viewport::_frameView()
        {
            FTK_P();
            ftk::V2I viewPos;
            double zoom = 1.0;
            const ftk::Box2I& g = getGeometry();
            const ftk::Size2I viewportSize = g.size();
            const ftk::Size2I renderSize = _getRenderSize();
            if (renderSize.w > 0 && renderSize.h > 0)
            {
                zoom = viewportSize.w / static_cast<double>(renderSize.w);
                if (zoom * renderSize.h > viewportSize.h)
                {
                    zoom = viewportSize.h / static_cast<double>(renderSize.h);
                }
                const ftk::V2I c(renderSize.w / 2, renderSize.h / 2);
                viewPos = ftk::V2I(
                    viewportSize.w / 2.F - c.x * zoom,
                    viewportSize.h / 2.F - c.y * zoom);
            }
            if (p.viewPosZoom->setIfChanged(std::make_pair(viewPos, zoom)))
            {
                p.viewPos->setIfChanged(viewPos);
                p.zoom->setIfChanged(zoom);
            }
        }
    }
}

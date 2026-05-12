// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/Render/Render.h>

#include "OIIOUtil.h"

#include <tl/IO/IOSystem.h>

#include <ftk/Core/Format.h>

#include <OpenImageIO/imagebufalgo.h>

#include <unordered_map>

namespace tl
{
    namespace render
    {
        struct VideoRenderer::Private
        {
            std::shared_ptr<ftk::Context> context;

            // Reader cache.
            std::unordered_map<timeline::Media*, std::shared_ptr<io::IRead>> readers;

            std::shared_ptr<io::IRead> getReader(
                const std::shared_ptr<timeline::Media>&);

            std::shared_ptr<ftk::Image> renderNode(
                const timeline::VideoNodePtr&);

            std::shared_ptr<ftk::Image> renderRead(
                const timeline::ReadVideo&);
            std::shared_ptr<ftk::Image> renderComposite(
                const std::vector<std::shared_ptr<ftk::Image>>& inputs);
            std::shared_ptr<ftk::Image> renderDissolve(
                const timeline::DissolveVideo&,
                const std::vector<std::shared_ptr<ftk::Image>>& inputs);
            std::shared_ptr<ftk::Image> renderColorTransform(
                const timeline::ColorTransformVideo&,
                const std::vector<std::shared_ptr<ftk::Image>>& inputs);
        };

        VideoRenderer::VideoRenderer(const std::shared_ptr<ftk::Context>& context) :
            _p(new Private)
        {
            FTK_P();
            p.context = context;
        }

        VideoRenderer::~VideoRenderer()
        {}

        std::shared_ptr<VideoRenderer> VideoRenderer::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            return std::shared_ptr<VideoRenderer>(new VideoRenderer(context));
        }

        std::shared_ptr<ftk::Image> VideoRenderer::render(
            const timeline::VideoGraph& graph)
        {
            FTK_P();
            if (!graph.root)
            {
                return nullptr;
            }
            return p.renderNode(graph.root);
        }

        std::shared_ptr<io::IRead> VideoRenderer::Private::getReader(
            const std::shared_ptr<timeline::Media>& media)
        {
            if (!media)
            {
                return nullptr;
            }
            auto it = readers.find(media.get());
            if (it != readers.end())
            {
                return it->second;
            }
            auto readSystem = context->getSystem<io::ReadSystem>();
            auto reader = media->mem.empty() ?
                readSystem->read(media->path) :
                readSystem->read(media->path, media->mem);
            if (reader)
            {
                readers[media.get()] = reader;
            }
            return reader;
        }

        std::shared_ptr<ftk::Image> VideoRenderer::Private::renderNode(
            const timeline::VideoNodePtr& node)
        {
            // Post-order: render inputs before dispatching on the op.
            std::vector<std::shared_ptr<ftk::Image>> inputs;
            inputs.reserve(node->inputs.size());
            for (const auto& child : node->inputs)
            {
                inputs.push_back(renderNode(child));
            }

            return std::visit([&](auto&& op) -> std::shared_ptr<ftk::Image>
            {
                using T = std::decay_t<decltype(op)>;
                if constexpr (std::is_same_v<T, timeline::ReadVideo>)
                {
                    return renderRead(op);
                }
                else if constexpr (std::is_same_v<T, timeline::CompositeVideo>)
                {
                    return renderComposite(inputs);
                }
                else if constexpr (std::is_same_v<T, timeline::DissolveVideo>)
                {
                    return renderDissolve(op, inputs);
                }
                else if constexpr (std::is_same_v<T, timeline::ColorTransformVideo>)
                {
                    return renderColorTransform(op, inputs);
                }
            }, node->op);
        }

        std::shared_ptr<ftk::Image> VideoRenderer::Private::renderRead(
            const timeline::ReadVideo& op)
        {
            auto reader = getReader(op.media);
            if (!reader)
            {
                return nullptr;
            }
            return reader->readVideo(op.sourceTime);
        }

        std::shared_ptr<ftk::Image> VideoRenderer::Private::renderComposite(
            const std::vector<std::shared_ptr<ftk::Image>>& inputs)
        {
            if (inputs.empty())
            {
                return nullptr;
            }
            if (inputs.size() == 1)
            {
                return inputs[0];
            }

            // Wrap inputs as ImageBufs and premultiply alpha (PNG and most
            // file formats store non-premultiplied). over() expects
            // premultiplied input.
            std::vector<OIIO::ImageBuf> premultiplied;
            premultiplied.reserve(inputs.size());
            for (const auto& input : inputs)
            {
                OIIO::ImageBuf wrapped = wrap(*input);
                OIIO::ImageBuf p;
                if (!OIIO::ImageBufAlgo::premult(p, wrapped))
                {
                    throw std::runtime_error(OIIO::geterror());
                }
                premultiplied.push_back(std::move(p));
            }

            // Fold over() from bottom to top. inputs[0] is bottom of stack,
            // inputs.back() is top (matches OTIO stack semantics).
            OIIO::ImageBuf result = std::move(premultiplied[0]);
            for (size_t i = 1; i < premultiplied.size(); ++i)
            {
                OIIO::ImageBuf composited;
                if (!OIIO::ImageBufAlgo::over(
                    composited,
                    premultiplied[i],   // A: top
                    result))            // B: accumulated bottom
                {
                    throw std::runtime_error(OIIO::geterror());
                }
                result = std::move(composited);
            }

            // Unpremultiply for output, since downstream consumers
            // (PNG writers, etc.) expect non-premultiplied alpha.
            OIIO::ImageBuf finalBuf;
            if (!OIIO::ImageBufAlgo::unpremult(finalBuf, result))
            {
                throw std::runtime_error(OIIO::geterror());
            }

            return materialize(finalBuf);
        }

        std::shared_ptr<ftk::Image> VideoRenderer::Private::renderDissolve(
            const timeline::DissolveVideo&,
            const std::vector<std::shared_ptr<ftk::Image>>& inputs)
        {
            throw std::runtime_error("renderDissolve: not implemented");
        }

        std::shared_ptr<ftk::Image> VideoRenderer::Private::renderColorTransform(
            const timeline::ColorTransformVideo&,
            const std::vector<std::shared_ptr<ftk::Image>>& inputs)
        {
            throw std::runtime_error("renderColorTransform: not implemented");
        }
    }
}

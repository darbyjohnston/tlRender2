// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/RenderApp/App.h>

#include <tl/Render/Render.h>

#include <ftk/Core/Context.h>
#include <ftk/Core/Format.h>

namespace tl
{
    namespace render_app
    {
        void App::_init(
            const std::shared_ptr<ftk::Context>& context,
            std::vector<std::string>& argv)
        {
            _cmdLine.input = ftk::CmdLineArg<std::string>::create(
                "input",
                "Input timeline or media file.");

            _cmdLine.output = ftk::CmdLineArg<std::string>::create(
                "output",
                "Output media file.");

            _cmdLine.info = ftk::CmdLineFlag::create(
                { "-info" },
                "Print information.");

            _cmdLine.frame = ftk::CmdLineOption<int>::create(
                { "-frame" },
                "Render the given frame.");

            IApp::_init(
                context,
                argv,
                "tlrender",
                "Render timelines and media.",
                {
                    _cmdLine.input,
                    _cmdLine.output
                },
                {
                    _cmdLine.info,
                    _cmdLine.frame
                });
        }

        App::App()
        {}

        App::~App()
        {}

        std::shared_ptr<App> App::create(
            const std::shared_ptr<ftk::Context>& context,
            std::vector<std::string>& argv)
        {
            auto out = std::shared_ptr<App>(new App);
            out->_init(context, argv);
            return out;
        }

        void App::run()
        {
            try
            {
                ftk::Path inputPath(_cmdLine.input->getValue());
                auto timeline = timeline::Timeline::create(_context, inputPath);
                const auto rate = timeline->getRate();

                ftk::Path outputPath(_cmdLine.output->getValue());
                auto writer = _context->getSystem<io::WriteSystem>()->write(outputPath);

                auto renderer = render::VideoRenderer::create(_context);

                if (_cmdLine.info->found())
                {
                    _printIndented(inputPath.get() + ":", 0);
                    _printIndented(ftk::Format("rate: {0}").
                        arg(core::to_string(rate)),
                        2);
                    const auto& videoInfo = timeline->getVideoInfo();
                    _printIndented(ftk::Format("video: {0}x{1} {2}").
                        arg(videoInfo.size.w).
                        arg(videoInfo.size.h).
                        arg(videoInfo.type),
                        2);
                    const auto& audioInfo = timeline->getAudioInfo();
                    _printIndented(ftk::Format("audio: {0} {1} {2}").
                        arg(audioInfo.channelCount).
                        arg(audioInfo.type).
                        arg(audioInfo.sampleRate),
                        2);
                }

                core::Time startT, endT;
                if (_cmdLine.frame->found())
                {
                    startT.frames = _cmdLine.frame->getValue();
                    endT.frames = startT.frames + 1;
                }
                else
                {
                    startT = timeline->getStartTime();
                    endT = startT + timeline->getDuration();
                }
                for (core::Time t = startT; t < endT; ++t.frames)
                {
                    _printIndented(ftk::Format("frame: {0}").
                        arg(core::to_string(t)),
                        2);
                    
                    if (auto graph = timeline->getVideo(t))
                    {
                        if (_cmdLine.info->found())
                        {
                            _printIndented("graph:", 4);
                            _printVideoNode(graph->root, 6);
                        }
                        
                        if (auto image = renderer->render(*graph))
                        {
                            writer->writeVideo(core::mediaTime(t, rate), image);
                        }
                    }
                }
            }
            catch (const std::exception& e)
            {
                _printError(e.what());
            }
        }

        void App::_printVideoNode(
            const std::shared_ptr<timeline::VideoNode>& node,
            int indent)
        {
            std::visit([this, indent](auto&& op)
            {
                _printIndented(ftk::Format("{0}:").arg(op.typeName), indent);
                _printOpFields(op, indent + 2);
            }, node->op);

            for (const auto& input : node->inputs)
            {
                _printVideoNode(input, indent + 2);
            }
        }

        void App::_printOpFields(const timeline::SolidColorVideo& op, int indent)
        {
            _printIndented(ftk::Format("size: {0}x{1}").
                arg(op.size.w).arg(op.size.h), indent);
            _printIndented(ftk::Format("type: {0}").
                arg(ftk::getLabel(op.type)), indent);
            _printIndented(ftk::Format("color: ({0}, {1}, {2}, {3})").
                arg(op.color[0]).arg(op.color[1]).
                arg(op.color[2]).arg(op.color[3]), indent);
        }

        void App::_printOpFields(const timeline::ReadVideo& op, int indent)
        {
            if (op.media)
            {
                _printIndented(ftk::Format("path: {0}").arg(op.media->path.get()), indent);
            }
            _printIndented(ftk::Format("source time: {0}").arg(core::to_string(op.sourceTime)), indent);
            _printIndented(ftk::Format("reference key: {0}").arg(op.referenceKey), indent);
        }

        void App::_printOpFields(const timeline::CompositeVideo&, int)
        {}

        void App::_printOpFields(const timeline::DissolveVideo& op, int indent)
        {
            _printIndented(ftk::Format("mix: {0}").arg(op.mix), indent);
        }

        void App::_printOpFields(const timeline::ColorTransformVideo& op, int indent)
        {
            _printIndented(ftk::Format("from: {0}").arg(op.fromSpace), indent);
            _printIndented(ftk::Format("to: {0}").arg(op.toSpace), indent);
        }

        void App::_printIndented(const std::string& s, int indent)
        {
            _print(std::string(indent, ' ') + s);
        }
    }
}

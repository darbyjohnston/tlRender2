// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/RenderApp/App.h>

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
                "Input file. Timelines and media files are supported.");

            _cmdLine.print = ftk::CmdLineFlag::create(
                { "-print" },
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
                    _cmdLine.input
                },
                {
                    _cmdLine.print,
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
                ftk::Path path(_cmdLine.input->getValue());
                auto timeline = timeline::Timeline::create(_context, path);

                int indent = 0;
                if (_cmdLine.print->found())
                    _printIndented(path.get() + ":", indent);
                indent += 2;
                if (_cmdLine.print->found())
                    _printIndented(ftk::Format("rate: {0}").
                        arg(core::to_string(timeline->getRate())),
                        indent);

                for (core::Time t = timeline->getStartTime();
                    t < timeline->getStartTime() + timeline->getDuration();
                    ++t.frames)
                {
                    if (_cmdLine.print->found())
                        _printIndented(ftk::Format("frame: {0}").
                            arg(core::to_string(t)),
                            indent);
                    
                    if (auto graph = timeline->getVideo(t))
                    {
                        if (_cmdLine.print->found())
                        {
                            _printIndented("graph:", indent);
                            _printVideoNode(graph->root, indent + 2);
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

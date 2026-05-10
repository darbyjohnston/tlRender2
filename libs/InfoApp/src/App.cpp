// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/InfoApp/App.h>

#include <tl/IO/IOSystem.h>

#include <ftk/Core/Context.h>
#include <ftk/Core/Format.h>

namespace tl
{
    namespace info_app
    {
        void App::_init(
            const std::shared_ptr<ftk::Context>& context,
            std::vector<std::string>& argv)
        {
            _cmdLine.inputs = ftk::CmdLineListArg<std::string>::create(
                "inputs",
                "The input files.");

            IApp::_init(
                context,
                argv,
                "tlinfo",
                "Print information about timelines and media.",
                {
                    _cmdLine.inputs
                },
                {});
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
            auto readSystem = _context->getSystem<io::ReadSystem>();
            const auto& exts = readSystem->getExts();
            for (const auto& input : _cmdLine.inputs->getList())
            {
                ftk::Path path(input);
                const auto i = exts.find(ftk::toLower(path.getExt()));
                if (i != exts.end())
                {
                    if (io::FileType::Seq == i->second)
                    {
                        path = ftk::expandSeq(path);
                    }
                    _print(path.get() + ":");
                    try
                    {
                        auto read = readSystem->read(path);
                        if (!read)
                        {
                            throw std::runtime_error("Unknown file: " + input);
                        }
                        const auto info = read->getInfo();
                        for (const auto& video : info.video)
                        {
                            _print("  video:");
                            _print(ftk::Format("    size: {0}x{1}").
                                arg(video.size.w).
                                arg(video.size.h));
                        }
                    }
                    catch (const std::exception& e)
                    {
                        _printError(e.what());
                    }
                }
                else
                {
                    _print("Unknown file: " + input);
                }
            }
        }
    }
}

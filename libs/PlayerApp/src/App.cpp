// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/PlayerApp/App.h>

#include <tl/PlayerApp/MainWindow.h>

#include <ftk/Core/Context.h>
#include <ftk/Core/Format.h>

namespace tl
{
    namespace player_app
    {
        void App::_init(
            const std::shared_ptr<ftk::Context>& context,
            const std::vector<std::string>& args)
        {
            _cmdLine.input = ftk::CmdLineArg<std::string>::create(
                "input",
                "Input timeline or media file.");

            ftk::App::_init(
                context,
                args,
                "tlplayer",
                "Playback timelines and media.",
                {
                    _cmdLine.input
                });
        }

        App::~App()
        {}

        std::shared_ptr<App> App::create(
            const std::shared_ptr<ftk::Context>& context,
            const std::vector<std::string>& args)
        {
            auto out = std::shared_ptr<App>(new App);
            out->_init(context, args);
            return out;
        }

        void App::run()
        {
            _mainWindow = MainWindow::create(
                _context,
                std::dynamic_pointer_cast<App>(shared_from_this()));

            ftk::App::run();
        }
    }
}

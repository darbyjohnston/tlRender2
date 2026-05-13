// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <tl/Timeline/Timeline.h>

#include <ftk/UI/App.h>

namespace tl
{
    // tlplayer application
    namespace player_app
    {
        class MainWindow;

        //! Command line.
        struct CmdLine
        {
            std::shared_ptr<ftk::CmdLineArg<std::string> > input;
        };

        //! Application.
        class App : public ftk::App
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::vector<std::string>&);

            App() = default;

        public:
            virtual ~App();

            static std::shared_ptr<App> create(
                const std::shared_ptr<ftk::Context>&,
                const std::vector<std::string>&);

            void run() override;

        private:
            CmdLine _cmdLine;
            std::shared_ptr<MainWindow> _mainWindow;
        };
    }
}

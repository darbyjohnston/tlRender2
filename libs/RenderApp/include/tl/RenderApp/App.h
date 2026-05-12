// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <tl/Timeline/Timeline.h>
#include <tl/IO/IOSystem.h>

#include <ftk/Core/CmdLine.h>
#include <ftk/Core/IApp.h>
#include <ftk/Core/Path.h>

namespace tl
{
    // tlrender application
    namespace render_app
    {
        //! Command line.
        struct CmdLine
        {
            std::shared_ptr<ftk::CmdLineArg<std::string> > input;
            std::shared_ptr<ftk::CmdLineFlag> print;
            std::shared_ptr<ftk::CmdLineOption<int>> frame;
        };

        //! Application.
        class App : public ftk::IApp
        {
            FTK_NON_COPYABLE(App);

        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                std::vector<std::string>&);
            App();

        public:
            ~App();

            //! Create a new application.
            static std::shared_ptr<App> create(
                const std::shared_ptr<ftk::Context>&,
                std::vector<std::string>&);

            //! Run the application.
            void run() override;

        private:
            void _printGraph(
                const std::shared_ptr<timeline::VideoGraph>&,
                int indent);
            void _printVideoNode(
                const std::shared_ptr<timeline::VideoNode>&,
                int indent);
            void _printIndented(const std::string&, int indent);

            CmdLine _cmdLine;
        };
    }
}

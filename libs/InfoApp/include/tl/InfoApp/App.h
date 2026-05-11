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
    // tlinfo application
    namespace info_app
    {
        //! Command line.
        struct CmdLine
        {
            std::shared_ptr<ftk::CmdLineListArg<std::string> > inputs;
            std::shared_ptr<ftk::CmdLineFlag> brief;
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
            // Print info for one media file. Returns false if the extension
            // isn't recognized by any plugin in the ReadSystem.
            bool _printMedia(
                const std::shared_ptr<io::ReadSystem>&,
                const timeline::Media&,
                int indent = 0);

            // Open a timeline and print info for each of its media references.
            void _printTimeline(
                const std::shared_ptr<io::ReadSystem>&,
                const ftk::Path&);

            void _printTrack(
                const std::shared_ptr<io::ReadSystem>&,
                const std::shared_ptr<timeline::Track>&,
                int indent);
            void _printClip(
                const std::shared_ptr<io::ReadSystem>&,
                const std::shared_ptr<timeline::Clip>&,
                int indent);
            void _printIndented(const std::string&, int indent);

            CmdLine _cmdLine;
        };
    }
}

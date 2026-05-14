// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <tl/UI/TimeUnitsModel.h>
#include <tl/Timeline/Timeline.h>
#include <tl/Timeline/Player.h>

#include <ftk/UI/App.h>

namespace tl
{
    // tlplayer application
    namespace player_app
    {
        class MainWindow;

        //! Command line.
        struct TL_API_TYPE CmdLine
        {
            std::shared_ptr<ftk::CmdLineArg<std::string> > input;
        };

        //! Application.
        class TL_API_TYPE App : public ftk::App
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::vector<std::string>&);

            App();

        public:
            virtual ~App();

            static std::shared_ptr<App> create(
                const std::shared_ptr<ftk::Context>&,
                const std::vector<std::string>&);

            //! Get the time units model.
            const std::shared_ptr<ui::TimeUnitsModel>& getTimeUnitsModel() const;
            
            //! Open a file.
            void open(const std::string&);

            //! Get the timeline.
            const std::shared_ptr<timeline::Timeline>& getTimeline() const;

            //! Get the player.
            const std::shared_ptr<timeline::Player>& getPlayer() const;

            //! Observe the video frame.
            std::shared_ptr<ftk::IObservable<std::shared_ptr<ftk::Image>>> observeVideoFrame() const;

            void run() override;

        private:
            void _render(const core::Time&);

            FTK_PRIVATE();
        };
    }
}

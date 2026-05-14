// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <tl/Timeline/Player.h>

#include <ftk/UI/MainWindow.h>

namespace tl
{
    namespace player_app
    {
        class App;

        //! Main window.
        class TL_API_TYPE MainWindow : public ftk::MainWindow
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<App>&);

            MainWindow();

        public:
            virtual ~MainWindow();

            //! Create a new window.
            static std::shared_ptr<MainWindow> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<App>&);

            //! Set the player.
            void setPlayer(const std::shared_ptr<timeline::Player>&);

        private:
            FTK_PRIVATE();
        };
    }
}

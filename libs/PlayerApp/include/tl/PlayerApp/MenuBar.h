// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <tl/Core/Util.h>

#include <ftk/UI/MenuBar.h>

namespace tl
{
    namespace player_app
    {
        class App;
        class FileActions;
        class FrameActions;
        class PlaybackActions;
        class WindowActions;

        //! Bottom tool bar.
        class TL_API_TYPE MenuBar : public ftk::MenuBar
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<FileActions>&,
                const std::shared_ptr<PlaybackActions>&,
                const std::shared_ptr<FrameActions>&,
                const std::shared_ptr<WindowActions>&);

            MenuBar();

        public:
            virtual ~MenuBar();

            //! Create a new menu bar.
            static std::shared_ptr<MenuBar> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<FileActions>&,
                const std::shared_ptr<PlaybackActions>&,
                const std::shared_ptr<FrameActions>&,
                const std::shared_ptr<WindowActions>&);

        private:
            FTK_PRIVATE();
        };
    }
}

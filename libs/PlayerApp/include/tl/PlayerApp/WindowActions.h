// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <tl/Core/Util.h>

#include <ftk/UI/Action.h>

namespace tl
{
    namespace player_app
    {
        class App;
        class MainWindow;

        //! Window actions.
        class TL_API_TYPE WindowActions : public std::enable_shared_from_this<WindowActions>
        {
            FTK_NON_COPYABLE(WindowActions);

        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<MainWindow>&);

            WindowActions();

        public:
            ~WindowActions();

            //! Create new actions.
            static std::shared_ptr<WindowActions> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<App>&,
                const std::shared_ptr<MainWindow>&);

            //! Get the actions.
            const std::map<std::string, std::shared_ptr<ftk::Action> >& getActions() const;

        private:
            FTK_PRIVATE();
        };
    }
}

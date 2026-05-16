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

        //! Frame actions.
        class FrameActions : public std::enable_shared_from_this<FrameActions>
        {
            FTK_NON_COPYABLE(FrameActions);

        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<App>&);

            FrameActions();

        public:
            ~FrameActions();

            //! Create new actions.
            static std::shared_ptr<FrameActions> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<App>&);

            //! Get the actions.
            const std::map<std::string, std::shared_ptr<ftk::Action> >& getActions() const;

        private:
            FTK_PRIVATE();
        };
    }
}

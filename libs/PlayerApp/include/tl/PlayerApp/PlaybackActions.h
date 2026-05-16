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

        //! Playback actions.
        class PlaybackActions : public std::enable_shared_from_this<PlaybackActions>
        {
            FTK_NON_COPYABLE(PlaybackActions);

        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<App>&);

            PlaybackActions();

        public:
            ~PlaybackActions();

            //! Create new actions.
            static std::shared_ptr<PlaybackActions> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<App>&);

            //! Get the actions.
            const std::map<std::string, std::shared_ptr<ftk::Action> >& getActions() const;

        private:
            FTK_PRIVATE();
        };
    }
}

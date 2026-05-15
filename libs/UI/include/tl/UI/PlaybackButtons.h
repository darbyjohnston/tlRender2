// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <tl/Timeline/Player.h>

#include <ftk/UI/IWidget.h>

namespace tl
{
    namespace ui
    {
        //! Playback buttons.
        class TL_API_TYPE PlaybackButtons : public ftk::IWidget
        {
        protected:
            void _init(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<ftk::IWidget>& parent = nullptr);

            PlaybackButtons();

        public:
            virtual ~PlaybackButtons();

            //! Create a new widget.
            static std::shared_ptr<PlaybackButtons> create(
                const std::shared_ptr<ftk::Context>&,
                const std::shared_ptr<IWidget>& parent = nullptr);

            //! Get the value.
            timeline::Playback getValue() const;

            //! Set the value.
            void setValue(timeline::Playback);

            //! Set the callback.
            void setCallback(const std::function<void(timeline::Playback)>&);
            
            ftk::Size2I getSizeHint() const override;
            void setGeometry(const ftk::Box2I&) override;

        private:
            FTK_PRIVATE();
        };
    }
}

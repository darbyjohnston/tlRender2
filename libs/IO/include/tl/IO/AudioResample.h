// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <tl/Core/Audio.h>

namespace tl
{
    namespace io
    {
        //! Resample audio data.
        class TL_API_TYPE AudioResample
        {
            FTK_NON_COPYABLE(AudioResample);

        protected:
            void _init(
                const core::AudioInfo& input,
                const core::AudioInfo& output);

            AudioResample();

        public:
            ~AudioResample();

            //! Create a new resampler.
            static std::shared_ptr<AudioResample> create(
                const core::AudioInfo& input,
                const core::AudioInfo& ouput);

            //! Get the input audio information.
            const core::AudioInfo& getInputInfo() const;

            //! Get the output audio information.
            const core::AudioInfo& getOutputInfo() const;

            //! Resample audio data.
            std::shared_ptr<core::Audio> process(const std::shared_ptr<core::Audio>&);

            //! Flush any remaining data.
            void flush();

        private:
            FTK_PRIVATE();
        };
    }
}


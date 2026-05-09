// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <tl/Core/Audio.h>

namespace tl
{
    namespace core
    {
        //! Resample audio data.
        class TL_API_TYPE AudioResample
        {
            FTK_NON_COPYABLE(AudioResample);

        protected:
            void _init(
                const AudioInfo& input,
                const AudioInfo& output);

            AudioResample();

        public:
            ~AudioResample();

            //! Create a new resampler.
            static std::shared_ptr<AudioResample> create(
                const AudioInfo& input,
                const AudioInfo& ouput);

            //! Get the input audio information.
            const AudioInfo& getInputInfo() const;

            //! Get the output audio information.
            const AudioInfo& getOutputInfo() const;

            //! Resample audio data.
            std::shared_ptr<Audio> process(const std::shared_ptr<Audio>&);

            //! Flush any remaining data.
            void flush();

        private:
            FTK_PRIVATE();
        };
    }
}


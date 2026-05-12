// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <IOTest/AudioResampleTest.h>

#include <tl/IO/AudioResample.h>

#include <ftk/Core/Assert.h>

namespace tl
{
    namespace io_test
    {
        AudioResampleTest::AudioResampleTest(
            const std::shared_ptr<ftk::Context>& context) :
            ITest(context, "tl::io_test::AudioResampleTest")
        {}

        AudioResampleTest::~AudioResampleTest()
        {}

        std::shared_ptr<AudioResampleTest> AudioResampleTest::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            return std::shared_ptr<AudioResampleTest>(new AudioResampleTest(context));
        }

        void AudioResampleTest::run()
        {
            _create();
            _process();
        }

        void AudioResampleTest::_create()
        {
            // Construct from valid input/output infos.
            const core::AudioInfo in(2, core::AudioType::S16, 48000);
            const core::AudioInfo out(2, core::AudioType::F32, 48000);
            auto r = io::AudioResample::create(in, out);
            FTK_ASSERT(r);
            FTK_ASSERT(r->getInputInfo() == in);
            FTK_ASSERT(r->getOutputInfo() == out);
        }

        void AudioResampleTest::_process()
        {
#if defined(TL_ENABLE_FFMPEG)
            // S16 stereo @ 48kHz -> F32 stereo @ 48kHz: same rate so output
            // sample count should match (or be very close to) input.
            const core::AudioInfo in(2, core::AudioType::S16, 48000);
            const core::AudioInfo out(2, core::AudioType::F32, 48000);
            auto resampler = io::AudioResample::create(in, out);

            const size_t sampleCount = 1024;
            auto inAudio = core::Audio::create(in, sampleCount);
            inAudio->zero();
            auto outAudio = resampler->process(inAudio);
            FTK_ASSERT(outAudio);
            FTK_ASSERT(outAudio->getInfo() == out);
            // Same-rate conversion: swresample may delay by a sample or two,
            // so allow a small slack.
            FTK_ASSERT(outAudio->getSampleCount() >= sampleCount - 4);
            FTK_ASSERT(outAudio->getSampleCount() <= sampleCount + 4);

            // Null input -> null output (process tolerates null safely).
            FTK_ASSERT(!resampler->process(nullptr));

            // flush() should not crash on a fresh resampler.
            resampler->flush();
#endif // TL_ENABLE_FFMPEG
        }
    }
}

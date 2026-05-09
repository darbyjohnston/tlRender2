// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <CoreTest/AudioTest.h>

#include <tl/Core/Audio.h>

#include <ftk/Core/Assert.h>
#include <ftk/Core/Format.h>

#include <cstring>
#include <sstream>

namespace tl
{
    namespace core_test
    {
        AudioTest::AudioTest(const std::shared_ptr<ftk::Context>& context) :
            ITest(context, "tl::core_test::AudioTest")
        {}

        AudioTest::~AudioTest()
        {}

        std::shared_ptr<AudioTest> AudioTest::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            return std::shared_ptr<AudioTest>(new AudioTest(context));
        }

        void AudioTest::run()
        {
            _types();
            _info();
            _audio();
            _objectCount();
            _combine();
            _reverse();
            _convert();
            _mix();
            _move();
        }

        void AudioTest::_types()
        {
            // AudioType enum utilities (from TL_ENUM_IMPL).
            // FTK_TEST_ENUM uses unqualified lookup for getXxxEnums(),
            // getLabel(), to_string(), from_string() (in tl::core) and
            // Format (in ftk). Pull both in for this scope.
            {
                using namespace core;
                using ftk::Format;
                FTK_TEST_ENUM(AudioType);
            }

            // Byte counts.
            FTK_ASSERT(core::getByteCount(core::AudioType::None) == 0);
            FTK_ASSERT(core::getByteCount(core::AudioType::S8) == 1);
            FTK_ASSERT(core::getByteCount(core::AudioType::S16) == 2);
            FTK_ASSERT(core::getByteCount(core::AudioType::S32) == 4);
            FTK_ASSERT(core::getByteCount(core::AudioType::F32) == 4);
            FTK_ASSERT(core::getByteCount(core::AudioType::F64) == 8);

            // Integer type lookup by byte count.
            FTK_ASSERT(core::getIntAudioType(1) == core::AudioType::S8);
            FTK_ASSERT(core::getIntAudioType(2) == core::AudioType::S16);
            FTK_ASSERT(core::getIntAudioType(4) == core::AudioType::S32);
            FTK_ASSERT(core::getIntAudioType(0) == core::AudioType::None);
            FTK_ASSERT(core::getIntAudioType(3) == core::AudioType::None);
            FTK_ASSERT(core::getIntAudioType(8) == core::AudioType::None);

            // Float type lookup by byte count.
            FTK_ASSERT(core::getFloatAudioType(4) == core::AudioType::F32);
            FTK_ASSERT(core::getFloatAudioType(8) == core::AudioType::F64);
            FTK_ASSERT(core::getFloatAudioType(2) == core::AudioType::None);
        }

        void AudioTest::_info()
        {
            // Default-constructed AudioInfo is invalid.
            {
                const core::AudioInfo info;
                FTK_ASSERT(!info.isValid());
                FTK_ASSERT(info.channelCount == 0);
                FTK_ASSERT(info.type == core::AudioType::None);
                FTK_ASSERT(info.sampleRate == 0);
            }
            // Three-arg constructor sets fields and gives valid info.
            {
                const core::AudioInfo info(2, core::AudioType::S16, 48000);
                FTK_ASSERT(info.isValid());
                FTK_ASSERT(info.channelCount == 2);
                FTK_ASSERT(info.type == core::AudioType::S16);
                FTK_ASSERT(info.sampleRate == 48000);
                // Per-frame byte count: 2 channels * 2 bytes/sample.
                FTK_ASSERT(info.getByteCount() == 4);
            }
            // Equality.
            {
                const core::AudioInfo a(2, core::AudioType::S16, 48000);
                const core::AudioInfo b(2, core::AudioType::S16, 48000);
                const core::AudioInfo c(1, core::AudioType::S16, 48000);
                FTK_ASSERT(a == b);
                FTK_ASSERT(a != c);
            }
            // Label round-trip just exercises getLabel; both forms.
            {
                const core::AudioInfo info(2, core::AudioType::S16, 48000);
                FTK_ASSERT(!core::getLabel(info, false).empty());
                FTK_ASSERT(!core::getLabel(info, true).empty());
            }
        }

        void AudioTest::_audio()
        {
            const core::AudioInfo info(2, core::AudioType::S16, 48000);
            const size_t sampleCount = 100;
            auto audio = core::Audio::create(info, sampleCount);
            FTK_ASSERT(audio);
            FTK_ASSERT(audio->isValid());
            FTK_ASSERT(audio->getInfo() == info);
            FTK_ASSERT(audio->getChannelCount() == 2);
            FTK_ASSERT(audio->getType() == core::AudioType::S16);
            FTK_ASSERT(audio->getSampleRate() == 48000);
            FTK_ASSERT(audio->getSampleCount() == sampleCount);
            // 100 samples * 2 channels * 2 bytes = 400.
            FTK_ASSERT(audio->getByteCount() == 400);
            FTK_ASSERT(audio->getData() != nullptr);

            // zero() should leave all bytes zero.
            audio->zero();
            const uint8_t* p = audio->getData();
            for (size_t i = 0; i < audio->getByteCount(); ++i)
            {
                FTK_ASSERT(p[i] == 0);
            }
        }

        void AudioTest::_objectCount()
        {
            const size_t before = core::Audio::getObjectCount();
            const size_t bytesBefore = core::Audio::getTotalByteCount();
            {
                const core::AudioInfo info(2, core::AudioType::S16, 48000);
                auto a = core::Audio::create(info, 100);
                auto b = core::Audio::create(info, 100);
                FTK_ASSERT(core::Audio::getObjectCount() == before + 2);
                FTK_ASSERT(core::Audio::getTotalByteCount() == bytesBefore + 800);
            }
            FTK_ASSERT(core::Audio::getObjectCount() == before);
            FTK_ASSERT(core::Audio::getTotalByteCount() == bytesBefore);
        }

        void AudioTest::_combine()
        {
            const core::AudioInfo info(1, core::AudioType::S16, 48000);
            // Three chunks of 10, 20, 30 samples = 60 total.
            auto a = core::Audio::create(info, 10);
            auto b = core::Audio::create(info, 20);
            auto c = core::Audio::create(info, 30);
            // Fill each with a distinguishable byte pattern so we can verify
            // the order is preserved.
            std::memset(a->getData(), 0x11, a->getByteCount());
            std::memset(b->getData(), 0x22, b->getByteCount());
            std::memset(c->getData(), 0x33, c->getByteCount());

            const std::list<std::shared_ptr<core::Audio> > chunks{ a, b, c };
            auto combined = core::combineAudio(chunks);
            FTK_ASSERT(combined);
            FTK_ASSERT(combined->getSampleCount() == 60);

            const uint8_t* p = combined->getData();
            for (size_t i = 0; i < a->getByteCount(); ++i)
            {
                FTK_ASSERT(p[i] == 0x11);
            }
            for (size_t i = 0; i < b->getByteCount(); ++i)
            {
                FTK_ASSERT(p[a->getByteCount() + i] == 0x22);
            }
            for (size_t i = 0; i < c->getByteCount(); ++i)
            {
                FTK_ASSERT(p[a->getByteCount() + b->getByteCount() + i] == 0x33);
            }

            // Empty list -> null.
            FTK_ASSERT(!core::combineAudio({}));

            // Total sample count across a list.
            FTK_ASSERT(core::getSampleCount(chunks) == 60);
            FTK_ASSERT(core::getSampleCount({}) == 0);
        }

        void AudioTest::_reverse()
        {
            const core::AudioInfo info(1, core::AudioType::S16, 48000);
            auto in = core::Audio::create(info, 4);
            int16_t* inP = reinterpret_cast<int16_t*>(in->getData());
            inP[0] = 1; inP[1] = 2; inP[2] = 3; inP[3] = 4;

            auto out = core::reverseAudio(in);
            FTK_ASSERT(out->getSampleCount() == 4);
            const int16_t* outP = reinterpret_cast<const int16_t*>(out->getData());
            FTK_ASSERT(outP[0] == 4);
            FTK_ASSERT(outP[1] == 3);
            FTK_ASSERT(outP[2] == 2);
            FTK_ASSERT(outP[3] == 1);
        }

        void AudioTest::_convert()
        {
            // Same-type convert is a memcpy.
            {
                const core::AudioInfo info(1, core::AudioType::S16, 48000);
                auto in = core::Audio::create(info, 4);
                int16_t* inP = reinterpret_cast<int16_t*>(in->getData());
                inP[0] = 100; inP[1] = 200; inP[2] = 300; inP[3] = 400;
                auto out = core::convertAudio(in, core::AudioType::S16);
                const int16_t* outP = reinterpret_cast<const int16_t*>(out->getData());
                for (int i = 0; i < 4; ++i)
                {
                    FTK_ASSERT(outP[i] == inP[i]);
                }
            }
            // S16 -> F32: full-scale becomes ~1.0.
            {
                const core::AudioInfo info(1, core::AudioType::S16, 48000);
                auto in = core::Audio::create(info, 1);
                reinterpret_cast<int16_t*>(in->getData())[0] =
                    std::numeric_limits<int16_t>::max();
                auto out = core::convertAudio(in, core::AudioType::F32);
                const float v = reinterpret_cast<const float*>(out->getData())[0];
                FTK_ASSERT(std::abs(v - 1.0f) < 1e-4f);
            }
            // S16 -> S32: shifts left by 16 bits.
            {
                const core::AudioInfo info(1, core::AudioType::S16, 48000);
                auto in = core::Audio::create(info, 1);
                reinterpret_cast<int16_t*>(in->getData())[0] = 1;
                auto out = core::convertAudio(in, core::AudioType::S32);
                const int32_t v = reinterpret_cast<const int32_t*>(out->getData())[0];
                FTK_ASSERT(v == 256 * 256);
            }
            // F32 -> S16: 1.0 saturates to max int16.
            {
                const core::AudioInfo info(1, core::AudioType::F32, 48000);
                auto in = core::Audio::create(info, 1);
                reinterpret_cast<float*>(in->getData())[0] = 1.0f;
                auto out = core::convertAudio(in, core::AudioType::S16);
                const int16_t v = reinterpret_cast<const int16_t*>(out->getData())[0];
                FTK_ASSERT(v == std::numeric_limits<int16_t>::max());
            }
        }

        void AudioTest::_mix()
        {
            const core::AudioInfo info(1, core::AudioType::F32, 48000);
            auto a = core::Audio::create(info, 4);
            auto b = core::Audio::create(info, 4);
            float* aP = reinterpret_cast<float*>(a->getData());
            float* bP = reinterpret_cast<float*>(b->getData());
            for (int i = 0; i < 4; ++i)
            {
                aP[i] = 0.25f;
                bP[i] = 0.25f;
            }
            // Mix at full volume: each output is sum of both inputs * 1.0.
            auto out = core::mixAudio({ a, b }, 1.0f);
            FTK_ASSERT(out);
            const float* outP = reinterpret_cast<const float*>(out->getData());
            for (int i = 0; i < 4; ++i)
            {
                FTK_ASSERT(std::abs(outP[i] - 0.5f) < 1e-6f);
            }
            // Mix at half volume: output samples are halved.
            auto outHalf = core::mixAudio({ a, b }, 0.5f);
            const float* outHalfP =
                reinterpret_cast<const float*>(outHalf->getData());
            for (int i = 0; i < 4; ++i)
            {
                FTK_ASSERT(std::abs(outHalfP[i] - 0.25f) < 1e-6f);
            }
            // Empty input -> null.
            FTK_ASSERT(!core::mixAudio({}, 1.0f));
        }

        void AudioTest::_move()
        {
            const core::AudioInfo info(1, core::AudioType::S16, 48000);

            // Three chunks totaling 60 samples; move 60 should drain.
            {
                auto a = core::Audio::create(info, 10);
                auto b = core::Audio::create(info, 20);
                auto c = core::Audio::create(info, 30);
                std::memset(a->getData(), 0x11, a->getByteCount());
                std::memset(b->getData(), 0x22, b->getByteCount());
                std::memset(c->getData(), 0x33, c->getByteCount());
                std::list<std::shared_ptr<core::Audio> > in{ a, b, c };

                std::vector<uint8_t> out(60 * info.getByteCount());
                core::moveAudio(in, out.data(), 60);
                FTK_ASSERT(in.empty());
            }

            // Move only 15 samples from a 30-sample list (10 + 20). The
            // first chunk drains entirely, the second is split.
            {
                auto a = core::Audio::create(info, 10);
                auto b = core::Audio::create(info, 20);
                std::list<std::shared_ptr<core::Audio> > in{ a, b };
                std::vector<uint8_t> out(15 * info.getByteCount());
                core::moveAudio(in, out.data(), 15);
                // Should have a single remaining chunk of 15 samples.
                FTK_ASSERT(in.size() == 1);
                FTK_ASSERT(in.front()->getSampleCount() == 15);
            }
        }
    }
}

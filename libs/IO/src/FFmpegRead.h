// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <tl/IO/Plugin.h>

extern "C"
{
#include <libavformat/avformat.h>
}

namespace tl
{
    namespace io
    {
        //! Get a FFmpeg error label.
        std::string avErrorLabel(int);

        //! Swap a FFmpeg rational.
        AVRational avSwap(AVRational);

        //! FFmpeg reader.
        class FFmpegRead : public IRead
        {
        protected:
            FFmpegRead(
                const ftk::Path&,
                const std::vector<ftk::MemFile>&,
                const ReadOptions&,
                const std::shared_ptr<ftk::LogSystem>&);

        public:
            ~FFmpegRead() override;
            
            static std::shared_ptr<FFmpegRead> create(
                const ftk::Path&,
                const ReadOptions&,
                const std::shared_ptr<ftk::LogSystem>&);
            
            static std::shared_ptr<FFmpegRead> create(
                const ftk::Path&,
                const std::vector<ftk::MemFile>&,
                const ReadOptions&,
                const std::shared_ptr<ftk::LogSystem>&);

            ReadInfo getInfo() override;
            std::shared_ptr<ftk::Image> getVideo(
                const core::MediaTime&,
                const ReadOptions& = ReadOptions()) override;
            std::shared_ptr<core::Audio> getAudio(
                const core::MediaTime&,
                size_t sampleCount,
                const ReadOptions& = ReadOptions()) override;
        
        private:
            FTK_PRIVATE();
        };
        
        //! FFmpeg read plugin.
        class FFmpegReadPlugin : public IReadPlugin
        {
        protected:
            void _init(const std::shared_ptr<ftk::LogSystem>&);

            FFmpegReadPlugin();

        public:
            virtual ~FFmpegReadPlugin();
            
            static std::shared_ptr<FFmpegReadPlugin> create(
                const std::shared_ptr<ftk::LogSystem>&);

            bool canRead(
                const ftk::Path&,
                const ReadOptions& = ReadOptions()) override;
            std::shared_ptr<IRead> read(
                const ftk::Path&,
                const ReadOptions& = ReadOptions()) override;
            std::shared_ptr<IRead> read(
                const ftk::Path&,
                const std::vector<ftk::MemFile>&,
                const ReadOptions& = ReadOptions()) override;

        private:
            FTK_PRIVATE();
        };
    }
}

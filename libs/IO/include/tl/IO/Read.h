// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <tl/Core/Audio.h>
#include <tl/Core/Time.h>

#include <ftk/Core/FileIO.h>
#include <ftk/Core/Image.h>
#include <ftk/Core/LogSystem.h>
#include <ftk/Core/Path.h>

namespace tl
{
    namespace io
    {
        //! File types.
        enum class TL_API_TYPE FileType
        {
            Media,
            Seq,

            Count,
            First = Media
        };
        TL_ENUM(FileType);

        //! Read information.
        struct TL_API_TYPE ReadInfo
        {
            std::vector<ftk::ImageInfo>    video;
            std::optional<core::MediaTime> videoStart;
            core::MediaDuration            videoDuration;
            
            std::vector<core::AudioInfo>   audio;
            std::optional<core::MediaTime> audioStart;
            core::MediaDuration            audioDuration;
            
            ftk::ImageTags                 tags;
        };

        //! Read options.
        struct TL_API_TYPE ReadOptions
        {
            int layer = 0;
        };

        //! Base class for readers.
        class TL_API_TYPE IRead : public std::enable_shared_from_this<IRead>
        {
        protected:
            IRead(
                const ftk::Path&,
                const std::vector<ftk::MemFile>&,
                const ReadOptions&,
                const std::shared_ptr<ftk::LogSystem>&);

        public:
            virtual ~IRead() = 0;

            //! Get information.
            virtual ReadInfo getInfo() = 0;

            //! Get video frames.
            virtual std::shared_ptr<ftk::Image> readVideo(
                const core::MediaTime&,
                const ReadOptions& = ReadOptions()) = 0;

            //! Get audio data.
            virtual std::shared_ptr<core::Audio> readAudio(
                const core::MediaTime&,
                const core::MediaDuration&,
                const ReadOptions& = ReadOptions()) = 0;

        protected:
            ftk::Path _path;
            std::vector<ftk::MemFile> _mem;
            ReadOptions _options;
            std::shared_ptr<ftk::LogSystem> _logSystem;
        };

        //! Base class for read plugins.
        class TL_API_TYPE IReadPlugin : public std::enable_shared_from_this<IReadPlugin>
        {
        protected:
            void _init(
                const std::string& name,
                const std::map<std::string, FileType>& exts,
                const std::shared_ptr<ftk::LogSystem>&);

            IReadPlugin() = default;

        public:
            virtual ~IReadPlugin() = 0;

            //! Get the plugin name.
            const std::string& getName() const;

            //! Get the supported file extensions.
            const std::map<std::string, FileType>& getExts() const;

            //! Check if the plugin can read the given file.
            virtual bool canRead(
                const ftk::Path&,
                const ReadOptions& = ReadOptions()) = 0;
            
            //! Get a reader for the given file.
            virtual std::shared_ptr<IRead> read(
                const ftk::Path&,
                const ReadOptions& = ReadOptions()) = 0;
            
            //! Get a reader for the given file.
            virtual std::shared_ptr<IRead> read(
                const ftk::Path&,
                const std::vector<ftk::MemFile>&,
                const ReadOptions& = ReadOptions()) = 0;
        
        protected:
            std::string _name;
            std::map<std::string, FileType> _exts;
            std::shared_ptr<ftk::LogSystem> _logSystem;
        };
    }
}

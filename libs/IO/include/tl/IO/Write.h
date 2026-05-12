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
        //! Write options.
        struct TL_API_TYPE WriteOptions
        {
        };

        //! Base class for writers.
        class TL_API_TYPE IWrite : public std::enable_shared_from_this<IWrite>
        {
        protected:
            IWrite(
                const ftk::Path&,
                const WriteOptions&,
                const std::shared_ptr<ftk::LogSystem>&);

        public:
            virtual ~IWrite() = 0;

            //! Write video frames.
            virtual void writeVideo(
                const core::MediaTime&,
                const std::shared_ptr<ftk::Image>&,
                const WriteOptions& = WriteOptions()) = 0;

            //! Write audio data.
            virtual void writeAudio(
                const core::MediaTime&,
                const std::shared_ptr<core::Audio>&,
                const WriteOptions& = WriteOptions()) = 0;

        protected:
            ftk::Path _path;
            WriteOptions _options;
            std::shared_ptr<ftk::LogSystem> _logSystem;
        };

        //! Base class for write plugins.
        class TL_API_TYPE IWritePlugin : public std::enable_shared_from_this<IWritePlugin>
        {
        protected:
            void _init(
                const std::string& name,
                const std::vector<std::string>& exts,
                const std::shared_ptr<ftk::LogSystem>&);

            IWritePlugin() = default;

        public:
            virtual ~IWritePlugin() = 0;

            //! Get the plugin name.
            const std::string& getName() const;

            //! Get the supported file extensions.
            const std::vector<std::string>& getExts() const;

            //! Check if the plugin can write the given file.
            virtual bool canWrite(
                const ftk::Path&,
                const WriteOptions& = WriteOptions()) = 0;
            
            //! Get a writer for the given file.
            virtual std::shared_ptr<IWrite> write(
                const ftk::Path&,
                const WriteOptions& = WriteOptions()) = 0;

        protected:
            std::string _name;
            std::vector<std::string> _exts;
            std::shared_ptr<ftk::LogSystem> _logSystem;
        };
    }
}

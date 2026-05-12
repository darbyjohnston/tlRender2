// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <tl/IO/Write.h>

namespace tl
{
    namespace io
    {
        //! OIIO writer.
        class OIIOWrite : public IWrite
        {
        protected:
            OIIOWrite(
                const ftk::Path&,
                const WriteOptions&,
                const std::shared_ptr<ftk::LogSystem>&);

        public:
            ~OIIOWrite() override;
            
            static std::shared_ptr<OIIOWrite> create(
                const ftk::Path&,
                const WriteOptions&,
                const std::shared_ptr<ftk::LogSystem>&);

            void writeVideo(
                const core::MediaTime&,
                const std::shared_ptr<ftk::Image>&,
                const WriteOptions& = WriteOptions()) override;
            void writeAudio(
                const core::MediaTime&,
                const std::shared_ptr<core::Audio>&,
                const WriteOptions& = WriteOptions()) override;
        };

        //! OIIO write plugin.
        class OIIOWritePlugin : public IWritePlugin
        {
            FTK_NON_COPYABLE(OIIOWritePlugin);

        protected:
            void _init(const std::shared_ptr<ftk::LogSystem>&);

            OIIOWritePlugin() = default;

        public:
            virtual ~OIIOWritePlugin();
            
            static std::shared_ptr<OIIOWritePlugin> create(
                const std::shared_ptr<ftk::LogSystem>&);

            bool canWrite(
                const ftk::Path&,
                const WriteOptions& = WriteOptions()) override;
            std::shared_ptr<IWrite> write(
                const ftk::Path&,
                const WriteOptions& = WriteOptions()) override;
        };
    }
}

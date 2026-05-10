// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <tl/IO/Plugin.h>

namespace tl
{
    namespace io
    {
        //! OIIO reader.
        class OIIORead : public IRead
        {
        protected:
            OIIORead(
                const ftk::Path&,
                const std::vector<ftk::MemFile>&,
                const ReadOptions&);

        public:
            ~OIIORead() override;
            
            static std::shared_ptr<OIIORead> create(
                const ftk::Path&,
                const ReadOptions& = ReadOptions());
            
            static std::shared_ptr<OIIORead> create(
                const ftk::Path&,
                const std::vector<ftk::MemFile>&,
                const ReadOptions& = ReadOptions());

            ReadInfo getInfo() override;
            std::shared_ptr<ftk::Image> getVideo(
                const core::MediaTime&,
                const ReadOptions& = ReadOptions()) override;
            std::shared_ptr<core::Audio> getAudio(
                const core::MediaTime&,
                size_t sampleCount,
                const ReadOptions& = ReadOptions()) override;
        };

        //! OIIO read plugin.
        class OIIOReadPlugin : public IReadPlugin
        {
            FTK_NON_COPYABLE(OIIOReadPlugin);

        protected:
            void _init();

            OIIOReadPlugin() = default;

        public:
            virtual ~OIIOReadPlugin();
            
            static std::shared_ptr<OIIOReadPlugin> create();

            bool canRead(
                const ftk::Path&,
                const ReadOptions& = ReadOptions()) override;
            bool canRead(
                const ftk::Path&,
                const std::vector<ftk::MemFile>&,
                const ReadOptions& = ReadOptions()) override;
            std::shared_ptr<IRead> read(
                const ftk::Path&,
                const ReadOptions& = ReadOptions()) override;
            std::shared_ptr<IRead> read(
                const ftk::Path&,
                const std::vector<ftk::MemFile>&,
                const ReadOptions& = ReadOptions()) override;
        };
    }
}

// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <tl/IO/Plugin.h>

namespace tl
{
    namespace io
    {
        class EXRReadPlugin : public IReadPlugin
        {
            FTK_NON_COPYABLE(EXRReadPlugin);

        protected:
            EXRReadPlugin();

        public:
            virtual ~EXRReadPlugin();
            
            static std::shared_ptr<EXRReadPlugin> create();

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

        private:
            FTK_PRIVATE();
        };
    }
}

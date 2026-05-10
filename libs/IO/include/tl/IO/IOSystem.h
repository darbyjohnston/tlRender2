// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <tl/IO/Plugin.h>

#include <ftk/Core/ISystem.h>

namespace tl
{
    namespace io
    {
        //! Read system.
        class TL_API_TYPE ReadSystem : public ftk::ISystem
        {
            FTK_NON_COPYABLE(ReadSystem);

        protected:
            ReadSystem(const std::shared_ptr<ftk::Context>&);

        public:
            virtual ~ReadSystem();

            //! Create a new system.
            static std::shared_ptr<ReadSystem> create(const std::shared_ptr<ftk::Context>&);

            //! Add a plugin.
            void addPlugin(const std::shared_ptr<IReadPlugin>&);

            //! Get the supported file extensions.
            const std::map<std::string, FileType>& getExts() const;

            //! Open a file for reading.
            std::shared_ptr<IRead> read(const ftk::Path&, const ReadOptions& = {});
        
        private:
            FTK_PRIVATE();
        };
    }
}

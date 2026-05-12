// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <tl/IO/Read.h>
#include <tl/IO/Write.h>

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
            std::shared_ptr<IRead> read(
                const ftk::Path&,
                const ReadOptions& = {});

            //! Open a file for reading.
            std::shared_ptr<IRead> read(
                const ftk::Path&,
                const std::vector<ftk::MemFile>&,
                const ReadOptions& = {});
        
        private:
            FTK_PRIVATE();
        };

        //! Write system.
        class TL_API_TYPE WriteSystem : public ftk::ISystem
        {
            FTK_NON_COPYABLE(WriteSystem);

        protected:
            WriteSystem(const std::shared_ptr<ftk::Context>&);

        public:
            virtual ~WriteSystem();

            //! Create a new system.
            static std::shared_ptr<WriteSystem> create(const std::shared_ptr<ftk::Context>&);

            //! Add a plugin.
            void addPlugin(const std::shared_ptr<IWritePlugin>&);

            //! Get the supported file extensions.
            const std::vector<std::string>& getExts() const;

            //! Open a file for writing.
            std::shared_ptr<IWrite> write(
                const ftk::Path&,
                const WriteOptions& = {});

        private:
            FTK_PRIVATE();
        };
    }
}

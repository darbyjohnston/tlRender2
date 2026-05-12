// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/IO/Read.h>

namespace tl
{
    namespace io
    {
        TL_ENUM_IMPL(
            FileType,
            "Media",
            "Seq");

        IRead::IRead(
            const ftk::Path& path,
            const std::vector<ftk::MemFile>& mem,
            const ReadOptions& options,
            const std::shared_ptr<ftk::LogSystem>& logSystem) :
            _path(path),
            _mem(mem),
            _options(options),
            _logSystem(logSystem)
        {}

        IRead::~IRead()
        {}

        void IReadPlugin::_init(
            const std::string& name,
            const std::map<std::string, FileType>& exts,
            const std::shared_ptr<ftk::LogSystem>& logSystem)
        {
            _name = name;
            _exts = exts;
            _logSystem = logSystem;
        }

        IReadPlugin::~IReadPlugin()
        {}

        const std::string& IReadPlugin::getName() const
        {
            return _name;
        }

        const std::map<std::string, FileType>& IReadPlugin::getExts() const
        {
            return _exts;
        }
    }
}

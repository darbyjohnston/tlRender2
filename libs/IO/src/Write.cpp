// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/IO/Write.h>

namespace tl
{
    namespace io
    {
        IWrite::IWrite(
            const ftk::Path& path,
            const WriteOptions& options,
            const std::shared_ptr<ftk::LogSystem>& logSystem) :
            _path(path),
            _options(options),
            _logSystem(logSystem)
        {}

        IWrite::~IWrite()
        {}

        void IWritePlugin::_init(
            const std::string& name,
            const std::vector<std::string>& exts,
            const std::shared_ptr<ftk::LogSystem>& logSystem)
        {
            _name = name;
            _exts = exts;
            _logSystem = logSystem;
        }

        IWritePlugin::~IWritePlugin()
        {}

        const std::string& IWritePlugin::getName() const
        {
            return _name;
        }

        const std::vector<std::string>& IWritePlugin::getExts() const
        {
            return _exts;
        }
    }
}

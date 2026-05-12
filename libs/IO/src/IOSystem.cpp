// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/IO/IOSystem.h>

#if defined(TL_IO_HAS_FFMPEG)
#include "FFmpegRead.h"
#endif // TL_IO_HAS_FFMPEG
#if defined(TL_IO_HAS_OIIO)
#include "OIIORead.h"
#include "OIIOWrite.h"
#endif // TL_IO_HAS_OIIO

#include <ftk/Core/Context.h>
#include <ftk/Core/Format.h>

namespace tl
{
    using namespace core;

    namespace io
    {
        struct ReadSystem::Private
        {
            std::vector<std::shared_ptr<IReadPlugin>> plugins;
            std::map<std::string, FileType> exts;
        };

        ReadSystem::ReadSystem(const std::shared_ptr<ftk::Context>& context) :
            ISystem(context, "tl::io::IOSystem"),
            _p(new Private)
        {
            FTK_P();
            auto logSystem = context->getLogSystem();
#if defined(TL_IO_HAS_OIIO)
            addPlugin(OIIOReadPlugin::create(logSystem));
#endif // TL_IO_HAS_OIIO
#if defined(TL_IO_HAS_FFMPEG)
            addPlugin(FFmpegReadPlugin::create(logSystem));
#endif // TL_IO_HAS_FFMPEG
        }

        ReadSystem::~ReadSystem()
        {}

        std::shared_ptr<ReadSystem> ReadSystem::create(const std::shared_ptr<ftk::Context>& context)
        {
            return std::shared_ptr<ReadSystem>(new ReadSystem(context));
        }

        void ReadSystem::addPlugin(const std::shared_ptr<IReadPlugin>& plugin)
        {
            FTK_P();
            p.plugins.push_back(plugin);
            const auto& exts = plugin->getExts();
            p.exts.insert(exts.begin(), exts.end());
        }

        const std::map<std::string, FileType>& ReadSystem::getExts() const
        {
            return _p->exts;
        }
        
        std::shared_ptr<IRead> ReadSystem::read(
            const ftk::Path& path,
            const ReadOptions& options)
        {
            FTK_P();
            for (auto plugin : p.plugins)
            {
                if (plugin->canRead(path, options))
                {
                    return plugin->read(path, options);
                }
            }
            return nullptr;
        }
        
        std::shared_ptr<IRead> ReadSystem::read(
            const ftk::Path& path,
            const std::vector<ftk::MemFile>& mem,
            const ReadOptions& options)
        {
            FTK_P();
            for (auto plugin : p.plugins)
            {
                if (plugin->canRead(path, options))
                {
                    return plugin->read(path, mem, options);
                }
            }
            throw std::runtime_error(ftk::Format("Cannot read \"{0}\": no plugin handles this format").arg(path.get()));
        }

        struct WriteSystem::Private
        {
            std::vector<std::shared_ptr<IWritePlugin>> plugins;
            std::vector<std::string> exts;
        };

        WriteSystem::WriteSystem(const std::shared_ptr<ftk::Context>& context) :
            ISystem(context, "tl::io::IOSystem"),
            _p(new Private)
        {
            FTK_P();
            auto logSystem = context->getLogSystem();
#if defined(TL_IO_HAS_OIIO)
            addPlugin(OIIOWritePlugin::create(logSystem));
#endif // TL_IO_HAS_OIIO
        }

        WriteSystem::~WriteSystem()
        {}

        std::shared_ptr<WriteSystem> WriteSystem::create(const std::shared_ptr<ftk::Context>& context)
        {
            return std::shared_ptr<WriteSystem>(new WriteSystem(context));
        }

        void WriteSystem::addPlugin(const std::shared_ptr<IWritePlugin>& plugin)
        {
            FTK_P();
            p.plugins.push_back(plugin);
            const auto& exts = plugin->getExts();
            p.exts.insert(p.exts.end(), exts.begin(), exts.end());
        }

        const std::vector<std::string>& WriteSystem::getExts() const
        {
            return _p->exts;
        }
        
        std::shared_ptr<IWrite> WriteSystem::write(
            const ftk::Path& path,
            const WriteOptions& options)
        {
            FTK_P();
            for (auto plugin : p.plugins)
            {
                if (plugin->canWrite(path, options))
                {
                    return plugin->write(path, options);
                }
            }
            throw std::runtime_error(ftk::Format("Cannot write \"{0}\": no plugin handles this format").arg(path.get()));
        }
    }
}

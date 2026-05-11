// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/InfoApp/App.h>

#include <ftk/Core/Context.h>
#include <ftk/Core/Format.h>

namespace tl
{
    namespace info_app
    {
        void App::_init(
            const std::shared_ptr<ftk::Context>& context,
            std::vector<std::string>& argv)
        {
            _cmdLine.inputs = ftk::CmdLineListArg<std::string>::create(
                "inputs",
                "The input files. Media files and .otio timelines are supported.");

            IApp::_init(
                context,
                argv,
                "tlinfo",
                "Print information about timelines and media.",
                {
                    _cmdLine.inputs
                },
                {});
        }

        App::App()
        {}

        App::~App()
        {}

        std::shared_ptr<App> App::create(
            const std::shared_ptr<ftk::Context>& context,
            std::vector<std::string>& argv)
        {
            auto out = std::shared_ptr<App>(new App);
            out->_init(context, argv);
            return out;
        }

        void App::run()
        {
            auto readSystem = _context->getSystem<io::ReadSystem>();
            for (const auto& input : _cmdLine.inputs->getList())
            {
                const ftk::Path path(input);
                const std::string ext = ftk::toLower(path.getExt());
                if (ext == ".otio" || ext == ".otioz" || ext == ".otiod")
                {
                    _printTimeline(readSystem, path);
                }
                else if (!_printMedia(readSystem, { path }))
                {
                    _printError("Unknown file: " + input);
                }
            }
        }

        bool App::_printMedia(
            const std::shared_ptr<io::ReadSystem>& readSystem,
            const timeline::Media& media,
            int indent)
        {
            const auto& exts = readSystem->getExts();
            const auto i = exts.find(ftk::toLower(media.path.getExt()));
            if (i == exts.end())
            {
                return false;
            }
            ftk::Path path = media.path;
            if (io::FileType::Seq == i->second)
            {
                path = ftk::expandSeq(path);
            }
            try
            {
                auto read = readSystem->read(path, media.mem);
                if (!read)
                {
                    throw std::runtime_error("Unknown file: " + path.get());
                }
                const auto info = read->getInfo();

                const std::string prefix(indent, ' ');
                _print(prefix + path.get() + ":");
                for (size_t i = 0; i < info.video.size(); ++i)
                {
                    const auto& video = info.video[i];
                    _print(prefix + ftk::Format("  video {0}:").arg(i).str());
                    _print(prefix + ftk::Format("    size: {0}x{1}").
                        arg(video.size.w).
                        arg(video.size.h).str());
                }
                if (!info.video.empty())
                {
                    if (info.videoStart)
                    {
                        _print(prefix + ftk::Format("  video start: {0}").
                            arg(core::to_string(*info.videoStart)).str());
                    }
                    _print(prefix + ftk::Format("  video duration: {0}").
                        arg(core::to_string(info.videoDuration)).str());
                }
                for (size_t i = 0; i < info.audio.size(); ++i)
                {
                    const auto& audio = info.audio[i];
                    _print(prefix + ftk::Format("  audio {0}:").arg(i).str());
                    _print(prefix + ftk::Format("    channels: {0}").
                        arg(audio.channelCount).str());
                }
                if (!info.audio.empty())
                {
                    if (info.audioStart)
                    {
                        _print(prefix + ftk::Format("  audio start: {0}").
                            arg(core::to_string(*info.audioStart)).str());
                    }
                    _print(prefix + ftk::Format("  audio duration: {0}").
                        arg(core::to_string(info.audioDuration)).str());
                }
                for (const auto& tag : info.tags)
                {
                    _print(prefix + ftk::Format("  tag \"{0}\": \"{1}\"").
                        arg(tag.first).
                        arg(tag.second).str());
                }
            }
            catch (const std::exception& e)
            {
                _printError(e.what());
            }
            return true;
        }

        void App::_printTimeline(
            const std::shared_ptr<io::ReadSystem>& readSystem,
            const ftk::Path& path)
        {
            try
            {
                auto timeline = timeline::Timeline::create(_context, path);

                _print(path.get() + ":");
                const auto& media = timeline->getMedia();
                _print(ftk::Format("  references: {0}").
                    arg(media.size()));
                for (const auto& i : media)
                {
                    if (!_printMedia(readSystem, i, 2))
                    {
                        _printError("Unknown file: " + i.path.get());
                    }
                }
            }
            catch (const std::exception& e)
            {
                _printError(e.what());
            }
        }
    }
}

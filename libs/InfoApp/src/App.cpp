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
            try
            {
                auto read = readSystem->read(media.path, media.mem);
                if (!read)
                {
                    throw std::runtime_error("Unknown file: " + media.path.get());
                }
                const auto info = read->getInfo();

                _printIndented(media.path.get() + ":", indent);
                for (size_t i = 0; i < info.video.size(); ++i)
                {
                    const auto& video = info.video[i];
                    _printIndented(ftk::Format("  video {0}:").arg(i), indent);
                    _printIndented(ftk::Format("    size: {0}x{1}").
                        arg(video.size.w).
                        arg(video.size.h),
                        indent);
                }
                if (!info.video.empty())
                {
                    if (info.videoStart)
                    {
                        _printIndented(ftk::Format("  video start: {0}").
                            arg(core::to_string(*info.videoStart)),
                            indent);
                    }
                    _printIndented(ftk::Format("  video duration: {0}").
                        arg(core::to_string(info.videoDuration)),
                        indent);
                }
                for (size_t i = 0; i < info.audio.size(); ++i)
                {
                    const auto& audio = info.audio[i];
                    _printIndented(ftk::Format("  audio {0}:").arg(i), indent);
                    _printIndented(ftk::Format("    channels: {0}").
                        arg(audio.channelCount),
                        indent);
                }
                if (!info.audio.empty())
                {
                    if (info.audioStart)
                    {
                        _printIndented(ftk::Format("  audio start: {0}").
                            arg(core::to_string(*info.audioStart)),
                            indent);
                    }
                    _printIndented(ftk::Format("  audio duration: {0}").
                        arg(core::to_string(info.audioDuration)),
                        indent);
                }
                for (const auto& tag : info.tags)
                {
                    _printIndented(ftk::Format("  tag \"{0}\": \"{1}\"").
                        arg(tag.first).
                        arg(tag.second),
                        indent);
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

                int indent = 0;
                _printIndented(path.get() + ":", indent);
                indent += 2;

                auto stack = timeline->getStack();
                _printIndented(ftk::Format("stack \"{0}\":").arg(stack->name), indent);
                indent += 2;
                for (const auto& stackChild : stack->children)
                {
                    if (auto track = std::dynamic_pointer_cast<timeline::Track>(stackChild))
                    {
                        _printIndented(ftk::Format("track \"{0}\":").arg(track->name), indent);
                        indent += 2;
                        for (const auto& trackChild : track->children)
                        {
                            if (auto clip = std::dynamic_pointer_cast<timeline::Clip>(trackChild))
                            {
                                _printIndented(ftk::Format("clip \"{0}\":").arg(clip->name), indent);
                                indent += 2;
                                for (const auto& media : clip->media)
                                {
                                    if (!_printMedia(readSystem, *media, indent))
                                    {
                                        _printError("Unknown file: " + media->path.get());
                                    }
                                }
                                indent -= 2;
                            }
                        }
                        indent -= 2;
                    }
                }
            }
            catch (const std::exception& e)
            {
                _printError(e.what());
            }
        }

        void App::_printIndented(const std::string& s, int indent)
        {
            _print(std::string(indent, ' ') + s);
        }
    }
}

// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/InfoApp/App.h>

#include <tl/IO/IOSystem.h>

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
                "The input files.");

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
            const auto& exts = readSystem->getExts();
            for (const auto& input : _cmdLine.inputs->getList())
            {
                ftk::Path path(input);
                const auto i = exts.find(ftk::toLower(path.getExt()));
                if (i != exts.end())
                {
                    if (io::FileType::Seq == i->second)
                    {
                        path = ftk::expandSeq(path);
                    }
                    try
                    {
                        auto read = readSystem->read(path);
                        if (!read)
                        {
                            throw std::runtime_error("Unknown file: " + input);
                        }
                        const auto info = read->getInfo();
                        
                        _print("\"" + path.get() + "\":");
                        for (size_t i = 0; i < info.video.size(); ++i)
                        {
                            const auto& video = info.video[i];
                            _print(ftk::Format("  video {0}:").arg(i));
                            _print(ftk::Format("    size: {0}x{1}").
                                arg(video.size.w).
                                arg(video.size.h));
                            _print(ftk::Format("    type: {0}").arg(video.type));
                        }
                        if (!info.video.empty())
                        {
                            if (info.videoStart)
                            {
                                _print(ftk::Format("  video start: {0}").
                                    arg(core::to_string(*info.videoStart)));
                            }
                            _print(ftk::Format("  video duration: {0}").
                                arg(core::to_string(info.videoDuration)));
                        }
                        for (size_t i = 0; i < info.audio.size(); ++i)
                        {
                            const auto& audio = info.audio[i];
                            _print(ftk::Format("  audio {0}:").arg(i));
                            _print(ftk::Format("    channels: {0}").arg(audio.channelCount));
                            _print(ftk::Format("    type: {0}").arg(audio.type));
                            _print(ftk::Format("    sample rate: {0}").arg(audio.sampleRate));
                        }
                        if (!info.audio.empty())
                        {
                            if (info.audioStart)
                            {
                                _print(ftk::Format("  audio start: {0}").
                                    arg(core::to_string(*info.audioStart)));
                            }
                            _print(ftk::Format("  audio duration: {0}").
                                arg(core::to_string(info.audioDuration)));
                        }
                        for (const auto& tag : info.tags)
                        {
                            _print(ftk::Format("  tag \"{0}\": \"{1}\"").
                                arg(tag.first).
                                arg(tag.second));
                        }
                    }
                    catch (const std::exception& e)
                    {
                        _printError(e.what());
                    }
                }
                else
                {
                    _printError("Unknown file: " + input);
                }
            }
        }
    }
}

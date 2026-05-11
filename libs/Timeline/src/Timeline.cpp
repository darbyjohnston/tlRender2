// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/Timeline/Timeline.h>

#include "TimeUtil.h"
#include "ZipUtil.h"

#include <ftk/Core/Format.h>

#include <opentimelineio/clip.h>
#include <opentimelineio/externalReference.h>
#include <opentimelineio/timeline.h>
#include <opentimelineio/track.h>

#include <filesystem>

namespace tl
{
    using namespace core;

    namespace timeline
    {
        namespace
        {
            // Resolve an ExternalReference's target_url relative to the
            // timeline file's directory. Handles absolute paths, relative
            // paths, and the file:// URL scheme. URL-encoded characters are
            // not currently decoded; OTIO files in the wild rarely use them
            // for local references.
            std::optional<ftk::Path> resolveExternalReference(
                const std::string& targetUrl,
                const std::filesystem::path& timelineDir)
            {
                if (targetUrl.empty())
                {
                    return std::nullopt;
                }

                std::string s = targetUrl;
                constexpr const char* fileScheme = "file://";
                if (s.compare(0, 7, fileScheme) == 0)
                {
                    s = s.substr(7);
                }

                std::filesystem::path p(s);
                if (p.is_relative() && !timelineDir.empty())
                {
                    p = timelineDir / p;
                }
                std::error_code ec;
                auto resolved = std::filesystem::weakly_canonical(p, ec);
                if (ec)
                {
                    resolved = p;
                }
                return ftk::Path(resolved.string());
            }
        }

        IItem::~IItem()
        {}

        struct Timeline::Private
        {
            std::shared_ptr<ftk::Context> context;
            ftk::Path path;
            std::shared_ptr<ftk::FileIO> fileIO;
            MediaRate rate;
            std::shared_ptr<ftk::Observable<Time> > startTime;
            std::shared_ptr<ftk::Observable<Duration> > duration;
            std::shared_ptr<Stack> stack;
        };

        void Timeline::_init(
            const std::shared_ptr<ftk::Context>& context,
            const ftk::Path& path)
        {
            FTK_P();
            p.context = context;
            p.path = path;
            p.startTime = ftk::Observable<Time>::create();
            p.duration = ftk::Observable<Duration>::create();

            // Read the timeline.
            OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline> otioTimeline;
            OTIO_NS::ErrorStatus errorStatus;
            std::map<std::string, ftk::MemFile> zipMem;
            if (".otioz" == ftk::toLower(path.getExt()))
            {
                // Open the ZIP and parse content.otio.
                ZipReader zip(path.get());
                auto id = zip.find("content.otio");
                if (!id)
                {
                    throw std::runtime_error(ftk::Format("Cannot find \"content.otio\": {0}").
                        arg(path.get()));
                }
                const std::string json = zip.readToString(*id);
                otioTimeline = dynamic_cast<OTIO_NS::Timeline*>(
                    OTIO_NS::Timeline::from_json_string(json, &errorStatus));
                if (!otioTimeline || OTIO_NS::is_error(errorStatus))
                {
                    throw std::runtime_error(ftk::Format("Cannot read timeline \"{0}\": {1}").
                        arg(path.get()).
                        arg(errorStatus.full_description));
                }
                
                // Memory-map the ZIP.
                p.fileIO = ftk::FileIO::create(
                    path.get(),
                    ftk::FileMode::Read,
                    ftk::FileRead::MMap);
                
                // Walk every entry under "media/" and record the byte range
                // for each one that's stored uncompressed.
                const uint8_t* zipBase = p.fileIO->getMemP();
                const size_t zipSize = p.fileIO->getSize();
                const mz_uint fileCount = zip.getFileCount();
                for (mz_uint i = 0; i < fileCount; ++i)
                {
                    const auto s = zip.stat(i);

                    // Skip directories and non-media entries.
                    if (s.m_is_directory) continue;
                    if (std::strncmp(s.m_filename, "media/", 6) != 0) continue;

                    if (auto dataOffset = zip.getStoredDataOffset(i, zipBase, zipSize))
                    {
                        zipMem[s.m_filename] =
                        {
                            zipBase + *dataOffset,
                            static_cast<size_t>(s.m_uncomp_size)
                        };
                    }
                }
            }
            else if (!path.get().empty())
            {
                otioTimeline = dynamic_cast<OTIO_NS::Timeline*>(
                    OTIO_NS::Timeline::from_json_file(path.get(), &errorStatus));
                if (!otioTimeline || OTIO_NS::is_error(errorStatus))
                {
                    throw std::runtime_error(ftk::Format("Cannot read timeline \"{0}\": {1}").
                        arg(path.get()).
                        arg(errorStatus.full_description));
                }
            }

            p.stack = std::make_shared<Stack>();
            if (otioTimeline)
            {
                p.rate = mediaDurationFromOTIO(otioTimeline->duration()).rate;
                p.startTime->setIfChanged(otioTimeline->global_start_time().has_value() ?
                    timeFromOTIO(otioTimeline->global_start_time().value(), p.rate.toDouble()) :
                    Time());
                p.duration->setIfChanged(
                    durationFromOTIO(otioTimeline->duration(), p.rate.toDouble()));

                auto otioStack = otioTimeline->tracks();
                p.stack->name = otioStack->name();
                p.stack->startTime = timeFromOTIO(otioStack->trimmed_range().start_time(), p.rate.toDouble());
                p.stack->duration = durationFromOTIO(otioStack->trimmed_range().duration(), p.rate.toDouble());

                // Read the tracks.
                const auto timelineDir = std::filesystem::path(path.get()).parent_path();
                std::map<std::string, std::shared_ptr<Media>> seen;
                for (const auto& otioTimelineChild : otioStack->children())
                {
                    if (auto otioTrack = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Track>(otioTimelineChild))
                    {
                        auto track = std::make_shared<Track>();
                        track->name = otioTrack->name();
                        track->startTime = timeFromOTIO(otioTrack->trimmed_range().start_time(), p.rate.toDouble());
                        track->duration = durationFromOTIO(otioTrack->trimmed_range().duration(), p.rate.toDouble());
                        p.stack->children.push_back(track);

                        // Read the clips.
                        for (const auto& otioTrackChild : otioTrack->children())
                        {
                            if (auto otioClip = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Clip>(otioTrackChild))
                            {
                                auto clip = std::make_shared<Clip>();
                                clip->name = otioClip->name();
                                clip->startTime = timeFromOTIO(otioClip->trimmed_range().start_time(), p.rate.toDouble());
                                clip->duration = durationFromOTIO(otioClip->trimmed_range().duration(), p.rate.toDouble());
                                track->children.push_back(clip);

                                // Read the media references.
                                if (auto otioExternalRef =
                                    dynamic_cast<OTIO_NS::ExternalReference*>(otioClip->media_reference()))
                                {
                                    const std::string targetUrl = otioExternalRef->target_url();

                                    std::vector<ftk::MemFile> refMem;
                                    if (!zipMem.empty())
                                    {
                                        auto it = zipMem.find(targetUrl);
                                        if (it == zipMem.end())
                                        {
                                            it = zipMem.find("media/" + targetUrl);
                                        }
                                        if (it != zipMem.end())
                                        {
                                            refMem.push_back(it->second);
                                        }
                                    }

                                    std::optional<ftk::Path> resolved;
                                    if (!refMem.empty())
                                    {
                                        resolved = ftk::Path(path.get() + "/" + targetUrl);
                                    }
                                    else
                                    {
                                        resolved = resolveExternalReference(targetUrl, timelineDir);
                                    }
                                    if (resolved)
                                    {
                                        const std::string key = resolved->get();
                                        const auto it = seen.find(key);
                                        if (it != seen.end())
                                        {
                                            clip->media.push_back(it->second);
                                        }
                                        else
                                        {
                                            auto media = std::make_shared<Media>();
                                            media->path = *resolved;
                                            media->mem = std::move(refMem);
                                            clip->media.push_back(media);
                                            seen[resolved->get()] = media;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        Timeline::Timeline() :
            _p(new Private)
        {}

        Timeline::~Timeline()
        {}

        std::shared_ptr<Timeline> Timeline::create(
            const std::shared_ptr<ftk::Context>& context)
        {
            auto out = std::shared_ptr<Timeline>(new Timeline);
            out->_init(context, ftk::Path());
            return out;
        }

        std::shared_ptr<Timeline> Timeline::create(
            const std::shared_ptr<ftk::Context>& context,
            const ftk::Path& path)
        {
            auto out = std::shared_ptr<Timeline>(new Timeline);
            out->_init(context, path);
            return out;
        }

        const ftk::Path& Timeline::getPath() const
        {
            return _p->path;
        }

        const std::shared_ptr<Stack>& Timeline::getStack() const
        {
            return _p->stack;
        }

        const Time& Timeline::getStartTime() const
        {
            return _p->startTime->get();
        }

        std::shared_ptr<ftk::IObservable<Time> > Timeline::observeStartTime() const
        {
            return _p->startTime;
        }

        const Duration& Timeline::getDuration() const
        {
            return _p->duration->get();
        }

        std::shared_ptr<ftk::IObservable<Duration> > Timeline::observeDuration() const
        {
            return _p->duration;
        }

        std::shared_ptr<IVideoNode> Timeline::getVideo(const Time&)
        {
            return nullptr;
        }

        std::future<std::shared_ptr<Audio> > Timeline::getAudio(int64_t seconds)
        {
            return std::future<std::shared_ptr<Audio> >();
        }
    }
}

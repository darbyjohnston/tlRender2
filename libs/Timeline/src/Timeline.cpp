// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include <tl/Timeline/Timeline.h>

#include "TimeUtil.h"
#include "TimelineUtil.h"
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
        TL_ENUM_IMPL(
            TrackType,
            "Unknown",
            "Video",
            "Audio");

        IItem::~IItem()
        {}

        struct Timeline::Private
        {
            void readTrack(OTIO_NS::Track*);
            void readClip(
                const std::shared_ptr<Track>&,
                OTIO_NS::Clip*);
            void readExtRef(
                const std::shared_ptr<Clip>&,
                const std::string&,
                OTIO_NS::ExternalReference*);

            std::shared_ptr<ftk::Context> context;
            ftk::Path path;
            std::shared_ptr<ftk::FileIO> fileIO;
            std::map<std::string, ftk::MemFile> zipMem;
            MediaRate rate = MediaRate{ 24, 1 };
            std::shared_ptr<ftk::Observable<Time> > startTime;
            std::shared_ptr<ftk::Observable<Duration> > duration;
            std::shared_ptr<Stack> stack;
            std::filesystem::path timelineDir;
            std::map<std::string, std::shared_ptr<Media>> seen;
        };

        void Timeline::Private::readTrack(OTIO_NS::Track* otioTrack)
        {
            auto track = std::make_shared<Track>();
            if (OTIO_NS::Track::Kind::video == otioTrack->kind())
            {
                track->type = TrackType::Video;
            }
            else if (OTIO_NS::Track::Kind::audio == otioTrack->kind())
            {
                track->type = TrackType::Audio;
            }
            track->name = otioTrack->name();
            auto range = otioTrack->trimmed_range_in_parent();
            if (!range.has_value())
            {
                range = otioTrack->trimmed_range();
            }
            if (range.has_value())
            {
                track->startTime = timeFromOTIO(range->start_time(), rate.toDouble());
                track->duration = durationFromOTIO(range->duration(), rate.toDouble());
            }
            stack->children.push_back(track);

            // Read the clips.
            for (const auto& otioTrackChild : otioTrack->children())
            {
                if (auto otioClip = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Clip>(otioTrackChild))
                {
                    readClip(track, otioClip);
                }
            }
        }

        void Timeline::Private::readClip(
            const std::shared_ptr<Track>& track,
            OTIO_NS::Clip* otioClip)
        {
            auto clip = std::make_shared<Clip>();
            clip->name = otioClip->name();
            auto range = otioClip->trimmed_range_in_parent();
            if (!range.has_value())
            {
                range = otioClip->trimmed_range();
            }
            if (range.has_value())
            {
                clip->startTime = timeFromOTIO(range->start_time(), rate.toDouble());
                clip->duration = durationFromOTIO(range->duration(), rate.toDouble());
            }
            track->children.push_back(clip);

            // Read the media references.
            clip->activeMediaReference = otioClip->active_media_reference_key();
            for (const auto& refIt : otioClip->media_references())
            {
                if (auto otioExternalRef =
                    dynamic_cast<OTIO_NS::ExternalReference*>(refIt.second))
                {
                    readExtRef(clip, refIt.first, otioExternalRef);
                }
            }
        }

        void Timeline::Private::readExtRef(
            const std::shared_ptr<Clip>& clip,
            const std::string& name,
            OTIO_NS::ExternalReference* otioExternalRef)
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
                auto mediaReference = std::make_shared<MediaReference>();
                auto range = otioExternalRef->available_range();
                if (range.has_value())
                {
                    mediaReference->availableRangeStart = mediaTimeFromOTIO(range->start_time());
                    mediaReference->availableRangeDuration = mediaDurationFromOTIO(range->duration());
                }
                clip->mediaReferences[name] = mediaReference;

                const std::string key = resolved->get();
                const auto it = seen.find(key);
                if (it != seen.end())
                {
                    mediaReference->media = it->second;
                }
                else
                {
                    mediaReference->media = std::make_shared<Media>();
                    mediaReference->media->path = *resolved;
                    mediaReference->media->mem = std::move(refMem);
                    seen[resolved->get()] = mediaReference->media;
                }
            }
        }

        void Timeline::_init(
            const std::shared_ptr<ftk::Context>& context,
            const ftk::Path& path)
        {
            FTK_P();
            p.context = context;
            p.path = path;
            p.startTime = ftk::Observable<Time>::create();
            p.duration = ftk::Observable<Duration>::create();
            p.stack = std::make_shared<Stack>();
            p.timelineDir = std::filesystem::path(path.get()).parent_path();

            // Read the timeline.
            OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline> otioTimeline;
            OTIO_NS::ErrorStatus errorStatus;
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
                        p.zipMem[s.m_filename] =
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
                p.stack->startTime = p.startTime->get();
                p.stack->duration = p.duration->get();

                // Read the tracks.
                for (const auto& otioTimelineChild : otioStack->children())
                {
                    if (auto otioTrack = OTIO_NS::dynamic_retainer_cast<OTIO_NS::Track>(otioTimelineChild))
                    {
                        p.readTrack(otioTrack);
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

        const core::MediaRate& Timeline::getRate() const
        {
            return _p->rate;
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

        const std::shared_ptr<Stack>& Timeline::getStack() const
        {
            return _p->stack;
        }

        std::shared_ptr<VideoGraph> Timeline::getVideo(const Time& time)
        {
            FTK_P();
    
            // Stack-time relative to stack origin
            const core::Time stackTime = { time.frames - p.stack->startTime.frames };
    
            // Collect active reads across all video tracks.
            std::vector<VideoNodePtr> reads;
            for (const auto& child : p.stack->children)
            {
                auto track = std::dynamic_pointer_cast<Track>(child);
                if (!track || track->type != TrackType::Video)
                {
                    continue;
                }

                // Time relative to this track
                const core::Time trackTime = { stackTime.frames - track->startTime.frames };
                if (trackTime.frames < 0 || trackTime.frames >= track->duration.frames)
                {
                    continue;
                }
                
                // Find the active clip
                if (auto active = findActiveClip(track, trackTime))
                {
                    const auto& [clip, clipTime] = *active;
                    if (auto readNode = buildReadNode(clip, clipTime, p.rate))
                    {
                        reads.push_back(readNode);
                    }
                }
            }
            if (reads.empty())
            {
                return nullptr;
            }
            
            auto graph = std::make_shared<VideoGraph>();
            if (reads.size() == 1)
            {
                graph->root = reads[0];
            }
            else
            {
                auto comp = std::make_shared<VideoNode>();
                comp->op = CompositeVideo{};
                comp->inputs = std::move(reads);
                graph->root = comp;
            }
            
            return graph;
        }

        std::shared_ptr<Audio> Timeline::getAudio(int64_t seconds)
        {
            return nullptr;
        }
    }
}

// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#include "TimelineUtil.h"

namespace tl
{
    namespace timeline
    {
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

        std::optional<std::pair<std::shared_ptr<Clip>, core::Time>>
        findActiveClip(
            const std::shared_ptr<Track>& track,
            const core::Time& trackTime)
        {
            for (const auto& child : track->children)
            {
                auto clip = std::dynamic_pointer_cast<Clip>(child);
                if (!clip) continue;
            
                const int64_t clipStart = clip->startTime.frames;
                const int64_t clipEnd = clipStart + clip->duration.frames;
                if (trackTime.frames >= clipStart && trackTime.frames < clipEnd)
                {
                    return std::make_pair(clip, core::Time{trackTime.frames - clipStart});
                }
            }
            return std::nullopt;
        }

        VideoNodePtr buildReadNode(
            const std::shared_ptr<Clip>& clip,
            const core::Time& clipTime,
            const core::MediaRate& timelineRate)
        {
            // Pick the active reference (or default to "DEFAULT_MEDIA")
            auto it = clip->mediaReferences.find(clip->activeMediaReference);
            if (it == clip->mediaReferences.end())
            {
                it = clip->mediaReferences.find("DEFAULT_MEDIA");
                if (it == clip->mediaReferences.end())
                {
                    return nullptr;
                }
            }
            auto mediaRef = it->second;
            if (!mediaRef || !mediaRef->media)
            {
                return nullptr;
            }
            
            // Compute source time at the media's rate
            core::MediaTime sourceTime;
            if (mediaRef->availableRangeStart)
            {
                sourceTime = *mediaRef->availableRangeStart;
                // Rescale clipTime (timeline rate) to media rate, then add
                const core::MediaTime clipMediaTime = core::rescale(
                    core::MediaTime{ clipTime.frames, timelineRate },
                    sourceTime.rate);
                sourceTime.frames += clipMediaTime.frames;
            }
            else
            {
                // No available range; treat source as starting at 0
                //
                //! \todo Need the media's own rate, which we don't have
                //! without opening it. Or does the rate come from the OTIO clip?
                sourceTime.rate = timelineRate;
            }
            
            auto node = std::make_shared<VideoNode>();
            node->op = ReadVideo{ mediaRef->media, sourceTime, it->first };
            return node;
        }
    }
}

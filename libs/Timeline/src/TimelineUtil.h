// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <tl/Timeline/Timeline.h>

#include <ftk/Core/Path.h>

#include <filesystem>

namespace tl
{
    namespace timeline
    {
        // Resolve an ExternalReference's target_url relative to the
        // timeline file's directory. Handles absolute paths, relative
        // paths, and the file:// URL scheme. URL-encoded characters are
        // not currently decoded; OTIO files in the wild rarely use them
        // for local references.
        std::optional<ftk::Path> resolveExternalReference(
            const std::string& targetUrl,
            const std::filesystem::path& timelineDir);

        // Find the active clip at the given track-time. Returns the clip and the
        // clip-relative time. Returns null if no clip is active.
        std::optional<std::pair<std::shared_ptr<Clip>, core::Time>>
        findActiveClip(
            const std::shared_ptr<Track>&,
            const core::Time& trackTime);

        // Build a Read node for a clip at the given clip-relative time.
        VideoNodePtr buildReadNode(
            const std::shared_ptr<Clip>&,
            const core::Time& clipTime,
            const core::MediaRate& timelineRate);
    }
}

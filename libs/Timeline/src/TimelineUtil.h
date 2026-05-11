// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

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
    }
}

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
    }
}

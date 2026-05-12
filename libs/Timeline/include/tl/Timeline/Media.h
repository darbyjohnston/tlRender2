// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <tl/Core/Time.h>

#include <ftk/Core/Path.h>
#include <ftk/Core/FileIO.h>

namespace tl
{
    namespace timeline
    {
        //! Media.
        struct Media
        {
            ftk::Path path;
            std::vector<ftk::MemFile> mem;
        };

        //! Media reference.
        struct MediaReference
        {
            std::shared_ptr<Media> media;
            std::optional<core::MediaTime> availableRangeStart;
            std::optional<core::MediaDuration> availableRangeDuration;
        };
        
        //! Default media reference key.
        const std::string defaultMediaReference = "DEFAULT_MEDIA";
    }
}

// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <tl/Timeline/Media.h>

namespace tl
{
    namespace timeline
    {
        //! Read video data.
        struct ReadVideo
        {
            std::shared_ptr<Media> media;
            core::MediaTime sourceTime;
            std::string referenceKey;
        };

        //! Composite multiple inputs.
        struct CompositeVideo
        {};

        //! Dissolve between two inputs.
        struct DissolveVideo
        {
            double mix = 0.0; // 0.0 = full input[0], 1.0 = full input[1]
        };

        //! Color transorm.
        struct ColorTransformVideo
        {
            std::string fromSpace;
            std::string toSpace;
        };

        //! Video operation variants.
        using VideoOp = std::variant<
            ReadVideo,
            CompositeVideo,
            DissolveVideo,
            ColorTransformVideo>;

        //! Video graph node.
        struct VideoNode;
        using VideoNodePtr = std::shared_ptr<VideoNode>;
        struct VideoNode
        {
            VideoOp op;
            std::vector<VideoNodePtr> inputs;
        };

        //! Video graph.
        class VideoGraph
        {
        public:
            VideoNodePtr root;
        };
    }
}

// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <tl/Timeline/Media.h>

#include <variant>

namespace tl
{
    namespace timeline
    {
        //! Read video data.
        struct ReadVideo
        {
            static constexpr const char* typeName = "ReadVideo";
            std::shared_ptr<Media> media;
            core::MediaTime sourceTime;
            std::string referenceKey = defaultMediaReference;
        };

        //! Composite multiple inputs.
        struct CompositeVideo
        {
            static constexpr const char* typeName = "CompositeVideo";
        };

        //! Dissolve between two inputs.
        struct DissolveVideo
        {
            static constexpr const char* typeName = "DissolveVideo";
            double mix = 0.0; // 0.0 = full input[0], 1.0 = full input[1]
        };

        //! Color transform.
        struct ColorTransformVideo
        {
            static constexpr const char* typeName = "ColorTransformVideo";
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
        struct VideoNode
        {
            VideoOp op;
            std::vector<std::shared_ptr<VideoNode>> inputs;
        };
        using VideoNodePtr = std::shared_ptr<VideoNode>;

        //! Video graph.
        class VideoGraph
        {
        public:
            VideoNodePtr root;
        };
    }
}

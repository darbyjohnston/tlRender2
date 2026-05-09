// SPDX-License-Identifier: BSD-3-Clause
// Copyright Contributors to the tlRender project.

#pragma once

#include <tl/Core/Time.h>

#include <ftk/Core/Image.h>

#include <future>
#include <memory>

namespace tl
{
    namespace timeline
    {
        struct TL_API_TYPE IVideoNode : public std::enable_shared_from_this<IVideoNode>
        {
            TL_API IVideoNode(const std::string& type);

            TL_API virtual ~IVideoNode() = 0;

            std::string type;
        };

        struct TL_API_TYPE InputVideoNode : public IVideoNode
        {
            std::future<std::shared_ptr<ftk::Image> > image;
        };
        
        struct TL_API_TYPE VideoNode : public IVideoNode
        {
            std::vector<std::shared_ptr<IVideoNode> > inputs;
        };
    }
}

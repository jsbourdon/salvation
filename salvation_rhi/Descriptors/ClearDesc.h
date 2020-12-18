#pragma once

#include "salvation_core/Math/Math.h"

namespace salvation::rhi
{
    using namespace math;

    namespace descriptors
    {
        struct ClearDescriptor
        {
            Vector4     ColorClearValue;
            float       DepthClearValue;
            uint8_t     StencilClearValue;
            bool        ClearColor;
            bool        ClearDepth;
            bool        ClearStencil;
        };
    }
}
#pragma once

#include "salvation_core/DataStructures/Vector.h"

namespace salvation::rhi
{
    namespace resources
    {
        class Shader;
    }

    namespace descriptors
    {
        enum class InputLayoutSemantic
        {
            Position,
            Normal,
            Tangent,
            UV,
            Float2,
            Float3,
            Float4,
            EnumCount
        };

        struct InputLayoutElement
        {
            InputLayoutSemantic Semantic;
            uint32_t            Slot;
        };

        struct InputLayoutDesc
        {
            data::Vector<InputLayoutElement> Elements;
        };
    }
}
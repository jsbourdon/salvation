#pragma once

#include "salvation_core/DataStructures/StaticArray.h"

namespace salvation::rhi
{
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
            data::StaticArray<InputLayoutElement> Elements;
        };
    }
}
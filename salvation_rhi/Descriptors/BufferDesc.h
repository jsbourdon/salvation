#pragma once

#include "ResourceUsage.h"

namespace salvation::rhi
{
    namespace descriptors
    {
        enum class BufferType
        {
            Constants,
            Structured,
            ReadWrite,
            Vertex,
            Index,
            EnumCount
        };

        struct BufferDesc
        {
            const char*     DebugName;
            size_t          ByteSize;
            size_t          StructureStride;
            BufferType      Type;
            ResourceUsage   Usage;
        };
    }
}
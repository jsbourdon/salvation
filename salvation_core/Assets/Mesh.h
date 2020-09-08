#pragma once

#include <stdint.h>

namespace salvation
{
    namespace asset
    {
        enum class VertexAttribute : uint32_t
        {
            Position,
            Color,
            Normal,
            Tangent,
            UV,
            Count
        };

        static constexpr const char* cppVertexAttributeSemantics[] =
        {
            "POSITION",
            "COLOR_0",
            "NORMAL",
            "TANGENT",
            "TEXCOORD_0"
        };

        static_assert(SALVATION_ARRAY_SIZE(cppVertexAttributeSemantics) == static_cast<size_t>(VertexAttribute::Count));

        struct BufferView
        {
            uint64_t m_byteSize { 0 };
            uint64_t m_byteOffset { 0 };
            uint64_t m_byteStride { 0 };
        };

        struct VertexStream : BufferView
        {
            VertexAttribute m_attribute {};
        };

        struct SubMesh
        {
            BufferView  m_indexBuffer {};
            uint32_t    m_textureIndex { std::numeric_limits<uint32_t>::max() };
            uint32_t    m_streamCount { 0 };
            // VertexStream    m_streams[1];
        };

        struct Mesh
        {
            uint64_t m_subMeshCount { 0 };
        };
    }
}
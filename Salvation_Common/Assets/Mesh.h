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
            Bitangent,
            UV,
            Count
        };

        struct VertexStream
        {
            uint64_t        m_byteSize;
            uint64_t        m_byteOffset;
            VertexAttribute m_attribute;
            uint32_t        m_stride;
        };

        struct SubMesh
        {
            uint32_t        m_textureIndex;
            uint32_t        m_streamCount;
            VertexStream    m_streams[1];
        };

        struct Mesh
        {
            uint64_t    m_subMeshCount;
            SubMesh     m_subMeshes[1];
        };
    }
}
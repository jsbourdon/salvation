#pragma once

#include <stdint.h>
#include "Buffer.h"
#include "Material.h"

namespace salvation
{
    namespace asset
    {
        enum class VertexAttribute : size_t
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
            Buffer          m_buffer;
            VertexAttribute m_attribute;
        };

        struct SubMesh
        {
            Buffer          m_indexBuffer;
            size_t          m_vertexStreamCount;
            Material        m_material;
            VertexStream    m_vertexStreams[1];
        };

        struct Mesh
        {
            size_t      m_subMeshCount;
            SubMesh     m_subMeshes[1];
        };
    }
}
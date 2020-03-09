#pragma once

#include <stdint.h>
#include "Buffer.h"
#include "Material.h"

namespace salvation
{
    namespace asset
    {
        enum class VertexAttribute
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
            Buffer          m_Buffer;
            size_t          m_Offset;
            size_t          m_Size;
            VertexAttribute m_Attribute;
        };

        struct SubMesh
        {
            Buffer          m_IndexBuffer;
            size_t          m_IndexBufferOffset;
            size_t          m_IndexBufferSize;

            VertexStream*   m_pVertexStreams;
            uint32_t        m_VertexStreamCount;

            Material        m_Material;
        };

        struct Mesh
        {
            SubMesh*    m_pSubMeshes;
            uint32_t    m_SubMeshCount;
        };
    }
}
#pragma once

#include <cstdint>
#include <stdio.h>
#include <vector>
#include "asset_assembler/rapidjson/fwd.h"
#include "Salvation_Common/Containers/Vector.h"

using namespace rapidjson;
using namespace salvation::containers;

namespace asset_assembler
{
    namespace database
    {
        /*
        enum class ComponentType
        {
            Unknown = -1,
            Scalar_Float,
            Scalar_Byte,
            Scalar_Short,
            Scalar_Int,
            Vec2,
            Vec3,
            Vec4,
            Matrix2x2,
            Matrix3x3,
            Matrix4x4,
            Count
        };
        */

        class AssetDatabaseBuilder
        {
        public:

            AssetDatabaseBuilder() = default;
            ~AssetDatabaseBuilder() = default;

            bool BuildDatabase(const char *pSrcPath, const char *pDstPath);

        private:

            struct PackedBufferMeta
            {
                PackedBufferMeta(uint64_t byteOffset, uint64_t byteSize) : m_byteOffset(byteOffset), m_byteSize(byteSize) {}
                uint64_t m_byteOffset;
                uint64_t m_byteSize;
            };

            static constexpr const char cpBuffersBinFileName[] = "Buffers.bin";
            static constexpr const char cpTexturesBinFileName[] = "Textures.bin";

            bool    BuildTextures(const Document &json, const char *pSrcRootPath, const char *pDestRootPath);
            int64_t CompressTexture(const char *pSrcFilePath, FILE *pDestFile);
            bool    BuildMeshes(const Document &json, const char *pSrcRootPath, const char *pDestRootPath);

        private:

            Vector<PackedBufferMeta> m_buffersMeta { 100 };
            Vector<PackedBufferMeta> m_texturesMeta { 100 };
        };
    }
}
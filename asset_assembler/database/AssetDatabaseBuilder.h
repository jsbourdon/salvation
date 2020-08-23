#pragma once

#include <cstdint>
#include <stdio.h>
#include <vector>
#include <limits>
#include "asset_assembler/rapidjson/fwd.h"
#include "Salvation_Common/DataStructures/Vector.h"

using namespace rapidjson;
using namespace salvation::data;

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

            static constexpr uint64_t   cInvalidIndex = std::numeric_limits<uint64_t>::max();
            static constexpr const char cpBuffersBinFileName[] = "Buffers.bin";
            static constexpr const char cpTexturesBinFileName[] = "Textures.bin";

            bool        PackData(
                            const Document& json, 
                            const char* pSrcRootPath, 
                            const char* pDestRootPath, 
                            uint64_t& oTexturesByteSize, 
                            uint64_t& oMeshesByteSize);

            bool        PackTextures(const Document &json, const char *pSrcRootPath, const char *pDestRootPath, uint64_t& oByteSize);
            bool        PackMeshes(const Document &json, const char *pSrcRootPath, const char *pDestRootPath, uint64_t& oByteSize);

            bool        InsertMeta(const Document& json, FILE* pDBFile);
            bool        InsertTexturesMeta(const Document& json, FILE* pDBFile);
            bool        InsertMeshesMeta(const Document& json, FILE* pDBFile);

            uint64_t    GetTextureIndex(const Document& json, uint64_t materialIndex);
            int64_t     CompressTexture(const char* pSrcFilePath, FILE* pDestFile);

        private:

            Vector<PackedBufferMeta> m_texturesMeta { 100 };
            Vector<PackedBufferMeta> m_buffersMeta { 100 };
        };
    }
}
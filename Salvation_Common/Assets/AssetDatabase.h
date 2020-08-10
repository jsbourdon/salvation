#pragma once

#include "Salvation_Common/Assets/Texture.h"
#include "Salvation_Common/Assets/Mesh.h"

namespace salvation
{
    namespace asset
    {
        struct Mesh;
        struct Texture;

        static constexpr size_t cMaxRscFilePathLen = 1024;

        struct AssetDatabaseHeader
        {
            char        m_pPackedBuffersFileName[16];
            char        m_pPackedTexturesFileName[16];
            uint32_t    m_meshCount;
            uint32_t    m_textureCount;
        };

        struct AssetDatabase
        {
            AssetDatabaseHeader m_header;
            uint8_t             m_Data[1];
        };

        AssetDatabase*  LoadDatabase(const char* pFilePath);
        void            DestroyDatabase(AssetDatabase* pDatabase);
        const Texture*  GetTextures(const AssetDatabase* pDatabase);
        const Mesh*     GetMeshes(const AssetDatabase* pDatabase);
    }
}
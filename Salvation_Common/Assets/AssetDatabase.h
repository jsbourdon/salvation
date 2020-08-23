#pragma once

#include "Salvation_Common/Assets/Texture.h"
#include "Salvation_Common/Assets/Mesh.h"

namespace salvation
{
    namespace asset
    {
        static constexpr size_t     cMaxRscFilePathLen = 1024;
        static constexpr uint32_t   cMagicNumber = 0xBADC0DE;
        static constexpr uint32_t   cVersion = 1;
        static constexpr size_t     cDataAlignment = std::max(alignof(Texture), alignof(Mesh));

        // Align header so data following it is properly aligned for either textures or meshes
        struct alignas(cHeaderAlignment) AssetDatabaseHeader
        {
            uint32_t    m_magicNumber { cMagicNumber };
            uint32_t    m_version { cVersion };
            uint64_t    m_textureByteSize { 0 };
            uint64_t    m_meshByteSize { 0 };
            char        m_pPackedBuffersFileName[16] { 0 };
            char        m_pPackedTexturesFileName[16] { 0 };
            uint32_t    m_meshCount { 0 };
            uint32_t    m_textureCount { 0 };
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
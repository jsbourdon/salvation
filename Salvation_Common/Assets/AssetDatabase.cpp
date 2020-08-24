#include <pch.h>
#include "AssetDatabase.h"
#include "Salvation_Common/FileSystem/FileSystem.h"
#include "Salvation_Common/Assets/Texture.h"

using namespace salvation::asset;
using namespace salvation::filesystem;

AssetDatabase* salvation::asset::LoadDatabase(const char* pFilePath)
{
    size_t fileSize;
    uint8_t* pData = ReadFileContent<ThreadHeapAllocator>(pFilePath, fileSize);

    if (fileSize < sizeof(AssetDatabaseHeader))
    {
        SALVATION_ASSERT_MSG(false, "Invalid database file");
        return nullptr;
    }

    AssetDatabase* pDatabase = reinterpret_cast<AssetDatabase*>(pData);
    if (pDatabase->m_header.m_magicNumber != cMagicNumber)
    {
        SALVATION_ASSERT_MSG(false, "Invalid database. Wrong magic number.");
        return nullptr;
    }

    return pDatabase;
}

void salvation::asset::DestroyDatabase(AssetDatabase* pDatabase)
{
    ThreadHeapAllocator::Release(pDatabase);
}

const Texture* salvation::asset::GetTextures(const AssetDatabase* pDatabase)
{
    const Texture* pTextures = reinterpret_cast<const Texture*>(pDatabase->m_Data);
    return pTextures;
}

const Mesh* salvation::asset::GetMeshes(const AssetDatabase* pDatabase)
{
    const uint32_t textureCount = pDatabase->m_header.m_textureCount;
    const Mesh* pMeshes = reinterpret_cast<const Mesh*>(pDatabase->m_Data + (textureCount * sizeof(Texture)));
    return pMeshes;
}
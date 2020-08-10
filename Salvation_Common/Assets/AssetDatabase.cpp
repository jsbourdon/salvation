#include <pch.h>
#include "AssetDatabase.h"
#include "Salvation_Common/FileSystem/FileSystem.h"

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
    const uint32_t meshCount = pDatabase->m_header.m_meshCount;
    const uint32_t textureCount = pDatabase->m_header.m_textureCount;
    const size_t dbSize = sizeof(AssetDatabaseHeader) + (meshCount * sizeof(Mesh)) + (textureCount * sizeof(Texture));

    if (fileSize != dbSize)
    {
        SALVATION_ASSERT_MSG(false, "Invalid database file");
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
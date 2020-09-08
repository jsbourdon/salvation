#include <pch.h>
#include "AssetDatabaseBuilder.h"
#include "salvation_core/Memory/ThreadHeapAllocator.h"
#include "salvation_core/Memory/ThreadHeapSmartPointer.h"
#include "salvation_core/FileSystem/FileSystem.h"
#include "salvation_core/Assets/AssetDatabase.h"
#include "salvation_core/Assets/Texture.h"
#include "rapidjson/document.h"
#include "3rd/Compressonator/Compressonator/CMP_Framework/CMP_Framework.h"

using namespace asset_assembler::database;
using namespace salvation;
using namespace salvation::asset;
using namespace salvation::memory;
using namespace salvation::filesystem;

template<typename T>
static bool AssetDatabaseBuilder::WriteData(const T& value, FILE* pFile)
{
    return fwrite(&value, sizeof(T), 1, pFile) == 1;
}

bool AssetDatabaseBuilder::BuildDatabase(const char* pSrcPath, const char* pDstPath)
{
    m_buffersMeta.Clear();
    m_texturesMeta.Clear();

    size_t jsonContentSize;
    char* pJsonContent = reinterpret_cast<char*>(ReadFileContent<ThreadHeapAllocator>(pSrcPath, jsonContentSize));

    if (!pJsonContent)
    {
        return false;
    }

    Document json;
    json.Parse(pJsonContent);

    // Pack data
    {
        ThreadHeapAllocator::Release(pJsonContent);

        const char* pDstRootPathEnd = strrchr(pDstPath, '/');
        const char* pSrcRootPathEnd = strrchr(pSrcPath, '/');

        if (!pDstRootPathEnd || !pSrcRootPathEnd)
        {
            return false;
        }

        size_t dstRootFolderStrLen =
            static_cast<size_t>(reinterpret_cast<uintptr_t>(pDstRootPathEnd) - reinterpret_cast<uintptr_t>(pDstPath)) + 1;
        size_t srcRootFolderStrLen =
            static_cast<size_t>(reinterpret_cast<uintptr_t>(pSrcRootPathEnd) - reinterpret_cast<uintptr_t>(pSrcPath)) + 1;

        char* pDstRootPath = static_cast<char*>(salvation::memory::StackAlloc(dstRootFolderStrLen + 1));
        char* pSrcRootPath = static_cast<char*>(salvation::memory::StackAlloc(srcRootFolderStrLen + 1));

        pDstRootPath[dstRootFolderStrLen] = 0;
        pSrcRootPath[srcRootFolderStrLen] = 0;

        memcpy(pDstRootPath, pDstPath, dstRootFolderStrLen);
        memcpy(pSrcRootPath, pSrcPath, srcRootFolderStrLen);

        if (!PackData(json, pSrcRootPath, pDstRootPath))
        {
            return false;
        }
    }

    // Write metadata
    {
        FILE* pDBFile = nullptr;
        if (fopen_s(&pDBFile, pDstPath, "wb") != 0)
        {
            return false;
        }

        FileHandleRAII fileRAII(pDBFile);

        AssetDatabaseHeader header {};
        strcpy_s(header.m_pPackedBuffersFileName, cpBuffersBinFileName);
        strcpy_s(header.m_pPackedTexturesFileName, cpTexturesBinFileName);

        header.m_meshCount = m_buffersMeta.Size();
        header.m_textureCount = m_texturesMeta.Size();

        if (!WriteData(header, pDBFile))
        {
            return false;
        }

        if (!InsertMeta(json, pDBFile))
        {
            return false;
        }
    }

    return true;
}

bool AssetDatabaseBuilder::PackData(
    const Document& json,
    const char* pSrcRootPath,
    const char* pDestRootPath)
{
    return
        PackTextures(json, pSrcRootPath, pDestRootPath) &&
        PackBuffers(json, pSrcRootPath, pDestRootPath);
}

bool AssetDatabaseBuilder::PackTextures(const Document &json, const char *pSrcRootPath, const char *pDestRootPath)
{
    static constexpr const char s_pTexturesBinFileName[] = "Textures.bin";
    static constexpr const char s_pImgProperty[] = "images";
    static constexpr const char s_pUriProperty[] = "uri";

    if (json.HasMember(s_pImgProperty) && json[s_pImgProperty].IsArray())
    {
        const Value &images = json[s_pImgProperty];
        SizeType imageCount = images.Size();

        if (imageCount > 0)
        {
            str_smart_ptr pDestFilePath = salvation::filesystem::AppendPaths(pDestRootPath, s_pTexturesBinFileName);
            FILE *pDestFile = nullptr;

            if (fopen_s(&pDestFile, pDestFilePath, "wb") != 0)
            {
                return false;
            }

            FileHandleRAII fileRAII(pDestFile);

            int64_t currentByteOffset = 0;

            for (SizeType i = 0; i < imageCount; ++i)
            {
                const Value &img = images[i];
                if (img.HasMember(s_pUriProperty) && img[s_pUriProperty].IsString())
                {
                    const Value &uri = img[s_pUriProperty];
                    const char *pTextureUri = uri.GetString();

                    str_smart_ptr pSrcFilePath = salvation::filesystem::AppendPaths(pSrcRootPath, pTextureUri);
                    int64_t textureByteSize = CompressTexture(pSrcFilePath, pDestFile);
                    if (textureByteSize <= 0)
                    {
                        return false;
                    }

                    m_texturesMeta.Emplace(currentByteOffset, textureByteSize);
                    currentByteOffset += textureByteSize;
                }
            }
        }
    }

    return true;
}

bool AssetDatabaseBuilder::PackBuffers(const Document &json, const char *pSrcRootPath, const char *pDestRootPath)
{
    static constexpr const char cpBuffersBinFileName[] = "Buffers.bin";
    static constexpr const char cpBuffersProperty[] = "buffers";
    static constexpr const char cpUriProperty[] = "uri";

    if (json.HasMember(cpBuffersProperty) && json[cpBuffersProperty].IsArray())
    {
        const Value &buffers = json[cpBuffersProperty];
        const SizeType bufferCount = buffers.Size();

        if (bufferCount > 0)
        {
            str_smart_ptr destFilePath = salvation::filesystem::AppendPaths(pDestRootPath, cpBuffersBinFileName);

            FILE *pDestFile = nullptr;
            if (fopen_s(&pDestFile, destFilePath, "wb") != 0)
            {
                return false;
            }

            FileHandleRAII fileRAII(pDestFile);

            uint64_t currentByteOffset = 0;

            for (SizeType i = 0; i < bufferCount; ++i)
            {
                const Value &buffer = buffers[i];
                if (buffer.HasMember(cpUriProperty) && buffer[cpUriProperty].IsString())
                {
                    const Value &uri = buffer[cpUriProperty];
                    const char *pBufferUri = uri.GetString();

                    size_t fileSize = 0;
                    str_smart_ptr pSrcFilePath = salvation::filesystem::AppendPaths(pSrcRootPath, pBufferUri);
                    uint8_t *pData = ReadFileContent<ThreadHeapAllocator>(pSrcFilePath, fileSize);

                    if (fileSize == 0 || fwrite(pData, sizeof(uint8_t), fileSize, pDestFile) != fileSize)
                    {
                        return false;
                    }

                    m_buffersMeta.Emplace(currentByteOffset, fileSize);

                    currentByteOffset += static_cast<uint64_t>(fileSize);

                    ThreadHeapAllocator::Release(pData);
                }
            }
        }
    }

    return true;
}

bool AssetDatabaseBuilder::InsertMeta(const Document& json, FILE* pDBFile)
{
    return
        InsertTexturesMeta(json, pDBFile) &&
        InsertMeshesMeta(json, pDBFile);
}

bool AssetDatabaseBuilder::InsertTexturesMeta(const Document& json, FILE* pDBFile)
{
    uint32_t textureCount = m_texturesMeta.Size();
    for (uint32_t i = 0; i < textureCount; ++i)
    {
        const PackedBufferMeta& meta = m_texturesMeta[i];
        Texture texture { meta.m_byteSize, meta.m_byteOffset, TextureFormat::BC3 };

        if (!WriteData(texture, pDBFile))
        {
            return false;
        }
    }

    return true;
}

bool AssetDatabaseBuilder::InsertMeshesMeta(const Document& json, FILE* pDBFile)
{
    static constexpr const char cpMeshesProperty[] = "meshes";
    static constexpr const char cpSubMeshesProperty[] = "primitives";
    static constexpr const char cpIndicesProperty[] = "indices";
    static constexpr const char cpMaterialProperty[] = "material";
    static constexpr const char cpAttributesProperty[] = "attributes";

    if (json.HasMember(cpMeshesProperty) && json[cpMeshesProperty].IsArray())
    {
        const Value& meshes = json[cpMeshesProperty];
        const SizeType meshCount = meshes.Size();

        // Write every mesh
        for (SizeType meshIndex = 0; meshIndex < meshCount; ++meshIndex)
        {
            const Value& mesh = meshes[meshIndex];

            if (mesh.HasMember(cpSubMeshesProperty) && mesh[cpSubMeshesProperty].IsArray())
            {
                const Value& subMeshes = mesh[cpSubMeshesProperty];
                const SizeType subMeshCount = subMeshes.Size();

                Mesh meshData { subMeshCount };
                if (!WriteData(meshData, pDBFile))
                {
                    return false;
                }

                // Write sub meshes for current mesh
                for (SizeType subMeshIndex = 0; subMeshIndex < subMeshCount; ++subMeshIndex)
                {
                    const Value& subMesh = subMeshes[subMeshIndex];

                    if (
                        subMesh.HasMember(cpIndicesProperty) && subMesh[cpIndicesProperty].IsInt() &&
                        subMesh.HasMember(cpMaterialProperty) && subMesh[cpMaterialProperty].IsInt() &&
                        subMesh.HasMember(cpAttributesProperty) && subMesh[cpAttributesProperty].IsObject())
                    {
                        SubMesh subMeshData {};

                        const SizeType indexBufferIndex = subMesh[cpIndicesProperty].GetInt();
                        const SizeType materialIndex = subMesh[cpMaterialProperty].GetInt();
                        const Value& attributes = subMesh[cpAttributesProperty];

                        subMeshData.m_textureIndex = GetTextureIndex(json, materialIndex);
                        subMeshData.m_streamCount = GetSupportedAttributeCount(attributes);
                        GetBufferView(json, indexBufferIndex, subMeshData.m_indexBuffer);
                        
                        if (!WriteData(subMeshData, pDBFile))
                        {
                            return false;
                        }

                        // Write streams for current sub mesh
                        for (size_t i = 0; i < SALVATION_ARRAY_SIZE(cppVertexAttributeSemantics); ++i)
                        {
                            const char* pAttributeSemantic = cppVertexAttributeSemantics[i];
                            if (attributes.HasMember(pAttributeSemantic) && attributes[pAttributeSemantic].IsInt())
                            {
                                const SizeType accessorIndex = attributes[pAttributeSemantic].GetInt();
                                VertexStream stream {};
                                stream.m_attribute = static_cast<VertexAttribute>(i);
                                GetBufferView(json, accessorIndex, stream);

                                if (!WriteData(stream, pDBFile))
                                {
                                    return false;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return true;
}

uint32_t AssetDatabaseBuilder::GetSupportedAttributeCount(const Value& attributes)
{
    SALVATION_ASSERT(attributes.IsObject());

    uint32_t count = 0;

    for (size_t i = 0; i < SALVATION_ARRAY_SIZE(cppVertexAttributeSemantics); ++i)
    {
        const char* pAttributeSemantic = cppVertexAttributeSemantics[i];
        if (attributes.HasMember(pAttributeSemantic) && attributes[pAttributeSemantic].IsInt())
        {
            ++count;
        }
    }

    return count;
}

uint32_t AssetDatabaseBuilder::GetTextureIndex(const Document& json, SizeType materialIndex)
{
    static constexpr const char s_pTexturesProperty[] = "textures";
    static constexpr const char s_pMaterialsProperty[] = "materials";
    static constexpr const char s_pPBRProperty[] = "pbrMetallicRoughness";
    static constexpr const char s_pBaseTextureProperty[] = "baseColorTexture";
    static constexpr const char s_pIndexProperty[] = "index";
    static constexpr const char s_pSourceProperty[] = "source";

    uint32_t index = cInvalidIndex;

    if (json.HasMember(s_pMaterialsProperty) && json[s_pMaterialsProperty].IsArray())
    {
        const Value& materials = json[s_pMaterialsProperty];
        const SizeType materialCount = materials.Size();

        //for (SizeType i = 0; i < materialCount; ++i)
        if (materialIndex < materialCount)
        {
            const Value& material = materials[materialIndex];
            if (material.HasMember(s_pPBRProperty) && material[s_pPBRProperty].IsObject())
            {
                const Value& pbr = material[s_pPBRProperty];
                if (pbr.HasMember(s_pBaseTextureProperty) && pbr[s_pBaseTextureProperty].IsObject())
                {
                    const Value& baseTexture = pbr[s_pBaseTextureProperty];
                    if (baseTexture.HasMember(s_pIndexProperty) && baseTexture[s_pIndexProperty].IsInt())
                    {
                        const Value& indexProperty = baseTexture[s_pIndexProperty];
                        SizeType textureIndex = indexProperty.GetInt();

                        if (json.HasMember(s_pTexturesProperty) && json[s_pTexturesProperty].IsArray())
                        {
                            const Value& textures = json[s_pTexturesProperty];
                            const SizeType textureCount = textures.Size();
                            if (textureIndex < textureCount)
                            {
                                const Value& texture = textures[textureIndex];
                                if (texture.HasMember(s_pSourceProperty) && texture[s_pSourceProperty].IsInt())
                                {
                                    index = texture[s_pSourceProperty].GetInt();
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return index;
}

void AssetDatabaseBuilder::GetBufferView(const Document& json, SizeType accessorIndex, BufferView& oView)
{
    static constexpr const char s_pAccessorsProperty[] = "accessors";
    static constexpr const char s_pBufferViewsProperty[] = "bufferViews";
    static constexpr const char s_pBufferViewProperty[] = "bufferView";
    static constexpr const char s_pByteOffsetProperty[] = "byteOffset";
    static constexpr const char s_pByteLengthProperty[] = "byteLength";
    static constexpr const char s_pByteStrideProperty[] = "byteStride";

    if (json.HasMember(s_pAccessorsProperty) && json[s_pAccessorsProperty].IsArray() &&
        json.HasMember(s_pBufferViewsProperty) && json[s_pBufferViewsProperty].IsArray())
    {
        const Value& accessors = json[s_pAccessorsProperty];
        const Value& bufferViews = json[s_pBufferViewsProperty];
        const SizeType accessorCount = accessors.Size();

        SALVATION_ASSERT(accessorIndex < accessorCount);

        const Value& accessor = accessors[accessorIndex];
        if (accessor.HasMember(s_pBufferViewProperty) && accessor[s_pBufferViewProperty].IsInt())
        {
            const int bufferViewIndex = accessor[s_pBufferViewProperty].GetInt();
            const Value& bufferView = bufferViews[bufferViewIndex];

            if (bufferView.HasMember(s_pByteLengthProperty) && bufferView[s_pByteLengthProperty].IsInt())
            {
                oView.m_byteSize = bufferView[s_pByteLengthProperty].GetInt();

                uint64_t accessorByteOffset = 0;
                uint64_t bufferViewByteOffset = 0;
                uint64_t bufferViewByteStride = 1;

                if (accessor.HasMember(s_pByteOffsetProperty) && accessor[s_pByteOffsetProperty].IsInt())
                {
                    accessorByteOffset = accessor[s_pByteOffsetProperty].GetInt();
                }

                if (bufferView.HasMember(s_pByteOffsetProperty) && bufferView[s_pByteOffsetProperty].IsInt())
                {
                    bufferViewByteOffset = bufferView[s_pByteOffsetProperty].GetInt();
                }

                if (bufferView.HasMember(s_pByteStrideProperty) && bufferView[s_pByteStrideProperty].IsInt())
                {
                    bufferViewByteStride = bufferView[s_pByteStrideProperty].GetInt();
                }

                oView.m_byteOffset = accessorByteOffset + bufferViewByteOffset;
                oView.m_byteStride = bufferViewByteStride;
            }
        }
    }
}

/// CMP_Feedback_Proc
/// Feedback function for conversion.
/// \param[in] fProgress The percentage progress of the texture compression.
/// \return non-NULL(true) value to abort conversion
static bool CMP_Feedback(CMP_FLOAT fProgress, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2)
{
    return false;
}

int64_t AssetDatabaseBuilder::CompressTexture(const char* pSrcFilePath, FILE* pDestFile)
{
    int64_t byteSize = 0;

    CMP_MipSet mipSetIn = {};
    CMP_MipSet mipSetOut = {};

    CMP_ERROR result = CMP_LoadTexture(pSrcFilePath, &mipSetIn);

    if (result == CMP_OK)
    {
        // Generate MIP chain if not already generated
        if (mipSetIn.m_nMipLevels <= 1)
        {
            static constexpr CMP_INT s_MinMipSize = 4; // 4x4
            CMP_GenerateMIPLevels(&mipSetIn, s_MinMipSize);
        }

        // Compress texture into BC3 for now #todo provide format as argument
        {
            KernelOptions kernelOptions = {};
            kernelOptions.format = CMP_FORMAT_BC3;
            kernelOptions.fquality = 1.0f;
            kernelOptions.threads = 0; // Auto setting

            result = CMP_ProcessTexture(&mipSetIn, &mipSetOut, kernelOptions, &CMP_Feedback);

            if (result == CMP_OK)
            {
                // #todo Properly save the whole mip chain
                for (int i = 0; i < 1/*mipSetOut.m_nMipLevels*/; ++i)
                {
                    CMP_MipLevel* pMipData;
                    CMP_GetMipLevel(&pMipData, &mipSetOut, i, 0);
                    int64_t mipByteSize = pMipData->m_dwLinearSize;

                    if (fwrite(pMipData->m_pbData, sizeof(uint8_t), mipByteSize, pDestFile) != mipByteSize)
                    {
                        byteSize = -1;
                        break;
                    }

                    byteSize += mipByteSize;
                }
            }
        }
    }

    CMP_FreeMipSet(&mipSetIn);
    CMP_FreeMipSet(&mipSetOut);

    return byteSize;
}

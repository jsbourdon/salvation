#include <pch.h>
#include "AssetDatabaseBuilder.h"
#include "Salvation_Common/Memory/ThreadHeapAllocator.h"
#include "Salvation_Common/Memory/ThreadHeapSmartPointer.h"
#include "Salvation_Common/FileSystem/FileSystem.h"
#include "Salvation_Common/Assets/AssetDatabase.h"
#include "Salvation_Common/sqlite/sqlite3.h"
#include "rapidjson/document.h"
#include "3rd/Compressonator/Compressonator/CMP_Framework/CMP_Framework.h"

using namespace asset_assembler::database;
using namespace salvation;
using namespace salvation::asset;
using namespace salvation::memory;
using namespace salvation::filesystem;


/// CMP_Feedback_Proc
/// Feedback function for conversion.
/// \param[in] fProgress The percentage progress of the texture compression.
/// \return non-NULL(true) value to abort conversion
static bool CMP_Feedback(CMP_FLOAT fProgress, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2)
{
    return false;
}

int64_t AssetDatabaseBuilder::CompressTexture(const char *pSrcFilePath, FILE *pDestFile)
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
                    CMP_MipLevel *pMipData;
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

/*
int64_t AssetDatabaseBuilder::InsertPackagedDataEntry(const char *pFilePath, salvation::asset::PackedDataType dataType)
{
    int64_t packageID = -1;
    
    // #todo Implement

    return packageID;
}

bool AssetDatabaseBuilder::InsertTextureDataEntry(int64_t byteSize, int64_t byteOffset, int32_t format, int64_t packedDataId)
{
    // #todo Implement
}

bool AssetDatabaseBuilder::InsertBufferDataEntry(int64_t byteSize, int64_t byteOffset, int64_t packedDataId)
{
    // #todo Implement
}

bool AssetDatabaseBuilder::InsertMaterialDataEntry(int64_t textureId)
{
    // #todo Implement
}

bool AssetDatabaseBuilder::InsertBufferViewDataEntry(int64_t bufferId, int64_t byteSize, int64_t byteOffset, int32_t stride)
{
    // #todo Implement
}

int64_t AssetDatabaseBuilder::InsertMeshDataEntry(const char *pName)
{
    // #todo Implement
}

int64_t AssetDatabaseBuilder::InsertSubMeshDataEntry(int64_t meshId, int64_t indexBufferViewId, int64_t materialId)
{
    // #todo Implement
}

bool AssetDatabaseBuilder::InsertVertexStreamDataEntry(int64_t subMeshId, int64_t bufferViewId, int32_t attribute)
{
    // #todo Implement
}

bool AssetDatabaseBuilder::UpdatePackagedDataEntry(int64_t packagedDataId, int64_t byteSize)
{
    // #todo Implement
}

FILE* AssetDatabaseBuilder::CreateMetadataFile()
{
    //static constexpr const char cpTexturesBinFileName[] = "Textures.bin";
    //static constexpr const char cpBuffersBinFileName[] = "Buffers.bin";
}
*/

bool AssetDatabaseBuilder::BuildTextures(const Document &json, const char *pSrcRootPath, const char *pDestRootPath)
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

                    m_texturesMeta.emplace_back(currentByteOffset, textureByteSize);
                    currentByteOffset += textureByteSize;
                }
            }
        }
    }

    return true;
}

bool AssetDatabaseBuilder::BuildMeshes(const Document &json, const char *pSrcRootPath, const char *pDestRootPath)
{
    static constexpr const char cpBuffersBinFileName[] = "Buffers.bin";
    static constexpr const char cpBuffersProperty[] = "buffers";
    static constexpr const char cpUriProperty[] = "uri";

    if (json.HasMember(cpBuffersProperty) && json[cpBuffersProperty].IsArray())
    {
        const Value &buffers = json[cpBuffersProperty];
        SizeType bufferCount = buffers.Size();

        if (bufferCount > 0)
        {
            str_smart_ptr destFilePath = salvation::filesystem::AppendPaths(pDestRootPath, cpBuffersBinFileName);

            FILE *pDestFile = nullptr;
            if (fopen_s(&pDestFile, destFilePath, "wb") != 0)
            {
                return false;
            }

            bool writeSucceeded = true;
            uint64_t currentByteOffset = 0;

            for (SizeType i = 0; i < bufferCount && writeSucceeded; ++i)
            {
                const Value &buffer = buffers[i];
                if (buffer.HasMember(cpUriProperty) && buffer[cpUriProperty].IsString())
                {
                    const Value &uri = buffer[cpUriProperty];
                    const char *pBufferUri = uri.GetString();

                    size_t fileSize = 0;
                    str_smart_ptr pSrcFilePath = salvation::filesystem::AppendPaths(pSrcRootPath, pBufferUri);
                    uint8_t *pData = ReadFileContent<ThreadHeapAllocator>(pSrcFilePath, fileSize);

                    writeSucceeded =
                        fileSize > 0 &&
                        fwrite(pData, sizeof(uint8_t), fileSize, pDestFile) == fileSize;

                    m_buffersMeta.push_back({ currentByteOffset, fileSize });

                    currentByteOffset += static_cast<uint64_t>(fileSize);

                    ThreadHeapAllocator::Release(pData);
                }
            }

            fclose(pDestFile);

            return writeSucceeded;
        }
    }

    return true;
}

/*
bool AssetDatabaseBuilder::InsertMaterialMetadata(Document &json)
{
    static constexpr const char s_pMaterialsProperty[] = "materials";
    static constexpr const char s_pPBRProperty[] = "pbrMetallicRoughness";
    static constexpr const char s_pBaseTextureProperty[] = "baseColorTexture";
    static constexpr const char s_pIndexProperty[] = "index";

    if (json.HasMember(s_pMaterialsProperty) && json[s_pMaterialsProperty].IsArray())
    {
        Value &materials = json[s_pMaterialsProperty];
        SizeType materialCount = materials.Size();

        for (SizeType i = 0; i < materialCount; ++i)
        {
            Value &material = materials[i];
            if (material.HasMember(s_pPBRProperty) && material[s_pPBRProperty].IsObject())
            {
                Value &pbr = material[s_pPBRProperty];
                if (pbr.HasMember(s_pBaseTextureProperty) && pbr[s_pBaseTextureProperty].IsObject())
                {
                    Value &baseTexture = pbr[s_pBaseTextureProperty];
                    if (baseTexture.HasMember(s_pIndexProperty) && baseTexture[s_pIndexProperty].IsInt())
                    {
                        Value &indexProperty = baseTexture[s_pIndexProperty];
                        int index = indexProperty.GetInt() + 1; // +1 since sqlite integer primary keys start at 1

                        if (!InsertMaterialDataEntry(index))
                        {
                            return false;
                        }
                    }
                }
            }
        }
    }

    return true;
}

ComponentType AssetDatabaseBuilder::GetComponentType(const char *pGLTFType, int glTFComponentType)
{
    static constexpr uint32_t cglTFByteCode = 5120;
    static constexpr uint32_t cglTFUnsignedByteCode = 5121;
    static constexpr uint32_t cglTFShortCode = 5122;
    static constexpr uint32_t cglTFUnsignedShortCode = 5123;
    static constexpr uint32_t cglTUnsignedIntCode = 5125;
    static constexpr uint32_t cglTFFloatCode = 5126;

    static constexpr const char *s_pGLTFVectorTypes[] =
    {
        "VEC2",
        "VEC3",
        "VEC4",
        "MAT2",
        "MAT3",
        "MAT4"
    };

    static constexpr ComponentType s_VectorTypes[] =
    {
        ComponentType::Vec2,
        ComponentType::Vec3,
        ComponentType::Vec4,
        ComponentType::Matrix2x2,
        ComponentType::Matrix3x3,
        ComponentType::Matrix4x4
    };

    if (strcmp(pGLTFType, "SCALAR") == 0)
    {
        switch (glTFComponentType)
        {
        case cglTFByteCode:
        case cglTFUnsignedByteCode:
            return ComponentType::Scalar_Byte;
        case cglTFShortCode:
        case cglTFUnsignedShortCode:
            return ComponentType::Scalar_Short;
        case cglTUnsignedIntCode:
            return ComponentType::Scalar_Int;
        case cglTFFloatCode:
            return ComponentType::Scalar_Float;
        default:
            return ComponentType::Unknown;
        }
    }

    for (size_t i = 0; i < ARRAY_SIZE(s_pGLTFVectorTypes); ++i)
    {
        if (strcmp(pGLTFType, s_pGLTFVectorTypes[i]) == 0)
        {
            return s_VectorTypes[i];
        }
    }

    return ComponentType::Unknown;
}

bool AssetDatabaseBuilder::InsertBufferViewMetadata(Document &json)
{
    static constexpr const char s_pAccessorsProperty[] = "accessors";
    static constexpr const char s_pBufferViewsProperty[] = "bufferViews";
    static constexpr const char s_pBufferViewProperty[] = "bufferView";
    static constexpr const char s_pByteOffsetProperty[] = "byteOffset";
    static constexpr const char s_pComponentTypeProperty[] = "componentType";
    static constexpr const char s_pTypeProperty[] = "type";
    static constexpr const char s_pCountProperty[] = "count";
    static constexpr const char s_pBufferProperty[] = "buffer";

    if (
        json.HasMember(s_pAccessorsProperty) && json[s_pAccessorsProperty].IsArray() &&
        json.HasMember(s_pBufferViewsProperty) && json[s_pBufferViewsProperty].IsArray())
    {
        Value &bufferViews = json[s_pBufferViewsProperty];
        Value &accessors = json[s_pAccessorsProperty];
        SizeType accessorCount = accessors.Size();

        for (SizeType i = 0; i < accessorCount; ++i)
        {
            Value &accessor = accessors[i];
            if (accessor.HasMember(s_pBufferViewProperty) && accessor[s_pBufferViewProperty].IsInt())
            {
                int bufferViewIndex = accessor[s_pBufferViewProperty].GetInt();
                Value &bufferView = bufferViews[bufferViewIndex];

                if (
                    accessor.HasMember(s_pTypeProperty) && accessor[s_pTypeProperty].IsString() &&
                    accessor.HasMember(s_pCountProperty) && accessor[s_pCountProperty].IsInt() &&
                    accessor.HasMember(s_pComponentTypeProperty) && accessor[s_pComponentTypeProperty].IsInt() &&
                    bufferView.HasMember(s_pBufferProperty) && bufferView[s_pBufferProperty].IsInt())
                {
                    int64_t bufferId = bufferView[s_pBufferProperty].GetInt() + 1; // +1 since sqlite integer primary keys start at 1

                    const char *pType = accessor[s_pTypeProperty].GetString();
                    int glTFComponentType = accessor[s_pComponentTypeProperty].GetInt();
                    int count = accessor[s_pCountProperty].GetInt();

                    ComponentType componentType = GetComponentType(pType, glTFComponentType);
                    int32_t stride = AssetDatabase::ComponentTypeByteSize(componentType);
                    int64_t byteSize = stride * static_cast<int64_t>(count);

                    int64_t accessorByteOffset = 0;
                    int64_t bufferViewByteOffset = 0;

                    if (accessor.HasMember(s_pByteOffsetProperty) && accessor[s_pByteOffsetProperty].IsInt())
                    {
                        accessorByteOffset = accessor[s_pByteOffsetProperty].GetInt();
                    }

                    if (bufferView.HasMember(s_pByteOffsetProperty) && bufferView[s_pByteOffsetProperty].IsInt())
                    {
                        bufferViewByteOffset = bufferView[s_pByteOffsetProperty].GetInt();
                    }

                    int64_t byteOffset = accessorByteOffset + bufferViewByteOffset;

                    if (!InsertBufferViewDataEntry(bufferId, byteSize, byteOffset, stride))
                    {
                        return false;
                    }
                }
            }
        }
    }


    return true;
}

bool AssetDatabaseBuilder::InsertMeshMetadata(Document &json)
{
    static constexpr const char cpMeshesProperty[] = "meshes";
    static constexpr const char cpPrimitivesProperty[] = "primitives";
    static constexpr const char cpIndicesProperty[] = "indices";
    static constexpr const char cpMaterialProperty[] = "material";
    static constexpr const char cpAttributesProperty[] = "attributes";

    if (json.HasMember(cpMeshesProperty) && json[cpMeshesProperty].IsArray())
    {
        Value &meshes = json[cpMeshesProperty];
        SizeType meshCount = meshes.Size();

        for (SizeType meshIndex = 0; meshIndex < meshCount; ++meshIndex)
        {
            Value &mesh = meshes[meshIndex];

            if (mesh.HasMember(cpPrimitivesProperty) && mesh[cpPrimitivesProperty].IsArray())
            {
                Value &primitives = mesh[cpPrimitivesProperty];
                SizeType primCount = primitives.Size();

                for (SizeType primIndex = 0; primIndex < primCount; ++primIndex)
                {
                    Value &primitive = primitives[primIndex];

                    if (
                        primitive.HasMember(cpIndicesProperty) && primitive[cpIndicesProperty].IsInt() &&
                        primitive.HasMember(cpMaterialProperty) && primitive[cpMaterialProperty].IsInt() &&
                        primitive.HasMember(cpAttributesProperty) && primitive[cpAttributesProperty].IsObject())
                    {
                        // +1 since sqlite integer primary keys start at 1
                        int64_t indexBufferId = primitive[cpIndicesProperty].GetInt() + 1;
                        int64_t materialId = primitive[cpMaterialProperty].GetInt() + 1;

                        int64_t meshId = InsertMeshDataEntry("Default Name");
                        int64_t subMeshId = InsertSubMeshDataEntry(meshId, indexBufferId, materialId);

                        Value &attributes = primitive[cpAttributesProperty];

                        if (meshId < 0 || subMeshId < 0 || !InsertVertexStreamsMetadata(attributes, subMeshId))
                        {
                            return false;
                        }
                    }
                }
            }
        }
    }

    return true;
}

bool AssetDatabaseBuilder::InsertVertexStreamsMetadata(Value &attributes, int64_t subMeshId)
{
    static constexpr const char *s_ppAttributeSemantics[] =
    {
        "POSITION",
        "NORMAL",
        "TANGENT",
        "TEXCOORD_0",
        "TEXCOORD_1",
        "COLOR_0"
    };

    static_assert(ARRAY_SIZE(s_ppAttributeSemantics) == static_cast<size_t>(AttributeSemantic::Count));

    for (size_t i = 0; i < ARRAY_SIZE(s_ppAttributeSemantics); ++i)
    {
        const char *pAttributeSemantic = s_ppAttributeSemantics[i];
        if (attributes.HasMember(pAttributeSemantic) && attributes[pAttributeSemantic].IsInt())
        {
            int bufferViewId = attributes[pAttributeSemantic].GetInt() + 1; // +1 since sqlite integer primary keys start at 1
            if (!InsertVertexStreamDataEntry(subMeshId, bufferViewId, static_cast<int32_t>(i)))
            {
                return false;
            }
        }
    }

    return true;
}

bool AssetDatabaseBuilder::InsertMetadata(Document &json)
{
    return 
        InsertMaterialMetadata(json) &&
        InsertBufferViewMetadata(json) && 
        InsertMeshMetadata(json);
}
*/

bool AssetDatabaseBuilder::BuildDatabase(const char *pSrcPath, const char *pDstPath)
{
    bool success = false;

    FILE* pDBFile = nullptr;
    if (fopen_s(&pDBFile, pDstPath, "wb") != 0)
    {
        return false;
    }

    FileHandleRAII fileRAII(pDBFile);

    AssetDatabaseHeader header;
    strcpy_s(header.m_pPackedBuffersFileName, cpBuffersBinFileName);
    strcpy_s(header.m_pPackedTexturesFileName, cpTexturesBinFileName);
    header.m_meshCount = header.m_textureCount = 0;

    if (fwrite(&header, sizeof(AssetDatabaseHeader), 1, pDBFile) != sizeof(AssetDatabaseHeader))
    {
        return false;
    }

    size_t jsonContentSize;
    char* pJsonContent = reinterpret_cast<char*>(ReadFileContent<ThreadHeapAllocator>(pSrcPath, jsonContentSize));

    if (pJsonContent)
    {
        Document json;
        json.Parse(pJsonContent);

        ThreadHeapAllocator::Release(pJsonContent);

        const char* pDstRootPathEnd = strrchr(pDstPath, '/');
        const char* pSrcRootPathEnd = strrchr(pSrcPath, '/');

        if (pDstRootPathEnd && pSrcRootPathEnd)
        {
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

            success =
                BuildTextures(json, pSrcRootPath, pDstRootPath) &&
                BuildMeshes(json, pSrcRootPath, pDstRootPath);
        }
    }

    return success;
}

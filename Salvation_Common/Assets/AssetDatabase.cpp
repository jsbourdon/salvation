#include <pch.h>
#include "AssetDatabase.h"

using namespace salvation::asset;

static constexpr size_t s_ComponentTypeByteSizes[] =
{
    4,  // Scalar_Float
    1,  // Scalar_Byte,
    2,  // Scalar_Short
    4,  // Scalar_Int
    8,  // Vec2
    12, // Vec3
    16, // Vec4
    16, // Matrix2x2
    36, // Matrix3x3
    64, // Matrix4x4
};

static_assert(ARRAY_SIZE(s_ComponentTypeByteSizes) == static_cast<size_t>(ComponentType::Count));

uint32_t AssetDatabase::ComponentTypeByteSize(ComponentType type)
{
    SALVATION_ASSERT(static_cast<int32_t>(type) >= 0 && static_cast<int32_t>(type) < static_cast<int32_t>(ComponentType::Count), 
        "AssetDatabase::ComponentTypeByteSize: Invalid component type");
    return static_cast<uint32_t>(s_ComponentTypeByteSizes[static_cast<size_t>(type)]);
}

AssetDatabase::AssetDatabase()
{

}

AssetDatabase::~AssetDatabase()
{

}

bool AssetDatabase::LoadMetadata(const char *pFilePath)
{
    return false;
}

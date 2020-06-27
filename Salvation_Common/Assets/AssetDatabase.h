#pragma once

namespace salvation
{
    namespace asset
    {
        enum class PackedDataType
        {
            Unknown = -1,
            Textures,
            Meshes,
            Count
        };

        enum class TextureFormat
        {
            Unknown = -1,
            RGBA,
            BC1,
            BC2,
            BC3,
            Count
        };

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

        enum class AttributeSemantic
        {
            Unknown = -1,
            Position,
            Normal,
            Tangent,
            TexCoord0,
            TexCoord1,
            Color,
            Count
        };

        class AssetDatabase
        {
        public:

            AssetDatabase();
            ~AssetDatabase();

            bool LoadMetadata(const char* pFolderPath, const char* pDbFileName);

            static uint32_t ComponentTypeByteSize(ComponentType type);

        private:

            static size_t ComputeRequiredMetadataByteSize(sqlite3* pDb);
        };
    }
}
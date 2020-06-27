#include <pch.h>
#include "AssetDatabase.h"
#include "Salvation_Common/sqlite/sqlite3.h"
#include "Salvation_Common/FileSystem/FileSystem.h"

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
    SALVATION_ASSERT_MSG(static_cast<int32_t>(type) >= 0 && static_cast<int32_t>(type) < static_cast<int32_t>(ComponentType::Count), 
        "AssetDatabase::ComponentTypeByteSize: Invalid component type");
    return static_cast<uint32_t>(s_ComponentTypeByteSizes[static_cast<size_t>(type)]);
}

AssetDatabase::AssetDatabase()
{

}

AssetDatabase::~AssetDatabase()
{

}

struct ScopedStatement
{
    ScopedStatement(sqlite3* pDb, const char* pStatement)
    {
        SALVATION_ASSERT_ALWAYS_EXEC(sqlite3_prepare_v2(pDb, pStatement, -1, &m_pStmt, nullptr) == SQLITE_OK);
    }

    ~ScopedStatement()
    {
        sqlite3_finalize(m_pStmt);
    }

    inline operator sqlite3_stmt* () { return m_pStmt; }

    sqlite3_stmt* m_pStmt { nullptr };
};

bool AssetDatabase::LoadMetadata(const char* pFolderPath, const char* pDbFileName)
{
    static constexpr const char* cPackedDataQuery = "SELECT ID, FilePath, ByteSize FROM PackedData ORDER BY ID;";

    sqlite3* pDb;
    str_smart_ptr spDbFilePath = filesystem::AppendPaths(pFolderPath, pDbFileName);

    int dbResult = sqlite3_open(spDbFilePath, &pDb);
    if (dbResult == SQLITE_OK)
    {
        ScopedStatement packedDataStmt(pDb, cPackedDataQuery);

        int code = sqlite3_step(packedDataStmt);
        while (code == SQLITE_ROW)
        {
            int id = sqlite3_column_int(packedDataStmt, 0);
            const char* filePath = reinterpret_cast<const char*>(sqlite3_column_text(packedDataStmt, 1));
            int byteSize = sqlite3_column_int(packedDataStmt, 2);
            code = sqlite3_step(packedDataStmt);
        }

        SALVATION_ASSERT(code == SQLITE_DONE);

        sqlite3_close(pDb);
        return true;
    }

    return false;
}

size_t AssetDatabase::ComputeRequiredMetadataByteSize(sqlite3* pDb)
{
    
}

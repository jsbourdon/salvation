#include "asset_assembler/database/AssetDatabaseBuilder.h"
#include "salvation_core/Memory/ThreadHeapAllocator.h"
#include "salvation_core/Core/Defines.h"
#include "salvation_core/FileSystem/FileSystem.h"

using namespace asset_assembler::database;
using namespace salvation::memory;
using namespace salvation;

int main(int argc, char* argv[])
{
    // All heavy memory allocations must go through salvation::memory::VirtualMemoryAllocator.
    ThreadHeapAllocator::Init(GiB(1), MiB(100));
    AssetDatabaseBuilder builder;

    SALVATION_ASSERT(argc >= 3);
    const char* pGltfFilePath = argv[1];
    const char* pDbFilePath = argv[2];

    bool success = builder.BuildDatabase("D:/GitHub/salvation/TestApp/Media/star_trek_danube_class/scene.gltf", "D:/Temp/StartTrek.db");
    //bool success = builder.BuildDatabase(pGltfFilePath, pDbFilePath);

    if (success)
    {
        printf_s("Asset generation successful");
    }
    else
    {
        printf_s("Asset generation FAILED!");
    }

    return 0;
}


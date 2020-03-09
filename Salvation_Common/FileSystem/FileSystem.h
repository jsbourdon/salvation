#pragma once

#include "Salvation_Common/Memory/ThreadHeapSmartPointer.h"

using namespace salvation::memory;

namespace salvation
{
    namespace filesystem
    {
        bool FileExists(const char *pFilePath);
        bool DirectoryExists(const char *pDirectoryPath);
        bool CreateDirectory(const char* pDirectoryPath);

        str_smart_ptr&& ExtractDirectoryPath(const char *pFilePath);
        str_smart_ptr &&AppendPaths(const char *pDirectoryPath, const char *pFilePath);
    }
}
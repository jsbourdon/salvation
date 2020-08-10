#pragma once

#include "Salvation_Common/Memory/ThreadHeapSmartPointer.h"

using namespace salvation::memory;

namespace salvation
{
    namespace filesystem
    {
        struct FileHandleRAII
        {
            FileHandleRAII(FILE* pFile) : m_pFile(pFile) {}
            ~FileHandleRAII() { fclose(m_pFile); }
            FILE* m_pFile;
        };

        bool FileExists(const char* pFilePath);
        bool DirectoryExists(const char* pDirectoryPath);
        bool CreateDirectory(const char* pDirectoryPath);

        template<typename AllocatorType>
        uint8_t* ReadFileContent(const char* pSrcPath, size_t& o_FileSize);

        str_smart_ptr&& ExtractDirectoryPath(const char* pFilePath);
        str_smart_ptr&& AppendPaths(const char* pDirectoryPath, const char* pFilePath);
    }
}

#include "Salvation_Common/FileSystem/FileSystem.inl"

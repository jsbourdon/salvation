#pragma once

template<typename AllocatorType>
uint8_t* salvation::filesystem::ReadFileContent(const char* pSrcPath, size_t& o_FileSize)
{
    uint8_t* pContent = nullptr;
    FILE* pFile;
    errno_t err = fopen_s(&pFile, pSrcPath, "r");
    o_FileSize = 0;

    if (err == 0 && pFile)
    {
        fseek(pFile, 0, SEEK_END);
        size_t fileSize = ftell(pFile);
        fseek(pFile, 0, SEEK_SET);

        pContent = static_cast<uint8_t*>(ThreadHeapAllocator::Allocate(fileSize));

        fread(pContent, sizeof(uint8_t), fileSize, pFile);
        fclose(pFile);

        o_FileSize = fileSize;
    }

    return pContent;
}

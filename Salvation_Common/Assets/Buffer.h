#pragma once

#include <stdint.h>

namespace salvation
{
    namespace asset
    {
        struct Buffer
        {
            size_t  m_packedDataId { 0 };
            size_t  m_byteOffset { 0 };
            size_t  m_ByteSize { 0 };
        };
    }
}
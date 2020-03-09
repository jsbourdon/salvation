#pragma once

#include <stdint.h>

namespace salvation
{
    namespace asset
    {
        struct Buffer
        {
            void*   m_pData;
            size_t  m_ByteSize;
        };
    }
}
#pragma once

#include <stdint.h>

namespace salvation
{
    namespace asset
    {
        struct PackedData
        {
            uint8_t*    m_pData { nullptr };
            size_t      m_byteSize { 0 };
        };
    }
}
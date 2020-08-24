#pragma once

#include <stdint.h>

namespace salvation
{
    namespace asset
    {
        enum class TextureFormat : int32_t
        {
            Undefined = -1,
            R_Float,
            RG_Float,
            RBG_Float,
            RBGA_Float,
            BC1,
            BC2,
            BC3,
            Count
        };

        struct Texture
        {
            uint64_t        m_byteSize;
            uint64_t        m_byteOffset;
            TextureFormat   m_format;
        };
    }
}
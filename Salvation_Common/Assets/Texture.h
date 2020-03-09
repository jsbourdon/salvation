#pragma once

#include <stdint.h>

namespace salvation
{
    namespace asset
    {
        enum class TextureFormat
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
            void*           m_pData;
            size_t          m_ByteSize;
            TextureFormat   m_Format;
        };
    }
}
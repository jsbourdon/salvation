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
            size_t          m_packedDataId;
            size_t          m_byteOffset { 0 };
            size_t          m_byteSize;
            TextureFormat   m_format;
        };
    }
}
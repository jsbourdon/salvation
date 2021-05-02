#pragma once

#include "salvation_rhi/Descriptors/SamplerStateDesc.h"

namespace salvation::rhi
{
    namespace descriptors
    {
        struct ShaderResourceLayoutDesc
        {
            SamplerStateDesc    StaticSamplers[5];
            uint32_t            StaticSamplerCount;
            uint32_t            ConstantCount;
            uint32_t            ConstantBufferViewCount;
        };
    }
}

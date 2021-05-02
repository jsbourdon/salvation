#pragma once

#include "salvation_rhi/Descriptors/InputLayoutDesc.h"
#include "salvation_rhi/Descriptors/BlendStateDesc.h"
#include "salvation_rhi/Descriptors/DepthStencilStateDesc.h"
#include "salvation_rhi/Descriptors/RasterizerStateDesc.h"
#include "salvation_rhi/Descriptors/PixelFormats.h"
#include "salvation_rhi/Resources/Resources.h"
#include "salvation_rhi/Resources/ResourceHandles.h"

namespace salvation::rhi
{
    using namespace resources;

    namespace descriptors
    {
        struct GfxPipelineDesc
        {
            static constexpr size_t     s_OMMaxRenderTargetCount = 8;

            ShaderResourceLayoutHandle  ResourceLayout {};
            InputLayoutDesc             InputLayout {};
            Shader                      VertexShader {};
            Shader                      FragmentShader {};
            BlendStateDesc              BlendState {};
            DepthStencilStateDesc       DepthStencilState {};
            RasterizerStateDesc         RasterizerState {};
            PixelFormat                 RenderTargetFormats[s_OMMaxRenderTargetCount] {};
            PixelFormat                 DepthFormat {};
        };

        struct ComputePipelineDesc
        {

        };
    }
}

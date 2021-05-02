#pragma once

#include "InputLayoutDesc.h"
#include "BlendStateDesc.h"
#include "ShaderDesc.h"
#include "DepthStencilStateDesc.h"
#include "RasterizerStateDesc.h"
#include "PixelFormats.h"


namespace salvation::rhi
{
    using namespace resources;

    namespace descriptors
    {
        struct PipelineDescriptor
        {
            static constexpr size_t     s_OMMaxRenderTargetCount = 8;

            InputLayoutDesc             InputLayout {};
            ShaderDesc                  VertexShader { nullptr, nullptr, nullptr, nullptr, ShaderType::VertexShader };
            ShaderDesc                  FragmentShader { nullptr, nullptr, nullptr, nullptr, ShaderType::FragmentShader };
            BlendStateDesc              BlendState {};
            DepthStencilStateDesc       DepthStencilState {};
            RasterizerStateDesc         RasterizerState {};
            PixelFormat                 RenderTargetFormats[s_OMMaxRenderTargetCount] {};
            PixelFormat                 DepthFormat {};
        };
    }
}

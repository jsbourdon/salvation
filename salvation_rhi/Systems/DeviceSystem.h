#pragma once

#include "salvation_rhi/Resources/ResourceHandles.h"
#include "salvation_rhi/Resources/Resources.h"
#include "salvation_rhi/Systems/SystemEnums.h"

namespace salvation::rhi::descriptors
{
    struct ShaderResourceLayoutDesc;
    struct GfxPipelineDesc;
    struct ComputePipelineDesc;
}

using namespace salvation::rhi::descriptors;
using namespace salvation::rhi::resources;

namespace salvation::rhi
{
    enum class CommandType;

    namespace device
    {
        struct CommandBuffer
        {
            CommandListHandle       m_cmdListHdl {};
            CommandAllocatorHandle  m_cmdAllocHdl {};
        };

        GpuDeviceHandle             CreateDevice();
        CommandQueueHandle          CreateCommandQueue(GpuDeviceHandle deviceHdl, CommandType type);
        bool                        CreateCommandBuffer(GpuDeviceHandle deviceHdl, CommandType type, CommandBuffer& outCmdBuffer);

        SwapChainHandle             CreateSwapChain(
                                        GpuDeviceHandle deviceHdl,
                                        CommandQueueHandle cmdQueueHdl,
                                        WindowHandle windowHdl,
                                        uint32_t backBufferCount,
                                        uint32_t pixelWidth,
                                        uint32_t pixelHeight);

        Shader                      CreateShader(GpuDeviceHandle deviceHdl, const char* pFilePath);
        ShaderResourceLayoutHandle  CreateShaderResourceLayout(GpuDeviceHandle deviceHdl, const ShaderResourceLayoutDesc& desc);
        GfxPipelineHandle           CreateGraphicsPipeline(GpuDeviceHandle deviceHdl, const GfxPipelineDesc& desc);
        ComputePipelineHandle       CreateComputePipeline(GpuDeviceHandle deviceHdl, const ComputePipelineDesc& desc);
        DescriptorHeapHandle        CreateDescriptorHeap(GpuDeviceHandle deviceHdl);

        void                        DestroyDevice(GpuDeviceHandle hdl);
        void                        DestroyCommandQueue(CommandQueueHandle hdl);
        void                        DestroyCommandAllocator(CommandAllocatorHandle hdl);
        void                        DestroyCommandBuffer(CommandBuffer& cmdBuffer);
        void                        DestroySwapChain(SwapChainHandle hdl);
        void                        DestroyShaderResourceLayout(ShaderResourceLayoutHandle hdl);
        void                        DestroyGfxPipeline(GfxPipelineHandle hdl);
        void                        DestroyComputePipeline(ComputePipelineHandle hdl);
        void                        DestroyDescriptorHeap(DescriptorHeapHandle hdl);

        void                        SignalFence(FenceHandle fenceHdl);
        void                        GPUWaitOnFence(FenceHandle fenceHdl);
        void                        CPUWaitOnFence(FenceHandle fenceHdl);
        void                        ExecuteCommandBuffer(CommandBuffer& cmdBuffer);
        void                        Present(SwapChainHandle swapChainHdl);
    }

}
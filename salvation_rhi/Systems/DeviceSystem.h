#pragma once

#include "salvation_rhi/Resources/ResourceHandles.h"
#include "salvation_rhi/Systems/SystemEnums.h"

namespace salvation::rhi
{
    enum class CommandType;

    namespace device
    {
        GpuDeviceHandle             CreateDevice();
        CommandQueueHandle          CreateCommandQueue(GpuDeviceHandle deviceHdl, CommandType type);
        CommandAllocatorHandle      CreateCommandAllocator(GpuDeviceHandle deviceHdl);
        CommandBufferHandle         CreateCommandBuffer(GpuDeviceHandle deviceHdl);
        SwapChainHandle             CreateSwapChain(GpuDeviceHandle deviceHdl);
        ShaderResourceLayoutHandle  CreateShaderResourceLayout(GpuDeviceHandle deviceHdl);
        PipelineHandle              CreatePipeline(GpuDeviceHandle deviceHdl);
        DescriptorHeapHandle        CreateDescriptorHeap(GpuDeviceHandle deviceHdl);

        void                        DestroyDevice(GpuDeviceHandle hdl);
        void                        DestroyCommandQueue(CommandQueueHandle hdl);
        void                        DestroyCommandAllocator(CommandAllocatorHandle hdl);
        void                        DestroyCommandBuffer(CommandBufferHandle hdl);
        void                        DestroySwapChain(SwapChainHandle hdl);
        void                        DestroyShaderResourceLayout(ShaderResourceLayoutHandle hdl);
        void                        DestroyPipeline(PipelineHandle hdl);
        void                        DestroyDescriptorHeap(DescriptorHeapHandle hdl);

        void                        SignalFence(FenceHandle fenceHdl);
        void                        GPUWaitOnFence(FenceHandle fenceHdl);
        void                        CPUWaitOnFence(FenceHandle fenceHdl);
        void                        ExecuteCommandBuffer(CommandBufferHandle cmdBufferHdl);
        void                        Present(SwapChainHandle swapChainHdl);
    }

}
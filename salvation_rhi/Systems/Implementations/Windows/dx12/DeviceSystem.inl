#pragma once

#include "salvation_rhi/Systems/DeviceSystem.h"
#include "salvation_rhi/Dependencies/d3d12.h"

using namespace salvation::rhi;

template<typename PtrType, typename HandleType>
static inline void AsType(PtrType& pPtr, HandleType handle)
{
    pPtr = reinterpret_cast<PtrType>(handle);
}

template<typename PtrType, typename HandleType>
static inline void AsHandle(const PtrType pPtr, HandleType& handle)
{
    handle = reinterpret_cast<HandleType>(pPtr);
}

static bool GetHardwareAdapter(
    IDXGIFactory1* pFactory,
    IDXGIAdapter1** ppAdapter)
{
    *ppAdapter = nullptr;

    ComPtr<IDXGIAdapter1> adapter;
    ComPtr<IDXGIFactory6> factory6;

    if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6))))
    {
        for (
            UINT adapterIndex = 0;
            DXGI_ERROR_NOT_FOUND != factory6->EnumAdapterByGpuPreference(
                adapterIndex,
                DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
                IID_PPV_ARGS(&adapter));
            ++adapterIndex)
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                // Don't select the Basic Render Driver adapter.
                // If you want a software adapter, pass in "/warp" on the command line.
                continue;
            }

            // Check to see whether the adapter supports Direct3D 12, but don't create the
            // actual device yet.
            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_1, _uuidof(ID3D12Device), nullptr)))
            {
                *ppAdapter = adapter.Detach();
                return true;
            }
        }
    }
    else
    {
        for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex)
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                // Don't select the Basic Render Driver adapter.
                // If you want a software adapter, pass in "/warp" on the command line.
                continue;
            }

            // Check to see whether the adapter supports Direct3D 12, but don't create the
            // actual device yet.
            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_1, _uuidof(ID3D12Device), nullptr)))
            {
                *ppAdapter = adapter.Detach();
                return true;
            }
        }
    }

    return false;
}

GpuDeviceHandle device::CreateDevice()
{
    GpuDeviceHandle deviceHdl = Handle_NULL;
    UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
    // Enable the debug layer (requires the Graphics Tools "optional feature").
    // NOTE: Enabling the debug layer after device creation will invalidate the active device.
    {
        ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
        {
            debugController->EnableDebugLayer();

            // Enable additional debug layers.
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }
    }
#endif

    ComPtr<IDXGIFactory4> factory;
    if (SUCCEEDED(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory))))
    {
        ComPtr<IDXGIAdapter1> hardwareAdapter;
        if (GetHardwareAdapter(factory.Get(), &hardwareAdapter))
        {
            ID3D12Device* pDevice;

            if (SUCCEEDED(D3D12CreateDevice(
                hardwareAdapter.Get(),
                D3D_FEATURE_LEVEL_12_1,
                IID_PPV_ARGS(&pDevice))))
            {
                AsHandle(pDevice, deviceHdl);
            }
        }
    }

    return deviceHdl;
}

CommandQueueHandle device::CreateCommandQueue(GpuDeviceHandle deviceHdl, CommandType type)
{
    CommandQueueHandle cmdQueueHandle = Handle_NULL;
    ID3D12Device* pDevice;
    AsType(pDevice, deviceHdl);

    D3D12_COMMAND_LIST_TYPE cmdType;
    switch (type)
    {
    case CommandType::Graphics:
        cmdType = D3D12_COMMAND_LIST_TYPE_DIRECT;
        break;
    case CommandType::AsyncCompute:
        cmdType = D3D12_COMMAND_LIST_TYPE_COMPUTE;
        break;
    case CommandType::Copy:
        cmdType = D3D12_COMMAND_LIST_TYPE_COPY;
        break;
    default:
        cmdType = D3D12_COMMAND_LIST_TYPE_DIRECT;
        SALVATION_ASSERT_MSG(false, "Invalid CommandType");
    }

    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = cmdType;

    ID3D12CommandQueue* pCmdQueue;
    if (SUCCEEDED(pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&pCmdQueue))))
    {
        AsHandle(pCmdQueue, cmdQueueHandle);
    }

    return cmdQueueHandle;
}

CommandAllocatorHandle device::CreateCommandAllocator(GpuDeviceHandle /*deviceHdl*/)
{
    return Handle_NULL;
}

CommandBufferHandle device::CreateCommandBuffer(GpuDeviceHandle /*deviceHdl*/)
{
    return Handle_NULL;
}

SwapChainHandle device::CreateSwapChain(GpuDeviceHandle /*deviceHdl*/)
{
    return Handle_NULL;
}

ShaderResourceLayoutHandle device::CreateShaderResourceLayout(GpuDeviceHandle /*deviceHdl*/)
{
    return Handle_NULL;
}

PipelineHandle device::CreatePipeline(GpuDeviceHandle /*deviceHdl*/)
{
    return Handle_NULL;
}

DescriptorHeapHandle device::CreateDescriptorHeap(GpuDeviceHandle /*deviceHdl*/)
{
    return Handle_NULL;
}

void device::DestroyDevice(GpuDeviceHandle hdl)
{
    ID3D12Device* pDevice = reinterpret_cast<ID3D12Device*>(hdl);
    pDevice->Release();
}

void device::DestroyCommandQueue(CommandQueueHandle hdl)
{
    ID3D12CommandQueue* pCmdQueue;
    AsType(pCmdQueue, hdl);
    pCmdQueue->Release();
}

void device::DestroyCommandAllocator(CommandAllocatorHandle /*hdl*/)
{

}

void device::DestroyCommandBuffer(CommandBufferHandle /*hdl*/)
{

}

void device::DestroySwapChain(SwapChainHandle /*hdl*/)
{

}

void device::DestroyShaderResourceLayout(ShaderResourceLayoutHandle /*hdl*/)
{

}

void device::DestroyPipeline(PipelineHandle /*hdl*/)
{

}

void device::DestroyDescriptorHeap(DescriptorHeapHandle /*hdl*/)
{

}

void device::SignalFence(FenceHandle /*fenceHdl*/)
{

}

void device::GPUWaitOnFence(FenceHandle /*fenceHdl*/)
{

}

void device::CPUWaitOnFence(FenceHandle /*fenceHdl*/)
{

}

void device::ExecuteCommandBuffer(CommandBufferHandle /*cmdBufferHdl*/)
{

}

void device::Present(SwapChainHandle /*swapChainHdl*/)
{

}

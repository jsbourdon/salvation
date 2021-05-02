#pragma once

#include "salvation_rhi/Systems/DeviceSystem.h"
#include "salvation_rhi/Dependencies/d3d12.h"
#include "salvation_rhi/Descriptors/ShaderResourceLayoutDesc.h"
#include "salvation_core/DataStructures/Vector.h"
#include "salvation_core/Memory/StackAllocator.h"

using namespace salvation;
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

static D3D12_COMMAND_LIST_TYPE ToNativeCmdType(CommandType type)
{
    D3D12_COMMAND_LIST_TYPE cmdType;
    switch(type)
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

    return cmdType;
}

CommandQueueHandle device::CreateCommandQueue(GpuDeviceHandle deviceHdl, CommandType type)
{
    CommandQueueHandle cmdQueueHandle = Handle_NULL;
    ID3D12Device* pDevice;
    AsType(pDevice, deviceHdl);

    D3D12_COMMAND_LIST_TYPE cmdType = ToNativeCmdType(type);

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

static ID3D12CommandAllocator* CreateCommandAllocator(ID3D12Device* pDevice, D3D12_COMMAND_LIST_TYPE cmdType)
{
    ID3D12CommandAllocator* pCmdAlloc;
    SALVATION_ASSERT_ALWAYS_EXEC(SUCCEEDED(pDevice->CreateCommandAllocator(cmdType, IID_PPV_ARGS(&pCmdAlloc))));
    return pCmdAlloc;
}

static ID3D12CommandList* CreateCommandList(ID3D12Device* pDevice, ID3D12CommandAllocator* pCmdAllocator, D3D12_COMMAND_LIST_TYPE cmdType)
{
    ID3D12CommandList* pCmdList = nullptr;
    SALVATION_ASSERT_ALWAYS_EXEC(SUCCEEDED(pDevice->CreateCommandList(0, cmdType, pCmdAllocator, nullptr, IID_PPV_ARGS(&pCmdList))));
    return pCmdList;
}

bool device::CreateCommandBuffer(GpuDeviceHandle deviceHdl, CommandType type, CommandBuffer& outCmdBuffer)
{
    ID3D12Device* pDevice;
    AsType(pDevice, deviceHdl);

    D3D12_COMMAND_LIST_TYPE cmdType = ToNativeCmdType(type);

    ID3D12CommandAllocator* pCmdAllocator = CreateCommandAllocator(pDevice, cmdType);
    ID3D12CommandList* pCmdList = CreateCommandList(pDevice, pCmdAllocator, cmdType);

    AsHandle(pCmdAllocator, outCmdBuffer.m_cmdAllocHdl);
    AsHandle(pCmdList, outCmdBuffer.m_cmdListHdl);

    return true;
}

SwapChainHandle device::CreateSwapChain(
    GpuDeviceHandle deviceHdl, 
    CommandQueueHandle cmdQueueHdl, 
    WindowHandle windowHdl,
    uint32_t backBufferCount, 
    uint32_t pixelWidth, 
    uint32_t pixelHeight)
{
    ID3D12CommandQueue* pCmdQueue;
    HWND windowHWND;

    AsType(pCmdQueue, cmdQueueHdl);
    AsType(windowHWND, windowHdl);

    SwapChainHandle swapChainHdl = Handle_NULL;

    UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
    // Enable the debug layer (requires the Graphics Tools "optional feature").
    // NOTE: Enabling the debug layer after device creation will invalidate the active device.
    {
        // Enable additional debug layers.
        dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
    }
#endif

    ComPtr<IDXGIFactory4> factory;
    if (SUCCEEDED(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory))))
    {
        ComPtr<IDXGIAdapter1> hardwareAdapter;
        if (GetHardwareAdapter(factory.Get(), &hardwareAdapter))
        {
            // Describe and create the swap chain.
            DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
            swapChainDesc.BufferCount = backBufferCount;
            swapChainDesc.Width = pixelWidth;
            swapChainDesc.Height = pixelHeight;
            swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
            swapChainDesc.SampleDesc.Count = 1;

            IDXGISwapChain1* pSwapChain;
            if(SUCCEEDED(factory->CreateSwapChainForHwnd(
                pCmdQueue,        // Swap chain needs the queue so that it can force a flush on it.
                windowHWND,
                &swapChainDesc,
                nullptr,
                nullptr,
                &pSwapChain
            )))
            {
                // This sample does not support fullscreen transitions.
                SALVATION_ASSERT_ALWAYS_EXEC(SUCCEEDED(factory->MakeWindowAssociation(windowHWND, DXGI_MWA_NO_ALT_ENTER)));
                AsHandle(pSwapChain, swapChainHdl);
            }
        }
    }

    return swapChainHdl;
}

static D3D12_TEXTURE_ADDRESS_MODE ToNativeAddressMode(SamplerAddressMode mode)
{
    switch(mode)
    {
    case SamplerAddressMode::Wrap:
        return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    case SamplerAddressMode::Clamp:
        return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    default:
        SALVATION_FAIL();
        return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    }
}

static D3D12_FILTER ToNativeFilter(SamplerFiltering filter)
{
    switch(filter)
    {
    case SamplerFiltering::Point:
        return D3D12_FILTER_MIN_MAG_MIP_POINT;
    case SamplerFiltering::Linear:
        return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    case SamplerFiltering::Anisotropic:
        return D3D12_FILTER_ANISOTROPIC;
    default:
        SALVATION_FAIL();
        return D3D12_FILTER_MIN_MAG_MIP_POINT;
    }
}

ShaderResourceLayoutHandle device::CreateShaderResourceLayout(GpuDeviceHandle deviceHdl, const ShaderResourceLayoutDesc& desc)
{
    D3D12_VERSIONED_ROOT_SIGNATURE_DESC d3dRootSigDesc;
    d3dRootSigDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;

    D3D12_ROOT_SIGNATURE_DESC1& d3dDesc = d3dRootSigDesc.Desc_1_1;
    d3dDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED;
    
    const size_t cStaticSamplerMaxCount = std::extent<decltype(desc.StaticSamplers)>::value;
    SALVATION_ASSERT(desc.StaticSamplerCount <= cStaticSamplerMaxCount);

    D3D12_STATIC_SAMPLER_DESC staticSamplerDescs[cStaticSamplerMaxCount];
    for(uint32_t samplerIndex = 0; samplerIndex < desc.StaticSamplerCount; ++samplerIndex)
    {
        const SamplerStateDesc& samplerDesc = desc.StaticSamplers[samplerIndex];
        D3D12_STATIC_SAMPLER_DESC& d3dSamplerDesc = staticSamplerDescs[samplerIndex];
        d3dSamplerDesc.AddressU = ToNativeAddressMode(samplerDesc.m_AddressU);
        d3dSamplerDesc.AddressV = ToNativeAddressMode(samplerDesc.m_AddressV);
        d3dSamplerDesc.AddressW = ToNativeAddressMode(samplerDesc.m_AddressW);
        d3dSamplerDesc.Filter = ToNativeFilter(samplerDesc.m_Filter);
        d3dSamplerDesc.MaxAnisotropy = samplerDesc.m_MaxAnisotropy;
        d3dSamplerDesc.MinLOD = 0;
        d3dSamplerDesc.MaxLOD = 999;
        d3dSamplerDesc.RegisterSpace = 0;
        d3dSamplerDesc.ShaderRegister = samplerIndex;
        d3dSamplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    }

    d3dDesc.pStaticSamplers = staticSamplerDescs;
    d3dDesc.NumStaticSamplers = desc.StaticSamplerCount;

    const uint32_t cParamCount = desc.ConstantCount + desc.ConstantBufferViewCount;
    d3dDesc.NumParameters = cParamCount;

    if(cParamCount > 0)
    {
        salvation::data::Vector<D3D12_ROOT_PARAMETER1, memory::StackAllocator> rootParams(cParamCount, cParamCount);

        for(uint32_t constantIndex = 0; constantIndex < desc.ConstantCount; ++constantIndex)
        {
            D3D12_ROOT_PARAMETER1& param = rootParams[constantIndex];
            param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
            param.Constants.Num32BitValues = 1;
            param.Constants.RegisterSpace = 0;
            param.Constants.ShaderRegister = constantIndex;
            param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
        }

        for(uint32_t descriptorIndex = 0; descriptorIndex < desc.ConstantBufferViewCount; ++descriptorIndex)
        {
            const uint32_t cParamIndex = descriptorIndex + desc.ConstantCount;

            D3D12_ROOT_PARAMETER1& param = rootParams[cParamIndex];
            param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
            param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
            param.Descriptor.RegisterSpace = 0;
            param.Descriptor.ShaderRegister = cParamIndex;
            param.Descriptor.Flags = D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE;
        }

        d3dDesc.pParameters = rootParams.Data();
    }

    ComPtr<ID3DBlob> pBlob, pErrBlob;
    if(SUCCEEDED(D3D12SerializeVersionedRootSignature(&d3dRootSigDesc, &pBlob, &pErrBlob)))
    {
        ShaderResourceLayoutHandle layoutHdl;
        ID3D12RootSignature* pRootSig;
        ID3D12Device* pDevice;
        AsType(pDevice, deviceHdl);

        if(SUCCEEDED(pDevice->CreateRootSignature(0, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), IID_PPV_ARGS(&pRootSig))))
        {
            AsHandle(pRootSig, layoutHdl);
            return layoutHdl;
        }
    }

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

void device::DestroyCommandBuffer(CommandBuffer& /*cmdBuffer*/)
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

void device::ExecuteCommandBuffer(CommandBuffer& /*cmdBuffer*/)
{

}

void device::Present(SwapChainHandle /*swapChainHdl*/)
{

}

#include <pch.h>
#include "GpuDevice.h"
#include "GfxPlatform.h"
#include "LWGL/Library/LibraryLoader.h"
#include "Salvation_Common/Memory/ThreadHeapAllocator.h"

using namespace lwgl;
using namespace lwgl::external;
using namespace salvation::memory;

GpuDevice::GpuDevice()
    : m_CmdBuffers(s_MaxCmdBufferCount)
{

}

GpuDevice::~GpuDevice()
{
    
}

GpuDevice* GpuDevice::CreateDevice()
{
    GpuDevice *pDevice = new GpuDevice();

    if (!pDevice->Init())
    {
        delete pDevice;
        pDevice = nullptr;
    }

    return pDevice;
}

bool GpuDevice::Init()
{
    m_DeviceHandle = Handle_NULL;

    return true; // m_DeviceHandle != Handle_NULL;
}

void GpuDevice::DestroyDevice(GpuDevice *pDevice)
{
    delete pDevice;
}

CommandQueueHandle GpuDevice::CreateCommandQueue(CommandQueueType type)
{
    SALVATION_ASSERT_MSG(type == CommandQueueType::Graphic, "Unsupported Command Queue type");
    return Handle_NULL;
}

void GpuDevice::DestroyCommandQueue(CommandQueueHandle cmdQueueHdl)
{
    
}

GfxCommandList* GpuDevice::CreateGfxCommandList(size_t memoryByteSize)
{
    GfxCommandList *pCmdList = new GfxCommandList(this, memoryByteSize);

    GfxCommandBuffer cmdBuffer;
    cmdBuffer.m_CmdbufferHdl = Handle_NULL;
    pCmdList->m_CmdBufferHdl = m_CmdBuffers.Add(cmdBuffer);

    return pCmdList;
}

void GpuDevice::CloseCommandList(Handle cmdBufferHdl, void *pPackets) const
{
    GfxCommandBuffer *pCmdBuffer = m_CmdBuffers.Get(cmdBufferHdl);
    pCmdBuffer->m_pPackets = pPackets;
}

void GpuDevice::DestroyGfxCommandList(GfxCommandList *pList)
{
    m_CmdBuffers.Remove(pList->m_CmdBufferHdl);
    delete pList;
}

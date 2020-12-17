#include <pch.h>
#include "GpuDevice.h"
#include "GfxPlatform.h"
#include "salvation_core/Libraries/LibraryLoader.h"
#include "salvation_core/Memory/ThreadHeapAllocator.h"
#include "salvation_rhi/Commands/GpuCommandQueue.h"
#include "salvation_rhi/Commands/GpuCommandList.h"

using namespace salvation::rhi;
using namespace salvation::external;
using namespace salvation::memory;

GpuDevice::GpuDevice()
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

GpuCommandQueue* GpuDevice::CreateCommandQueue(GpuCommandQueueType type)
{
    SALVATION_ASSERT_MSG(type == GpuCommandQueueType::Graphic, "Unsupported Command Queue type");

    return nullptr;
}

void GpuDevice::DestroyCommandQueue(GpuCommandQueue* /*pCmdQueue*/)
{
    
}

GpuCommandList* GpuDevice::CreateCommandList()
{
    return nullptr;
}

void GpuDevice::DestroyCommandList(GpuCommandList* pList)
{
    
}

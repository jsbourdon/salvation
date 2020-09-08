#include <pch.h>
#include "GpuDevice.h"
#include "GfxPlatform.h"
#include "salvation_core/Libraries/LibraryLoader.h"
#include "salvation_core/Memory/ThreadHeapAllocator.h"

using namespace salvation_rhi;
using namespace salvation_rhi::external;
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

CommandQueueHandle GpuDevice::CreateCommandQueue(/*CommandQueueType type*/)
{
    //SALVATION_ASSERT_MSG(type == CommandQueueType::Graphic, "Unsupported Command Queue type");
    return Handle_NULL;
}

void GpuDevice::DestroyCommandQueue(CommandQueueHandle cmdQueueHdl)
{
    
}

/*GfxCommandList*/void* GpuDevice::CreateGfxCommandList(size_t /*memoryByteSize*/)
{
    return nullptr;
}

void GpuDevice::CloseCommandList(Handle /*cmdBufferHdl*/, void* /*pPackets*/) const
{
    
}

void GpuDevice::DestroyGfxCommandList(/*GfxCommandList *pList*/)
{
    
}

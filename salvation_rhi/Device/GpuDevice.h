#pragma once

#include "salvation_core/Libraries/LibraryLoader.h"
#include "salvation_core/Memory/Memory.h"
#include "salvation_core/Core/Defines.h"
#include "salvation_core/DataStructures/PackedArray.h"

using namespace salvation::memory;
using namespace salvation_rhi::external;

namespace salvation_rhi
{
    enum class GfxPlatform;

    class GpuDevice
    {
    public:

        static GpuDevice*   CreateDevice();
        static void         DestroyDevice(GpuDevice *pDevice);

        CommandQueueHandle  CreateCommandQueue(/*CommandQueueType type*/);
        void                DestroyCommandQueue(CommandQueueHandle cmdQueueHdl);

        /*GfxCommandList*/void*     CreateGfxCommandList(size_t memoryByteSize = MiB(1));
        void                CloseCommandList(Handle cmdBufferHdl, void *pPackets) const;
        void                DestroyGfxCommandList(/*GfxCommandList *pList*/);

    private:

        static constexpr uint32_t s_MaxCmdBufferCount = 100;

        GpuDevice();
        ~GpuDevice();

        bool Init();

        GpuDeviceHandle                 m_DeviceHandle { Handle_NULL };
    };
}
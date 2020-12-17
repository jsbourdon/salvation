#pragma once

#include "salvation_core/Libraries/LibraryLoader.h"
#include "salvation_core/Memory/Memory.h"
#include "salvation_core/Core/Defines.h"
#include "salvation_core/DataStructures/PackedArray.h"

using namespace salvation::memory;
using namespace salvation::external;

namespace salvation::rhi
{
    enum class GfxPlatform;
    enum class GpuCommandQueueType;
    class GpuCommandQueue;
    class GpuCommandList;

    class GpuDevice
    {
    public:

        static GpuDevice*   CreateDevice();
        static void         DestroyDevice(GpuDevice *pDevice);

        GpuCommandQueue*    CreateCommandQueue(GpuCommandQueueType type);
        void                DestroyCommandQueue(GpuCommandQueue* pCmdQueue);

        GpuCommandList*     CreateCommandList();
        void                DestroyCommandList(GpuCommandList* pList);

    private:

        static constexpr uint32_t s_MaxCmdBufferCount = 100;

        GpuDevice();
        ~GpuDevice();

        bool Init();

        GpuDeviceHandle                 m_DeviceHandle { Handle_NULL };
    };
}
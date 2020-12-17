// LWGL_TestApp.cpp : Defines the entry point for the application.
//
#include "framework.h"
#include "TestApp.h"
#include <cstdint>
#include <thread>
#include <chrono>
#include "salvation_rhi/WindowFactory/WindowFactory.h"
#include "salvation_rhi/Events/MessagePump.h"
#include "salvation_rhi/Resources/ResourceHandles.h"
#include "salvation_core/SystemInfo/SystemInfo.h"
#include "salvation_core/Memory/VirtualMemoryAllocator.h"
#include "salvation_core/Memory/ThreadHeapAllocator.h"
#include "salvation_core/Threading/WorkerThread.h"
#include "salvation_core/Assets/AssetDatabase.h"
#include "salvation_rhi/Device/GfxPlatform.h"
#include "salvation_rhi/Device/GpuDevice.h"

using namespace salvation::rhi;
using namespace salvation::memory;
using namespace salvation::threading;
using namespace salvation::asset;
using namespace std::chrono_literals;

static int WorkerFunction(int iterationCount)
{
    int total = 0;
    for (int i = 0; i < iterationCount; ++i)
    {
        total += i;
    }

    return total;
}

typedef int (WorkerFnctType)(int);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    ThreadHeapAllocator::Init(GiB(1), MiB(100));

    WorkerThread<WorkerFnctType> worker0(WorkerFunction, GiB(1), MiB(100));
    WorkerThread<WorkerFnctType> worker1(WorkerFunction, GiB(1), MiB(100));
    worker0.Init();
    worker1.Init();

    worker0.Run(1000);
    worker1.Run(100);

    int value0 = worker0.Wait();
    int value1 = worker1.Wait();

    WindowHandle hwnd = factory::CreateNewWindow(reinterpret_cast<salvation::rhi::AppHandle>(hInstance), 1920, 1080, L"Salvation");
    factory::DisplayWindow(hwnd);

    GpuDevice *pDevice = GpuDevice::CreateDevice();
    
    // Assets loading
//     AssetDatabase* pAssetDb = LoadDatabase("D:/Temp/StartTrek.db");
//     const Texture* pTextures = GetTextures(pAssetDb);
//     const Mesh* pMeshes = GetMeshes(pAssetDb);
//     DestroyDatabase(pAssetDb);

    while (!salvation::rhi::events::PumpMessages())
    {
        std::this_thread::sleep_for(100ms);
    }

    GpuDevice::DestroyDevice(pDevice);

    return 0;
}

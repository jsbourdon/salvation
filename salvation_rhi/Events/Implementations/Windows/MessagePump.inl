#pragma once

#include <pch.h>
#include "salvation_rhi/Events/MessagePump.h"

bool salvation::rhi::events::PumpMessages()
{
    MSG msg;
    msg.message = WM_NULL;

    if (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.message == WM_QUIT;
}

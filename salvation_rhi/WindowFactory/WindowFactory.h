#pragma once

#include "salvation_rhi/Resources/ResourceHandles.h"

namespace salvation_rhi
{
    namespace factory
    {
        WindowHandle CreateNewWindow(AppHandle owner, uint32_t width, uint32_t height, const wchar_t* pTitle);
        void DisplayWindow(const WindowHandle &hwnd);
        void HideWindow(const WindowHandle &hwnd);
    }
}

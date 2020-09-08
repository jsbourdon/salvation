#pragma once

namespace salvation_rhi
{
    namespace descriptors
    {
        struct ViewportDescriptor
        {
            float m_X;
            float m_Y;
            float m_Width;
            float m_Height;
            float m_MinDepth;
            float m_MaxDepth;
        };
    }
}

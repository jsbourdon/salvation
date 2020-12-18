#pragma once

#include "salvation_rhi/Resources/ResourceHandles.h"

namespace salvation::rhi
{
    struct DescriptorTable
    {
        DescriptorHeapHandle    m_heapHdl;
        uint32_t                m_offset;
        uint32_t                m_count;
    };
}

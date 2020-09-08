#include <pch.h>
#include "salvation_core/Memory/SubAllocator.h"
#include "salvation_core/Memory/ThreadHeapAllocator.h"

using namespace salvation::memory;

SubAllocator::~SubAllocator()
{
    if (m_pAllocation)
    {
        ThreadHeapAllocator::Release(m_pAllocation);
    }
}

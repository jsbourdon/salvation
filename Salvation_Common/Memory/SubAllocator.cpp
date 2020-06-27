#include <pch.h>
#include "Salvation_Common/Memory/SubAllocator.h"
#include "Salvation_Common/Memory/ThreadHeapAllocator.h"

using namespace salvation::memory;

SubAllocator::~SubAllocator()
{
    if (m_pAllocation)
    {
        ThreadHeapAllocator::Release(m_pAllocation);
    }
}

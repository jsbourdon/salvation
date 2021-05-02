#pragma once

#include <stdint.h>
#include "salvation_core/Memory/Memory.h"

namespace salvation
{
    namespace memory
    {
        class StackAllocator
        {
        public:

            StackAllocator() = default;
            ~StackAllocator() = default;

            static void*    Allocate(size_t byteSize) { return salvation::memory::StackAlloc(byteSize); }
            static void     Release(void* /*pMemory*/) {}
        };
    }
}
#pragma once

#include <cstdint>
#include "Salvation_Common/Memory/ThreadHeapAllocator.h"

using namespace salvation::memory;

namespace salvation
{
    namespace containers
    {
        template<typename ValueType, typename AllocatorType = ThreadHeapAllocator>
        class Vector
        {
        public:
            Vector(uint32_t reservedSize);
            ~Vector();

            uint32_t            Add(ValueType& value);
            ValueType&          Remove(uint32_t index);
            ValueType*          Data();
            uint32_t            Size() const { return m_size; }

            ValueType&          operator[](size_t index);
            const ValueType&    operator[](size_t index) const;

        private:

            void                EnsureCapacity();

            ValueType*  m_pData;
            uint32_t    m_size;
            uint32_t    m_reservedSize;
        };
    }
}

#include "Vector.inl"

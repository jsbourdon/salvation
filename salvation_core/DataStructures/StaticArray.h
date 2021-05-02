#pragma once

#include <cstdint>
#include "salvation_core/Memory/ThreadHeapAllocator.h"

using namespace salvation::memory;

namespace salvation
{
    namespace data
    {
        template<typename ValueType, typename AllocatorType = ThreadHeapAllocator>
        class StaticArray
        {
        public:

            StaticArray();
            StaticArray(uint32_t size);
            StaticArray(StaticArray&& other);
            StaticArray(const StaticArray& other) = delete;

            ~StaticArray();

            ValueType*          Data();
            const ValueType*    Data() const;
            uint32_t            Size() const { return m_size; }

            ValueType&          operator[](size_t index);
            const ValueType&    operator[](size_t index) const;
            void                operator=(StaticArray<ValueType, AllocatorType>&& other);

        private:

            ValueType*  m_pData;
            uint32_t    m_size;
        };
    }
}

#include "StaticArray.inl"

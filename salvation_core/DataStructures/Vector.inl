#pragma once

#include "Vector.h"
#include <cstdint>
#include "salvation_core/Core/Defines.h"

using namespace salvation::data;

template<typename ValueType, typename AllocatorType>
Vector<ValueType, AllocatorType>::Vector(uint32_t reservedSize)
    : m_pData(static_cast<ValueType*>(AllocatorType::Allocate(sizeof(ValueType) * reservedSize)))
    , m_size(0)
    , m_reservedSize(reservedSize)
{
    SALVATION_ASSERT(reservedSize > 0);
}

template<typename ValueType, typename AllocatorType>
Vector<ValueType, AllocatorType>::~Vector()
{
    AllocatorType::Release(m_pData);
}

template<typename ValueType, typename AllocatorType>
uint32_t Vector<ValueType, AllocatorType>::Add(ValueType& value)
{
    EnsureCapacity();
    m_pData[m_size] = value;

    const uint32_t index = m_size++;
    return index;
}

template<typename ValueType, typename AllocatorType>
ValueType& Vector<ValueType, AllocatorType>::Remove(uint32_t index)
{
    SALVATION_ASSERT(index < m_size);
    const uint32_t startIndex = index + 1;

    for (uint32_t i = startIndex; i < m_size; ++i)
    {
        m_pData[i - 1] = m_pData[i];
    }
}

template<typename ValueType, typename AllocatorType>
ValueType* Vector<ValueType, AllocatorType>::Data()
{
    return m_pData;
}

template<typename ValueType, typename AllocatorType>
void Vector<ValueType, AllocatorType>::Clear()
{
    m_size = 0;
}

template<typename ValueType, typename AllocatorType>
ValueType& Vector<ValueType, AllocatorType>::operator[](size_t index)
{
    SALVATION_ASSERT(index < m_size);
    return m_pData[index];
}

template<typename ValueType, typename AllocatorType>
const ValueType& Vector<ValueType, AllocatorType>::operator[](size_t index) const
{
    SALVATION_ASSERT(index < m_size);
    return m_pData[index];
}

template<typename ValueType, typename AllocatorType>
template<typename ...T>
uint32_t Vector<ValueType, AllocatorType>::Emplace(T... args)
{
    EnsureCapacity();
    new (m_pData + m_size) ValueType(args...);

    const uint32_t index = m_size++;
    return index;
}

template<typename ValueType, typename AllocatorType>
void Vector<ValueType, AllocatorType>::EnsureCapacity()
{
    if (m_size + 1 > m_reservedSize)
    {
        m_reservedSize *= 2;
        ValueType* pNewArray = static_cast<ValueType*>(AllocatorType::Allocate(m_reservedSize * sizeof(ValueType)));
        memcpy(pNewArray, m_pData, m_size * sizeof(ValueType));
        AllocatorType::Release(m_pData);
        m_pData = pNewArray;
    }
}

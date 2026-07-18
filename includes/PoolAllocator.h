#pragma once
#include <cstddef>
#include <algorithm>

template<typename T>
class PoolAllocator
{
public:
	PoolAllocator(const size_t capacity);
	PoolAllocator(const PoolAllocator& other) = delete;
	PoolAllocator& operator=(const PoolAllocator& other) = delete;
	PoolAllocator(PoolAllocator&& other) = delete;
	PoolAllocator& operator=(PoolAllocator&& other) = delete;
	~PoolAllocator();

	T* Allocate();
	void Free(T* ptr);

private:
	size_t m_elementSize;
	size_t m_capacity;
	size_t m_size;
	std::byte* m_bytes;
	T* m_freeNode;
};

template<typename T>
inline PoolAllocator<T>::PoolAllocator(const size_t capacity)
{
	if (sizeof(T) >= sizeof(void*))
	{
		//use pointer for linking list
	}
	else {
		//todo return to later.
	}
	m_elementSize = sizeof(T);
	m_capacity = capacity;
	//todo check max size

	m_size = m_capacity * m_elementSize;
	m_bytes = new std::byte[m_size];
	std::byte* start = m_bytes;
	for (size_t i = 0; i < m_capacity; i++)
	{
		std::memset(start, 'U', m_elementSize);
		uintptr_t nextAddress = 0;
		if (i != m_capacity - 1) 
		{
			auto startAddress = reinterpret_cast<uintptr_t>(start);
			nextAddress = startAddress + m_elementSize;
		}
		uintptr_t* startIWant = reinterpret_cast<uintptr_t*>(start);
		*startIWant = nextAddress;
		start += m_elementSize;
	}
	m_freeNode = reinterpret_cast<T*>(m_bytes);
}


template<typename T>
inline T* PoolAllocator<T>::Allocate()
{
	auto temp = m_freeNode;
	uintptr_t* nextAddress = reinterpret_cast<uintptr_t*>(m_freeNode);
	m_freeNode = reinterpret_cast<T*>(*nextAddress);
	std::memset(temp, 'A', m_elementSize);
	return temp;
}


template<typename T>
inline void PoolAllocator<T>::Free(T* ptr)
{
	std::memset(ptr, 'F', m_elementSize);
	auto freeAdd = &m_freeNode;
	uintptr_t freeAddress = reinterpret_cast<uintptr_t>(m_freeNode);
	uintptr_t* writing = reinterpret_cast<uintptr_t*>(ptr);
	*writing = freeAddress;
	m_freeNode = ptr;
}
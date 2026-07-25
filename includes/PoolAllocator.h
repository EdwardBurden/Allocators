/*
PoolAllocator.
This allocator requests a block of memory and then portions out it into equally sized "buckets" of the size of T.
Each bucket will start empty, except for a pointer to the next unallocated bucket, creating a linked list of pointers.
When an allocation request is made the m_freeElement is updated to the next bucket, a new element is created using placement new and this is returned.
When freeing, the memory is wiped for the object being freed and it points to the next free bucket, and become the top of the linked list.
You must specify the type of object used for the pool and that object must have a size larger than a pointer.
T's constructor must be noexecpt, to avoid handling exceptions in the allocation (this can be improved on later).
You may allocate and free elemetns in any order, however if there is no room the pool will return a null pointer if you try to allocate.
The allocator is thread-safe.
*/

#pragma once
#include "AllocatorUtils.h"
#include "Allocator.h"
#include <stdexcept>

template<typename T>
class PoolAllocator : public Allocator
{
	static_assert(sizeof(T) >= sizeof(void*), "Elements of the pool allocator must be larger than or equal to the size of a pointer");
	static_assert(alignof(T) <= alignof(std::max_align_t), "Pool allocator does not support overaligned types");
public:
	PoolAllocator(const size_t capacity);
	PoolAllocator(const PoolAllocator& other) = delete;
	PoolAllocator& operator=(const PoolAllocator& other) = delete;
	PoolAllocator(PoolAllocator&& other) = delete;
	PoolAllocator& operator=(PoolAllocator&& other) = delete;
	~PoolAllocator();

	template<typename... Args>
	T* Allocate(Args&&... args);

	void Free(T* ptr);
	void Reset() override;
private:
	void SetPoolElements();
	size_t m_elementSize;
	size_t m_capacity;
	T* m_freeElement;
};

template<typename T>
inline PoolAllocator<T>::PoolAllocator(const size_t capacity)
{
	m_elementSize = sizeof(T);
	m_capacity = capacity;
	m_size = m_capacity * m_elementSize;
	if (m_size > AllocatorUtils::MAX_POOL_SIZE)
		throw std::runtime_error("Pool size in memory cannot be larger than MAX_POOL_SIZE");

	m_bytes = new std::byte[m_size];
	m_limit = m_bytes + m_size;
	SetPoolElements();
	m_freeElement = reinterpret_cast<T*>(m_bytes);
}

template<typename T>
inline PoolAllocator<T>::~PoolAllocator()
{
	delete[] m_bytes;
}

template<typename T>
template<typename... Args>
inline T* PoolAllocator<T>::Allocate(Args&&... args)
{
	static_assert(std::is_nothrow_constructible_v<T, Args...>, "Allocate requires T's constructor to be no exexcept");
	std::byte* allocation = nullptr;
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		if (m_freeElement == nullptr)
			return nullptr;

		allocation = reinterpret_cast<std::byte*>(m_freeElement);
		std::byte** elementPointer = reinterpret_cast<std::byte**>(m_freeElement);
		m_freeElement = reinterpret_cast<T*>(*elementPointer);
	}
#ifndef NDEBUG
	std::memset(allocation, 'A', m_elementSize);
#endif //NDEBUG

	return new (allocation) T(std::forward<Args>(args)...);
}

template<typename T>
inline void PoolAllocator<T>::Free(T* ptr)
{
	if (ptr == nullptr || ptr == m_freeElement)
		return;

	if (!AllocatorUtils::CheckMemoryBounds(ptr, m_bytes, m_limit))
		return;

	if (!AllocatorUtils::IsAligned(reinterpret_cast<const std::byte*>(ptr) - m_bytes, m_elementSize))
		return;

	ptr->~T();
#ifndef NDEBUG
	std::memset(ptr, 'F', m_elementSize);
#endif //NDEBUG

	std::lock_guard<std::mutex> lock(m_mutex);
	std::byte** elementPointer = reinterpret_cast<std::byte**>(ptr);
	*elementPointer = reinterpret_cast<std::byte*>(m_freeElement);
	m_freeElement = ptr;
}

template<typename T>
inline void PoolAllocator<T>::Reset()
{
	Allocator::Reset();
	std::lock_guard<std::mutex> lock(m_mutex);
	SetPoolElements();
	m_freeElement = reinterpret_cast<T*>(m_bytes);
}

template<typename T>
inline void PoolAllocator<T>::SetPoolElements() 
{
	std::byte* element = m_bytes;
	for (size_t i = 0; i < m_capacity; i++)
	{
#ifndef NDEBUG
		std::memset(element, 'U', m_elementSize);
#endif //NDEBUG
		std::byte* next = nullptr;
		if (i != m_capacity - 1)
		{
			next = element + m_elementSize;
		}
		std::byte** elementPointer = reinterpret_cast<std::byte**>(element);
		*elementPointer = next;
		element += m_elementSize;
	}
}
#pragma once
#include <cstddef>
#include <algorithm>
#include <cstring>

#include "AllocatorUtils.h"
#include <cassert>

class StackAllocator
{
public:
	StackAllocator(size_t size);
	StackAllocator(const StackAllocator& other) = delete;
	StackAllocator& operator=(const StackAllocator& other) = delete;
	StackAllocator(StackAllocator&& other) = delete;
	StackAllocator& operator=(StackAllocator&& other) = delete;
	~StackAllocator();

	template<typename T>
	std::byte* Allocate()
	{
		return Allocate(sizeof(T), alignof(T));
	}

	std::byte* Allocate(size_t size, size_t alignment);
	void FreeToMarker(std::byte* marker);
	void Reset();
private:
	static constexpr size_t MAX_STACK_SIZE = 1024 * 1024 * 8; // 8MB
	size_t m_size;
	std::byte* m_bytes;
	std::byte* m_marker;
};

inline StackAllocator::StackAllocator(size_t size)
{
	m_size = std::min(size, MAX_STACK_SIZE);
	m_bytes = new std::byte[m_size];
	m_marker = m_bytes;
	std::memset(m_marker, 'U', m_size);
}

inline StackAllocator::~StackAllocator()
{
	delete[] m_bytes;
}

inline std::byte* StackAllocator::Allocate(size_t size, size_t alignment = alignof(std::max_align_t))
{
	std::byte* limit = m_bytes + m_size;
	return AllocatorUtils::AllocateUp(m_marker, limit, size, alignment);
}

inline void StackAllocator::FreeToMarker(std::byte* marker)
{
	if (marker == nullptr)
		return;

	ptrdiff_t offset = m_marker - marker;
	std::byte* max = m_bytes + m_size;
	if (marker < m_bytes || marker > max)
		return;

	std::memset(marker, 'F', offset);
	m_marker = marker;
}

inline void StackAllocator::Reset()
{
	std::memset(m_bytes, 'R', m_size);
	m_marker = m_bytes;
}
#pragma once
#include <cstddef>
#include <algorithm>
#include <cstring>

std::byte* Align(std::byte* marker, size_t alignment)
{
	// check alignment provided is multiple first 
	uintptr_t maxAlignValue = alignment - 1;
	uintptr_t upperBound = reinterpret_cast<uintptr_t>(marker) + maxAlignValue;
	uintptr_t mask = ~maxAlignValue;
	auto alignedAddress = upperBound & mask;
	return reinterpret_cast<std::byte*>(alignedAddress);
}

class StackAllocator
{
	static constexpr size_t MAX_STACK_SIZE = 1024 * 1024 * 8; // 8MB
public:
	StackAllocator(size_t size);
	StackAllocator(const StackAllocator& other) = delete;
	StackAllocator& operator=(const StackAllocator& other) = delete;
	StackAllocator(StackAllocator&& other) = delete;
	StackAllocator& operator=(StackAllocator&& other) = delete;
	~StackAllocator();

	std::byte* Allocate(size_t byteSize, size_t);
	void FreeToMarker(std::byte* marker);
private:
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

inline std::byte* StackAllocator::Allocate(size_t byteSize, size_t alignment = alignof(std::max_align_t))
{
	std::byte* alignedCurrentMarker = Align(m_marker, alignment);
	std::byte* newMarker = alignedCurrentMarker + byteSize;
	std::byte* max = m_bytes + m_size;
	if (newMarker > max)
		return nullptr;

	std::memset(m_marker, 'P', alignedCurrentMarker - m_marker);
	std::memset(alignedCurrentMarker, 'A', byteSize);
	m_marker = newMarker;
	return alignedCurrentMarker;
}

inline void StackAllocator::FreeToMarker(std::byte* marker)
{
	if (marker == nullptr)
		return;

	auto offset = m_marker - marker;
	auto newMarker = m_marker - offset;
	auto max = m_bytes + m_size;
	if (newMarker < m_bytes || newMarker >max)
		return;

	std::memset(newMarker, 'U', offset);
	m_marker = newMarker;
}
#include "AllocatorUtils.h"
#include "StackAllocator.h"

#include <cstring>
#include <stdexcept>

StackAllocator::StackAllocator(const size_t size)
{
	if (size > AllocatorUtils::MAX_STACK_SIZE)
		throw std::runtime_error("Stack size in memory cannot be larger than MAX_STACK_SIZE");
	m_size = size;
	m_bytes = new std::byte[m_size];
	m_marker = m_bytes;
	m_limit = m_bytes + m_size;
#ifndef NDEBUG
	std::memset(m_marker, 'U', m_size);
#endif //NDEBUG
}

StackAllocator::~StackAllocator()
{
	delete[] m_bytes;
}

void* StackAllocator::Allocate(const size_t size, const size_t alignment) //todo move this to the utils
{
	if (!AllocatorUtils::AddressIsPowerOf2(alignment))
		return nullptr;

	std::byte* alignedMarker = nullptr;
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		alignedMarker = m_marker + (alignment - 1);
		AllocatorUtils::AlignPointer(alignedMarker, alignment);
		if (!AllocatorUtils::CheckMemoryBounds(alignedMarker + size, m_bytes, m_limit))
			return nullptr;

#ifndef NDEBUG
		std::memset(m_marker, 'P', alignedMarker - m_marker);
		std::memset(alignedMarker, 'A', size);
#endif //NDEBUG

		m_marker = alignedMarker + size;
	}
	return static_cast<void*>(alignedMarker);
}

void StackAllocator::FreeToMarker(std::byte* marker)
{
	if (marker == nullptr)
		return;

	if (!AllocatorUtils::CheckMemoryBounds(marker, m_bytes, m_limit))
		return;

	std::lock_guard<std::mutex> lock(m_mutex);
#ifndef NDEBUG	
	ptrdiff_t offset = m_marker - marker;
	std::memset(marker, 'F', offset);
#endif //NDEBUG
	m_marker = marker;
}

void StackAllocator::Reset()
{
	Allocator::Reset();
	std::lock_guard<std::mutex> lock(m_mutex);
	m_marker = m_bytes;
}
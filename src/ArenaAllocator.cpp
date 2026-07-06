#include "AllocatorUtils.h"
#include "ArenaAllocator.h"
#include <algorithm>

ArenaAllocator::ArenaAllocator(const size_t size)
{
	m_size = std::min(size, AllocatorUtils::MAX_STACK_SIZE);
	m_bytes = new std::byte[m_size];
	m_marker = m_bytes;
	m_limit = m_bytes + m_size;
	std::memset(m_marker, 'U', m_size);
}

ArenaAllocator::~ArenaAllocator()
{
	delete[] m_bytes;
}

void* ArenaAllocator::Allocate(const size_t size, const size_t alignment)
{
	if (!AllocatorUtils::AddressIsPowerOf2(alignment))
		return nullptr;

	std::byte* alignedMarker = m_marker + (alignment - 1);
	AllocatorUtils::AlignPointer(alignedMarker, alignment);

	if (alignedMarker + size > m_limit)
		return nullptr;

	std::memset(m_marker, 'P', alignedMarker - m_marker);
	std::memset(alignedMarker, 'A', size);

	m_marker = alignedMarker + size;
	return static_cast<void*>(alignedMarker);
}

void ArenaAllocator::Free()
{
	std::memset(m_bytes, 'F', m_size);
	m_marker = m_bytes;
}
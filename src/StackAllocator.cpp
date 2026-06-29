#include "AllocatorUtils.h"
#include "StackAllocator.h"
#include <algorithm>

StackAllocator::StackAllocator(size_t size)
{
	m_size = std::min(size, MAX_STACK_SIZE);
	m_bytes = new std::byte[m_size];
	m_marker = m_bytes;
	m_limit = m_bytes + m_size;
	std::memset(m_marker, 'U', m_size);
}

 StackAllocator::~StackAllocator()
{
	delete[] m_bytes;
}

 void* StackAllocator::Allocate(size_t size, size_t alignment)
{
	if (!AllocatorUtils::AddressIsPowerOf2(alignment))
		return nullptr;

	std::byte* aligneMarker = m_marker + (alignment - 1);
	AllocatorUtils::AlignedPointer(aligneMarker, alignment);

	if (aligneMarker + size > m_limit)
		return nullptr;

	std::memset(m_marker, 'P', aligneMarker - m_marker);
	std::memset(aligneMarker, 'A', size);

	m_marker = aligneMarker + size;
	return static_cast<void*>(aligneMarker);
}

 void StackAllocator::FreeToMarker(void* marker)
{
	if (marker == nullptr)
		return;

	ptrdiff_t offset = m_marker - marker;
	std::byte* max = m_bytes + m_size;
	if (marker < m_bytes || marker > max)
		return;

	std::memset(marker, 'F', offset);
	m_marker = static_cast<std::byte*>(marker);
}

 void StackAllocator::Reset()
{
	std::memset(m_bytes, 'R', m_size);
	m_marker = m_bytes;
}
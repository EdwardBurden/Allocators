#include "AllocatorUtils.h"
#include "LinearAllocator.h"
#include <algorithm>

LinearAllocator::LinearAllocator(const size_t size)
{
	m_size = std::min(size, MAX_STACK_SIZE);
	m_bytes = new std::byte[m_size];
	m_marker = m_bytes;
	m_limit = m_bytes + m_size;
	std::memset(m_marker, 'U', m_size);

}

LinearAllocator::~LinearAllocator()
{
	delete[] m_bytes;
}


void* LinearAllocator::Allocate(const size_t size, const size_t alignment = alignof(std::max_align_t))
{
	if (!AllocatorUtils::AddressIsPowerOf2(alignment))
		return nullptr;

	std::byte* limit = m_bytes + m_size;
	std::byte* aligneMarker = m_marker + (alignment - 1);
	AllocatorUtils::AlignedPointer(aligneMarker, alignment);

	if (aligneMarker + size > m_limit)
		return nullptr;

	std::memset(m_marker, 'P', aligneMarker - m_marker);
	std::memset(aligneMarker, 'A', size);

	m_marker = aligneMarker + size;
	return static_cast<void*>(aligneMarker);
}

void LinearAllocator::Reset()
{
	std::memset(m_bytes, 'R', m_size);
	m_marker = m_bytes;
}
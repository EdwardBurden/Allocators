#include "AllocatorUtils.h"
#include "LinkedStackAllocator.h"
#include <algorithm>

LinkedStackAllocator::LinkedStackAllocator(size_t size)
{
	m_size = std::min(size, AllocatorUtils::MAX_STACK_SIZE);
	m_bytes = new std::byte[m_size];
	m_marker = m_bytes;
	m_header = m_bytes;
	m_limit = m_bytes + m_size;
	std::memset(m_marker, 'U', m_size);
}

LinkedStackAllocator::~LinkedStackAllocator()
{
	delete[] m_bytes;
}

void* LinkedStackAllocator::Allocate(const size_t size, const size_t alignment)
{
	if (!AllocatorUtils::AddressIsPowerOf2(alignment))
		return nullptr;

	auto maxAlignment = std::max(alignof(Header), alignment);
	std::byte* alignedMarker = m_marker + (maxAlignment - 1) + sizeof(Header); // might be the worse case? so can use it if padding is 0???
	AllocatorUtils::AlignPointer(alignedMarker, maxAlignment);
	ptrdiff_t padding = alignedMarker - m_marker;
	std::memset(m_marker, 'P', alignedMarker - m_marker);
	Header* header = reinterpret_cast<Header*>(alignedMarker - sizeof(Header));
	header->offset = alignedMarker - sizeof(Header) - m_header; //store here the last allocated pointer(m_previousMarker), assign m_previousMarker = alignedMarker at the end too.
	header->padding = alignedMarker - m_marker;
	std::memset(alignedMarker, 'A', size);
	m_header = (alignedMarker - sizeof(Header));
	m_marker = alignedMarker + size;
	return static_cast<void*>(alignedMarker);
}

void LinkedStackAllocator::FreeLastMarker()
{
	auto temp = m_marker;
	Header* header = reinterpret_cast<Header*>(m_header);
	m_marker = m_header - (header->padding - sizeof(Header));
	m_header = m_header - header->offset;
	std::memset(m_marker, 'F', temp - m_marker);
}

void LinkedStackAllocator::FreeMarker(void* ptr) // find the pointer, then free free all
{
	auto temp = m_marker;
	Header* header = reinterpret_cast<Header*>(m_header);
	m_marker = m_header - (header->padding - sizeof(Header));
	m_header = m_header - header->offset;
	std::memset(m_marker, 'F', temp - m_marker);
}
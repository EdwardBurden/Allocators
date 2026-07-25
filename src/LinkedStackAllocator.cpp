#include "AllocatorUtils.h"
#include "LinkedStackAllocator.h"
#include <algorithm>

LinkedStackAllocator::LinkedStackAllocator(const size_t size)
{
	if (size > AllocatorUtils::MAX_STACK_SIZE)
		throw std::runtime_error("Linked stack size in memory cannot be larger than MAX_STACK_SIZE");
	m_size = size;
	m_bytes = new std::byte[m_size];
	m_marker = m_bytes;
	m_header = reinterpret_cast<Header*>(m_marker);
	m_limit = m_bytes + m_size;
#ifndef NDEBUG
	std::memset(m_marker, 'U', m_size);
#endif //NDEBUG
}

LinkedStackAllocator::~LinkedStackAllocator()
{
	delete[] m_bytes;
}

void* LinkedStackAllocator::Allocate(const size_t size, const size_t alignment)
{
	if (!AllocatorUtils::AddressIsPowerOf2(alignment))
		return nullptr;

	size_t maxAlignment = std::max(alignof(Header), alignment);
	size_t headerSize = sizeof(Header);
	std::byte* alignedMarker = m_marker + (maxAlignment - 1) + headerSize;
	AllocatorUtils::AlignPointer(alignedMarker, maxAlignment);
	if (!AllocatorUtils::CheckMemoryBounds(alignedMarker + size, m_bytes, m_limit))
		return nullptr;

#ifndef NDEBUG
	std::memset(m_marker, 'P', alignedMarker - m_marker);
	std::memset(alignedMarker - headerSize, 'H', headerSize);
#endif //NDEBUG
	std::byte* headerByte = alignedMarker - headerSize;
	std::byte* currentHeaderByte = reinterpret_cast<std::byte*>(m_header);
	Header* header = reinterpret_cast<Header*>(headerByte);
	header->offset = headerByte - currentHeaderByte;
	header->padding = alignedMarker - m_marker;
#ifndef NDEBUG
	std::memset(alignedMarker, 'A', size);
#endif //NDEBUG
	m_header = header;
	m_marker = alignedMarker + size;
	return static_cast<void*>(alignedMarker);
}

void LinkedStackAllocator::FreeLastMarker()
{
	if (IsEmpty())
		return;

	SetHeader(m_header);
}

void LinkedStackAllocator::FreeMarker(void* ptr)
{
	if (IsEmpty())
		return;

	if (ptr == nullptr)
		return;

	if (!AllocatorUtils::CheckMemoryBounds(ptr, m_bytes, m_marker))
		return;

	std::byte* marker = static_cast<std::byte*>(ptr);
	Header* header = m_header;
	while (true)
	{
		std::byte* headerByte = reinterpret_cast<std::byte*>(header);
		std::byte* markerByte = headerByte + sizeof(Header);
		if (markerByte == marker)
		{
			SetHeader(header);
			return;
		}
		if (header->offset == 0)
			break;

		header = reinterpret_cast<Header*>(headerByte - header->offset);
	}
}

void LinkedStackAllocator::SetHeader(Header* header)
{
#ifndef NDEBUG
	std::byte* temp = m_marker;
#endif //NDEBUG
	std::byte* headerByte = reinterpret_cast<std::byte*>(header);
	m_marker = headerByte - (header->padding - sizeof(Header));
	m_header = reinterpret_cast<Header*>(headerByte - header->offset);
#ifndef NDEBUG
	std::memset(m_marker, 'F', temp - m_marker);
#endif //NDEBUG
}

void LinkedStackAllocator::Reset()
{
	Allocator::Reset();
	std::lock_guard<std::mutex> lock(m_mutex);
	m_marker = m_bytes;
	m_header = reinterpret_cast<Header*>(m_marker);
}
#include "AllocatorUtils.h"
#include "DualStackAllocator.h"

#include <cstring>
#include <stdexcept>

DualStackAllocator::DualStackAllocator(const size_t size)
{
	if (size > AllocatorUtils::MAX_STACK_SIZE)
		throw std::runtime_error("Dual stack size in memory cannot be larger than MAX_STACK_SIZE");
	m_size = size;
	m_bytes = new std::byte[m_size];
	m_topMarker = m_bytes + m_size;
	m_bottomMarker = m_bytes;
	m_limit = m_bytes + m_size;
#ifndef NDEBUG
	std::memset(m_bytes, 'U', m_size);
#endif //NDEBUG
}

DualStackAllocator::~DualStackAllocator()
{
	delete[] m_bytes;
}

std::byte* DualStackAllocator::GetMarker(const StackArea area) const
{
	switch (area)
	{
	case StackArea::Top:
		return m_topMarker;
	case StackArea::Bottom:
		return m_bottomMarker;
	}
}

void* DualStackAllocator::Allocate(const size_t size, const StackArea area, const size_t alignment)
{
	switch (area)
	{
	case StackArea::Top:
		return AllocateTop(size, alignment);
	case StackArea::Bottom:
		return AllocateBottom(size, alignment);
	}
}

void DualStackAllocator::FreeToMarker(std::byte* marker, const StackArea area)
{
	switch (area)
	{
	case StackArea::Top:
		FreeTop(marker);
		break;
	case StackArea::Bottom:
		FreeBottom(marker);
		break;
	}
}

void* DualStackAllocator::AllocateTop(const size_t size, const size_t alignment) //todo use the version in utils copied form stack
{
	if (!AllocatorUtils::AddressIsPowerOf2(alignment))
		return nullptr;

	std::byte* alignedMarker = nullptr;
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		alignedMarker = m_topMarker - size;
		AllocatorUtils::AlignPointer(alignedMarker, alignment);
		if (!AllocatorUtils::CheckMemoryBounds(alignedMarker, m_bytes, m_limit))
			return nullptr;
#ifndef NDEBUG
		std::memset(alignedMarker + size, 'P', (m_topMarker - size) - alignedMarker);
		std::memset(alignedMarker, 'A', size);
#endif //NDEBUG
		m_topMarker = alignedMarker;
	}
	return static_cast<void*>(alignedMarker);;
}

void* DualStackAllocator::AllocateBottom(const size_t size, const size_t alignment) //todo add to utils.
{
	if (!AllocatorUtils::AddressIsPowerOf2(alignment))
		return nullptr;

	std::byte* alignedMarker = nullptr;
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		alignedMarker = m_bottomMarker + (alignment - 1);
		AllocatorUtils::AlignPointer(alignedMarker, alignment);

		if (!AllocatorUtils::CheckMemoryBounds(alignedMarker + size, m_bytes, m_limit))
			return nullptr;
#ifndef NDEBUG
		std::memset(m_bottomMarker, 'P', alignedMarker - m_bottomMarker);
		std::memset(alignedMarker, 'A', size);
#endif //NDEBUG

		m_bottomMarker = alignedMarker + size;
	}
	return static_cast<void*>(alignedMarker);
}

void DualStackAllocator::FreeTop(std::byte* marker)
{
	if (marker == nullptr)
		return;

	if (!AllocatorUtils::CheckMemoryBounds(marker, m_bytes, m_limit))
		return;

	std::lock_guard<std::mutex> lock(m_mutex);
#ifndef NDEBUG
	ptrdiff_t offset = marker - m_topMarker;
	std::memset(m_topMarker, 'F', offset);
#endif //NDEBUG
	m_topMarker = marker;
}

void DualStackAllocator::FreeBottom(std::byte* marker)
{
	if (marker == nullptr)
		return;

	if (!AllocatorUtils::CheckMemoryBounds(marker, m_bytes, m_limit))
		return;

	std::lock_guard<std::mutex> lock(m_mutex);
#ifndef NDEBUG	
	ptrdiff_t offset = m_bottomMarker - marker;
	std::memset(marker, 'F', offset);
#endif //NDEBUG
	m_bottomMarker = marker;
}

void DualStackAllocator::Reset()
{
	Allocator::Reset();
	std::lock_guard<std::mutex> lock(m_mutex);
	m_topMarker = m_bytes + m_size;
	m_bottomMarker = m_bytes;
}
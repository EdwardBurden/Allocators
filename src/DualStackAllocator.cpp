#include "AllocatorUtils.h"
#include "DualStackAllocator.h"
#include <algorithm>

DualStackAllocator::DualStackAllocator(const size_t size)
{
	m_size = std::min(size, AllocatorUtils::MAX_STACK_SIZE);
	m_bytes = new std::byte[m_size];
	m_topMarker = m_bytes + m_size;
	m_bottomMarker = m_bytes;
	m_limit = m_bytes + m_size;
	std::memset(m_bytes, 'U', m_size);
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

	std::byte* alignedMarker = m_topMarker - size;
	AllocatorUtils::AlignPointer(alignedMarker, alignment);
	if (alignedMarker < m_bytes)
		return nullptr;

	std::memset(alignedMarker + size, 'P', (m_topMarker - size) - alignedMarker);
	std::memset(alignedMarker, 'A', size);
	m_topMarker = alignedMarker;
	return static_cast<void*>(alignedMarker);;
}

void* DualStackAllocator::AllocateBottom(const size_t size, const size_t alignment) //todo add to utils.
{
	if (!AllocatorUtils::AddressIsPowerOf2(alignment))
		return nullptr;

	std::byte* alignedMarker = m_bottomMarker + (alignment - 1);
	AllocatorUtils::AlignPointer(alignedMarker, alignment);

	if (alignedMarker + size > m_limit)
		return nullptr;

	std::memset(m_bottomMarker, 'P', alignedMarker - m_bottomMarker);
	std::memset(alignedMarker, 'A', size);

	m_bottomMarker = alignedMarker + size;
	return static_cast<void*>(alignedMarker);
}

void DualStackAllocator::FreeTop(std::byte* marker)
{
	if (marker == nullptr)
		return;

	if (marker < m_bytes || marker > m_limit)
		return;

	ptrdiff_t offset = marker - m_topMarker;
	std::memset(m_topMarker, 'F', offset);
	m_topMarker = marker;
}

void DualStackAllocator::FreeBottom(std::byte* marker)
{
	if (marker == nullptr)
		return;

	if (marker < m_bytes || marker > m_limit)
		return;

	ptrdiff_t offset = m_bottomMarker - marker;
	std::memset(marker, 'F', offset);
	m_bottomMarker = marker;
}
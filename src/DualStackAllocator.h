#pragma once

enum StackArea
{
	Top = 0,
	Bottom = 1
};

class DualStackAllocator
{
	static constexpr size_t MAX_STACK_SIZE = 1024 * 1024 * 8; // 8MB
public:
	DualStackAllocator(size_t size);
	DualStackAllocator(const DualStackAllocator& other) = delete;
	DualStackAllocator& operator=(const DualStackAllocator& other) = delete;
	DualStackAllocator(const DualStackAllocator&& other) = delete;
	DualStackAllocator& operator=(const DualStackAllocator&& other) = delete;
	~DualStackAllocator();

	std::byte* Allocate(size_t size, StackArea area, size_t alignment);
	void FreeToMarker(std::byte* marker, StackArea area);
	std::byte* GetTopMarker();
private:
	std::byte* AllocateTop(size_t size, size_t alignment);
	std::byte* AllocateBottom(size_t size, size_t alignment);

	size_t m_size;
	std::byte* m_bytes;
	std::byte* m_topMarker;
	std::byte* m_bottomMarker;
};

inline DualStackAllocator::DualStackAllocator(size_t size)
{
	m_size = std::min(size, MAX_STACK_SIZE);
	m_bytes = new std::byte[m_size];
	m_topMarker = m_bytes + m_size;
	m_bottomMarker = m_bytes;
	std::memset(m_bytes, 'U', m_size);
}

inline DualStackAllocator::~DualStackAllocator()
{
	delete[] m_bytes;
}

inline std::byte* DualStackAllocator::Allocate(size_t size, StackArea area, size_t alignment = alignof(std::max_align_t))
{
	switch (area)
	{
	case Top:
		return AllocateTop(size, alignment);
	case Bottom:
		return AllocateBottom(size, alignment);
	default:
		break;
	}
}

inline void DualStackAllocator::FreeToMarker(std::byte* marker, StackArea area)
{
	switch (area)
	{
	case Top:
		ptrdiff_t offset = marker - m_topMarker;
		std::memset(m_topMarker, 'F', offset);
		m_topMarker = marker;
		break;
	}
}

inline std::byte* DualStackAllocator::AllocateTop(size_t size, size_t alignment)
{
	 AllocatorUtils::AllocateDown(m_topMarker, m_bytes, size, alignment);
	 return m_topMarker;
}

inline std::byte* DualStackAllocator::AllocateBottom(size_t size, size_t alignment)
{
	std::byte* limit = m_bytes + m_size;
	return AllocatorUtils::AllocateUp(m_bottomMarker, limit, size, alignment);
}

inline std::byte* DualStackAllocator::GetTopMarker() 
{
	return m_topMarker;
}
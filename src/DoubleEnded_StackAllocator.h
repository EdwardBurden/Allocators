#pragma once

enum StackDirection
{
	Top = 0,
	Bottom = 1
};

class DoubleEnded_StackAllocator
{
	static constexpr size_t MAX_STACK_SIZE = 1024 * 1024 * 8; // 8MB
public:
	DoubleEnded_StackAllocator(size_t size);
	DoubleEnded_StackAllocator(const DoubleEnded_StackAllocator& other) = delete;
	DoubleEnded_StackAllocator& operator=(const DoubleEnded_StackAllocator& other) = delete;
	DoubleEnded_StackAllocator(const DoubleEnded_StackAllocator&& other) = delete;
	DoubleEnded_StackAllocator& operator=(const DoubleEnded_StackAllocator&& other) = delete;
	~DoubleEnded_StackAllocator();

	uint8_t* Allocate(size_t size, StackDirection direction);

private:
	uint8_t* AllocateTop(size_t byteSize);
	uint8_t* AllocateBottom(size_t byteSize);

	size_t m_size;
	uint8_t* m_bytes;
	uint8_t* m_topMarker;
	uint8_t* m_bottomMarker;
};

inline DoubleEnded_StackAllocator::DoubleEnded_StackAllocator(size_t size) 
{
	m_size = std::clamp(size, size_t{ 0 }, MAX_STACK_SIZE);
	m_bytes = new uint8_t[m_size];
	m_topMarker = m_bytes + m_size;
	m_bottomMarker = m_bytes;
	std::memset(m_bytes, 'A', m_size);
}

inline DoubleEnded_StackAllocator::~DoubleEnded_StackAllocator()
{
	delete[] m_bytes;
}

inline uint8_t* DoubleEnded_StackAllocator::Allocate(size_t size, StackDirection direction) 
{
	switch (direction)
	{
	case Top:
		return AllocateTop(size);
	case Bottom:
		return AllocateBottom(size);
	default:
		break;
	}
}

inline uint8_t* DoubleEnded_StackAllocator::AllocateTop(size_t byteSize)
{
	auto pos = m_topMarker - byteSize;
	if (pos < m_bottomMarker)
		return nullptr;

	std::memset(pos, 'T', byteSize);
	return std::exchange(m_topMarker, pos);
}

inline uint8_t* DoubleEnded_StackAllocator::AllocateBottom(size_t byteSize)
{
	auto pos = m_bottomMarker + byteSize;
	if (pos > m_topMarker)
		return nullptr;

	std::memset(m_bottomMarker, 'B', byteSize);
	return std::exchange(m_bottomMarker, pos);
}

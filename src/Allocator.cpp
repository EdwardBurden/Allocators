#include "Allocator.h"

void Allocator::Reset()
{
#ifndef NDEBUG
	std::memset(m_bytes, 'R', m_size);
#endif //NDEBUG
}
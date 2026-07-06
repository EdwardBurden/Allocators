#include "AllocatorUtils.h"
#include "LinkedStackAllocator.h"
#include <algorithm>

LinkedStackAllocator::LinkedStackAllocator(size_t size)
{
	m_size = std::min(size, AllocatorUtils::MAX_STACK_SIZE);
	m_bytes = new std::byte[m_size];
	m_marker = m_bytes;
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

	//todo restart here
	//assume the marker begins ona address ending in 5. forget about storing padding for now
	//create a pointer that looks at the current marker + header space required and check its valid.
	// allocate enough space for both, in the starting space insert rhe previous marker.
	// then return the pointer of the user space withotu the header.
}
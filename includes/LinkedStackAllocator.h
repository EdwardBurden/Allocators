// This stack allocator will save the position of the previous marker before allocating the requirees space(with padding). This creates a linked list of headers which point back toward the base.
// You can use this to rooll back to any valid marker.

#pragma once
#include <cstddef>
#include <cstdint>

struct Header
{
	uintptr_t PreviousMarker;
};

class LinkedStackAllocator
{
public:
	LinkedStackAllocator(const size_t size);
	LinkedStackAllocator(const LinkedStackAllocator& other) = delete;
	LinkedStackAllocator& operator=(const LinkedStackAllocator& other) = delete;
	LinkedStackAllocator(LinkedStackAllocator&& other) = delete;
	LinkedStackAllocator& operator=(LinkedStackAllocator&& other) = delete;
	~LinkedStackAllocator();

	void* Allocate(const size_t size, const size_t alignment = alignof(std::max_align_t));
private:
	size_t m_size;
	std::byte* m_bytes;
	std::byte* m_marker;
	std::byte* m_limit;
};
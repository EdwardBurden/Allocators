/*
LinkedStackAllocator.
This allocator works like the stack allocator but does not require the user to store th emarker position before allocating.
Instead you can now allocate some space and pass back that same pointer as a marker to free it.
A header is used to keep a lousy linked list of allocations so that you can roll back to any valid point, this will unallocate the blocks between though so bear that in mind.
The allocator is thread-safe.
*/

#pragma once
#include "Allocator.h"

struct Header
{
	uint32_t offset; // The offset from the current marker to the last marker.
	uint8_t padding; // the padding added before actual allocation.
};

class LinkedStackAllocator : public Allocator
{
public:
	LinkedStackAllocator(const size_t size);
	LinkedStackAllocator(const LinkedStackAllocator& other) = delete;
	LinkedStackAllocator& operator=(const LinkedStackAllocator& other) = delete;
	LinkedStackAllocator(LinkedStackAllocator&& other) = delete;
	LinkedStackAllocator& operator=(LinkedStackAllocator&& other) = delete;
	~LinkedStackAllocator();

	inline bool IsEmpty() const { return m_marker == m_bytes; };
	void* Allocate(const size_t size, const size_t alignment = alignof(std::max_align_t));
	void FreeLastMarker();
	void FreeMarker(void* marker);
	void Reset() override;
private:
	void SetHeader(Header* header);

	std::byte* m_marker;
	Header* m_header;
};
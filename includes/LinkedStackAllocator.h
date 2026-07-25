// This stack allocator will save the position of the previous marker before allocating the requirees space(with padding). This creates a linked list of headers which point back toward the base.
// You can use this to rooll back to any valid marker.

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
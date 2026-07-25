/*
ArenaAllocator.
Simple allocator, you can allocate up to the limit but can only reset back to the start, no incremental freeing of allocations.
The allocator will add padding so your memory requests are properly aligned.
The allocator is thread-safe.
*/
#pragma once
#include "Allocator.h"

class ArenaAllocator : public Allocator
{
public:
	ArenaAllocator(const size_t size);
	ArenaAllocator(const ArenaAllocator& other) = delete;
	ArenaAllocator& operator=(const ArenaAllocator& other) = delete;
	ArenaAllocator(ArenaAllocator&& other) = delete;
	ArenaAllocator& operator=(ArenaAllocator&& other) = delete;
	~ArenaAllocator();

	void* Allocate(const size_t size, const size_t alignment = alignof(std::max_align_t));
	void Reset() override;

private:
	std::byte* m_marker;
};
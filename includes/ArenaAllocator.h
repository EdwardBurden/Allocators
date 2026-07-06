// Only allocate and free all
// todo rename to arean allocator 

#pragma once
#include <cstddef>

class ArenaAllocator
{
public:
	ArenaAllocator(const size_t size);
	ArenaAllocator(const ArenaAllocator& other) = delete;
	ArenaAllocator& operator=(const ArenaAllocator& other) = delete;
	ArenaAllocator(ArenaAllocator&& other) = delete;
	ArenaAllocator& operator=(ArenaAllocator&& other) = delete;
	~ArenaAllocator();

	void* Allocate(const size_t size, const size_t alignment = alignof(std::max_align_t));
	void Free();

private:
	size_t m_size;
	std::byte* m_bytes;
	std::byte* m_marker;
	std::byte* m_limit;
};
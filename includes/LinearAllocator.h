// Only allocate and free all
// todo rename to arean allocator 

#pragma once
#include <cstddef>

class LinearAllocator
{
public:
	LinearAllocator(const size_t size);
	LinearAllocator(const LinearAllocator& other) = delete;
	LinearAllocator& operator=(const LinearAllocator& other) = delete;
	LinearAllocator(LinearAllocator&& other) = delete;
	LinearAllocator& operator=(LinearAllocator&& other) = delete;
	~LinearAllocator();

	void* Allocate(const size_t size, const size_t alignment = alignof(std::max_align_t));
	void Free();

private:
	static constexpr size_t MAX_STACK_SIZE = 1024 * 1024 * 8; // 8MB
	size_t m_size;
	std::byte* m_bytes;
	std::byte* m_marker;
	std::byte* m_limit;
};
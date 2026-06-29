// Simpliest version, allocates and padds but does not store allocations, so you must give a pointer to return too.
// does not check its a valid pointer so can cuase issues returning to any point in the allocated memory.

#pragma once
#include <cstddef>

class StackAllocator //todo use header so we only rollback without suppliying marker
{
public:
	StackAllocator(size_t size);
	StackAllocator(const StackAllocator& other) = delete;
	StackAllocator& operator=(const StackAllocator& other) = delete;
	StackAllocator(StackAllocator&& other) = delete;
	StackAllocator& operator=(StackAllocator&& other) = delete;
	~StackAllocator();

	inline const std::byte* GetMarker() const { return m_marker; };
	void* Allocate(size_t size, const size_t alignment = alignof(std::max_align_t));
	void FreeToMarker(void* marker);
	void Reset();
private:
	static constexpr size_t MAX_STACK_SIZE = 1024 * 1024 * 8; // 8MB
	size_t m_size;
	std::byte* m_bytes;
	std::byte* m_marker;
	std::byte* m_limit;
};
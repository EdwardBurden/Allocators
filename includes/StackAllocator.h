// Simpliest version, allocates and padds but does not store allocations, so you must give a pointer to return too.
// does not check its a valid pointer so can cuase issues returning to any point in the allocated memory.
// use this by storing the marker and then allocate, use the marker as a point to return to.
// do not use the pointer returned as the marker it is not the same thing.

#pragma once
#include <cstddef>

class StackAllocator //todo use header so we only rollback without suppliying marker
{
public:
	StackAllocator(const size_t size);
	StackAllocator(const StackAllocator& other) = delete;
	StackAllocator& operator=(const StackAllocator& other) = delete;
	StackAllocator(StackAllocator&& other) = delete;
	StackAllocator& operator=(StackAllocator&& other) = delete;
	~StackAllocator();

	template<typename T>
	void* Allocate() 
	{
		return Allocate(sizeof(T), alignof(T));
	}

	inline std::byte* GetMarker() const { return m_marker; };
	void* Allocate(const size_t size, const size_t alignment = alignof(std::max_align_t));
	void FreeToMarker(std::byte* marker);
	void Reset();
private:
	size_t m_size;
	std::byte* m_bytes;
	std::byte* m_marker;
	std::byte* m_limit;
};
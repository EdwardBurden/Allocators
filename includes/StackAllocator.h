/*
StackAllocator.
Allows for allocating and deallocating parts of the memory block.
The allocator will add padding so your memory requests are properly aligned.
You must call GetMarker() to cache the curent marker before requesting allocations, then you use this marker to reset back.
NO DOT USE THE RETURNED POINTER AS A MARKER.
You can pass any pointer within the memory block back to reset to, meaning its up to the user to reset it properly.
The allocator is thread-safe.
*/

#pragma once
#include "Allocator.h"

class StackAllocator : public Allocator
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
	void Reset() override;
private:
	std::byte* m_marker;
};
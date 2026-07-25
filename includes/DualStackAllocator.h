/*
DualStackAllocator.
Works just like the regular stack allocator but allows you to allocate on either end of the memory block.
Use the StackArea enum to pick which area of the memory block you wnat to use.
The allocator is thread-safe.
*/

#pragma once
#include "Allocator.h"

enum class StackArea
{
	Top = 0,
	Bottom = 1
};

class DualStackAllocator : public Allocator
{

public:
	DualStackAllocator(const size_t size);
	DualStackAllocator(const DualStackAllocator& other) = delete;
	DualStackAllocator& operator=(const DualStackAllocator& other) = delete;
	DualStackAllocator(const DualStackAllocator&& other) = delete;
	DualStackAllocator& operator=(const DualStackAllocator&& other) = delete;
	~DualStackAllocator();


	template<typename T>
	void* Allocate(const StackArea area)
	{
		return Allocate(sizeof(T), area, alignof(T)); //todo check they cant overlap areas
	}

	std::byte* GetMarker(const StackArea area) const;
	void* Allocate(const size_t size, const StackArea area, const size_t alignment = alignof(std::max_align_t));
	void FreeToMarker(std::byte* marker, const StackArea area);
	void Reset() override;
private:
	void* AllocateTop(size_t size, size_t alignment);
	void* AllocateBottom(size_t size, size_t alignment);
	void FreeTop(std::byte* marker);
	void FreeBottom(std::byte* marker);

	std::byte* m_topMarker;
	std::byte* m_bottomMarker;
};
// Used just like the stack alocator, keep the marker around so you can free memory.
// you can assign from the top or bottom of the stack.

#pragma once
#include <cstddef>

enum class StackArea
{
	Top = 0,
	Bottom = 1
};

class DualStackAllocator
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
		return Allocate(sizeof(T), area, alignof(T));
	}

	std::byte* GetMarker(const StackArea area) const;
	void* Allocate(const size_t size, const StackArea area, const size_t alignment = alignof(std::max_align_t));
	void FreeToMarker(std::byte* marker, const StackArea area);
private:
	void* AllocateTop(size_t size, size_t alignment);
	void* AllocateBottom(size_t size, size_t alignment);
	void FreeTop(std::byte* marker);
	void FreeBottom(std::byte* marker);

	size_t m_size;
	std::byte* m_bytes;
	std::byte* m_topMarker;
	std::byte* m_bottomMarker;
	std::byte* m_limit;
};
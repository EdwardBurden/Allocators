#pragma once
#include <cstddef>
#include <cstring>

namespace AllocatorUtils
{
	inline std::byte* CreateAlignedBytePointerUp(std::byte* marker, const size_t alignment)
	{
		uintptr_t maxAlignValue = alignment - 1;
		uintptr_t upperBound = reinterpret_cast<uintptr_t>(marker) + maxAlignValue;
		uintptr_t mask = ~maxAlignValue;
		uintptr_t alignedAddress = upperBound & mask;
		return reinterpret_cast<std::byte*>(alignedAddress);
	}

	inline std::byte* CreateAlignedBytePointerDown(std::byte* marker, const size_t alignment)
	{
		uintptr_t mask = ~(alignment - 1);
		uintptr_t alignedAddress = reinterpret_cast<uintptr_t>(marker) & mask;
		return reinterpret_cast<std::byte*>(alignedAddress);
	}

	inline std::byte* AllocateUp(std::byte*& marker, std::byte* limit, size_t size, size_t alignment)
	{
		if (alignment == 0 || (alignment & (alignment - 1)) != 0)
			return nullptr;

		std::byte* alignedCurrentMarker = CreateAlignedBytePointerUp(marker, alignment);
		std::byte* newMarker = alignedCurrentMarker + size;
		if (newMarker > limit)
			return nullptr;

		std::memset(marker, 'P', alignedCurrentMarker - marker);
		std::memset(alignedCurrentMarker, 'A', size);
		marker = newMarker;
		return alignedCurrentMarker;
	}

	inline void AllocateDown(std::byte*& marker, std::byte* limit, size_t size, size_t alignment)
	{
		if (alignment == 0 || (alignment & (alignment - 1)) != 0)
			return;

		std::byte* alignedCurrentMarker = CreateAlignedBytePointerDown(marker - size, alignment);
		std::byte* unaligned = marker - size;
		if (alignedCurrentMarker < limit)
			return;

		std::memset(alignedCurrentMarker + size, 'P', unaligned - alignedCurrentMarker);
		std::memset(alignedCurrentMarker, 'A', size);
		marker = alignedCurrentMarker;
	}
}
#pragma once
#include <cstddef>
#include <cstring>

namespace AllocatorUtils
{
	inline bool AddressIsPowerOf2(const size_t alignment)
	{
		return alignment > 0 && (alignment & (alignment - 1)) == 0;
	}

	inline uintptr_t AlignAddress(uintptr_t address, const size_t alignment)
	{
		uintptr_t mask = ~(alignment - 1);
		uintptr_t alignedAddress = address & mask;
		return alignedAddress;
	}

	inline void AlignedPointer(std::byte*& ptr, const size_t alignment)
	{
		uintptr_t address = reinterpret_cast<uintptr_t>(ptr);
		uintptr_t alignedAddress = AlignAddress(address, alignment);
		ptr = reinterpret_cast<std::byte*>(alignedAddress);
	}


	// Updates the marker to the next aligned position plus the size. Returns the aligned current marker.
	/*inline std::byte* AllocateUp(std::byte*& marker, const std::byte* limit, const size_t size, const size_t alignment)
	{
		if (!AddressIsPowerOf2(alignment))
			return nullptr;
		
		std::byte* targetMarker = marker + (alignment - 1);
		std::byte* alignedCurrentMarker = CreateAlignedPointer(targetMarker, alignment);
		std::byte* newMarker = alignedCurrentMarker + size;
		if (newMarker > limit)
			return nullptr;

		std::memset(marker, 'P', alignedCurrentMarker - marker);
		std::memset(alignedCurrentMarker, 'A', size);
		marker = newMarker;
		return alignedCurrentMarker;
	}

	inline std::byte* AllocateDown(std::byte*& marker, const std::byte* limit, const size_t size, const size_t alignment)
	{
		if (!AddressIsPowerOf2(alignment))
			return nullptr;

		std::byte* targetMarker = marker - size;
		std::byte* alignedCurrentMarker = CreateAlignedPointer(targetMarker, alignment);
		if (alignedCurrentMarker < limit)
			return nullptr;

		std::memset(alignedCurrentMarker + size, 'P', targetMarker - alignedCurrentMarker);
		std::memset(alignedCurrentMarker, 'A', size);
		marker = alignedCurrentMarker;
		return marker;
	}*/
}
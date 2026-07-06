#pragma once
#include <cstddef>
#include <cstring>

namespace AllocatorUtils
{
	static size_t MAX_STACK_SIZE = 1024 * 1024 * 8; // 8MB

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

	inline void AlignPointer(std::byte*& ptr, const size_t alignment)
	{
		uintptr_t address = reinterpret_cast<uintptr_t>(ptr);
		uintptr_t alignedAddress = AlignAddress(address, alignment);
		ptr = reinterpret_cast<std::byte*>(alignedAddress);
	}
}
#pragma once
#include <cstddef>

namespace AllocatorUtils
{
	static size_t MAX_STACK_SIZE = 1024 * 1024 * 8; // 8MB
	static size_t MAX_POOL_SIZE = 1024 * 1024 * 8; // 8MB

	inline bool IsAligned(ptrdiff_t ptr, const size_t elementSize)
	{
		return ptr % elementSize == 0;
	}

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

	template<typename T>
	inline bool CheckMemoryBounds(T* ptr, const std::byte* minimum, const std::byte* maximum)
	{
		std::byte* pointer = reinterpret_cast<std::byte*>(ptr);
		return pointer >= minimum && pointer <= maximum;
	}
}
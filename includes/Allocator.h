#pragma once
#include <cstddef>
#include <mutex>
#include <cstring>

class Allocator
{
public:
	Allocator() = default;
	virtual ~Allocator() = default;
	Allocator(const Allocator& other) = delete;
	Allocator& operator=(const Allocator& other) = delete;
	Allocator(Allocator&& other) = delete;
	Allocator& operator=(Allocator&& other) = delete;

	virtual void Reset();
protected:
	size_t m_size;
	std::byte* m_bytes;
	std::byte* m_limit;
	std::mutex m_mutex;
};
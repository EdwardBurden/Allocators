# Allocators

A collection of custom memory allocators for C++, each suited to different allocation patterns (linear, stack-based, dual-ended, and pooled). All derive from a common `Allocator` interface and are thread-safe.

## ArenaAllocator
A simple bump allocator. Memory is handed out sequentially and can't be freed individually, only `Reset()` all at once, invalidating every pointer returned so far. Thread-safe, no copy/move.
### Usage
```cpp
ArenaAllocator arena(1024 * 1024);
void* a = arena.Allocate(64);
if (a == nullptr) { /* out of space */ }
arena.Reset(); // a is now invalid
```
## StackAllocator
A bump allocator like Arena, but you can rewind to any earlier point instead of only the very start. You must cache a marker with `GetMarker()` before allocating, then pass it to `FreeToMarker()` to free everything allocated since. Thread-safe, no copy/move.
### Usage
```cpp
StackAllocator stack(1024 * 1024);
std::byte* marker = stack.GetMarker();
void* a = stack.Allocate(64);
if (a == nullptr) { /* out of space */ }
stack.FreeToMarker(marker); // a is now invalid
```
## DualStackAllocator
Two stack allocators sharing one block, growing toward each other from opposite ends (Top and Bottom) — allocate/free/reset each side independently via `StackArea::Top` or `StackArea::Bottom`. Thread-safe, no copy/move.
### Usage
```cpp
DualStackAllocator dual(1024 * 1024);
std::byte* topMarker = dual.GetMarker(StackArea::Top);
void* a = dual.Allocate(64, StackArea::Top);
void* b = dual.Allocate(64, StackArea::Bottom);
dual.FreeToMarker(topMarker, StackArea::Top); // a is now invalid, b unaffected
```
## LinkedStackAllocator
A stack allocator like StackAllocator, but tracks each allocation's marker internally via a hidden linked-list header, so you can free the most recent allocation with `FreeLastMarker()` or rewind to any pointer previously returned by `Allocate()` with `FreeMarker()`, without having to cache markers yourself. Thread-safe, no copy/move.
### Usage
```cpp
LinkedStackAllocator stack(1024 * 1024);
void* a = stack.Allocate(64);
void* b = stack.Allocate(64);
stack.FreeMarker(a); // frees b and a
stack.FreeLastMarker(); // frees only the most recent allocation
```
## PoolAllocator
A fixed capacity pool of equally sized elements of type `T`, handed out and reclaimed in any order (unlike the stack allocators above). Free elements store a pointer to the next free element creating an internal linked list, so `Allocate()` and `Free()` are both O(1). `T` must be at least pointer  sized, must not be overaligned, and its constructor must be `noexcept`. Thread-safe, no copy/move.
### Usage
```cpp
PoolAllocator<MyStruct> pool(1000); // capacity of 1000 elements
MyStruct* a = pool.Allocate(/* constructor args */);
if (a == nullptr) { /* pool is full */ }
pool.Free(a); // a's destructor runs, slot returned to the pool
pool.Reset(); // frees all elements at once, without running destructors

```
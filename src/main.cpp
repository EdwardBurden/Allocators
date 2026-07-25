#include "ArenaAllocator.h"
#include "StackAllocator.h"
#include "DualStackAllocator.h"
#include "LinkedStackAllocator.h"
#include "PoolAllocator.h"
#include <new>

struct ExampleClass {
	ExampleClass(float x) : m_x(x) {}
	float m_x;
};

void test_arena()
{
	ArenaAllocator* linear = new ArenaAllocator(1024);
	linear->Allocate(128, 16); 
	linear->Allocate(512, 9); 
	linear->Allocate(512, 4);
	linear->Allocate(8); 
	linear->Reset(); 
	delete linear;
}

void test_stack()
{
	StackAllocator* stack = new StackAllocator(256);
	auto m1 = stack->GetMarker();
	float* f = static_cast<float*> (stack->Allocate<float>());
	auto p1 = stack->Allocate(16, 4);
	ExampleClass* example = new (p1) ExampleClass(5);
	example->m_x = 45.66666f;
	auto m2 = stack->GetMarker();
	auto p2 = stack->Allocate<ExampleClass>();
	example = new (p2) ExampleClass(5);
	stack->FreeToMarker(m2);
	stack->FreeToMarker(m1);
	stack->Reset();
	delete stack;
}


void test_dual_stack()
{
	DualStackAllocator* dualStack = new DualStackAllocator(32);
	auto bottom = dualStack->GetMarker(StackArea::Bottom);
	auto top = dualStack->GetMarker(StackArea::Top);
	dualStack->Allocate<uint32_t>(StackArea::Bottom);
	dualStack->Allocate<char>(StackArea::Top);
	dualStack->Allocate<char>(StackArea::Top);
	dualStack->Allocate<char>(StackArea::Top);
	dualStack->Allocate<char>(StackArea::Top);
	auto beforepadding = dualStack->GetMarker(StackArea::Top);
	dualStack->Allocate<uint64_t>(StackArea::Top);
	dualStack->Allocate<uint32_t>(StackArea::Bottom);
	dualStack->Allocate<uint32_t>(StackArea::Bottom);
	dualStack->Allocate<uint32_t>(StackArea::Bottom);
	dualStack->Allocate<uint32_t>(StackArea::Bottom);
	dualStack->FreeToMarker(beforepadding, StackArea::Top);
	 dualStack->Allocate<char>(StackArea::Bottom);
	 dualStack->Allocate<char>(StackArea::Top);
	dualStack->FreeToMarker(bottom, StackArea::Bottom);
	dualStack->FreeToMarker(top, StackArea::Top);
	delete  dualStack;

}

void test_linked_stack()
{
	LinkedStackAllocator* linked = new LinkedStackAllocator(256);
	auto t1 = static_cast<std::byte*>(linked->Allocate(5, 4));
	auto t2 = linked->Allocate(3, 4);
	auto t3 = linked->Allocate(8, 4);
	auto t4 = linked->Allocate(4, 4);
	linked->FreeMarker(t1 + 1);
	linked->FreeMarker(t2);
	linked->FreeMarker(t3);
	linked->FreeMarker(t2);
	linked->FreeMarker(t1);
	linked->Allocate(5, 4);
	linked->Allocate(5, 4);
	linked->FreeMarker(t1 + 1);
	linked->FreeLastMarker();
	linked->FreeLastMarker();
	linked->FreeLastMarker();
	linked->Allocate(4, 4);
	linked->Reset();
	delete linked;
}

void test_pool()
{
	//PoolAllocator<float>* floatPool = new PoolAllocator<float>(8);

	struct vector
	{
		float x;
		float y;
		float z;
		vector(int f) noexcept
		{
			x = f;
			y = 2 * f;
			z = 3 * f;
		}vector() noexcept {
			x, y, z = 0;
		}
	};


	PoolAllocator<vector>* vectorPool = new PoolAllocator<vector>(2);
	auto v1 = vectorPool->Allocate(1);
	auto v2 = vectorPool->Allocate(2);
	vectorPool->Free(v2);
	vectorPool->Free(v2);

	auto v3 = vectorPool->Allocate(3);
	std::byte* test = reinterpret_cast<std::byte*>(v1) + 1;
	vectorPool->Free(reinterpret_cast<vector*>(test));
	vectorPool->Free(v3);
	auto v4 = vectorPool->Allocate(4);
	vectorPool->Free(v2);
	auto v5 = vectorPool->Allocate(5);
	auto v6 = vectorPool->Allocate(6);
	auto v7 = vectorPool->Allocate(7);
	auto v8 = vectorPool->Allocate(8);
	auto v9 = vectorPool->Allocate(9);
	auto v10 = vectorPool->Allocate();
	auto v11 = vectorPool->Allocate();
	auto v12 = vectorPool->Allocate();
	auto v13 = vectorPool->Allocate();
	vectorPool->Free(v8);
	vectorPool->Free(v9);
	vectorPool->Reset();
	delete vectorPool;
}


int main()
{
	test_arena();
	test_stack();
	test_dual_stack();
	test_linked_stack();
	test_pool();

	return 0;
}
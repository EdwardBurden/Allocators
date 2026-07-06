#include "ArenaAllocator.h"
#include "StackAllocator.h"
#include "DualStackAllocator.h"
#include "LinkedStackAllocator.h"
#include <new>

struct ExampleClass {
	ExampleClass(float x) : m_x(x) {}
	float m_x;
};

void test_arena()
{
	ArenaAllocator* linear = new ArenaAllocator(1024);
	linear->Allocate(128, 16); // todo use examples
	linear->Allocate(512, 9); // todo use examples
	linear->Allocate(512, 4); // todo use examples
	linear->Allocate(8); // todo use examples
	linear->Free(); // frees all
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
}


void test_dual_stack()
{
	DualStackAllocator* dualStack = new DualStackAllocator(256);
	auto m1 = dualStack->GetMarker(StackArea::Bottom);
	int* intexample = static_cast<int*>(dualStack->Allocate(4, StackArea::Bottom));
	intexample = static_cast<int*>(dualStack->Allocate<int>(StackArea::Bottom));
	auto stackpointer = dualStack->Allocate<ExampleClass>(StackArea::Bottom);
	ExampleClass* example = new (stackpointer) ExampleClass(5);
	dualStack->FreeToMarker(m1, StackArea::Bottom);

	 m1 = dualStack->GetMarker(StackArea::Top);
	auto l = dualStack->Allocate<long>(StackArea::Top);
	l = dualStack->Allocate<char>(StackArea::Top);
	l = dualStack->Allocate<int>(StackArea::Top);
	dualStack->FreeToMarker(m1, StackArea::Top);
}

void test_linked_stack()
{
	LinkedStackAllocator* linked = new LinkedStackAllocator(256);


}


int main()
{
	// todo expans tests with failure catching and proper examples
	test_arena();
	test_stack();
	test_dual_stack();
	test_linked_stack();

	return 0;
}
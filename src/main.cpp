#include "LinearAllocator.h"
#include "StackAllocator.h"
#include <new>

struct ExampleClass {
	ExampleClass(float x) : m_x(x) {}
	float m_x;
};

int main()
{
	LinearAllocator* linear = new LinearAllocator(1024);
	linear->Allocate(128, 16); // todo use examples
	linear->Allocate(512, 9); // todo use examples
	linear->Allocate(512, 4); // todo use examples
	linear->Allocate(8); // todo use examples
	linear->Free(); // frees all


	StackAllocator* stack = new StackAllocator(256);

	auto m1 = stack->GetMarker();
	float* f = static_cast<float*> stack->Allocate(4);
	auto p1 = stack->Allocate(16, 4);
	ExampleClass* example = new (p1) ExampleClass(5);
	example->m_x = 45.66666f;
	auto m2 = stack->GetMarker();
	auto p2 = stack->Allocate<ExampleClass>();

	stack->FreeToMarker(p2);
	stack->FreeToMarker(m2);


	return 0;
}
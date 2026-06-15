#include <iostream>
#include <random>
#include "StackAllocator.h"
#include "DualStackAllocator.h"

class Container
{
public:
	char a;
	int x;
	float y;
	char t;
};

int RandomInt(int min, int max)
{
	static std::random_device rd;
	static std::mt19937 rng(rd());
	std::uniform_int_distribution<int> dist(min, max);
	return dist(rng);
}

int main()
{
	DualStackAllocator dualStack(1024);
	auto s1 = dualStack.GetTopMarker();
	auto p1 = dualStack.Allocate(16, StackArea::Top, 4);
	Container* cc = new (p1) Container();
	auto s2 = dualStack.GetTopMarker();
	auto p2 = dualStack.Allocate(1, StackArea::Top);

	dualStack.FreeToMarker(s2 , StackArea::Top);
	dualStack.FreeToMarker(s1, StackArea::Top);



		StackAllocator stack(256);
	std::vector<std::byte*> ptrs;

	ptrs.push_back(stack.Allocate(7));
	ptrs.push_back(stack.Allocate(1));
	ptrs.push_back(stack.Allocate(17));
	ptrs.push_back(stack.Allocate(17, 3));

	for (int i = 0; i < 20; i++)
	{
		auto ptr = stack.Allocate<Container>();
		if (ptr != nullptr) {
			auto container = new (ptr) Container();
			container->a = '{';
			container->x = 6;
			container->y = 612.4;
			container->t = 'C';
		}
		ptrs.push_back(ptr);
		ptrs.push_back(stack.Allocate(1));
	}

	while (!ptrs.empty())
	{
		stack.FreeToMarker(ptrs.back());
		ptrs.pop_back();
	}

	stack.Reset();

	return 0;
}
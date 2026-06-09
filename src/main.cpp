#include <iostream>
#include <random>
#include "StackAllocator.h"
#include "DoubleEnded_StackAllocator.h"

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
	StackAllocator stack(256);
	Container* c[20];
	//int bytes[20];

	stack.Allocate(7, 4);
	stack.Allocate(1, 1);
	stack.Allocate(17, 16);


	for (int i = 0; i < 20; i++)
	{
		//bytes[i] = RandomInt(0, 50);

		auto ptr = stack.Allocate(sizeof(Container), alignof(Container));
		if (ptr != nullptr) {
			c[i] = new (ptr) Container();
			c[i]->a = '{';
			c[i]->x = 6;
			c[i]->y = 612.4;
			c[i]->t = 'C';
		}
	}

	for (int i = 19; i >= 0; i--)
	{
		stack.FreeToMarker(reinterpret_cast<std::byte*>(c[i]));
	}

	/*DoubleEnded_StackAllocator de_stack(1024);

	for (int i = 0; i < 100; i++)
	{
		auto rand= RandomInt(0, 50);
		auto dir = RandomInt(0, 1);
		auto ptr = de_stack.Allocate(rand, (StackDirection)dir);
	}*/
	return 0;
}
#include "../includes/LinearAllocator.h"
#include <new>

struct TestStruct1
{
	int t;
};



void testLinearAllocate()
{
	LinearAllocator* linear = new LinearAllocator(256);
	//float* floats = static_cast<float*>(linear->Allocate(10 * sizeof(float), alignof(float)));
	void* largeAllocation = linear->Allocate(200, 16);
	auto test = new (largeAllocation) TestStruct1(); //then check can access 
	 largeAllocation = linear->Allocate(55, 16);
	// check 


	linear->Reset();


	float* floats = static_cast<float*>(linear->Allocate(10 * sizeof(float), alignof(float)));

}

void testLinearReset()
{

}
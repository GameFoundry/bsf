---
title: Memory allocation
---

When allocating memory in bs::f it is prefered (but not required) to use bs::f allocator functions instead of the standard *new* / *delete* operators or *malloc* / *free*.

- Use @bs::bs_new instead of *new* and @bs::bs_delete instead of *delete*.
- Use @bs::bs_newN instead of *new[]* and @bs::bs_deleteN instead of *delete[]*.
- Use @bs::bs_alloc instead of *malloc* and @bs::bs_free instead of *free*.

This ensures the bs::f can keep track of all allocated memory, which ensures better debugging and profiling, as well as ensuring that internal memory allocation method can be changed in the future.

~~~~~~~~~~~~~{.cpp}
// Helper structure
struct MyStruct 
{ 
	MyStruct() {}
	MyStruct(int a, bool b)
		:a(a), b(b)
	{ }
	
	int a; 
	bool b; 
};

// Allocating memory the normal way
MyStruct* ptr = new MyStruct(123, false);
MyStruct** ptrArray = new MyStruct[5];
void* rawMem = malloc(12);

delete ptr;
delete[] ptrArray;
free(rawMem);

// Allocating memory the bs::f way
MyStruct* bsPtr = bs_new<MyStruct>(123, false);
MyStruct** bsPtrArray = bs_newN<MyStruct>(5);
void* bsRawMem = bs_alloc(12);

bs_delete(bsPtr);
bs_deleteN(bsPtrArray, 5);
bs_free(bsRawMem);
~~~~~~~~~~~~~

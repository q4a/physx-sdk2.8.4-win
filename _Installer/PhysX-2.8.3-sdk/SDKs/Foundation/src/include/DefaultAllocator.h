#ifndef NX_FOUNDATION_DEFAULTALLOCATOR
#define NX_FOUNDATION_DEFAULTALLOCATOR
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/
#include "NxUserAllocator.h"
namespace NxFoundation
	{

/**
Default implementation of memory allocator using standard C malloc / free / realloc.
*/
class DefaultAllocator : public NxUserAllocator
	{
	public:
	/**
	Allocates size bytes of memory.

	Compatible with the standard C malloc().
	*/
	void* malloc(size_t size);

	/**
	Allocates size bytes of memory.

	Same as above, but with extra debug info fields.
	*/
	void* mallocDEBUG(size_t size, const char* fileName, int line);

	/**
	Resizes the memory block previously allocated with malloc() or
	realloc() to be size() bytes, and returns the possibly moved memory.

	Compatible with the standard C realloc().
	*/
	void* realloc(void* memory, size_t size);

	/**
	Frees the memory previously allocated by malloc() or realloc().

	Compatible with the standard C free().
	*/
	void free(void* memory);
	};
	}
#endif

#ifndef PXN_MEMORY_H
#define PXN_MEMORY_H

#include "px_config.h"

#if !defined (PXD_PLATFORM_CELL) && !defined(PXD_PLATFORM_WII) && !defined(PXD_PLATFORM_WIIU) && !defined(PXD_PLATFORM_OSX)
#	include <malloc.h>
#	include <new>
#endif

void* PXD_RESTRICT PxnMalloc(size_t size, const char* file, unsigned int line);
void PxnFree(void* mem, const char* file, unsigned int line);

#ifdef	__SPU__
#define	PXN_MALLOC(x)	malloc(x)
#define	PXN_FREE(x)		free(x)
#else
#define PXN_MALLOC(x) PxnMalloc(x,__FILE__,__LINE__)
#define PXN_FREE(x) PxnFree(x,__FILE__,__LINE__)
#endif

#define PXN_DELETE(x) delete x
#define PXN_DELETE_SINGLE(x) if (x) { delete x;		x = NULL; }
#define PXN_DELETE_ARRAY(x) if (x) { delete []x;	x = NULL; }

// Simple allocation class
class PxnAllocator
{
public:
	// Simple typedefs
	typedef size_t					size_type;
	typedef ptrdiff_t				difference_type;


	// Allocation methods
	static PXD_INLINE PxByte* PXD_RESTRICT malloc(size_type n)
	{
		return (PxByte*)PXN_MALLOC(n);
	}

	static PXD_INLINE void free(void* ptr)
	{
		PXN_FREE(ptr);
	}
};

// Simple allocateable class
class PxnAllocateable
{
public:
	// Allocation operators
	void* operator new(size_t size)
	{
		return PXN_MALLOC(size);
	}

	void operator delete(void* p)
	{
		PXN_FREE(p);
	}
};

// Allocate an object on the stack
#ifdef PXD_COMPILER_VC
	#define PxnStackAlloc(x) _alloca(x)
#elif defined PXD_COMPILER_GNUC
#if defined(__flash__) || defined(__webgl__)
	#include <stdlib.h>
#else
	#include <alloca.h>
#endif
	#define PxnStackAlloc(x) alloca(x)
#elif defined(PXD_COMPILER_CW)
	#include <alloca.h>
	#define PxnStackAlloc(x) alloca(x)
#elif defined(PXD_COMPILER_GHS)
	#include <alloca.h>
	#define PxnStackAlloc(x) alloca(x)
#else
	#error "You need to define a way of doing stack-allocation on your platform"
#endif

#endif


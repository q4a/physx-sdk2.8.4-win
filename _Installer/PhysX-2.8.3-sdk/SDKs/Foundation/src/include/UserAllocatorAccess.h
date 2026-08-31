#ifndef NX_FOUNDATION_USER_ALLOCATOR_ACCESS
#define NX_FOUNDATION_USER_ALLOCATOR_ACCESS
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/
#include "NxUserAllocator.h"
#include "Nx.h"
//#include "MemAlloc.h"
#include "NxAlignedMalloc.h"

#include <stdlib.h>
#ifdef WIN32
	#include <crtdbg.h>
#endif
namespace NxFoundation
	{
extern NxUserAllocator * nxFoundationSDKAllocator;
/**
Allocator, which is used to access the global NxUserAllocator instance
(used for dynamic data types template instantiation).
*/
class UserAllocatorAccess
	{
	public:
		/**
		Allocates size bytes of memory.
		*/
		NX_INLINE void* malloc(size_t size, NxMemoryType type)
			{
#if USE_HEAP_MANAGER
			return MemAlloc(size, type);
#else
				return nxFoundationSDKAllocator->malloc(size,type);
#endif
			}

		/**
		Allocates size bytes of memory.
		Same as above, but with extra debug info fields.
		*/
		NX_INLINE void* mallocDEBUG(size_t size, const char* fileName, int line, const char* className, NxMemoryType type)
			{
#if USE_HEAP_MANAGER
			return MemAlloc(size, type);
#else
			return nxFoundationSDKAllocator->mallocDEBUG(size, fileName, line, className, type );
#endif
			}

		/**
		Resizes the memory block previously allocated with malloc() or
		realloc() to be size() bytes, and returns the possibly moved memory.
		*/
		NX_INLINE void* realloc(void* memory, size_t size)
			{
#if USE_HEAP_MANAGER
			return MemRealloc(memory, size);
#else
			return nxFoundationSDKAllocator->realloc( memory, size );
#endif
			}

		/**
		Frees the memory previously allocated by malloc() or realloc().
		*/
		NX_INLINE void free(void* memory)
			{
#if USE_HEAP_MANAGER
			if(memory)	MemFree(memory);
#else
			if ( memory ) nxFoundationSDKAllocator->free(memory);
#endif
			}

		void checkDEBUG(void)
		{
#if USE_HEAP_MANAGER
			MemCheck();
			nxFoundationSDKAllocator->checkDEBUG();
#else
			nxFoundationSDKAllocator->checkDEBUG();
#endif
		}
	};

/**
Allocator, which is used to access the global NxUserAllocator instance
(used for dynamic data types template instantiation).

This allocator can align memory.
*/

template<NxU32 A> //A == Alignment
class UserAllocAccessAlign
	{
	public:
		/**
		Allocates size bytes of memory.
		*/
		NX_INLINE void* malloc(size_t size, NxMemoryType type)
			{
				return NxAlignedMalloc(size, A);//TODO: use correct type.
			}

		/**
		Allocates size bytes of memory.
		Same as above, but with extra debug info fields.
		*/
		NX_INLINE void* mallocDEBUG(size_t size, const char* fileName, int line, const char* className, NxMemoryType type)
			{
				return NxAlignedMalloc(size, A); // TODO: use type and debug info.
			}

		/**
		Resizes the memory block previously allocated with malloc() or
		realloc() to be size() bytes, and returns the possibly moved memory.
		*/
		NX_INLINE void* realloc(void* memory, size_t size)
			{
				void *newMemory = NxAlignedReAlloc(memory, size, A);
				return newMemory;
			}

		/**
		Frees the memory previously allocated by malloc() or realloc().
		*/
		NX_INLINE void free(void* memory)
			{
				NxAlignedFree(memory);
			}

		void checkDEBUG(void)
		{
#if USE_HEAP_MANAGER
			MemCheck();
			nxFoundationSDKAllocator->checkDEBUG();
#else
			nxFoundationSDKAllocator->checkDEBUG();
#endif
		}

	};

	}

#endif






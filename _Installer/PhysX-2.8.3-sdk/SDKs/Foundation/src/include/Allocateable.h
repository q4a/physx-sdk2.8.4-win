#ifndef NX_FOUNDATION_ALLOCATEABLE
#define NX_FOUNDATION_ALLOCATEABLE
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/
#include "NxUserAllocator.h"
#include "NxFoundationSDK.h"
//#include "MemAlloc.h"
#include <new>
//#include "NxThread.h"

namespace NxFoundation
	{
extern NxUserAllocator* nxFoundationSDKAllocator;	//the SDK defines this.
extern NxU32			nxAllocaThreshold;
extern NxU32			nxUserAllocaThreshold;
extern bool				nxAutoThreshold;
/**
Subclasses of this base class automatically take part in user memory management
Should be called Allocateable but then we collide with Ice::Allocateable.
*/
class NxAllocateable
	{
	public:
	NX_INLINE void* operator new(size_t size, NxMemoryType type);
	NX_INLINE void* operator new[](size_t size, NxMemoryType type);
	// VangelisK: According to the C++ standards, operator new should never return NULL
	// DS: WTF is this anyway - &&'d out out on Oct 11th 2007
#if !defined(__CELLOS_LV2__) && 0 
	NX_INLINE void* operator new(size_t,void*) { return NULL; }
#endif

	NX_INLINE void* operator new(size_t size, const char* fileName, int line, const char* className, NxMemoryType type);
	NX_INLINE void* operator new[](size_t size, const char* fileName, int line, const char* className, NxMemoryType type);
	NX_INLINE void  operator delete(void* p, const char*, int, const char*, NxMemoryType type);
	NX_INLINE void  operator delete[](void* p, const char*, int, const char*, NxMemoryType type);

	// delete parameters have been added to fix warning C4291 in VC7. But they're not used.
	NX_INLINE void operator delete(void*,void*) {}
	NX_INLINE void  operator delete(void* p);
	NX_INLINE void  operator delete(void* p,NxMemoryType type);
	NX_INLINE void  operator delete[](void* p);
	NX_INLINE void  operator delete[](void* p, NxMemoryType type);
	};

NX_INLINE void* NxAllocateable::operator new(size_t size, NxMemoryType type)
	{
	#if USE_HEAP_MANAGER
	return MemAlloc(size, type);
	#else
	return nxFoundationSDKAllocator->malloc(size, type);
	#endif
	}

NX_INLINE void* NxAllocateable::operator new[](size_t size, NxMemoryType type)
	{
	#if USE_HEAP_MANAGER
	return MemAlloc(size, type);
	#else
	return nxFoundationSDKAllocator->malloc(size, type);
	#endif
	}

NX_INLINE void NxAllocateable::operator delete(void* p)
	{
	#if USE_HEAP_MANAGER
	MemFree(p);
	#else
	nxFoundationSDKAllocator->free(p);
	#endif
	}

NX_INLINE void NxAllocateable::operator delete(void* p,NxMemoryType type)
	{
	#if USE_HEAP_MANAGER
	MemFree(p);
	#else
	nxFoundationSDKAllocator->free(p);
	#endif
	}

NX_INLINE void NxAllocateable::operator delete[](void* p)
	{
	#if USE_HEAP_MANAGER
	MemFree(p);
	#else
	nxFoundationSDKAllocator->free(p);
	#endif
	}

NX_INLINE void  NxAllocateable::operator delete[](void* p, NxMemoryType type)
	{
	#if USE_HEAP_MANAGER
	MemFree(p);
	#else
	nxFoundationSDKAllocator->free(p);
	#endif
	}

NX_INLINE void* NxAllocateable::operator new(size_t size, const char* fileName, int line, const char* className, NxMemoryType type)
	{
	#if USE_HEAP_MANAGER
	return MemAlloc(size, type);
	#else
	return nxFoundationSDKAllocator->mallocDEBUG(size, fileName, line, className, type);
	#endif
	}

NX_INLINE void* NxAllocateable::operator new[](size_t size, const char* fileName, int line, const char* className, NxMemoryType type)
	{
	#if USE_HEAP_MANAGER
	return MemAlloc(size, type);
	#else
	return nxFoundationSDKAllocator->mallocDEBUG(size, fileName, line, className, type);
	#endif
	}

NX_INLINE void NxAllocateable::operator delete(void* p, const char*, int, const char*, NxMemoryType type)
	{
	#if USE_HEAP_MANAGER
	MemFree(p);
	#else
	nxFoundationSDKAllocator->free(p);
	#endif
	}

NX_INLINE void NxAllocateable::operator delete[](void* p, const char*, int, const char*, NxMemoryType type)
	{
	#if USE_HEAP_MANAGER
	MemFree(p);
	#else
	nxFoundationSDKAllocator->free(p);
	#endif
	}
	}

#if USE_HEAP_MANAGER
	#define NX_ALLOC(x,y)					NxFoundation::MemAlloc(x,NX_MEMORY_##y)
	#define NX_ALLOC_PERSISTENT(x)			NxFoundation::MemAlloc(x,NX_MEMORY_PERSISTENT)
	#define NX_ALLOC_TEMP(x)				NxFoundation::MemAlloc(x,NX_MEMORY_TEMP)
	#define NX_ALLOC_ARRAY(x, y, z)			(x *)NxFoundation::MemAlloc(sizeof(x)*(y),NX_MEMORY_##z)
	#define	NX_FREE(x)						if(x)	{ NxFoundation::MemFree(x); x = NULL;	}
#else
	#ifdef _DEBUG
		#define NX_ALLOC(x,y)					NxFoundation::nxFoundationSDKAllocator->mallocDEBUG(x, (const char *)__FILE__, __LINE__, #x, NX_MEMORY_##y)
		#define NX_ALLOC_PERSISTENT(x)			NxFoundation::nxFoundationSDKAllocator->mallocDEBUG(x, (const char *)__FILE__, __LINE__, #x, NX_MEMORY_PERSISTENT)
		#define NX_ALLOC_TEMP(x)				NxFoundation::nxFoundationSDKAllocator->mallocDEBUG(x, (const char *)__FILE__, __LINE__, #x, NX_MEMORY_TEMP)
		#define NX_ALLOC_ARRAY(x, y, z)			(x *)NxFoundation::nxFoundationSDKAllocator->mallocDEBUG(sizeof(x) * (y), (const char *)__FILE__, __LINE__, #x, NX_MEMORY_##z)
	#else
		#define NX_ALLOC(x,y)					NxFoundation::nxFoundationSDKAllocator->malloc(x, NX_MEMORY_##y)
		#define NX_ALLOC_PERSISTENT(x)			NxFoundation::nxFoundationSDKAllocator->malloc(x, NX_MEMORY_PERSISTENT)
		#define NX_ALLOC_TEMP(x)				NxFoundation::nxFoundationSDKAllocator->malloc(x, NX_MEMORY_TEMP)
		#define NX_ALLOC_ARRAY(x, y, z)			(x *)NxFoundation::nxFoundationSDKAllocator->malloc(sizeof(x) * (y), NX_MEMORY_##z)
	#endif
	#define	NX_FREE(x)							if(x)	{ NxFoundation::nxFoundationSDKAllocator->free(x); x = NULL;	}

	//#define NX_ALLOCA(var, type, number)		var = (type *)_alloca(sizeof(type)*number);
	//#define NX_FREEA(var)						NULL

#if defined (_XBOX) | defined (WIN32) | defined (__CELLOS_LV2__) | defined (LINUX) | defined(__PPCGEKKO__) | defined(CAFE)
	// Stack allocation with alloc fallback for large allocations (>50% of default stack size for platform)
	#define NX_ALLOCA(var, type, number)		bool alloced_##var = false;\
	if (sizeof(type)*number*2 > NxFoundation::nxAllocaThreshold) { var = (type *)NX_ALLOC_TEMP(sizeof(type)*number); alloced_##var = true;} else { var = (type *)NxAlloca(sizeof(type)*number);};
	#define NX_FREEA(var) if (alloced_##var) NX_FREE(var);

	//// Stack allocation with alloc fallback using Structured Exception Handling
	//#define NX_ALLOCA(var, type, number)		bool alloced_##var = false;\
	//__try { var = (type *)_alloca(sizeof(type)*number); } __except(1) { var = (type *)NX_ALLOC_TEMP(sizeof(type)*number); alloced_##var = true;}
	//#define NX_FREEA(var) if (alloced_##var) NX_FREE(var);

	//if (sizeof(type)*number > 1000) printf("%i bytes at %s(%i)\n", sizeof(type)*number, __FILE__, __LINE__);\
	//NxFoundation::NxScratchAllocation scratch_##var(sizeof(type)*number);\
												//var = (type *)scratch_##var.getMemory();	
#else
	#define NX_ALLOCA(var, type, number)		var = (type *)NxAlloca(sizeof(type)*number);
	#define NX_FREEA(var)						0;
#endif
#endif


#endif

#ifndef PXC_FASTMEMORY_H
#define PXC_FASTMEMORY_H

/*
Memory routines for operating on _cached_ memory. On some platforms the system provides faster routines
to copy memory if we know we are dealing with cached memory (which we should always be doing).
*/

#include "px_config.h"
#include "PxnError.h"
#include <string.h>

#ifdef PXD_PLATFORM_XBOX360
	#include "xbox360/PxcFastMemory_XBOX.h"
#else

CUDA_CALLABLE PXD_FORCE_INLINE void* PxcMemset(void* PXD_RESTRICT dest, int c, size_t count)
{
	return memset(dest,c,count);
}

CUDA_CALLABLE_HOST PXD_FORCE_INLINE void* PxcMemcpy(void* PXD_RESTRICT dest, const void* PXD_RESTRICT src, size_t count)
{
	return memcpy(dest,src,count);
}

CUDA_CALLABLE_HOST PXD_FORCE_INLINE void* PxcMemMove(void* dest, const void* src, size_t count)
{
	return memmove(dest,src,count);
}

CUDA_CALLABLE PXD_FORCE_INLINE void PxcPrefetch(const void* ptr, PxU32 offset = 0)
{
}

CUDA_CALLABLE PXD_FORCE_INLINE void PxcBlockInvalidate(void* PXD_RESTRICT dest, size_t count)
{
#ifdef _DEBUG
	memset(dest, 0xab, count);// try and catch incorrect usage.
#endif
}

CUDA_CALLABLE PXD_FORCE_INLINE void PxcBlockInvalidate128(void* PXD_RESTRICT dest, size_t offset)
{
#ifdef _DEBUG
	void *ptr = (void *)(((size_t)dest + offset) & ~0x7f);
	PxcBlockInvalidate(ptr, 128);
#endif
}

#endif

#endif

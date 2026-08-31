#ifndef PXC_FASTMEMORY_XBOX_H
#define PXC_FASTMEMORY_XBOX_H

/*
Memory routines for operating on _cached_ memory. On some platforms(eg xbox360) the system provides faster routines
to copy memory if we know we are dealing with cached memory(which we should always be doing).

Note: In general the PxcMem*() routines should be used when dealing with large blocks of data(eg > 50bytes).
Intrinsics are likely faster for small blocks. However, at least on xbox360 XbMemset() is always faster.
*/

#include "PxcPlatformInclude.h"

PXD_FORCE_INLINE void* PxcMemset(void* PXD_RESTRICT dest, int c, size_t count)
{
	return XMemSet(dest,c,count);
}

PXD_FORCE_INLINE void* PxcMemcpy(void* PXD_RESTRICT dest, const void* PXD_RESTRICT src, size_t count)
{
	return XMemCpy(dest,src,count);
}

PXD_FORCE_INLINE void* PxcMemMove(void* dest, const void* src, size_t count)
{
	return memmove(dest,src,count);
}

PXD_FORCE_INLINE void PxcPrefetch(const void* ptr, PxU32 offset = 0)
{
	__dcbt(offset, ptr);
}

// On xbox360 it is faster to zero cache lines(with special instruction)
// than write to them if the entire cache line is written.
// Since the cache line does not have to be loaded into the cache.
PXD_FORCE_INLINE void PxcBlockInvalidate(void* PXD_RESTRICT dest, size_t count)
{
#ifdef _DEBUG
	PxcMemset(dest, 0xab, count);
#else
	PxcMemset(dest, 0, count);
#endif
}

PXD_FORCE_INLINE void PxcBlockInvalidate128(void* PXD_RESTRICT dest, size_t offset)
{
	PXN_ASSERT(((size_t(dest) + offset) & 0x7f) == 0);

	__dcbz128(offset, dest);
}

#endif

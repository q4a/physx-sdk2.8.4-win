/* NVIDIA/SCE CONFIDENTIAL
   NVIDIA PhysX(TM) SDK for PlayStation(R)3 Version PS3_PhysX_283_B2_forSDK330_hlsrc
*/
// SCE CONFIDENTIAL
// Copyright (C) Sony Computer Entertainment Inc.
// All Rights Reserved.

#ifndef _CELLUTIL_H_
#define _CELLUTIL_H_

#include "CellSPU.h"

#ifdef __CELLOS_LV2__
	#define OFFSET_OF(Class, Member)	__builtin_offsetof(Class, Member)
#else
	#define OFFSET_OF(Class, Member)	(size_t)&(((Class*)0)->Member)
#endif

#define SIZE_OF(Class, Member)   (size_t)(sizeof(((Class *) 0)->Member))

#ifdef __SPU__
	#define CELL_MM_PTR(x) uint32_t
#else 
	#define CELL_MM_PTR(x) x *
#endif

#ifdef _MSC_VER
	#define CELL_ALIGN(alignment, decl) __declspec(align(alignment)) decl
#elif __GNUC__
	#define CELL_ALIGN(alignment, decl) decl __attribute__ ((aligned(alignment)))
#endif

#define CELL_ALIGN_SIZE_8(size) (((unsigned)(size)+7)&((unsigned)~7))
#define CELL_ALIGN_SIZE_16(size) (((unsigned)(size)+15)&((unsigned)~15))


#define CELL_BIT(b) (1<<(b))
#define CELL_MIN(a,b)      ((a) < (b) ? (a) : (b))

/**
 * Fetch a chunk of data that isn't necessarily 16 bytes aligned.
 * The buffer size should be 32 bytes larger than the transfer size to allow for padding.
 * Returns the actual start within the buffer to the data.
 */
inline void* cellDmaGetUnaligned(void *ls, uintptr_t ea, uint32_t size, uint32_t tag)
{
	uint32_t offset = ea & 0x0f;

	ea &= ~0x0f;
	size = CELL_ALIGN_SIZE_16(size + offset);

	cellDmaGet(ls, (uint64_t)ea, size, tag, 0, 0);

	return (void*) ((unsigned char*) ls + offset);
}

/**
 * Fetch a chunk of data that isn't necessarily 16 bytes aligned.
 * The buffer size should be 32 bytes larger than the transfer size to allow for padding.
 * Returns the actual start within the buffer to the data.
 *
 * Note: cellDmaLargeGet version
 */
inline void* cellDmaLargeGetUnaligned(void *ls, uintptr_t ea, uint32_t size, uint32_t tag)
{
	uint32_t offset = ea & 0x0f;

	ea &= ~0x0f;
	size = CELL_ALIGN_SIZE_16(size + offset);

	cellDmaLargeGet(ls, (uint64_t)ea, size, tag, 0, 0);

	return (void*) ((unsigned char*) ls + offset);
}

#endif // _CELLUTIL_H_



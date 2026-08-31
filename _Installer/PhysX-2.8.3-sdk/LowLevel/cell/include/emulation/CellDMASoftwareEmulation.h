/* NVIDIA/SCE CONFIDENTIAL
   NVIDIA PhysX(TM) SDK for PlayStation(R)3 Version PS3_PhysX_283_B2_forSDK330_hlsrc
*/
// SCE CONFIDENTIAL
// Copyright (C) Sony Computer Entertainment Inc.
// All Rights Reserved.

#ifndef _CELLDMASOFTWAREEMULATION_H_
#define _CELLDMASOFTWAREEMULATION_H_

#include <assert.h>
#include <stdio.h>
#include <string.h>

// Disable annoying warning
#pragma warning(disable: 4311 )


// copy paste from spu/include/sys/integertypes.h
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed short int16_t;
typedef unsigned short uint16_t;
typedef signed int int32_t;
typedef unsigned int uint32_t;
typedef signed int intptr_t;
typedef unsigned int uintptr_t;
typedef unsigned long long uint64_t;


// copy paste from spu/include/spu_mfcio.h

typedef struct mfc_list_element {
  uint64_t notify       :  1;   /** Stall-and-notify bit  */
  uint64_t reserved     : 15;
  uint64_t size         : 16;   /** Transfer size */
  uint64_t eal          : 32;   /** Lower word of effective address */
} mfc_list_element_t;


// copied from spu/include/cell/dma.h and modified

typedef mfc_list_element_t CellDmaListElement;

// DMA Assertions
#define cellDmaNormalAssert(ls,ea,size,tag) \
	assert((((uintptr_t)(ls) & 0xf) == 0)	& \
				  (((uintptr_t)(ea) & 0xf) == 0)	& \
				  (((size) & 0xf) == 0)				& \
				  ((size) <= (16<<10))				& \
				  ((tag) < 32))

#define cellDmaSmallAssert(ls,ea,size,tag) \
	assert((((uintptr_t)(ls) & 0xf) == ((uintptr_t)(ea) & 0xf))	& \
				  (((uintptr_t)(ls) & (size - 1)) == 0)					& \
				  (((size)==1)||((size)==2)||((size)==4)||((size)==8))	& \
				  ((tag) < 32))

#define cellDmaListAssert(ls,ea,la,lsize,tag) \
	assert((((uintptr_t)(ls) & 0xf) == 0)	& \
				  (((uintptr_t)(ea) & 0xf) == 0)	& \
				  (((uintptr_t)(la) &   7) == 0)	& \
				  (((lsize) & 7) == 0)				& \
				  ((lsize) <= (16<<10))				& \
				  ((tag) < 32))

#define cellDmaAtomicAssert(ls,ea) \
	assert((((uintptr_t)(ls) & 0x7f) == 0)	& \
				  (((uintptr_t)(ea) & 0x7f) == 0))

#define cellDmaPutqllucAssert(ls,ea,tag) \
	assert((((uintptr_t)(ls) & 0x7f) == 0)	& \
				  (((uintptr_t)(ea) & 0x7f) == 0)	& \
				  ((tag) < 32))

#define cellDmaLargeAssert(ls,ea,tag) \
	assert((((uintptr_t)(ls) & 0xf) == 0)	& \
				  (((uintptr_t)(ea) & 0xf) == 0)	& \
				  ((tag) < 32))


// DMA which transfer size is a multiple of 16bytes

static void cellDmaPut(const void *ls, uint64_t ea, uint32_t size, uint32_t tag, uint32_t tid, uint32_t rid)
{
	cellDmaNormalAssert(ls, ea, size, tag);
	//mfc_put((volatile void*)(uintptr_t)ls, ea, size, tag, tid, rid);
	memcpy((void*)ea, ls, size);
}

static void cellDmaPutf(const void *ls, uint64_t ea, uint32_t size, uint32_t tag, uint32_t tid, uint32_t rid)
{
	cellDmaNormalAssert(ls, ea, size, tag);
	//mfc_putf((volatile void*)(uintptr_t)ls, ea, size, tag, tid, rid);
	assert(true);
}

static void cellDmaPutb(const void *ls, uint64_t ea, uint32_t size, uint32_t tag, uint32_t tid, uint32_t rid)
{
	cellDmaNormalAssert(ls, ea, size, tag);
	//mfc_putb((volatile void*)(uintptr_t)ls, ea, size, tag, tid, rid);
	assert(true);
}

static void cellDmaGet(void *ls, uint64_t ea, uint32_t size, uint32_t tag, uint32_t tid, uint32_t rid)
{
	cellDmaNormalAssert(ls, ea, size, tag);
	//mfc_get(ls, ea, size, tag, tid, rid);
	memcpy(ls, (void*)ea, size);
}

static void cellDmaGetf(void *ls, uint64_t ea, uint32_t size, uint32_t tag, uint32_t tid, uint32_t rid)
{
	cellDmaNormalAssert(ls, ea, size, tag);
	//mfc_getf(ls, ea, size, tag, tid, rid);
	assert(true);
}

static void cellDmaGetb(void *ls, uint64_t ea, uint32_t size, uint32_t tag, uint32_t tid, uint32_t rid)
{
	cellDmaNormalAssert(ls, ea, size, tag);
	//mfc_getb(ls, ea, size, tag, tid, rid);
	assert(true);
}

// DMA which transfer size is within 16bytes

static void cellDmaSmallPut(const void *ls, uint64_t ea, uint32_t size, uint32_t tag, uint32_t tid, uint32_t rid)
{
	cellDmaSmallAssert(ls, ea, size, tag);
	//mfc_put((volatile void*)(uintptr_t)ls, ea, size, tag, tid, rid);
	memcpy((void*)ea, ls, size);
}

static void cellDmaSmallPutf(const void *ls, uint64_t ea, uint32_t size, uint32_t tag, uint32_t tid, uint32_t rid)
{
	cellDmaSmallAssert(ls, ea, size, tag);
	//mfc_putf((volatile void*)(uintptr_t)ls, ea, size, tag, tid, rid);
	assert(true);
}

static void cellDmaSmallPutb(const void *ls, uint64_t ea, uint32_t size, uint32_t tag, uint32_t tid, uint32_t rid)
{
	cellDmaSmallAssert(ls, ea, size, tag);
	//mfc_putb((volatile void*)(uintptr_t)ls, ea, size, tag, tid, rid);
	assert(true);
}

static void cellDmaSmallGet(void *ls, uint64_t ea, uint32_t size, uint32_t tag, uint32_t tid, uint32_t rid)
{
	cellDmaSmallAssert(ls, ea, size, tag);
	//mfc_get(ls, ea, size, tag, tid, rid);
	memcpy(ls, (void*)ea, size);
}

static void cellDmaSmallGetf(void *ls, uint64_t ea, uint32_t size, uint32_t tag, uint32_t tid, uint32_t rid)
{
	cellDmaSmallAssert(ls, ea, size, tag);
	//mfc_getf(ls, ea, size, tag, tid, rid);
	assert(true);
}

static void cellDmaSmallGetb(void *ls, uint64_t ea, uint32_t size, uint32_t tag, uint32_t tid, uint32_t rid)
{
	cellDmaSmallAssert(ls, ea, size, tag);
	//mfc_getb(ls, ea, size, tag, tid, rid);
	assert(true);
}

// List DMA

static void cellDmaListPut(const void *ls, uint64_t ea, const CellDmaListElement *list, uint32_t lsize, uint32_t tag, uint32_t tid, uint32_t rid)
{
	cellDmaListAssert(ls, ea, list, lsize, tag);
	//mfc_putl((volatile void*)(uintptr_t)ls, ea, list, lsize, tag, tid, rid);
	assert(true);
}

static void cellDmaListPutf(const void *ls, uint64_t ea, const CellDmaListElement *list, uint32_t lsize, uint32_t tag, uint32_t tid, uint32_t rid)
{
	cellDmaListAssert(ls, ea, list, lsize, tag);
	//mfc_putlf((volatile void*)(uintptr_t)ls, ea, list, lsize, tag, tid, rid);
	assert(true);
}

static void cellDmaListPutb(const void *ls, uint64_t ea, const CellDmaListElement *list, uint32_t lsize, uint32_t tag, uint32_t tid, uint32_t rid)
{
	cellDmaListAssert(ls, ea, list, lsize, tag);
	//mfc_putlb((volatile void*)(uintptr_t)ls, ea, list, lsize, tag, tid, rid);
	assert(true);
}

static void cellDmaListGet(void *ls, uint64_t ea, const CellDmaListElement *list, uint32_t lsize, uint32_t tag, uint32_t tid, uint32_t rid)
{
	cellDmaListAssert(ls, ea, list, lsize, tag);
	//mfc_getl(ls, ea, list, lsize, tag, tid, rid);
	assert(true);
}

static void cellDmaListGetf(void *ls, uint64_t ea, const CellDmaListElement *list, uint32_t lsize, uint32_t tag, uint32_t tid, uint32_t rid)
{
	cellDmaListAssert(ls, ea, list, lsize, tag);
	//mfc_getlf(ls, ea, list, lsize, tag, tid, rid);
	assert(true);
}

static void cellDmaListGetb(void *ls, uint64_t ea, const CellDmaListElement *list, uint32_t lsize, uint32_t tag, uint32_t tid, uint32_t rid)
{
	cellDmaListAssert(ls, ea, list, lsize, tag);
	//mfc_getlb(ls, ea, list, lsize, tag, tid, rid);
	assert(true);
}

// DMA which transfer size is larger than 16k Bytes

static void cellDmaLargeGet(void *ls, uint64_t ea, uint32_t size, uint32_t tag, uint32_t tid, uint32_t rid)
{
	cellDmaLargeAssert(ls, ea, tag);
	memcpy(ls, (void*)ea, size);
}

static inline void
cellDmaLargePut(const void *ls, uint64_t ea, uint32_t size, uint32_t tag, uint32_t tid, uint32_t rid)
{
	cellDmaLargeAssert(ls, ea, tag);
	memcpy((void*)ea, ls, size);
}

// DMA Utilities - Tag wait

static inline void cellDmaCancelTagStatusUpdate(void)
{
	//mfc_write_tag_update_immediate();
	//do {} while(__builtin_expect(mfc_stat_tag_update() == 0,0));
	//mfc_read_tag_status();
}

static inline uint32_t cellDmaCancelAndWaitTagStatusAny(uint32_t tagmask)
{
	//cellDmaCancelTagStatusUpdate();
	//mfc_write_tag_mask(tagmask);
	//return mfc_read_tag_status_any();
	return tagmask;
}

static inline uint32_t cellDmaCancelAndWaitTagStatusAll(uint32_t tagmask)
{
	//cellDmaCancelTagStatusUpdate();
	//mfc_write_tag_mask(tagmask);
	//return mfc_read_tag_status_all();
	return tagmask;
}

static inline uint32_t cellDmaWaitTagStatusImmediate(uint32_t tagmask)
{
	//mfc_write_tag_mask(tagmask);
	//return mfc_read_tag_status_immediate();
	return tagmask;
}

static inline uint32_t cellDmaWaitTagStatusAny(uint32_t tagmask)
{
	//mfc_write_tag_mask(tagmask);
	//return mfc_read_tag_status_any();
	return tagmask;
}

static inline uint32_t cellDmaWaitTagStatusAll(uint32_t tagmask)
{
	//mfc_write_tag_mask(tagmask);
	//return mfc_read_tag_status_all();
	return tagmask;
}


#endif // _CELLDMASOFTWAREEMULATION_H_



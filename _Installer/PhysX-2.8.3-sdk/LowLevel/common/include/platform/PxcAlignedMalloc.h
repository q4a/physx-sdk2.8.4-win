#ifndef PXC_ALIGNEDMALLOC_H
#define PXC_ALIGNEDMALLOC_H

#include "PxnMemory.h"

/*!
Allocate aligned memory.
Alignment must be a power of 2!
*/
PXD_INLINE void* PxcAlignedMalloc(size_t size, size_t alignment)
{
	const size_t sizePad = alignment+sizeof(PxU8*);
	const size_t sizePlusPad = size + sizePad;

	PxU8* blockPtr = reinterpret_cast<PxU8*>(PXN_MALLOC(sizePlusPad));

	PxU8* retPtr = blockPtr;

	//Move up padding amount
	retPtr += sizePad;

	//Round down to alignment border
	size_t off = (((size_t)retPtr)&(alignment-1));
	retPtr -= off; 

	//Store block-ptr
	retPtr -= sizeof(PxU8*);
	*((PxU8**)retPtr) = blockPtr;
	retPtr += sizeof(PxU8*);

	return retPtr;
}

/*!
Free memory allocated with PxcAlignedMalloc
*/
PXD_INLINE void PxcAlignedFree(void* ptr)
{
	if(!ptr)
		return;

	//Get pointer to block
	PxU8* blockPtr = reinterpret_cast<PxU8*>(ptr);
	blockPtr -= sizeof(PxU8*);
	PxU8* storedPtr = *((PxU8**)blockPtr);

	PXN_FREE(storedPtr);
}

#endif

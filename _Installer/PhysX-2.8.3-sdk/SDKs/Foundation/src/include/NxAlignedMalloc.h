#ifndef NX_FOUNDATION_ALIGNEDMALLOC_H
#define NX_FOUNDATION_ALIGNEDMALLOC_H

#include "Allocateable.h"

/*!
Allocate aligned memory.
Alignment must be a power of 2!
*/
NX_INLINE void* NxAlignedMalloc(size_t size, size_t alignment)
{
	const size_t sizePad = alignment + sizeof(NxU8*) + sizeof(size_t);//must store size + ptr for realloc.
	const size_t sizePlusPad = size + sizePad;

	// pass in memory block type?
	NxU8* blockPtr = reinterpret_cast<NxU8*>(NX_ALLOC(sizePlusPad,PERSISTENT));

	NxU8* retPtr = blockPtr;

	//Move up padding amount
	retPtr += sizePad;

	//Round down to alignment border
	size_t off = (((size_t)retPtr)&(alignment-1));
	retPtr -= off; 

	//Store block-ptr and size
	retPtr -= (sizeof(NxU8*) + sizeof(size_t));
	
	*((size_t *)retPtr) = size;
	retPtr += sizeof(size_t);
	
	*((NxU8**)retPtr) = blockPtr;
	retPtr += sizeof(NxU8*);

	return retPtr;
}

/*!
Free memory allocated with NxAlignedMalloc
*/
NX_INLINE void NxAlignedFree(void* ptr)
{
	if(!ptr)
		return;

	//Get pointer to block
	NxU8* blockPtr = reinterpret_cast<NxU8*>(ptr);
	blockPtr -= sizeof(NxU8*);
	NxU8* storedPtr = *((NxU8**)blockPtr);

	NX_FREE(storedPtr);
}

NX_INLINE size_t NxAlignedSize(void *ptr)
{
	NxU8 *bytePtr = (NxU8 *)ptr;

	bytePtr -= (sizeof(NxU8 *) + sizeof(size_t));
	return *((size_t *)bytePtr);
}

NX_INLINE void *NxAlignedReAlloc(void *ptr, size_t size, size_t alignment)
{
	if(!ptr)
		return NULL;

	void *newMem = NxAlignedMalloc(size, alignment);
	if(newMem == NULL)
		return NULL;

	size_t old_size = NxAlignedSize(ptr);
	memcpy(newMem, ptr, (old_size < size) ? old_size : size);

	NxAlignedFree(ptr);
	ptr = NULL;

	return newMem;
}

#endif

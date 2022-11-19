// ===============================================================================
//						  NVIDIA PHYSX SDK TRAINING PROGRAMS
//							       USER ALLOCATOR
//
//						    Written by Bob Schade, 5-1-06
// ===============================================================================

#ifndef USERALLOCATOR_H
#define USERALLOCATOR_H

#include "SampleMutex.h"

class UserAllocator : public NxUserAllocator
{
public:
	UserAllocator();
	virtual ~UserAllocator();

public:
	void	reset();

	void*	malloc(size_t size);
	void*	malloc(size_t size, NxMemoryType type);
	void*	mallocDEBUG(size_t size, const char* file, int line);
	void*	mallocDEBUG(size_t size, const char* file, int line, const char* className, NxMemoryType type);
	void*	realloc(void* memory, size_t size);
	void	free(void* memory);

private:
#if defined(_DEBUG)
	NxPtr*	mMemBlockList;
	NxU32	mMemBlockListSize;
	NxU32	mMemBlockFirstFree;
	NxU32	mMemBlockUsed;
#endif

	size_t	mNbAllocatedBytes;
	size_t	mHighWaterMark;
	NxI32	mTotalNbAllocs;
	NxI32	mNbAllocs;
	NxI32	mNbReallocs;

	// to protect the mMemBlockList & the statistics data
	SampleMutex	mAllocatorLock;
};

#endif  // USERALLOCATOR_H


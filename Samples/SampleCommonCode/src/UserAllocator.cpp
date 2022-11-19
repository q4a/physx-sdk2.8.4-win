// ===============================================================================
//						  NVIDIA PHYSX SDK TRAINING PROGRAMS
//							       USER ALLOCATOR
//
//						    Written by Bob Schade, 5-1-06
// ===============================================================================

#include <stdio.h>
#include "NxPhysics.h"
#include "UserAllocator.h"

#define MEMBLOCKSTART		64

#define USE_MUTEX

#if defined(USE_MUTEX)
	#define	LOCK()          mAllocatorLock.lock()
	#define	UNLOCK()        mAllocatorLock.unlock()
#else
	#define	LOCK()
	#define	UNLOCK()
#endif

struct BlockHeader
{
	static const size_t marker;

	size_t		sign;
	size_t		size;
#if defined(_DEBUG)
	const char*	file;
	size_t		line;
	const char*	name;
	size_t		indx;
#endif
};

const size_t BlockHeader::marker = (size_t)0xbadbad00deadbabeLL;

UserAllocator::UserAllocator() : mNbAllocatedBytes(0), mHighWaterMark(0), mTotalNbAllocs(0), mNbAllocs(0), mNbReallocs(0)
{
#if defined(_DEBUG)
	// Initialize the Memory blocks list (DEBUG mode only)
	mMemBlockList = reinterpret_cast<NxPtr *>(::malloc(MEMBLOCKSTART * sizeof (NxPtr)));
	memset(mMemBlockList, 0, MEMBLOCKSTART * sizeof (NxPtr));
	mMemBlockListSize	= MEMBLOCKSTART;
	mMemBlockFirstFree	= 0;
	mMemBlockUsed		= 0;
#endif
}

UserAllocator::~UserAllocator()
{
	LOCK();

	if (mNbAllocatedBytes)	printf("Memory leak detected: %d bytes non released\n", mNbAllocatedBytes);
	if (mNbAllocs)		printf("Remaining allocs: %d\n", mNbAllocs);
	printf("Nb alloc: %d\n", mTotalNbAllocs);
	printf("Nb realloc: %d\n", mNbReallocs);
	printf("High water mark: %d Kb\n", mHighWaterMark/1024);

#if defined(_DEBUG)
	// Scanning for memory leaks
	if (NULL != mMemBlockList && 0 != mMemBlockUsed)
	{
		NxU32 NbLeaks = 0;
		printf("\n\n  ICE Message Memory leaks detected :\n\n");
		for (NxU32 i = 0; i < mMemBlockListSize; ++i)
		{
			if (NULL == mMemBlockList[i]) continue;
			BlockHeader *ptr = reinterpret_cast<BlockHeader *>(mMemBlockList[i]);
			printf(" Address 0x%08x, %u bytes (%s), allocated in: %s(%d):\n\n", &ptr[1], ptr->size, ptr->name, ptr->file, ptr->line);
			++NbLeaks;
		}
		printf("\n  Dump complete (%d leaks)\n\n", NbLeaks);
	}

	// Free the Memory Block list
	::free(mMemBlockList);
	mMemBlockList = NULL;
#endif

	UNLOCK();
}

void UserAllocator::reset()
{
	LOCK();

	mNbAllocatedBytes	= 0;
	mHighWaterMark		= 0;
	mNbAllocs			= 0;

	UNLOCK();
}

void* UserAllocator::malloc(size_t size)
{
	printf("Obsolete code called!\n");
	return NULL;
}

void* UserAllocator::malloc(size_t size, NxMemoryType type)
{
#if defined(_DEBUG)
	return mallocDEBUG(size, NULL, 0, "Undefined", type);
#else
	if (0 == size)
	{
		printf("Warning: trying to allocate 0 bytes\n");
		return NULL;
	}

	LOCK();

	BlockHeader *ptr = reinterpret_cast<BlockHeader *>(::malloc(size + sizeof (BlockHeader)));
	ptr->sign = BlockHeader::marker;
	ptr->size = size;

	++mTotalNbAllocs;
	++mNbAllocs;
	mNbAllocatedBytes += size;
	if (mNbAllocatedBytes > mHighWaterMark)	mHighWaterMark = mNbAllocatedBytes;

	UNLOCK();

	return &ptr[1];
#endif
}

void* UserAllocator::mallocDEBUG(size_t size, const char* file, int line)
{
	printf("Obsolete code called!\n");
	return NULL;
}

void* UserAllocator::mallocDEBUG(size_t size, const char* file, int line, const char* className, NxMemoryType type)
{
#if !defined(_DEBUG)
	return malloc(size, type);
#else
	if (0 == size)
	{
		printf("Warning: trying to allocate 0 bytes\n");
		return NULL;
	}

	LOCK();

	BlockHeader *ptr = reinterpret_cast<BlockHeader *>(::malloc(size + sizeof (BlockHeader)));
	ptr->sign = BlockHeader::marker;
	ptr->size = size;
	ptr->file = file;
	ptr->line = line;
	ptr->name = className;
	ptr->indx = ~0;

	++mTotalNbAllocs;
	++mNbAllocs;
	mNbAllocatedBytes += size;
	if (mNbAllocatedBytes > mHighWaterMark)	mHighWaterMark = mNbAllocatedBytes;

	// Insert the allocated block in the debug memory block list
	if (NULL != mMemBlockList)
	{
		ptr->indx = mMemBlockFirstFree;
		mMemBlockList[mMemBlockFirstFree] = ptr;

		++mMemBlockUsed;
		if (mMemBlockUsed >= mMemBlockListSize)
		{
			NxPtr *tps = reinterpret_cast<NxPtr *>(::malloc((mMemBlockListSize + MEMBLOCKSTART) * sizeof (NxPtr)));
			memcpy(tps, mMemBlockList, mMemBlockListSize * sizeof (NxPtr));
			memset(&tps[mMemBlockListSize], 0, MEMBLOCKSTART * sizeof (NxPtr));
			::free(mMemBlockList);
			mMemBlockList = tps;
			mMemBlockListSize += MEMBLOCKSTART;
		}

		while (mMemBlockFirstFree < mMemBlockListSize && mMemBlockList[mMemBlockFirstFree]) ++mMemBlockFirstFree;
		if (mMemBlockFirstFree >= mMemBlockListSize)
		{
			mMemBlockFirstFree = 0;
			while (mMemBlockFirstFree < mMemBlockListSize && mMemBlockList[mMemBlockFirstFree]) ++mMemBlockFirstFree;
		}

		if (mMemBlockFirstFree >= mMemBlockListSize)
		{
			fprintf(stderr, "Oops: no free block!!!!\n");
			fflush(stderr);
		}
	}

	UNLOCK();

	return &ptr[1];
#endif
}

void* UserAllocator::realloc(void* memory, size_t size)
{
	if (NULL == memory)
	{
		printf("Warning: trying to realloc null pointer\n");
		return NULL;
	}

	if (0 == size)
	{
		printf("Warning: trying to realloc 0 bytes\n");
	}

	LOCK();

	BlockHeader *ptr = reinterpret_cast<BlockHeader *>(memory) - 1;
	if (BlockHeader::marker != ptr->sign)
	{
		fprintf(stderr, "Error: realloc unknown memory!!\n");
		fflush(stderr);
	}

	++mNbReallocs;
	mNbAllocatedBytes -= ptr->size;
	mNbAllocatedBytes += size;
	if (mNbAllocatedBytes > mHighWaterMark)	mHighWaterMark = mNbAllocatedBytes;

#if defined(_DEBUG)
	//need to store this for removal later
	size_t oldMemBlockIndex = ptr->indx;
#endif

	ptr->sign = 0;
	ptr->size = 0;
	BlockHeader *ptr2 = reinterpret_cast<BlockHeader *>(::realloc(ptr, size + sizeof (BlockHeader)));
	ptr2->sign = BlockHeader::marker;
	ptr2->size = size;

#if defined(_DEBUG)
	ptr2->file = NULL;
	ptr2->line = 0;
	ptr2->name = NULL;
	ptr2->indx = oldMemBlockIndex;

	if (NULL != mMemBlockList)
	{
		if (ptr2->indx >= mMemBlockListSize)
		{
			fprintf(stderr, "Oops: index (%u) >= list size (%u)\n", ptr2->indx, mMemBlockListSize);
			fflush(stderr);
		}
		else
		{
			mMemBlockList[ptr2->indx] = ptr2;
		}
	}
#endif

	UNLOCK();

	return &ptr2[1];
}

void UserAllocator::free(void* memory)
{
	if (NULL == memory)
	{
		printf("Warning: trying to free null pointer\n");
		return;
	}

	LOCK();

	do
	{
		BlockHeader *ptr = reinterpret_cast<BlockHeader *>(memory) - 1;
		if (BlockHeader::marker != ptr->sign)
		{
			fprintf(stderr, "Oops: free unknown memory!!\n");
			fflush(stderr);
			break;
		}

		if (mNbAllocatedBytes < ptr->size)
		{
			fprintf(stderr, "Oops: size = %u\n", ptr->size);
			fflush(stderr);
			break;
		}

		mNbAllocatedBytes -= ptr->size;
		--mNbAllocs;

#if defined(_DEBUG)

		// Remove the block from the Memory block list
		if (NULL != mMemBlockList)
		{
			if (ptr->indx >= mMemBlockListSize)
			{
				fprintf(stderr, "Oops: index = %u (>= %u)\n", ptr->indx, mMemBlockListSize);
				fflush(stderr);
				break;
			}

			mMemBlockList[ptr->indx] = NULL;
			--mMemBlockUsed;
		}
#endif

		ptr->sign = 0;
		ptr->size = 0;
		::free(ptr);
	} while (false);

	UNLOCK();
}


#ifndef PXC_HEAPMANAGERINTERFACE__H
#define PXC_HEAPMANAGERINTERFACE__H

#include "px_config.h"
#include "PxcHeapManagerAllocations.h"

enum PxcHeapManagerStatsFlags 
{
	PXCHS_SMALL_STATS		= 1 << 0,
	PXCHS_EXTENDED_STATS	= 1 << 1,
	PXCHS_ALL				= 0xFFFFFFFF,
};

class PxcHeapStats
{
public:
	PxcHeapStats(): 
		heapSize(0), 
		totalAllocated(0), 
		biggestFreeBlock(0), 
		internalFragmentation(0), 
		numEntries(0),
		maxAllocated(0),
		maxInternalFragmentation(0)
		{}

	// PXCHS_SMALL_STATS
	PxU32	heapSize;
	PxU32	totalAllocated;
	PxU32	maxAllocated;

	// PXCHS_EXTENDED_STATS
	PxU32	internalFragmentation;
	PxU32	maxInternalFragmentation;
	PxU32	biggestFreeBlock;

	PxU32	freeBuddyHistogram[32];
	PxU32	allocatedBuddyHistogram[32];
	PxU32	numEntries;

	// stats per allocation ID
	AllocIdStats allocIdStats[PxcAllocId::NUM_IDS];
};

class PxcHeapManagerInterface
{
public:
	// destructor
	virtual		~PxcHeapManagerInterface() {};

	// init the HeapManager by passing it a block of memory and the smallest size of a memory block.
	// returns true if init was successful
	virtual		bool		init(const void* startAddr, const PxU32 size, const PxU32 minBlockSize)		= 0;

	// returns the address of an allocated block of the givcn size.
	// returns a NULL ptr if alloc was not successful.
	virtual		void*		alloc(const PxU32 size, PXC_ALLOC_INFO_PARAMS_DECL(NULL, 0, NULL, UNASSIGNED))	= 0;

	// frees a given block.
	// returns true if the operation was successful
	virtual		bool		free(void* addr)															= 0;

	// returns stats into a PxcHeapStats object, stats can be seleced with PxcHeapManagerStatsFlags.
	// returns true if the operation was successful
	virtual		bool		getStats(PxcHeapStats& stats, const PxU32 flags)							= 0;

	// discretisize memory into an array such that it can be visualized
	// returns true if the operation was successful
	virtual		bool		visualizeMemory(PxU32* array, const PxU32 size)								= 0;
};


#endif

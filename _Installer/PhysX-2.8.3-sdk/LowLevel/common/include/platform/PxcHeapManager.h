#ifndef PXC_HEAPMANAGER__H
#define PXC_HEAPMANAGER__H

#define DUMP_HEAP_USAGE_TO_FILE 0

#include "PxcHeapManagerInterface.h"
#include "PxcSimpleMutex.h"

#if DUMP_HEAP_USAGE_TO_FILE
#include "stdio.h"
#endif

class PxcHeap;
class PxcBuddy;
class PxcBuddyList;

enum PxcBuddyFlags
{
	BF_LAST_BUDDY = 1 << 0  // if we use more than one buddy for an alloc to reduce internal fragmentation, we need a way to mark the last buddy.
};

class PxcHeapManager: public PxcHeapManagerInterface
{
public:

	PxcHeapManager();
	PxcHeapManager(PxU32 maxIntFrag);	
	virtual ~PxcHeapManager();

	// INTERFACE METHODS
	virtual		bool		init(const void* startAddr, const PxU32 size, const PxU32 minBlockSize);
	virtual		void*		alloc(const PxU32 size, PXC_ALLOC_INFO_PARAMS_DECL(NULL, 0, NULL, UNASSIGNED));
	virtual		bool		free(void* addr);
	virtual		bool		getStats(PxcHeapStats& stats, const PxU32 flags);
	virtual		bool		visualizeMemory(PxU32* array, const PxU32 size);

	// INTERNAL METHODS
private:

	// split buddy b with size 2^level into two buddies with level 2^(level-1) and append those to the free list. deletes b, assumes that b was removed from the list before.
	bool		splitBuddy(PxcBuddy* b);
	// merge 2 buddies to next bigger one. deletes b0 and b1, assumes that they are already removed from their array.
	bool		mergeBuddies(PxcBuddy* b0, PxcBuddy* b1);

	// split of right, free children of a buddy if the internal fragmentation of a buddy is bigger than a threshold
	// returns the size of all allocated buddies
	PxU32		reduceIntFragment(PxcBuddy& b, PxU32 threshold);

	// insertion sort into linked list
	bool		insertSorted(PxcBuddy& b, PxcBuddyList& list);

	// find PxcBuddy in array and remove it
	PxcBuddy*	findAndPop(const size_t addr, PxcBuddyList& array);
	PxcBuddy*	popBodyFromList(PxcBuddy& b, PxcBuddyList& list);
	PxcBuddy*	popHeadFromList(PxcBuddyList& list);

	// find a PxcBuddy by splitting a PxcBuddy at searchLevel
	PxcBuddy*	findBySplitting(PxU8 searchLevel);

	PxcBuddy*	findByMerging(PxU8 searchLevel);
	PxU32		findPairAndMerge(PxcBuddyList& list, PxU32 numToFind);

	// searches 2^pow >= val,  searches pow in [min, max]
	PxU32		findNextPow2(const PxU32 val, const PxU32 min = 0, const PxU32 max = 31);
	PxU32		findNextPow2(PxU8& pow, const PxU32 val, const PxU32 min = 0, const PxU32 max = 31);

	bool		sanityTest();

	// vars
private:
	PxcHeap*		mHeap;
	PxU32			mMaxIntFrag;
	PxcSimpleMutex	mMutex;

#if DUMP_HEAP_USAGE_TO_FILE
	FILE*				mLogFile;
	unsigned __int64    m_qpc;
	unsigned __int64    m_qpf;
#endif
};

class PxcBuddy
{
public:
	PxcBuddy(): addr(0), occupiedSize(0), flags(BF_LAST_BUDDY), level(0), next(0), allocId(PxcAllocId::UNASSIGNED) {}
	PxcBuddy(PxcBuddy& b): addr(b.addr), occupiedSize(b.occupiedSize), flags(b.flags), level(b.level), next(b.next), allocId(b.allocId) {}

	size_t			addr;
	PxcBuddy*		next;
	PxU32			occupiedSize;
	PxcAllocId::Enum	allocId;
	PxU8			flags;
	PxU8			level;
};

class PxcBuddyList
{
public:
	PxcBuddyList(): head(NULL), numElts(0), buddySize(0) {}

	void clear();

	PxcBuddy*		head;
	PxU32			numElts;
	PxU32			buddySize;  // = 2^level
};


class PxcHeap
{
public:
	PxcHeap(const size_t baseAddr, const PxU32 minBlockSize, const PxU8 minBlockLog2, const PxU8 maxLevel, const PxU32 size):
		baseAddr(baseAddr),
		minBlockSize(minBlockSize),
		minBlockLog2(minBlockLog2),
		maxLevel(maxLevel),
		memorySize(size),
		freeMem(size),
		allocMem(0),
		internalFragmentation(0),
		freeBuddiesAtLevel(NULL),
		maxAllocMem(0),
		maxInternalFragmentation(0)
		{	
		}

	~PxcHeap();
	bool init();

	const size_t		baseAddr;
	PxU32				minBlockSize;
	PxU8				minBlockLog2;
	PxU8				maxLevel;		// 2^maxLevel <= memorySize
	PxU32				memorySize;
	PxU32				freeMem;		// fragmented
	PxU32				allocMem;		// fragmented
	PxU32				maxAllocMem;
	PxU32				internalFragmentation;
	PxU32				maxInternalFragmentation;

	// stats per allocation ID
	AllocIdStats		allocIdStats[PxcAllocId::NUM_IDS];

	PxcBuddyList*		freeBuddiesAtLevel; 
	PxcBuddyList		allocatedBuddies;
};



#endif

#ifndef PXC_HEAPMANAGER_ALLOCATIONS__H
#define PXC_HEAPMANAGER_ALLOCATIONS__H

// some macros to keep the source code more readable
#define PXC_ALLOC_INFO(name, ID) __FILE__, __LINE__, name, PxcAllocId::ID
#define PXC_ALLOC_INFO_PARAMS_DECL(p0, p1, p2, p3)  const char* file = p0, PxU32 line = p1, const char* allocName = p2, PxcAllocId::Enum allocId = PxcAllocId::p3
#define PXC_ALLOC_INFO_PARAMS_DEF()  const char* file, PxU32 line, const char* allocName, PxcAllocId::Enum allocId
#define PXC_ALLOC_INFO_PARAMS_INPUT()  file, line, allocName, allocId

// ID of the Feature which owns/allocated memory from the heap
struct PxcAllocId
{
	enum Enum
	{
		UNASSIGNED,		// default
		FLUID,			// all fluid related
		DEFORMABLE,		// all deformable related (cloth & softbody)
		GPU_UTIL,		// e.g. RadixSort (used in SPH and deformable self collision)
		MESH_CACHE,		// fluid mesh cach allocates one big block from GPU heap
		NUM_IDS			// number of IDs, be aware that PxcHeapStats contains AllocIdStats[NUM_IDS]
	};
};

// class to track allocation stats, check PxgMirrored
class PxcAllocInfo
{
public:
	PxcAllocInfo() {}

	PxcAllocInfo(const char* file, int line, const char* allocName, PxcAllocId::Enum allocId)
	: mFileName(file)
	, mLine(line)
	, mAllocName(allocName)
	, mAllocId(allocId)
	{}

	inline	const char*			getFileName() const		{ return mFileName; }
	inline	int					getLine() const			{ return mLine; }
	inline	const char*			getAllocName() const	{ return mAllocName; }
	inline	PxcAllocId::Enum	getAllocId() const		{ return mAllocId; }

private:
	const char*			mFileName;
	int					mLine;
	const char*			mAllocName;
	PxcAllocId::Enum	mAllocId;
};

// stats collected per AllocationId by PxcHeapManager.
struct AllocIdStats
{
	PxU32 size;			// currently allocated memory by this ID
	PxU32 maxSize;		// max allocated memory by this ID
	PxU32 elements;		// number of current allocations by this ID
	PxU32 maxElements;	// max number of allocations by this ID
};

#endif

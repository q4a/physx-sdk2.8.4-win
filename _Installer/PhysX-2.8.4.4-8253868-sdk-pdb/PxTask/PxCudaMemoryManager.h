#ifndef __PX_CUDA_MEMORY_MANAGER_H
#define __PX_CUDA_MEMORY_MANAGER_H

/*
 * Copyright 2009-2010 NVIDIA Corporation.  All rights reserved.
 *
 * NOTICE TO USER:
 *
 * This source code is subject to NVIDIA ownership rights under U.S. and
 * international Copyright laws.  Users and possessors of this source code
 * are hereby granted a nonexclusive, royalty-free license to use this code
 * in individual and commercial software.
 *
 * NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE
 * CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR
 * IMPLIED WARRANTY OF ANY KIND.  NVIDIA DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOURCE CODE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.
 * IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOURCE CODE.
 *
 * U.S. Government End Users.   This source code is a "commercial item" as
 * that term is defined at  48 C.F.R. 2.101 (OCT 1995), consisting  of
 * "commercial computer  software"  and "commercial computer software
 * documentation" as such terms are  used in 48 C.F.R. 12.212 (SEPT 1995)
 * and is provided to the U.S. Government only as a commercial end item.
 * Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through
 * 227.7202-4 (JUNE 1995), all U.S. Government End Users acquire the
 * source code with only those rights set forth herein.
 *
 * Any use of this source code in individual and commercial software must
 * include, in the user documentation and internal comments to the code,
 * the above Disclaimer and U.S. Government End Users Notice.
 */
 
#include "PxSimpleTypes.h"

// some macros to keep the source code more readable
#define NV_ALLOC_INFO(name, ID) __FILE__, __LINE__, name, physx::pxtask::AllocId::ID
#define NV_ALLOC_INFO_PARAMS_DECL(p0, p1, p2, p3)  const char* file = p0, PxU32 line = p1, const char* allocName = p2, physx::pxtask::AllocId::Enum allocId = physx::pxtask::AllocId::p3
#define NV_ALLOC_INFO_PARAMS_DEF()  const char* file, PxU32 line, const char* allocName, physx::pxtask::AllocId::Enum allocId
#define NV_ALLOC_INFO_PARAMS_INPUT()  file, line, allocName, allocId

#ifndef NULL // don't want to include <string.h>
#define NULL 0
#endif

namespace physx
{
namespace pxtask
{
	using namespace pubfnd2;

// ID of the Feature which owns/allocated memory from the heap
// Maximum of 64k IDs allowed.
struct AllocId
{
	enum Enum
	{
		UNASSIGNED,		// default
		APEX,			// APEX stuff not further classified
		FLUID,			// all fluid related
		DEFORMABLE,		// all deformable related (cloth & softbody)
		GPU_UTIL,		// e.g. RadixSort (used in SPH and deformable self collision)
		NUM_IDS			// number of IDs, be aware that ApexHeapStats contains AllocIdStats[NUM_IDS]
	};
};

struct CudaBufferMemorySpace
{
	enum Enum
	{
		T_GPU,
		T_PINNED_HOST,
		T_WRITE_COMBINED,
		T_HOST,
		COUNT
	};
};

// class to track allocation stats, check PxgMirrored
class AllocInfo
{
public:
	AllocInfo() {}

	AllocInfo(const char* file, int line, const char* allocName, AllocId::Enum allocId)
	: mFileName(file)
	, mLine(line)
	, mAllocName(allocName)
	, mAllocId(allocId)
	{}

	inline	const char*			getFileName() const		{ return mFileName; }
	inline	int					getLine() const			{ return mLine; }
	inline	const char*			getAllocName() const	{ return mAllocName; }
	inline	AllocId::Enum		getAllocId() const		{ return mAllocId; }

private:
	const char*			mFileName;
	int					mLine;
	const char*			mAllocName;
	AllocId::Enum		mAllocId;
};

// stats collected per AllocationId by HeapManager.
struct AllocIdStats
{
	size_t size;		// currently allocated memory by this ID
	size_t maxSize;		// max allocated memory by this ID
	size_t elements;	// number of current allocations by this ID
	size_t maxElements;	// max number of allocations by this ID
};

class CudaMemoryManager;
typedef size_t CudaBufferPtr;

// Hint flag to tell how the buffer will be used
struct CudaBufferFlags
{
	enum Enum
	{
		F_READ			= (1 << 0),
		F_WRITE			= (1 << 1),
		F_READ_WRITE	= F_READ | F_WRITE
	};
};


// Memory statistics struct returned by CudaMemMgr::getStats()
struct CudaMemoryManagerStats
{
	
	size_t			heapSize;		// Size of all pages allocated for this memory type (allocated + free).
	size_t			totalAllocated; // Size occupied by the current allocations.
	size_t			maxAllocated;	// High water mark of allocations since the SDK was created.
	AllocIdStats	allocIdStats[AllocId::NUM_IDS]; // Stats for each allocation ID, see AllocIdStats
};


// Buffer type: made of hint flags and the memory space (Device Memory, Pinned Host Memory, ...)
struct CudaBufferType
{
	PX_INLINE CudaBufferType(const CudaBufferType& t)
		: memorySpace(t.memorySpace)
		, flags(t.flags)
	{}
	PX_INLINE CudaBufferType(CudaBufferMemorySpace::Enum _memSpace, CudaBufferFlags::Enum _flags)
		: memorySpace(_memSpace)
		, flags(_flags)
	{}

	CudaBufferMemorySpace::Enum		memorySpace;
	CudaBufferFlags::Enum			flags;
};


// Buffer which keeps informations about allocated piece of memory.
class NvCudaBuffer
{
public:
	// Retrieves the manager over which the buffer was allocated.
	virtual	CudaMemoryManager*			getCudaMemoryManager() const = 0;

	// Releases the buffer and the memory it used, returns true if successful.
	virtual	bool						free() = 0;

	// Realloc memory. Use to shrink or resize the allocated chunk of memory of this buffer.
	// Returns true if successful. Fails if the operation would change the address and need a memcopy.
	// In that case the user has to allocate, copy and free the memory with separate steps.
	virtual	bool						realloc(size_t size) = 0;

	// Returns the type of the allocated memory.
	virtual	const CudaBufferType&		getType() const = 0;

	// Returns the pointer to the allocated memory.
	virtual	CudaBufferPtr				getPtr() const = 0;

	// Returns the size of the allocated memory.
	virtual	size_t						getSize() const = 0;
};


// Allocator class for different kinds of CUDA related memory.
class CudaMemoryManager
{
public:
	// Allocate memory of given type and size. Returns a CudaBuffer if successful. Returns NULL if failed.
	virtual NvCudaBuffer*				alloc(const CudaBufferType& type, size_t size, NV_ALLOC_INFO_PARAMS_DECL(NULL, 0, NULL, UNASSIGNED)) = 0;

	// Basic heap allocator without NvCudaBuffer
	virtual CudaBufferPtr				alloc(CudaBufferMemorySpace::Enum memorySpace, size_t size, NV_ALLOC_INFO_PARAMS_DECL(NULL, 0, NULL, UNASSIGNED)) = 0;
	
	// Basic heap deallocator without NvCudaBuffer
	virtual bool						free(CudaBufferMemorySpace::Enum memorySpace, CudaBufferPtr addr) = 0;

	// Basic heap realloc without NvCudaBuffer
	virtual bool						realloc(CudaBufferMemorySpace::Enum memorySpace, CudaBufferPtr addr, size_t size) = 0;

	// Retrieve stats for the memory of given type. See CudaMemoryManagerStats.
	virtual void						getStats(const CudaBufferType& type, CudaMemoryManagerStats& outStats) = 0;

	// Ensure that a given amount of free memory is available. Triggers CUDA allocations in size of (2^n * pageSize) if necessary.
	// Returns false if page allocations failed.
	virtual bool						reserve(const CudaBufferType& type, size_t size) = 0;

	// Set the page size. The managed memory grows by blocks 2^n * pageSize. Page allocations trigger CUDA driver allocations,
	// so the page size should be reasonably big. Returns false if input size was invalid, i.e. not power of two.
	// Default is 2 MB.
	virtual bool						setPageSize(const CudaBufferType& type, size_t size) = 0;
	
	// Set the upper limit until which pages of a given memory type can be allocated.
	// Reducing the max when it is already hit does not shrink the memory until it is deallocated by releasing the buffers which own the memory.
	virtual bool						setMaxMemorySize(const CudaBufferType& type, size_t size) = 0;

	// Returns the base size. The base memory block stays persistently allocated over the SDKs life time.
	virtual size_t						getBaseSize(const CudaBufferType& type) = 0;

	// Returns the currently set page size. The memory grows and shrinks in blocks of size (2^n pageSize)
	virtual size_t						getPageSize(const CudaBufferType& type) = 0;

	// Returns the upper limit until which the manager is allowed to allocate additional pages from the CUDA driver.
	virtual size_t						getMaxMemorySize(const CudaBufferType& type) = 0;
};

} // end pxtask namespace
} // end physx namespace

#endif

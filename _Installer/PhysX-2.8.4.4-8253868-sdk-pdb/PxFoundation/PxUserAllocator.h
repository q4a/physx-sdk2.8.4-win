// This code contains NVIDIA Confidential Information and is disclosed 
// under the Mutual Non-Disclosure Agreement.
//
// Notice
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES 
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO 
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT, 
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable. 
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such 
// information or for any infringement of patents or other rights of third parties that may 
// result from its use. No license is granted by implication or otherwise under any patent 
// or patent rights of NVIDIA Corporation. Details are subject to change without notice. 
// This code supersedes and replaces all information previously supplied. 
// NVIDIA Corporation products are not authorized for use as critical 
// components in life support devices or systems without express written approval of 
// NVIDIA Corporation.
//
// Copyright © 2009 NVIDIA Corporation. All rights reserved.
// Copyright © 2002-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright © 2001-2006 NovodeX. All rights reserved.   

#ifndef PX_FOUNDATION_PXUSERALLOCATOR_H
#define PX_FOUNDATION_PXUSERALLOCATOR_H

/** \addtogroup foundation
@{
*/

#include "Px.h"

namespace physx
{
namespace pubfnd2
{

PX_PUSH_PACK_DEFAULT

/**
\brief Abstract base class for an application defined memory allocator that can be used by the Px library.

\note The SDK state should not be modified from within any allocation/free function.

<b>Threading:</b> All methods of this class should be thread safe as it can be called from the user thread 
or the physics processing thread(s).
*/

class PxUserAllocator
{
public:
	
	/**
	\brief destructor
	*/
	virtual ~PxUserAllocator() {}

	/**
	\brief Registers a name for subsequent allocations.

	<b>Threading:</b> This function should be thread safe as it can be called in the context of the user thread 
	and physics processing thread(s).

	\param name Name of allocation class (type name or specific use).
	\return Handle to use when allocating memory for this class.
	*/
	virtual PxU32 getHandle(const char*) { return 0; } 

	/**
	\brief Allocates size bytes of memory.

	This method hould never return NULL.  If you run out of memory, then
	you should terminate the app or take some other appropriate action.

	<b>Threading:</b> This function should be thread safe as it can be called in the context of the user thread 
	and physics processing thread(s).

	\param size Number of bytes to allocate.
	\param handle Name previously registered by getHandle().
	\param file The source file which allocated the memory
	\param line The source line which allocated the memory
	\return The allocated block of memory.
	*/
	virtual void* allocate(size_t size, PxU32 handle, const char* filename, int line) = 0;

	/**
	\brief Frees memory previously allocated by allocate().

	<b>Threading:</b> This function should be thread safe as it can be called in the context of the user thread 
	and physics processing thread(s).

	\param ptr Memory to free.
	*/
	virtual void deallocate(void* ptr) = 0;
};

PX_POP_PACK

} // namespace pubfnd2
} // end namespace physx

/** @} */
#endif

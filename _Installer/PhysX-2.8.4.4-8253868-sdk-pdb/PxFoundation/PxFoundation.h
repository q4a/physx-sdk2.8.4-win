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

#ifndef PX_FOUNDATION_PXFOUNDATION_H
#define PX_FOUNDATION_PXFOUNDATION_H

/** \addtogroup foundation
  @{
*/

#include "PxErrors.h"

namespace physx
{
namespace pubfnd2
{

PX_PUSH_PACK_DEFAULT

class PxUserOutputStream;
class PxUserAllocator;
class PxProfilingZone;

/**
\brief Foundation SDK singleton class.

You need to have an instance of this class to instance the higher level SDKs.
*/
class PxFoundation
{
public:
	/**
	retrieves error stream
	*/
	virtual PxUserOutputStream& getErrorStream() const = 0;

	/**
	Sets mask of errors to report to stream.
	*/
	virtual void setErrorLevel(PxErrorCode::Enum mask = PxErrorCode::Enum(~0)) = 0;

	/**
	Retrieves mask of errors reported to stream.
	*/
	virtual PxErrorCode::Enum getErrorLevel() const = 0;

	/**
	retrieves the current allocator.
	*/
	virtual PxUserAllocator& getAllocator() const = 0;

protected:
	virtual ~PxFoundation() {}
};

PX_POP_PACK

} // namespace pubfnd2
} // end namespace physx

/**
The constant PX_PUBLIC_FOUNDATION_VERSION is used when creating the PxFoundation object, 
which is an internally created object. This is to ensure that the application is using 
the same header version as the library was built with.
*/
#define PX_PUBLIC_FOUNDATION_VERSION ((PX_PUBLIC_FOUNDATION_VERSION_MAJOR   <<24)		\
									 +(PX_PUBLIC_FOUNDATION_VERSION_MINOR   <<16)		\
									 +(PX_PUBLIC_FOUNDATION_VERSION_BUGFIX  <<8) + 0)

 /** @} */
#endif

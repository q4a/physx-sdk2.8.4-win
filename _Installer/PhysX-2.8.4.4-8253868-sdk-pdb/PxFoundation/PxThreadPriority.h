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

#ifndef PX_FOUNDATION_PX_THREADPRIORITY_H
#define PX_FOUNDATION_PX_THREADPRIORITY_H
/** \addtogroup foundation
@{
*/

#include "Px.h"

namespace physx
{
namespace pubfnd2
{
	/**
	\brief Used to specify a thread priority.
	*/
	struct PxThreadPriority // todo: put in some other header file
	{
		enum Enum
		{
			/**
			\brief High priority
			*/
			eHIGH			= 0,

			/**
			\brief Above Normal priority
			*/
			eABOVE_NORMAL	= 1,

			/**
			\brief Normal/default priority
			*/
			eNORMAL			= 2,

			/**
			\brief Below Normal priority
			*/
			eBELOW_NORMAL	= 3,

			/**
			\brief Low priority.
			*/
			eLOW			= 4,

			eFORCE_DWORD	= 0xffFFffFF
		};
	};

} // namespace pubfnd2
} // end namespace physx

/** @} */
#endif

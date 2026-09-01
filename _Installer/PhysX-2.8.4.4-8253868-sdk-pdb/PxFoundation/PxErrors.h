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

#ifndef PX_FOUNDATION_PX_ERRORS_H
#define PX_FOUNDATION_PX_ERRORS_H
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
\brief Error codes

These error codes are passed to #PxUserOutputStream

@see PxUserOutputStream
*/

struct PxErrorCode
{
	enum Enum
	{
		eNO_ERROR				= 0,

		//! \brief An informational message.
		eDEBUG_INFO				= 1,

		//! \brief a warning message for the user to help with debugging
		eDEBUG_WARNING			= 2,

		//! \brief method called with invalid parameter(s)
		eINVALID_PARAMETER		= 4,

		//! \brief method was called at a time when an operation is not possible
		eINVALID_OPERATION		= 8,

		//! \brief method failed to allocate some memory
		eOUT_OF_MEMORY			= 16,

		/** \brief The library failed for some reason.
		Possibly you have passed invalid values like NaNs, which are not checked for.
		*/
		eINTERNAL_ERROR			= 32,

// PX_SERIALIZATION
		//! \brief A missing reference has been found when deserializing a collection
		eSERIALIZATION_ERROR	= 64,
//~PX_SERIALIZATION

		/**
		\brief an assertion failed.
		*/
		eASSERTION				= 128,

	};
};

PX_POP_PACK

} // namespace pubfnd2
} // end namespace physx

/** @} */
#endif

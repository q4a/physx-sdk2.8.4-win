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

#ifndef PX_FOUNDATION_PX_H
#define PX_FOUNDATION_PX_H

/*! \file top level include file for public foundation */

/** \addtogroup foundation
@{
*/

#include "PxVersionNumber.h"
#include "PxSimpleTypes.h"

/** files to always include */
#include <string.h>
#include <stdlib.h>

namespace physx
{
	namespace pubfnd2 
	{
		class PxVec3;
		class PxVec4;
		class PxMat33;
		class PxMat44;
		class PxQuat;
		class PxTransform;
		class PxBounds3;

		class PxFoundation;
		class PxStream;

		class PxRenderBuffer;
		struct PxDebugText;

		class PxProfilerData;
		class PxUserAllocator;
		class PxUserOutputStream;

		// PX_SERIALIZATION
		class PxRefResolver;
		class PxUserReferences;
		class PxCollection;
		class PxSerializable;
		//~PX_SERIALIZATION

		// todo: forward declare all classes
	}

} // namespace physx


/** @} */
#endif

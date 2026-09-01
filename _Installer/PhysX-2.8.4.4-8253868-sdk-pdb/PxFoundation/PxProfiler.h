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

#ifndef PX_FOUNDATION_PXPROFILER_H
#define PX_FOUNDATION_PXPROFILER_H

#include "Px.h"

/** \addtogroup foundation
  @{
*/

namespace physx
{
namespace pubfnd2
{

PX_PUSH_PACK_DEFAULT

//#define PX_ENABLE_PROFILER		//!< Switch indicating if profiler is active.  Customer requested that this define be exposed so that user code can be conditionally compiled.
//#define PX_ENABLE_PROFILER_COUNTER //!< Enable an additional performance counter(eg L2 cache misses)

/**
\brief A profiling zone. 

A profiling zone is a named piece of code whose performance characteristics have been measured.
*/
class PxProfileZone
	{
	public:
	const char * name;			//!< Name of the zone.
	PxU32  callCount;			//!< The number of times this zone was executed over the last profiling run (since readProfileData(true) was called.)
	PxU32  hierTime;			//!< Time in micro seconds that it took to execute the total of the calls to this zone.
	PxU32  selfTime;			//!< Time in micro seconds that it took to execute the total of the calls to this zone, minus the time it took to execute the zones called from this zone.  
	PxU32  recursionLevel;		//!< The number of parent zones this zone has, each of which called the next until this zone was called.  Can be used to indent a tree display of the zones.  Sometimes a zone could have multiple rec. levels as it was called from different places.  In this case the first encountered rec level is displayed.
	PxReal percent;				//!< The percentage time this zone took of its parent zone's time.  If this zone has multiple parents (the code was called from multiple places), this is zero. 

#ifdef PX_ENABLE_PROFILER_COUNTER
	PxU32 counter;
#endif
	};


/**
\brief Array of profiling data. 

 profileZones points to an array of numZones profile zones.  Zones are sorted such that the parent zones always come before their children.  
 Some zones have multiple parents (code called from multiple places) in which case only the relationship to the first parent is displayed.
 returned by PxScene::readProfileData().
*/
class PxProfilerData
	{
	public:
	PxU32 numZones;
	PxProfileZone * profileZones;
	};

PX_POP_PACK

} // namespace pubfnd2
} // end namespace physx

 /** @} */
#endif

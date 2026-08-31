#ifndef PXC_DISTANCELINELINE_H
#define PXC_DISTANCELINELINE_H

// Line and Segment distance quieries

#include "px_config.h"
#include "PxcVector.h"
#include "PxcSegment.h"

//Uses an old Wild Magic function, which suffers from some problems addressed 
//in the new version, see TTP 4617. However, some of the colliders seems to work 
//badly with the new version (e.g. Capsule/Mesh), so we keep the old one around 
//for a while.
PxFloat PxcDistanceSegmentSegmentSquaredOLD(const PxcVector& origin0,
											const PxcVector& extent0,
											const PxcVector& origin1,
											const PxcVector& extent1,
											PxFloat* param0=NULL, 
											PxFloat* param1=NULL);

//Uses the new Wild Magic function, but needs to do 2 square roots in order
//to find the normalized direction and length of the segments, and then
//a division in order to renormalize the output
PxFloat PxcDistanceSegmentSegmentSquared(	const PxcSegment& segment0,
											const PxcSegment& segment1,
											PxFloat* param0=NULL, 
											PxFloat* param1=NULL);


#endif

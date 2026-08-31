#ifndef PXC_SHAPEHELPERFUNCTION_H
#define PXC_SHAPEHELPERFUNCTION_H

// Some functions to compute shape related features

#include "px_config.h"
#include "PxcVector.h"
#include "PxcSegment.h"
#include "PxcShapeData.h"

/*!
Helper function to compute segment for capsule shapes in world space
*/
// TODO: This should move to a capsule shape specific class if we ever introduce that in the LL
PXD_FORCE_INLINE void PxcGetCapsuleWorldSegment(const PxcTransform& transform, const PxcShapeCapsule& shape, PxcSegment& segment)
{
	PXN_ASSERT(shape.mType == PXD_SHAPE_TYPE_CAPSULE);

	segment.point0 = transform.getBasisVector0() * shape.mHalfExtent;
	segment.point1 = -segment.point0;
	segment.point0 += transform.translation;
	segment.point1 += transform.translation;
}


#endif	// PXC_SHAPEHELPERFUNCTION_H

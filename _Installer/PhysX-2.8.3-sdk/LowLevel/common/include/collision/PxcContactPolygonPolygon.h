#ifndef PXC_CONTACTPOLYGONPOLYGON_H
#define PXC_CONTACTPOLYGONPOLYGON_H

#include "px_config.h"

#include "PxcContactMethod.h"

class PxcVector;
class PxcMatrix34;
class PxcPlane;

bool PxcContactPolygonPolygon(PxU32 numVerts0, const PxcVector* vertices0, const PxU8* indices0, 
							  const PxcMatrix34& worldTo0, const PxcPlane& localPlane0,
							  PxU32 numVerts1, const PxcVector* vertices1, const PxU8* indices1, 
							  const PxcMatrix34& worldTo1, const PxcPlane& localPlane1,
							  const PxcVector& worldSepAxis,
							  const PxcMatrix34& transform0to1, const PxcMatrix34& transform1to0,
							  PxU32 polyIndex0, PxU32 polyIndex1, PxcContactCallback& callback,
							  bool flipNormal = false);

#endif

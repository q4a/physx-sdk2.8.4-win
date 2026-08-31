#ifndef PXC_DISTANCEPOINTTRIANGLE_H
#define PXC_DISTANCEPOINTTRIANGLE_H

#include "px_config.h"
#include "PxcVector.h"

PxFloat PxcDistancePointTriangleSquared(const PxcVector& point, 
										const PxcVector& triangleOrigin, 
										const PxcVector& triangleEdge0, 
										const PxcVector& triangleEdge1,
										PxFloat* param0=NULL, 
										PxFloat* param1=NULL);


#endif

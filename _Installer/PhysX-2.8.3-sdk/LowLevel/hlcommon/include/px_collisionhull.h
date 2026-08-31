#ifndef PX_COLLISIONHULL_H
#define PX_COLLISIONHULL_H

#include "px_config.h"

struct PxConvexHullData;
struct PxValenciesData;

//Data
#pragma PXD_PUSH_PACK(1)

struct PxCollisionHullData
{
	PxConvexHullData*	mConvexHullData;
	PxValenciesData*	mValenciesData;
};

#pragma PXD_POP_PACK

#endif

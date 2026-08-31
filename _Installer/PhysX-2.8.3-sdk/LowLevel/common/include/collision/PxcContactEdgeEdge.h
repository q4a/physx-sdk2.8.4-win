#ifndef PXC_CONTACTEDGEEDGE_H
#define PXC_CONTACTEDGEEDGE_H

#include "px_config.h"
#include "PxcVector.h"

// collide edge (p1,p2) moving in direction (dir) colliding
// width edge (p3,p4). Return true on a collision with
// collision distance (dist) and intersection point (ip)
bool PxcContactEdgeEdge(const PxcVector& p1,
						const PxcVector& p2,
						const PxcVector& dir,
						const PxcVector& p3,
						const PxcVector& p4,
						PxFloat& dist,
						PxcVector& ip);

// TODO:
// Write the PxcSegment wrapper.

#endif

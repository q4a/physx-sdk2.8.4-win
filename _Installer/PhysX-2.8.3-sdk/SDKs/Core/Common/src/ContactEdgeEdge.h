#ifndef NX_PHYSICS_CONTACTEDGEEDGE
#define NX_PHYSICS_CONTACTEDGEEDGE
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/


#include "Physics.h"
#include "Opcode.h"

BOOL edge_collision(const NxVec3& p1, const NxVec3& p2, const NxVec3& dir, const NxVec3& p3, const NxVec3& p4, float& dist, NxVec3& ip);

#endif

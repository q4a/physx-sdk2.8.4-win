#ifndef PXS_FLUID_COLLISION_METHODS_H
#define PXS_FLUID_COLLISION_METHODS_H


#include "PxsFluidConfig.h"
#ifdef PXS_ENABLE_FLUIDS

#include "PxsFluidCollisionData.h"
#include "PxsShape.h"

#define ORIGINAL_FLUID_IMPL

#ifdef _XBOX
#undef ORIGINAL_FLUID_IMPL
#include "PxcSimd.h"
#endif

//#ifdef _PS3
//#undef ORIGINAL_CLOTH_IMPL
//#include "Cloth_PS3.h"
//#endif

/*!
Structure with all the needed components for mesh collision queries
*/
struct PxsMeshCollider
{
	IceCore::Container				meshCollTriBuffer;
	Opcode::OBBCache				cache;
	Opcode::HybridOBBCollider		collider;

public:
	PxsMeshCollider()
	{
		cache.TouchedPrimitives = &meshCollTriBuffer;
	}

	PXD_FORCE_INLINE void freeMemory()
	{
		meshCollTriBuffer.Empty();
	}
};


/*!
Collision routines for fluid particles
*/

void collideWithPlane(PxsFluidCollShapeData* collShapeData, PxU32 numCollShapeData, const PxsShape& planeShape,
					  PxFloat collRadius, PxFloat proxRadius);

void collideWithConvexPlanes(PxsFluidCollShapeData& csd, const PxU8* convexPlanes,
							 PxU32 convexPlaneByteStride, PxU32 numPlanes,
							 const PxFloat& collRadius, const PxFloat& proxRadius);

void collideWithConvex(PxsFluidCollShapeData* collShapeData, PxU32 numCollShapeData, const PxsShape& convexShape,
					   PxFloat collRadius, PxFloat proxRadius);

void collideWithBox(PxsFluidCollShapeData* collShapeData, PxU32 numCollShapeData, const PxsShape& boxShape,
					PxFloat collRadius, PxFloat proxRadius);

void collideWithCapsule(PxsFluidCollShapeData* collShapeData, PxU32 numCollShapeData, const PxsShape& capsuleShape,
						PxFloat collRadius, PxFloat proxRadius);

void collideWithSphere(PxsFluidCollShapeData* collShapeData, PxU32 numCollShapeData, const PxsShape& sphereShape,
					   PxFloat collRadius, PxFloat proxRadius);

void collideWithStaticMesh(PxsFluidCollShapeData* collShapeData, PxsMeshCollider& meshCollider,
						   const PxsFluidLocalCellHash& localCellHash, const PxsShape& meshShape,
						   const PxcTransform& world2Shape, PxFloat cellSize, PxFloat collisionRange,
						   PxFloat collRadius, PxFloat proxRadius);

void collideWithStaticHeightField(PxsFluidCollShapeData* collShapeData, PxU32 numCollShapeData,
								  const PxsShape& heightFieldShape, PxFloat collRadius, PxFloat proxRadius);

#endif	// PXS_ENABLE_FLUIDS

#endif

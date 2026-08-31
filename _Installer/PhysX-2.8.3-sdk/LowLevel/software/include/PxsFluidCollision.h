#ifndef PXS_FLUID_COLLISION_H
#define PXS_FLUID_COLLISION_H


#include "PxsFluidConfig.h"
#ifdef PXS_ENABLE_FLUIDS

#include "PxcTransform.h"
#include "PxcPlane.h"
#include "PxsFluidCollisionData.h"
#include "PxsFluidCollisionMethods.h"
#include "PxsFluidParticle.h"


class PxsFluid;
class PxsFluidShape;
class PxsFluidBodyContactManager;
class PxsBodyAtom;
class PxsShapeTransformHash;


struct PxsFluidCollisionParameters
{
	PxFloat		staticRestitution;
	PxFloat		staticFriction;
	PxFloat		staticAttraction;
	PxFloat		dynamicRestitution;
	PxFloat		dynamicFriction;
	PxFloat		dynamicAttraction;
	PxFloat		responseCoefficient;
	PxFloat		cellSize;
	PxU32		packetMultLog;
	PxU32		packetMult;
	PxFloat		packetSize;
	PxFloat		collisionRadius;
	PxFloat		proximityRadius;
	PxFloat		maxMotionDistance;
	PxFloat		collisionRange;
	PxFloat		timeStep;
	PxFloat		invTimeStep;
	PxcPlane	projectionPlane;
	PxU32		flags;
};


class PxsFluidCollision
{
public:

											PxsFluidCollision();
											~PxsFluidCollision();

						void				destroy();

						void				updateCollision(PxsFluidParticleArray& particles, PxsFluidConstraintPair* outputConstraints,
															const PxsFluidConstraintPair* inputConstraints, const PxcBitMap& deletedShapesMap,
															const PxsShapeTransformHash& world2ShapeHash, PxsFluidShape* const* fluidShapes,
															PxU32 numFluidShapes, PxFloat timeStep);


private:
						void				updateFluidShapeCollision(	PxsFluidShape* fluidshape, PxsFluidParticleArray& particles,
																		PxsMeshCollider& meshCollider,
																		PxsFluidConstraintPair* outputConstraints,
																		const PxsFluidConstraintPair* inputConstraints,
																		const PxcBitMap& deletedShapesMap,
																		const PxcArray<PxsWorld2ShapePair>& world2ShapeTransforms);

						void				updateFluidBodyContactPair(	PxsFluidBodyContactManager* contactManager,
																		PxsFluidParticle* particles, PxU32 numParticles,
																		PxsParticleCollData* particleCollData,
																		PxsMeshCollider& meshCollider,
																		PxsFluidLocalCellHash& localCellHash,
																		const PxsWorld2ShapePair& w2sPair);

						void				collideCellsWithStaticMesh(	PxsFluidCollShapeData* csd,
																		PxsMeshCollider& meshCollider,
																		const PxsFluidLocalCellHash& localCellHash,
																		const PxsShape& meshShape,
																		const PxcTransform& world2Shape,
																		PxFloat collRadius,
																		PxFloat proxRadius);

	PXD_FORCE_INLINE	void				initCollShapeData(	PxsFluidCollShapeData& collShapeData, 
																const PxsParticleCollData& collData,
																const PxcTransform& world2Shape,
																const PxcTransform& world2ShapeOld);

						void				updateCollData(	PxsParticleCollData& collData,
															const PxsFluidCollShapeData& collShapeData,
															const PxsBodyAtom* bodyAtom,
															const PxcTransform& shapeToWorld,
															PxdShape shapeId,
															bool isShapeStatic);

						void				updateMeshCollData(	PxsParticleCollData& collData,
																const PxsFluidCollShapeData& collShapeData,
																const PxcTransform& shapeToWorld,
																PxdShape shapeId,
																bool isShapeStatic);

						void				collisionResponse(	PxsParticleCollData& collData, bool collisionWithDynamic,
																bool clearCollData);

						PxsFluidConstraintInfo	applyConstraints(	const PxcVector& rayOrig, PxcVector& rayDir, const PxcVector& oldVelocity, 
																	PxdShape& shapeId, PxcVector& shapeNormal, 
																	const PxsFluidConstraint& constr0, const PxsFluidConstraint& constr1,
																	const PxsFluidConstraintInfo constr0Info, const PxsFluidConstraintInfo constr1Info,
																	const PxcBitMap& deletedShapesMap);

						void				reflectVelocity(PxcVector& outVel, const PxcVector& inVel, 
															const PxcVector& oldVel, const PxcVector& surfaceNormal, 
															const PxcVector& surfaceVel, PxFloat friction, PxFloat restitution);

	PXD_FORCE_INLINE	void				setFrictionAndRestitution(PxFloat& friction, PxFloat& restitution, PxU8 constraintInfo);

	PXD_FORCE_INLINE	void				addDiscreteCollision(	PxsParticleCollData& collData,
																	const PxcVector& newSurfaceNormal,
																	const PxcVector& newSurfacePos,
																	const PxcVector& newSurfaceVel,
																	const PxUInt& dcNum);

	PXD_FORCE_INLINE	void				addContinuousCollision(	PxsParticleCollData& collData, PxFloat newTime,
																	const PxcVector& newSurfaceNormal,
																	const PxcVector& newSurfacePos,
																	const PxcVector& newSurfaceVel);

	PXD_FORCE_INLINE	void				addConstraint(	PxsParticleCollData& collData,
															const PxcVector& newSurfaceNormal,
															const PxcVector& newSurfacePos,
															const PxcVector& newSurfaceVel,
															const PxdShape shapeId,
															bool isShapeStatic);

	PXD_FORCE_INLINE	void				clampToMaxMotion(PxsParticleCollData& collData);
	PXD_FORCE_INLINE	void				updateParticle(	PxsFluidParticle& particle,
															const PxsParticleCollData& collData);

	PXD_FORCE_INLINE	void				computeLocalCellHash(PxsFluidLocalCellHash& localCellHash,
																PxsFluidParticle* particles, PxU32 numParticles);

public:
						PxsFluidCollisionParameters	mParams;

private:
						PxsMeshCollider				mMeshColliders[PXS_FLUID_NUM_PACKETS_PARALLEL_COLLISION];
						PxcArray<PxsWorld2ShapePair> mWorld2ShapeTransforms[PXS_FLUID_NUM_PACKETS_PARALLEL_COLLISION];
};

#ifdef _XBOX
#include "xbox360/PxsFluidCollision_XBOX.h" 
#endif

PXD_FORCE_INLINE void PxsFluidCollision::initCollShapeData(PxsFluidCollShapeData& collShapeData,
														   const PxsParticleCollData& collData,
														   const PxcTransform& worldToShape,
														   const PxcTransform& worldToShapeOld)
{
	collShapeData.ccTime = collData.ccTime;

	// Set the continuous collision flag if a continuous collision occured already for the particle.
	// If this flag is set, we do not add discrete or proximity collisions for the current collision shape
	// (or any shape that might follow).
	collShapeData.localFlags = (collData.flags & PXS_FLUID_COLL_FLAG_CC);

	// Transform position from world to shape space
	collShapeData.localNewPos = worldToShape.transform(collData.newPos);
	collShapeData.localOldPos = worldToShapeOld.transform(collData.oldPos);

	collShapeData.numConstraints = 0;
}

PXD_FORCE_INLINE void PxsFluidCollision::setFrictionAndRestitution(PxFloat& friction, PxFloat& restitution,
																   PxU8 constraintInfo)
{
	if (constraintInfo == static_cast<PxU8>(PXS_FLUID_CONSTRAINT_SHAPE_STATIC))
	{
		friction = mParams.staticFriction;
		restitution = mParams.staticRestitution;
	}
	else
	{
		PXN_ASSERT(constraintInfo == static_cast<PxU8>(PXS_FLUID_CONSTRAINT_SHAPE_DYNAMIC));
		friction = mParams.dynamicFriction;
		restitution = mParams.dynamicRestitution;
	}
}

PXD_FORCE_INLINE void PxsFluidCollision::addDiscreteCollision(PxsParticleCollData& collData,
															  const PxcVector& newSurfaceNormal,
															  const PxcVector& newSurfacePos,
															  const PxcVector& newSurfaceVel,
															  const PxUInt& dcNum)
{
	PXN_ASSERT(!(collData.flags & PXS_FLUID_COLL_FLAG_CC));

	collData.flags |= PXS_FLUID_COLL_FLAG_DC;

	// Discrete collisions will be averaged
	collData.surfaceNormal += newSurfaceNormal;
	collData.surfacePos += newSurfacePos;
	collData.surfaceVel += newSurfaceVel;
	collData.dcNum += dcNum;	// The passed surface normal/position/velocity can itself consist of
								// summed up normals/positions/velocities (for meshes for instance).
}

PXD_FORCE_INLINE void PxsFluidCollision::addContinuousCollision(PxsParticleCollData& collData,
																PxFloat newTime,
																const PxcVector& newSurfaceNormal,
																const PxcVector& newSurfacePos,
																const PxcVector& newSurfaceVel)		
{
	if (newTime >= collData.ccTime)
		return;		// We want the collision that happened first.
	
	collData.ccTime = newTime;

	collData.flags &= ~PXS_FLUID_COLL_FLAG_DC;	// Continuous collisions take precedence over discrete collisions
	collData.flags |= PXS_FLUID_COLL_FLAG_CC;

	collData.surfaceNormal = newSurfaceNormal;
	collData.surfacePos = newSurfacePos;
	collData.surfaceVel = newSurfaceVel;
}

PXD_FORCE_INLINE void PxsFluidCollision::addConstraint(PxsParticleCollData& collData,
													   const PxcVector& newSurfaceNormal,
													   const PxcVector& newSurfacePos,
													   const PxcVector& newSurfaceVel,
													   const PxdShape shapeId,
													   bool isShapeStatic)
{
	PxU16 constraintInfo = isShapeStatic ? PXS_FLUID_CONSTRAINT_SHAPE_STATIC : PXS_FLUID_CONSTRAINT_SHAPE_DYNAMIC;
	PxcVector n = newSurfaceNormal;
	n = n.getNormalized();	//!!! Isn't the normal always normalized?

	PxsFluidConstraint cN(n, newSurfacePos, newSurfaceVel, shapeId);

	if (collData.c0.isZero())
	{
		collData.c0 = cN;
		collData.c0Info = constraintInfo;
	}
	else if (collData.c1.isZero())
	{
		collData.c1 = cN;
		collData.c1Info = constraintInfo;
	}
	else
	{
		// Important: If the criterion to select the overwrite constraint changes, the fluid vs. static
		//            mesh code needs to be adjusted accordingly.

		// Overwrite constraint with the largest distance {old position} <--> {shape surface}.
		// The old position must be used since the new position is corrected after each collision occurence.
		PxFloat dist0 = collData.c0.normal.dot(collData.oldPos) - collData.c0.d;
		PxFloat dist1 = collData.c1.normal.dot(collData.oldPos) - collData.c1.d;
		PxFloat distN = cN.normal.dot(collData.oldPos) - cN.d;
		
		if (distN < dist0 && distN < dist1)
		{
			if (dist0 < dist1)
			{
				collData.c1 = cN;
				collData.c1Info = constraintInfo;
			}
			else
			{
				collData.c0 = cN;
				collData.c0Info = constraintInfo;
			}
		}
		else if (distN < dist0)
		{
			collData.c0 = cN;
			collData.c0Info = constraintInfo;
		}
		else if (distN < dist1)
		{
			collData.c1 = cN;
			collData.c1Info = constraintInfo;
		}
	}
}

#ifndef _XBOX
PXD_FORCE_INLINE void PxsFluidCollision::clampToMaxMotion(PxsParticleCollData& collData)
{
	PxcVector motionVec = collData.newPos - collData.oldPos;
	PxFloat motionDistance = motionVec.magnitude();
	if (motionDistance > mParams.maxMotionDistance)
	{
		collData.newPos = collData.oldPos + (motionVec * (mParams.maxMotionDistance / motionDistance));
	}
}
#endif

PXD_FORCE_INLINE void PxsFluidCollision::updateParticle(PxsFluidParticle& particle, const PxsParticleCollData& collData)
{
	if (!(mParams.flags & PXD_FF_PROJECT_TO_PLANE))
	{
		particle.velocity = collData.velocity; 
		particle.position = collData.newPos;
		particle.forceOrNormal = collData.shapeImpulseOrNormal;
	}
	else
	{
		particle.velocity = mParams.projectionPlane.projectVector(collData.velocity);
		particle.position = mParams.projectionPlane.projectPoint(collData.newPos);

		// Don't project the collision normal or shape impulse
		//particle.forceOrNormal = mParams.projectionPlane.projectVector(collData.shapeImpulseOrNormal);

		/*if (!(mParams.flags & PXD_FF_COLLISION_TWOWAY))
			particle.forceOrNormal.normalize();*/
		particle.forceOrNormal = collData.shapeImpulseOrNormal;
	}

	particle.shapeId = collData.shapeId;
}

PXD_FORCE_INLINE void PxsFluidCollision::computeLocalCellHash(PxsFluidLocalCellHash& localCellHash,
															  PxsFluidParticle* particles, PxU32 numParticles)
{
	PXN_ASSERT(numParticles <= PXS_FLUID_SUBPACKET_PARTICLE_LIMIT_COLLISION);
	
	PxU32 numHashEntries = PxcNextPower2(numParticles << 1);
	numHashEntries = PxcMin((PxU32)PXS_FLUID_LOCAL_HASH_SIZE_MESH_COLLISION, numHashEntries);

	// Make sure the number of hash entries is a power of 2 (requirement for the used hash function)
	PXN_ASSERT((((numHashEntries-1)^numHashEntries)+1) == (2*numHashEntries));
	PXN_ASSERT(numHashEntries > numParticles);

	// Get local cell hash for the current subpacket
	PxsFluidSpatialHash::buildLocalHash(particles, numParticles, localCellHash.hashEntries,
										localCellHash.particleIndices, numHashEntries, false);

	localCellHash.numHashEntries = numHashEntries;
	localCellHash.numParticles = numParticles;
	localCellHash.isHashValid = true;
}


#endif	// PXS_ENABLE_FLUIDS

#endif

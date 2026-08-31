/* NVIDIA/SCE CONFIDENTIAL
   NVIDIA PhysX(TM) SDK for PlayStation(R)3 Version PS3_PhysX_283_B2_forSDK330_hlsrc
*/
// PS3 specific collision definitions
#ifndef CLOTHCOLLISION_PS3_H
#define CLOTHCOLLISION_PS3_H

#include "CellUtil.h"

#if __SPU__
#include "gen/NxShapeType.gen.h"
#include "NxBounds3.spu.h"

#include "gen/NxU32.gen.h"
#include "gen/NxMat34.gen.h"
#endif

struct CellClothShape;

struct CellClothCollisionInfo 
{
	void init(const NxVec3 &p0, const NxVec3 &p1, NxReal r) {
		pos0 = p0;
		pos1 = p1;
		radius = r;
		proximityDist = r;
		t = -1.0f;
		shape = NULL;
		shapeFlags = 0;
		surfacePoint.zero();
	}
	NxVec3 lerp(NxReal s) const { return pos0*(1.0f-s) + pos1*s; }

	void addDiscreteCollision(const NxVec3 &s, const NxVec3 &n, const CellClothShape *newShape);
	bool addContinuousCollision(NxReal newT, const NxVec3 &n, const CellClothShape *newShape);
	bool addProximity(NxReal dist, const NxVec3 &s, const NxVec3 &n, const CellClothShape *newShape);
	bool addFluidContinuousCollision(NxReal newT, const NxVec3 &n, const NxVec3 &targetPosition);

	NxVec3 pos0;			// original particle position
	NxVec3 pos1;			// predicted new particle position
	NxReal radius;			// radius of the particle
	NxReal proximityDist;	// distance within which we register proximities
	bool   prevCollision;
	NxVec3 surfacePoint;	// closest surface point (discrete) or entry point (continuous)
	NxVec3 normal;			// normal at the surfacePoint
	NxReal t;				// no collision: -1, 
							// proximity: -1 < t < 0
							// continuous: 0 <= t < 1,	
							// discrete: t = 1,2,3...
	const CellClothShape *shape;		// the shape we collided with
	NxU32 shapeFlags;
};

class ClothCollision_PS3_impl //: public ClothCollision_impl
{
	static bool smallerQuadraticSolution(NxReal a, NxReal b, NxReal c, NxReal tMin, NxReal tMax, NxReal &t);

	static bool combinedSpherePlane(const CellClothShape *planeShape, CellClothCollisionInfo &info);
	static bool combinedSphereBox(const CellClothShape *boxShape, CellClothCollisionInfo &info);
	static bool combinedSphereSphere(const CellClothShape *sphereShape, CellClothCollisionInfo &info);
	static bool combinedSphereCapsule(const CellClothShape *capsuleShape, CellClothCollisionInfo &info);
public:
	static bool combinedContactSphereShape(const CellClothShape* shape, CellClothCollisionInfo &info, bool disableCCD = false);
};

#endif




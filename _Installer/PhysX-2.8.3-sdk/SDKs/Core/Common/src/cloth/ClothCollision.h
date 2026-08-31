#ifndef CLOTH_COLLISION_H
#define CLOTH_COLLISION_H

#include "Physics.h"
#include "NxPlane.h"
#include "Actor.h"
#include "PhysicsSDK.h"
#include "NxSimd.h"


class Shape;
class PlaneShape;
class BoxShape;
class SphereShape;
class CapsuleShape;
class ConvexShape;
class TriangleMeshShape;
class HeightFieldShape;
class Body;

enum PCParticleFlags
{
	PC_PARTICLE_NO_MORE_TUNNELING			=	1 << 0,
	PC_PARTICLE_DYNAMIC_COLLISION			=	1 << 1,
	PC_PARTICLE_TUNNELED					=	1 << 2,
	PC_PARTICLE_STATIC_DISCRETE_COLLISION	=	1 << 3,
	PC_PARTICLE_STATIC_CCD_COLLISION		=	1 << 4,
	PC_PARTICLE_WAS_DAMPED					=	1 << 5
};

// mmf: we need something like scene->overlapAABB_CCD_Shapes(..)
// for now we simply fatten the AABB with this magic constant
#ifndef CLOTH_MAX_BODY_DISPLACEMENT_PER_TIME_STEP
	#define CLOTH_MAX_BODY_DISPLACEMENT_PER_TIME_STEP 0.1
#endif

// we set the collision plane slightly above the collision so that
// CCD gets triggered next time
#define CLOTH_COLLISION_SAFETY 0.0001f						

// ------------------------------------------------------------------------------

struct ClothCollisionInfo 
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

	void addDiscreteCollision(const NxVec3 &s, const NxVec3 &n, const Shape *newShape);
	bool addContinuousCollision(NxReal newT, const NxVec3 &n, const Shape *newShape);
	bool addProximity(NxReal dist, const NxVec3 &s, const NxVec3 &n, const Shape *newShape);
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
	const Shape *shape;		// the shape we collided with
	NxU32 shapeFlags;
};

// ------------------------------------------------------------------------------

struct ClothMovingPoint {
	ClothMovingPoint() {};
	ClothMovingPoint(const NxVec3 &p0, const NxVec3 &p1) : pos0(p0), pos1(p1) {}
	void getBounds(NxBounds3 &bounds) const {
		bounds.setEmpty();
		bounds.include(pos0); bounds.include(pos1); 
	}
	NxVec3 lerp(NxReal t) const { return pos0*(1.0f-t) + pos1*t; }
	void clampStart(NxReal maxDist) {
		if (pos0.distanceSquared(pos1) < maxDist*maxDist) return;
		NxVec3 d = pos1 - pos0; d.normalize(); pos0 = pos1 - d*maxDist;
	}
	void clampEnd(NxReal maxDist) {
		if (pos0.distanceSquared(pos1) < maxDist*maxDist) return;
		NxVec3 d = pos1 - pos0; d.normalize(); pos1 = pos0 + d*maxDist;
	}
	NxVec3 pos0;
	NxVec3 pos1;
};

// ------------------------------------------------------------------------------

struct ClothMovingTriangle {
	ClothMovingTriangle() {};
	ClothMovingTriangle(const ClothMovingPoint &_a, const ClothMovingPoint &_b, const ClothMovingPoint &_c) :
		a(_a), b(_b), c(_c) {}
	void getBounds(NxBounds3 &bounds) const {
		bounds.setEmpty();
		bounds.include(a.pos0); bounds.include(b.pos0); bounds.include(c.pos0);
		bounds.include(a.pos1); bounds.include(b.pos1); bounds.include(c.pos1);
	}
	ClothMovingPoint a,b,c;
};

// ------------------------------------------------------------------------------

class ClothCollision_impl
{
public:
	static bool discreteContactPointShape(const Shape *shape, const NxVec3 &p, NxReal radius, NxVec3 &surfacePoint, NxVec3 &normal);
	static bool continuousContactTriangleShape(const Shape *shape, const ClothMovingTriangle &t, NxReal radius, NxPlane &plane);
	static bool collisionMovingPointMovingTriangle(const ClothMovingPoint &p, const ClothMovingTriangle &triangle, NxReal &collisionTime);
	static bool collisionMovingSphereMovingTriangle(const ClothMovingPoint &p, NxReal radius, const ClothMovingTriangle &triangle, NxReal &collisionTime);
	static bool collisionMovingSphereMovingEdge(const ClothMovingPoint &p, NxReal radius, const ClothMovingPoint &a, const ClothMovingPoint &b, NxReal &collisionTime);
	static bool collisionMovingSphereMovingPoint(const ClothMovingPoint &p, NxReal radius, const ClothMovingPoint &a, NxReal &collisionTime);
	static bool raycastTriangle(const NxRay& worldRay, const NxVec3 &p0, const NxVec3 &p1, const NxVec3 &p2, NxVec3 &hit, NxReal &t);
	static bool raycastBounds(const NxRay& worldRay, const NxBounds3 &bounds, NxVec3& hit, NxReal& t);
	static void barycentricTriangleCoordinates(const NxVec3 &pa, const NxVec3 &pb, const NxVec3 &pc, const NxVec3 &p, NxVec3 &baryCoords);
	static bool raySphere(const NxRay& ray, const NxVec3 center, NxReal rSquared, NxReal &t);
	static bool combinedContactSphereShape(const Shape *shape, ClothCollisionInfo &info, bool disableCCD = false);
#if !defined(__PPCGEKKO__)  //compiler bug
protected:
#endif
	static bool closestPointInTriangle(const NxVec3 &pa, const NxVec3 &pb, const NxVec3 &pc, const NxVec3 &p);
	static void closestPointOnTriangle(const NxVec3 &pa, const NxVec3 &pb, const NxVec3 &pc, const NxVec3 &p, NxVec3 &closest);
	static bool discretePointPlane(const PlaneShape *planeShape, const NxVec3 &p, NxReal radius, NxVec3 &surfacePoint, NxVec3 &normal);
	static bool discretePointBox(const BoxShape *boxShape, const NxVec3 &p, NxReal radius, NxVec3 &surfacePoint, NxVec3 &normal);
	static bool discretePointSphere(const SphereShape *sphereShape, const NxVec3 &p, NxReal radius, NxVec3 &surfacePoint, NxVec3 &normal);
	static bool discretePointCapsule(const CapsuleShape *capsuleShape, const NxVec3 &p, NxReal radius, NxVec3 &surfacePoint, NxVec3 &normal);
	static bool discretePointConvex(const ConvexShape *convexShape, const NxVec3 &p, NxReal radius, NxVec3 &surfacePoint, NxVec3 &normal);
	static bool discretePointTriangleMesh(const TriangleMeshShape *meshShape, const NxVec3 &p, NxReal radius, NxVec3 &surfacePoint, NxVec3 &normal);
	static bool combinedSphereCapsule(const CapsuleShape *capsuleShape, ClothCollisionInfo &info);	
	static bool combinedSphereHeightField(const HeightFieldShape *heightfieldShape, ClothCollisionInfo &info);
    static bool combinedSpherePlane(const PlaneShape *planeShape, ClothCollisionInfo &info);
	static bool combinedSphereSphere(const SphereShape *sphereShape, ClothCollisionInfo &info);
	static bool combinedSphereConvex(const ConvexShape *convexShape, ClothCollisionInfo &info);
	static bool combinedSphereTriangleMesh(const TriangleMeshShape *meshShape, ClothCollisionInfo &info);
	static bool combinedSphereBox(const BoxShape *boxShape, ClothCollisionInfo &info);
	static bool collisionMovingSphereStaticEdge(const ClothMovingPoint &p, NxReal radius, const NxVec3 &a, const NxVec3 &b, NxReal &t, NxVec3 &hit);
	static bool collisionMovingSphereStaticPoint(const ClothMovingPoint &p, NxReal radius, const NxVec3 &a, NxReal &t);
	static bool smallerQuadraticSolution(NxReal a, NxReal b, NxReal c, NxReal tMin, NxReal tMax, NxReal &t);
	static bool getLocalShapeVertices(const Shape *shape, const NxBounds3 &localBounds, NxFoundation::NxArraySDK<NxVec3> &vertices, NxFoundation::NxArraySDK<NxVec3> &normals);
};

// below select the right ClothCollision type to use
#ifdef _XBOX

#include "xbox360/ClothCollision_XBOX.h"
typedef ClothCollision_XBOX_impl ClothCollision;

#else

typedef ClothCollision_impl ClothCollision;

#endif



#endif

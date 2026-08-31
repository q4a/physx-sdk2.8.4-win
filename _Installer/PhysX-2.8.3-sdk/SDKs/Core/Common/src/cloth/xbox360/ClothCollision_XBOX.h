#ifndef CLOTH_COLLISION_XBOX_H
#define CLOTH_COLLISION_XBOX_H

#ifndef CLOTH_COLLISION_H
#error you must include ClothCollision.h instead
#endif

struct ClothCollisionInfoSIMD 
{
	void init(const NxVec3 &p0, const NxVec3 &p1, NxReal r) {
		pos0 = NxSimd::load(p0);
		pos1 = NxSimd::load(p1);
		radius = r;		
		proximityDist = r;
		t = -1.0f;	
		shape = NULL;
		shapeFlags = 0;
		surfacePoint = NxSimd::zero();			
	}
	NxSimd::Vector4 lerpV(NxSimd::Vector4 s) const { return NxSimd::lerpV(pos0,pos1,s); }
	NxSimd::Vector4 lerp(NxReal s) const { return NxSimd::lerp(pos0,pos1,s); }


	void addDiscreteCollision(const NxSimd::Vector4 &s, const NxSimd::Vector4 &n, const Shape *newShape);	

	bool addContinuousCollision(NxReal newT, const NxSimd::Vector4 &n, const Shape *newShape);	

	bool addProximity(NxReal dist, const NxSimd::Vector4 &s, const NxSimd::Vector4 &n, const Shape *newShape);

	bool addFluidContinuousCollision(NxReal newT, const NxSimd::Vector4 &n, const NxSimd::Vector4 &targetPosition);

	NxSimd::Vector4 pos0;			// original particle position
	NxSimd::Vector4 pos1;			// predicted new particle position
	NxReal radius;			// radius of the particle
	NxReal proximityDist;	// distance within which we register proximities
	bool   prevCollision;
	NxSimd::Vector4 surfacePoint;	// closest surface point (discrete) or entry point (continuous)
	NxSimd::Vector4 normal;			// normal at the surfacePoint
	NxReal t;				// no collision: -1, 
							// proximity: -1 < t < 0
							// continuous: 0 <= t < 1,	
							// discrete: t = 1,2,3...

	const Shape *shape;		// the shape we collided with
	NxU32 shapeFlags;
	
};

class Cloth_Collision_Shape
{
public:
	Shape *				shape;
	NxShapeType			type;
	NxSimd::Matrix34	globalPose;
	NxSimd::Matrix34	oldGlobalPose;
	NxSimd::Vector4		boundsMin;
	NxSimd::Vector4		boundsMax;
};

// ------------------------------------------------------------------------------

class ClothCollision_XBOX_impl: public ClothCollision_impl {
	// re implement the functions here, the same as overloaded functions
public: 
	static bool closestPointInTriangle(const NxSimd::Vector4 &pa, const NxSimd::Vector4 &pb, const NxSimd::Vector4 &pc, const NxSimd::Vector4 &p);
	static void closestPointOnTriangle(const NxSimd::Vector4 &pa, const NxSimd::Vector4 &pb, const NxSimd::Vector4 &pc, const NxSimd::Vector4 &p, NxSimd::Vector4 &closest);
	static bool combinedContactSphereShape(const Cloth_Collision_Shape *cShape, ClothCollisionInfoSIMD &info);

protected:
	static bool collisionMovingSphereStaticEdge(const NxSimd::Vector4 &pos0, const NxSimd::Vector4 &pos1, NxReal radius, const NxSimd::Vector4 &a, const NxSimd::Vector4 &b, NxReal &t, NxSimd::Vector4 &hit);
	static bool collisionMovingSphereStaticPoint(const NxSimd::Vector4 &pos0, const NxSimd::Vector4 &pos1, NxReal radius, const NxSimd::Vector4 &a, NxReal &t);
	static bool collisionMovingSphereStaticTriangle(const NxSimd::Vector4 &verts0, const NxSimd::Vector4 &verts1, const NxSimd::Vector4 &verts2, const TriangleMeshShape *meshShape, ClothCollisionInfoSIMD& info);

	static bool combinedSpherePlane(const PlaneShape *planeShape, ClothCollisionInfoSIMD &info);
	static bool combinedSphereSphere(const SphereShape *sphereShape, ClothCollisionInfoSIMD &info);
    static bool combinedSphereBox(const BoxShape *boxShape, ClothCollisionInfoSIMD &info);
    static bool combinedSphereCapsule(const CapsuleShape *capsuleShape, ClothCollisionInfoSIMD &info);
	static bool combinedSphereTriangleMesh(const TriangleMeshShape *meshShape, ClothCollisionInfoSIMD &info);
    static bool combinedSphereConvex(const ConvexShape *convexShape, ClothCollisionInfoSIMD &info);
	static bool combinedSphereHeightField(const HeightFieldShape *heightfieldShape, ClothCollisionInfoSIMD &info);
};

#endif // CLOTH_COLLISION_XBOX_H

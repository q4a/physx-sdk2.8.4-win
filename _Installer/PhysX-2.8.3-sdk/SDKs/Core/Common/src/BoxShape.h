#ifndef NX_COLLISION_BOXSHAPE
#define NX_COLLISION_BOXSHAPE
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Shape.h"
#include "NxBox.h"
#include "ConvexMeshInterface.h"
#define MESHMERIZER_API 
struct SurfaceInterface;
#include "ConvexHull.h"

/**
A box shaped collision detection primitive.
*/
//namespace NxCollision {
class ShapeInstancePair;
class NPhaseContext;

class BoxShape : public Shape, public ConvexMeshInterface
	{
	protected:
											BoxShape(const NvShapeDesc & desc, Scene *, CoreActor& actor, NxU32 uid);
	virtual									~BoxShape();

	public:

	//Convex interface
	virtual		const Point&				GetCenter()				const;
	virtual		udword						GetNbVerts()			const;
	virtual		const Point*				GetVerts()				const;
	virtual		udword						GetNbPolygons()			const;
	virtual		const PxHullPolygonData&	GetPolygon(udword i)	const;
	virtual		const PxHullPolygonData*	GetPolygons()			const;
	virtual		udword						GetNbEdges()			const;
//#ifdef SUPPORT_CONVEX_EDGE_DATA
	virtual		const Vertices*				GetEdgeDirections()		const;
	virtual		const NxPhysics::Edge8*		GetEdges8()				const;
	virtual		const PxEdgeDescData*		GetEdgeToTriangles()	const;
	virtual		const uword*				GetFacesByEdges16()		const;
//#endif
	virtual		udword						SelectClosestPolygon(const Point& world_witness, const Matrix4x4* world=null)					const;
	virtual		udword						SelectClosestEdge(const Point& world_witness, const Matrix4x4* world=null, udword* code=null)	const;

	virtual		void						ProjectHull(NPhaseContext& context, float& min, float& max, const Point& dir, const Matrix4x4& world4x4/*, const SupportVertexMap* supportVertex*/) const;

	virtual		void						GetLocalBoundingSphere(NxSphere& sphere)	const;
	//~Convex interface

				const Point*				GetEdgeNormals()		const;

	// Shape interface
	virtual		Shape&						getShape()			{ return *this;	}
	virtual		const Shape&				getShape()	const	{ return *this;	}
	virtual		operator					Shape&()			{ return *this;	}
	virtual		void						visualize(NxFoundation::DebugRenderable& renderer)																const;
	virtual		bool						raycast(const NxRay& worldRay, NxReal maxDist, NxU32 hintFlags, NxRaycastHit& hit, bool firstHit)	const;
	virtual		bool						checkOverlapSphere(const NxSphere& worldSphere)														const;
	virtual		bool						checkOverlapOBB(const NxBox& worldBox)																const;
	virtual		bool						checkOverlapAABB(const NxBounds3& worldBounds)														const;
	virtual		bool						checkOverlapCapsule(const NxCapsule& worldCapsule)													const;

#ifdef SUPPORT_INTERNAL_RADIUS
	virtual		bool						getInternalRadius(NxReal& radius, const NxVec3& dir)												const;
#endif
	virtual		void						computeLocalBounds(NxBounds3& bounds)																const;
	//~Shape interface

	// NvShape
	virtual		NxVec3						getBoxDimensions()				const	{ return dimensions;	}
	// ~NvShape
				void						setDimensions(const NxVec3&);
	NX_INLINE	const NxVec3&				getDimensionsFast()				const	{ return dimensions;	}
	NX_INLINE	void						getWorldOBB(NxBox& worldOBB)	const
											{
											const NxMat34& absPose = getAbsPoseFast();
											worldOBB.center		= absPose.t;
											worldOBB.rot		= absPose.M;
											worldOBB.extents	= dimensions;
											}
	// double buffering
										// No external calls in Core anymore!
//				void						getWorldOBBExternal(NxBox& world_obb)	const;

				void						vertex2World(int vx, int vy, int vz, NxVec3 & worldVertex) const;

	private:
				NxVec3						dimensions;			//radii of OBB (1/2 height/width/depth)
	//Convex interface
				NxVec3						boxVerts[8];		// Temp while refactoring	// GLOBALPOS?
				PxHullPolygonData			mPolygons[6];		// Temp while refactoring
				void						createHullData();
	//~Convex interface
	virtual		void						computeWorldBounds(NxBounds3&)	const;
	virtual		void						computeWorldSphere(NxSphere&)	const;
	virtual		void						computeLocalSphere(NxSphere&)	const;

	friend class Scene;	// For factory function
	};

//	}
#endif

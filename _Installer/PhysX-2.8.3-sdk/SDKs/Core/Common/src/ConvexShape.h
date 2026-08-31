#ifndef NX_COLLISION_CONVEXSHAPE
#define NX_COLLISION_CONVEXSHAPE
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/


#include "NxSimpleTriangleMesh.h"
#include "Shape.h"
#include "Opcode.h"

class NxConvexShapeDesc;
class ConvexMeshRuntime;
class NpConvexMesh;
class NvConvexMesh;

//namespace NxCollision {
class TriangleMesh;

class ConvexShape : public Shape
	{
	protected:
									ConvexShape(const NvShapeDesc & desc, Scene *, CoreActor& actor, NxU32 uid);
	virtual							~ConvexShape();

	public:

	// Shape interface
	virtual		Shape&				getShape()			{ return *this;	}
	virtual		const Shape&		getShape()	const	{ return *this;	}
	virtual		operator			Shape&()			{ return *this;	}
	virtual		void				visualize(NxFoundation::DebugRenderable& renderer)																const;
	virtual		bool				raycast(const NxRay& worldRay, NxReal maxDist, NxU32 hintFlags, NxRaycastHit& hit, bool firstHit)	const;
	virtual		bool				checkOverlapSphere(const NxSphere& worldSphere)														const;
	virtual		bool				checkOverlapOBB(const NxBox& worldBox)																const;
	virtual		bool				checkOverlapAABB(const NxBounds3& worldBounds)														const;
	virtual		bool				checkOverlapCapsule(const NxCapsule& worldCapsule)													const;

#ifdef SUPPORT_INTERNAL_RADIUS
	virtual		bool				getInternalRadius(NxReal& radius, const NxVec3& dir)												const;
#endif
	virtual		void				computeLocalBounds(NxBounds3& bounds)																const;
	//~Shape interface

				NpConvexMesh*		getConvexMesh();	//TODO: make this inline again if possible after refactor
				NvConvexMesh*		getNvConvexMesh();
				const NvConvexMesh*	getNvConvexMesh() const;
				void				setConvexMesh(NvConvexMesh*);

	//for mesh-mesh colldet.
//	void contact(const NxShape & s1, const NxShape & s2, NxReal separation, const NxVec3 & contactPoint, const NxVec3 & contactNormal);

	NX_INLINE	const ConvexMeshRuntime&	getConvexMeshFast()	const	{ return *triangleMesh;	}
	NX_INLINE	ConvexMeshRuntime&			getConvexMeshFast()			{ return *triangleMesh;	}
	NX_INLINE	void						setConvexMeshFast(ConvexMeshRuntime* tm)	{ triangleMesh = tm;	}

	NX_INLINE	NxU32				getMeshFlags()			const	{ return meshFlags;		}
	NX_INLINE	void				setMeshFlags(NxU32 f)			{ meshFlags = f;		}

#ifdef NX_SUPPORT_CONVEX_SCALE
	NX_INLINE	NxReal				getScale()				const	{ return scale;			}
	NX_INLINE	void				setScale(NxReal s)				{ scale = s;			}
#endif

	//OPCODE integration:
	NX_INLINE	void				vertex2World(const NxPoint& localVertex, NxVec3& worldVertex) const
									{
									const NxMat34 &absPose = getAbsPoseFast();
									worldVertex = absPose.t + absPose.M * localVertex;
									}
	private:
				ConvexMeshRuntime*	triangleMesh;	//!< Actual triangle mesh data
				NxU32				meshFlags;		//!< Combination of NxTriangleMeshShape::MeshFlag(s)
#ifdef NX_SUPPORT_CONVEX_SCALE
				NxReal				scale;			//!< Scale factor for this instance TODO: non-uniform scale ?
#endif
	// Internal methods
	virtual		void				computeWorldBounds(NxBounds3&)	const;
	virtual		void				computeWorldSphere(NxSphere&)	const;
	virtual		void				computeLocalSphere(NxSphere&)	const;

	friend class Scene;	// For factory function
	};
//	}
#endif

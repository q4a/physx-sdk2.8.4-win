#ifndef NX_COLLISION_PLANESHAPE
#define NX_COLLISION_PLANESHAPE
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/


#include "NxPlane.h"
#include "Shape.h"

//namespace NxCollision {

	enum PlaneType
		{
		PLANE_X,
		PLANE_Y,
		PLANE_Z,
		PLANE_ARBITRARY,

		PLANE_FORCE_DWORD	= 0x7fffffff
		};

class PlaneShape : public Shape
	{
	protected:
									PlaneShape(const NvShapeDesc & desc, Scene *, CoreActor& actor, NxU32 uid);
	virtual							~PlaneShape();

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

	virtual		void				computeLocalBounds(NxBounds3& bounds)																const;
	//~Shape interface

	NX_INLINE	const NxPlane&		getPlaneFast()		const	{ return plane;		}
	NX_INLINE	const NxVec3&		getTangent1()		const	{ return tangent1;	}
	NX_INLINE	const NxVec3&		getTangent2()		const	{ return tangent2;	}
	NX_INLINE	PlaneType			getPlaneType()		const	{ return planeType;	}

	// NvShape
	virtual		NxPlane				getPlaneEquation()	const	{ return plane;		}
	virtual		void				setPlaneEquation(const NxPlane &p);
	// ~NvShape
	void							getLowLevelTransform(PxdTransform& transform) const;

	private:
				NxPlane				plane;		//!< plane data (a*x + b*y + c*z + d = 0)
				NxVec3				tangent1;	//!< two arbitrary, mutually orthogonal tangents. Used by plane-mesh colldet at the moment.
				NxVec3				tangent2;
				PlaneType			planeType;	//!< type of plane

				void				makeTangents();

	// Internal methods
	virtual		void				computeWorldBounds(NxBounds3&)	const;
	virtual		void				computeWorldSphere(NxSphere&)	const;
	virtual		void				computeLocalSphere(NxSphere&)	const;

	friend class Scene;	// For factory function
	};
//	}
#endif

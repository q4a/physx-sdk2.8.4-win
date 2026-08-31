#ifndef NX_COLLISION_SPHERESHAPE
#define NX_COLLISION_SPHERESHAPE
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "NxSphere.h"
#include "Shape.h"

/**
A sphere shaped collision detection primitive.
*/
//namespace NxCollision {

class SphereShape : public Shape
	{
	protected:
								SphereShape(const NvShapeDesc & desc, Scene *, CoreActor& actor, NxU32 uid);
	virtual						~SphereShape();

	public:

	// Shape interface
	virtual		Shape&			getShape()			{ return *this;	}
	virtual		const Shape&	getShape()	const	{ return *this;	}
	virtual		operator		Shape&()			{ return *this;	}
	virtual		void			visualize(NxFoundation::DebugRenderable& renderer)													const;
	virtual		bool			raycast(const NxRay& worldRay, NxReal maxDist, NxU32 hintFlags, NxRaycastHit& hit, bool firstHit)	const;
	virtual		bool			checkOverlapSphere(const NxSphere& worldSphere)														const;
	virtual		bool			checkOverlapOBB(const NxBox& worldBox)																const;
	virtual		bool			checkOverlapAABB(const NxBounds3& worldBounds)														const;
	virtual		bool			checkOverlapCapsule(const NxCapsule& worldCapsule)													const;

#ifdef SUPPORT_INTERNAL_RADIUS
	virtual		bool			getInternalRadius(NxReal& radius, const NxVec3& dir)												const;
#endif
	virtual		void			computeLocalBounds(NxBounds3& bounds)																const;
	//~Shape interface

	NX_INLINE	NxReal			getRadiusFast()					const	{ return radius;				}
	NX_INLINE	void			getWorldSphereFast(NxSphere& dest) const	//redundant, see computeWorldSphere() which is common to all shapes.
								{
								dest.center = getAbsPoseFast().t;
								dest.radius = radius;
								}
	// NvShape
	virtual		void			setRadius(NxReal radius);
	virtual		NxReal			getRadius()						const	{ return getRadiusFast();		}
//@	virtual		void			getWorldSphere(NxSphere& dest)	const	{ getWorldSphereFast(dest);		}
	// ~NvShape

	private:
				NxReal			radius;

	// Internal methods
	virtual		void			computeWorldBounds(NxBounds3&)	const;
	virtual		void			computeWorldSphere(NxSphere&)	const;
	virtual		void			computeLocalSphere(NxSphere&)	const;

	friend class Scene;	// For factory function
	};
//	}
#endif

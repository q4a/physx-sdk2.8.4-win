#ifndef NX_COLLISION_CAPSULESHAPE
#define NX_COLLISION_CAPSULESHAPE
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "NxSegment.h"
#include "NxCapsule.h"
#include "Shape.h"


//namespace NxCollision {

class CapsuleShape : public Shape
	{
	protected:
									CapsuleShape(const NvShapeDesc & desc, Scene *, CoreActor& actor, NxU32 uid);
	virtual							~CapsuleShape();

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

	NX_INLINE	NxReal				getRadiusFast()			const	{ return radius;			}
	NX_INLINE	NxReal				getHeightFast()			const	{ return 2.0f*halfHeight;	}
	NX_INLINE	NxReal				getHalfHeightFast()		const	{ return halfHeight;		}
	NX_INLINE	NxU32				getCapsuleFlagsFast()	const	{ return capsuleFlags;		}

	NX_INLINE	void				getWorldSegmentFast(NxSegment& worldSegment)	const
									{
									const NxMat34 &absPose = getAbsPoseFast();
									absPose.M.getColumn(1, worldSegment.p1);
									worldSegment.p1 *= halfHeight;
									worldSegment.p0 = -worldSegment.p1;
									worldSegment.p0 += absPose.t;
									worldSegment.p1 += absPose.t;
									}
	NX_INLINE	void				getWorldCapsuleFast(NxCapsule& worldCapsule)	const
									{
									getWorldSegmentFast(worldCapsule);
									worldCapsule.radius = radius;
									}
	// NvShape
	virtual		void				setRadius(NxReal radius);
	virtual		void				setHeight(NxReal height);	
	virtual		void				setCapsuleFlags(NxU32 f)							{ capsuleFlags = f;						}

	virtual		NxReal				getRadius()									const	{ return getRadiusFast();				}
	virtual		NxReal				getHeight()									const	{ return getHeightFast();				}
	virtual		NxU32				getCapsuleFlags()							const	{ return getCapsuleFlagsFast();			}
	virtual		void				getWorldCapsule(NxCapsule& worldCapsule)	const	{ getWorldCapsuleFast(worldCapsule);	}
	// ~NvShape

				void				setDimensions(NxReal radius, NxReal height);
	protected:
				NxReal				radius;
				NxReal				halfHeight;
				NxU32				capsuleFlags;
	public:
	// Internal methods
	virtual		void				computeWorldBounds(NxBounds3&)	const;
	virtual		void				computeWorldSphere(NxSphere&)	const;
	virtual		void				computeLocalSphere(NxSphere&)	const;

	friend class Scene;	// For factory function
	};
//}
#endif

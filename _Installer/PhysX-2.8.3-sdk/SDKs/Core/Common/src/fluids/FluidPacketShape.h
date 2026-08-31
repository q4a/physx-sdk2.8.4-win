#ifndef NX_PHYSICS_FLUIDSHAPE
#define NX_PHYSICS_FLUIDSHAPE
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Nxp.h"
#if NX_USE_FLUID_API

#include "CoreElement.h"
#include "NvScene.h"

class Fluid;
struct NxFluidPacket;


/**
Descriptor for fluid packet shape.
*/
class FluidPacketShapeDesc
{
public:
	NxVec3					origin;
	NxVec3					extent;
	Fluid*					fluid;
};


/**
A collision detection primitive for fluids.
*/
#ifdef NX_FLUID_IN_PRIMARY_SCENE
class FluidPacketShape : public NvRawBounds, public CoreElement	// Inherit from NvRawBounds for force field interaction.
#else
class FluidPacketShape : public CoreElement
#endif
{
	public:
											FluidPacketShape(const FluidPacketShapeDesc & desc, NxU32 uid);
											~FluidPacketShape();
	
	// PxElement implementation
	virtual		void						destroy();
	virtual		bool						isActive() const { return false; }
	// ~PxElement

	public:
				void						release();

				void						setIndex(NxU32 index) { NX_ASSERT(index < ((1 << 16) - 1)); mIndex = static_cast<NxU16>(index); }
	NX_INLINE	NxU16						getIndex()	const { return mIndex; }

				void						setExtent(const NxVec3&);
	NX_INLINE	NxVec3						getExtent()		const	{ return mExtent;	}
				void						setOrigin(const NxVec3&);
	NX_INLINE	NxVec3						getOrigin()		const	{ return mOrigin; }

	NX_INLINE	Fluid&						getFluid()	const { return (Fluid&)getCoreActor(); }
				bool						getFluidPacket(NxFluidPacket& fluidPacket) const;

	NX_INLINE	NxU16						getParticleCount() const;
	NX_INLINE	NxU16						getParticleOffset() const;

				void						computeWorldBounds(NxBounds3&)	const;

	NX_INLINE	PxdShape					getLowLevelFluidShape() const { return mLLFluidShape; }
				void						setLowLevelFluidShape(PxdShape handle);

	private:
				NxVec3						mExtent;			//radii of AABB (1/2 height/width/depth)
				NxVec3						mOrigin;			//center of AABB

				NxU16						mIndex;

				PxdShape					mLLFluidShape;		// Low level handle of fluid packet
};


NX_INLINE NxU16 FluidPacketShape::getParticleCount() const
{
	if (mLLFluidShape)
		return static_cast<NxU16>(PxdShapeGetInt(mLLFluidShape, PXD_SHAPE_FLUID_PARTICLE_COUNT));
	else
		return 0;
}

NX_INLINE NxU16	FluidPacketShape::getParticleOffset() const
{
	if (mLLFluidShape)
		return static_cast<NxU16>(PxdShapeGetInt(mLLFluidShape, PXD_SHAPE_FLUID_PARTICLE_OFFSET));
	else
		return 0;
}


#endif	// NX_USE_FLUID_API

#endif

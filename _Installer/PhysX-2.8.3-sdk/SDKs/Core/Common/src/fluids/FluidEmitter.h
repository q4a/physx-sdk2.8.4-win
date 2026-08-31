#ifndef NX_PHYSICS_FLUIDEMITTER
#define NX_PHYSICS_FLUIDEMITTER
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Nxp.h"
#if NX_USE_FLUID_API

#include "Physics.h"
#include "Allocateable.h"
#include "NvFluidEmitter.h"
#include "fluids/NxFluidEmitterDesc.h"
#include "NxVec3.h"
#include "NxMat34.h"
#include "Fluid.h"
#include "Shape.h"

class NpFluidEmitter;
class NxDebugRenderable;

#define FLUID_EMITTER_RANDOMIZE_EMISSION 1

class FluidEmitter : public NvFluidEmitter, public NxFoundation::NxAllocateable
{
public:
						FluidEmitter(const NxFluidEmitterDesc &, Fluid &);
	virtual				~FluidEmitter();

	//DDI methods:

	void				visualize(NxFoundation::DebugRenderable & dr);
	void				setNpFluidEmitter(NpFluidEmitter* e)		{ mNpFluidEmitter = e; }
	NpFluidEmitter *	getNpFluidEmitter()							{ return mNpFluidEmitter; }

	NvFluid&			getFluid()							const	{ return mFluid; }

	void				setGlobalPose(const NxMat34&);
	NxMat34				getGlobalPoseVal()					const	{ return mGlobalPose; } 

	void				setGlobalPosition(const NxVec3 &p) 			{ mGlobalPose.t = p; setGlobalPose(mGlobalPose); }
	void				setGlobalOrientation(const NxMat33 &M) 		{ mGlobalPose.M = M; setGlobalPose(mGlobalPose); }

	NxVec3				getGlobalPositionVal()				const	{ return mGlobalPose.t; }
	NxMat33				getGlobalOrientationVal()			const	{ return mGlobalPose.M; }

	void				setLocalPose(const NxMat34& pose) 			{ mLocalPose = pose; updatePose(); }
	void				setLocalPosition(const NxVec3& t) 			{ mLocalPose.t = t; updatePose(); }
	void				setLocalOrientation(const NxMat33& M) 		{ mLocalPose.M = M; updatePose(); }

	NxMat34				getLocalPoseVal()					const	{ return mLocalPose; }
	NxVec3				getLocalPositionVal()				const	{ return mLocalPose.t; }
	NxMat33				getLocalOrientationVal()			const	{ return mLocalPose.M; }

	void 				setFrameShape(NvShape* shape);
	NvShape* 			getFrameShape()						const;

	void 				setRandomPos(NxVec3 t)						{ mRandomPos = t; }
	NxVec3 				getRandomPos()						const	{ return mRandomPos; }
	void 				setRandomAngle(NxReal t)					{ mRandomAngle = t; }
	NxReal 				getRandomAngle()					const	{ return mRandomAngle; }

	NxReal 				getDimensionX()						const	{ return mDimensionX; }
	NxReal 				getDimensionY()						const	{ return mDimensionY; }

	void 				setFluidVelocityMagnitude(NxReal t)			{ mVelocity = t; }
	NxReal 				getFluidVelocityMagnitude()			const	{ return mVelocity; }
	void 				setRate(NxReal t)							{ mRate = t; }
	NxReal 				getRate()							const	{ return mRate; }
	void 				setParticleLifetime(NxReal t)				{ mParticleLifetime = t; }
	NxReal 				getParticleLifetime()				const	{ return mParticleLifetime; }
	void				setRepulsionCoefficient(NxReal r)			{ mRepulsionCoefficient = r; }
	NxReal				getRepulsionCoefficient()			const	{ return mRepulsionCoefficient; }
	
	void				resetEmission(NxU32);
	NxU32				getMaxParticles()					const 	{ return mMaxParticles; }
	NxU32				getNbParticlesEmitted()				const	{ return mParticlesEmittedTotal; }
	
	void				setFlags(NxU32 flags);
	NxU32				getFlags()							const	{ return mFlags; }

	NxU32				getShapes()							const	{ return mShape; }
	NxU32				getTypes()							const	{ return mType; }

	//non-DDI methods:

	void updatePose();

	void step(	ParticleBirthDataArray* newParticles, NxU16 particleQuotaExternal, NxReal dt);

	void swapBuffers();

protected:

	NX_INLINE void computePositionNoise(NxVec3& posNoise);
	void computeSiteVelocity(NxVec3& siteVel, const NxVec3& sitePos);
	NX_INLINE bool isOutsideShape(NxU32 x, NxU32 y, NxReal offset) { return mShape == NX_FE_ELLIPSE && outsideEllipse(x,y,offset); }

	/**
	Just does position, velocity and lifetime update. Rest is done by Fluid.
	*/
	bool spawnParticle(const NxVec3& position, const NxVec3& velocity);

	static NX_INLINE NxReal randInRange(NxReal a,NxReal b);
	static NX_INLINE NxU32 randInRange(NxU32 range);

protected:

	Fluid&				mFluid;
	NxReal				mRate;
	NxVec3				mRandomPos;
	NxReal				mRandomAngle;

	//derived quantities
	NxU32				mNumSites;
	NxU32				mNumX;
	NxU32				mNumY;
	NxReal				mSpacingX;
	NxReal				mSpacingY;
	NxReal				mSpacingZ;
	NxReal				mLimitedVelocity;

	//only needed during step computations.
	NxVec3				mAxisX;
	NxVec3				mAxisY;
	NxVec3				mAxisZ;	
	NxVec3				mBasePos;

private:

	void initStep(ParticleBirthDataArray* newParticles, NxU16 particleQuotaExternal, NxReal dt);
	void updateDerivedBase();
	bool outsideEllipse(NxU32 x, NxU32 y, NxReal offset);

private:

	NpFluidEmitter*		mNpFluidEmitter;
	Shape*				mFrameShape;

	NxMat34				mLocalPose;
	NxReal				mDimensionX;
	NxReal				mDimensionY;
	NxU32				mFlags;
	NxU32				mShape;
	NxU32				mType;
	NxReal				mParticleLifetime;
	NxReal				mVelocity;
	NxReal				mRepulsionCoefficient;
	NxU32				mMaxParticles;
	NxU32				mParticlesEmittedTotal; //double buffered counter
	NxU32				mParticlesEmittedTotalInternal;
	NxU16				mParticlesEmittedFrame;
	bool				mEmpty;

	//state derived quantities
	NxMat34				mGlobalPose;
	bool				mGlobalPoseUpdated;
	NxVec3				mFrameBodyCenter;

	NxReal				mEllipticRadius2;
	NxReal				mEllipticConstX0;
	NxReal				mEllipticConstX1;
	NxReal				mEllipticConstY0;
	NxReal				mEllipticConstY1;

	//only needed during step computations.
	ParticleBirthDataArray*	mDstParticles;
	NxU16				mQuotaExternal;
	NxVec3				mLinMomentum;
	NxVec3				mAngMomentum;
	NxVec3				mBodyAngVel;
	NxVec3				mBodyLinVel;
};

/*-------------------------------------------------------------------------*/

NX_INLINE void FluidEmitter::computePositionNoise(NxVec3& posRand)
{
	posRand.x = randInRange(-mRandomPos.x, mRandomPos.x);
	posRand.y = randInRange(-mRandomPos.y, mRandomPos.y);
	posRand.z = randInRange(-mRandomPos.z, mRandomPos.z);
}

NX_INLINE NxReal FluidEmitter::randInRange(NxReal a,NxReal b)
{
#if FLUID_EMITTER_RANDOMIZE_EMISSION
	return NxMath::rand(a, b);
#else
	return a + (b-a)/2.0f;
#endif
}

NX_INLINE NxU32 FluidEmitter::randInRange(NxU32 range)
{
#if FLUID_EMITTER_RANDOMIZE_EMISSION
	NxU32 retval = rand();
	if(range > 0x7fff)
	{
		retval = (retval << 15) | rand();
		retval = (retval << 15) | rand();
	}
	return retval % range;
#else
	static NxU32 noRandomVal = 0;
	return noRandomVal++;
#endif
}


#endif	// NX_USE_FLUID_API

#endif

//NVIDIACOPYRIGHTBEGIN
///////////////////////////////////////////////////////////////////////////
// Copyright (c) 2010 NVIDIA Corporation
// All rights reserved. www.nvidia.com
///////////////////////////////////////////////////////////////////////////
//NVIDIACOPYRIGHTEND

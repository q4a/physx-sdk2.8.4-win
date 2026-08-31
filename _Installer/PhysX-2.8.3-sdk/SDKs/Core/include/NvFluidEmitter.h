#ifndef NX_FLUID_NV_FLUIDEMITTER
#define NX_FLUID_NV_FLUIDEMITTER
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

//#include "NxBounds3.h"
//#include "Allocateable.h"
//#include "Physics.h"

#include "fluids/NxFluidEmitterDesc.h"
#include "DebugRenderable.h"
#include "NvQueuedEvent.h"
#include "NxFluidUserNotify.h"


class NpFluidEmitter;
class NvShape;
class NvFluid;
class NvFluidEmitter;

class NvFluidEmitterEvent : public NvQueuedEvent
	{
	public:
	NvFluidEmitter& mEmitter;
	NxFluidEmitterEventType mType;
	NvFluidEmitterEvent(NvFluidEmitter& emitter, NxFluidEmitterEventType type) : NvQueuedEvent(NX_QE_FLUID_EMITTER), mEmitter(emitter), mType(type) {}
	};

class NvFluidEmitter
	{
	public:
	virtual void                visualize(NxFoundation::DebugRenderable & dr) = 0;
	virtual void				setNpFluidEmitter(NpFluidEmitter*) = 0;
	virtual NpFluidEmitter *	getNpFluidEmitter()	= 0;
	
	//interface
	virtual NvFluid&			getFluid() const = 0;

	virtual	void				setGlobalPose(const NxMat34&) = 0;
	virtual	void				setGlobalPosition(const NxVec3&) = 0;
	virtual	void				setGlobalOrientation(const NxMat33&) = 0;

	virtual	NxMat34				getGlobalPoseVal()						const = 0;
	virtual	NxVec3				getGlobalPositionVal()					const = 0;
	virtual	NxMat33				getGlobalOrientationVal()				const = 0;

	virtual	void				setLocalPose(const NxMat34&) = 0;
	virtual	void				setLocalPosition(const NxVec3&) = 0;
	virtual	void				setLocalOrientation(const NxMat33&) = 0;

	virtual	NxMat34				getLocalPoseVal()						const = 0;
	virtual	NxVec3				getLocalPositionVal()					const = 0;
	virtual	NxMat33				getLocalOrientationVal()				const = 0;

	virtual	void 				setFrameShape(NvShape*) = 0;
	virtual	NvShape* 			getFrameShape()							const = 0;

	virtual	void 				setRandomPos(NxVec3) = 0;
	virtual	NxVec3 				getRandomPos()							const = 0;
	virtual	void 				setRandomAngle(NxReal) = 0;
	virtual	NxReal 				getRandomAngle()						const = 0;
				
	virtual	NxReal 				getDimensionX()							const = 0;
	virtual	NxReal 				getDimensionY()							const = 0;

	virtual	void 				setFluidVelocityMagnitude(NxReal) = 0;
	virtual	NxReal 				getFluidVelocityMagnitude()				const = 0;
	virtual	void 				setRate(NxReal) = 0;
	virtual	NxReal 				getRate()								const = 0;
	virtual	void 				setParticleLifetime(NxReal) = 0;
	virtual	NxReal 				getParticleLifetime()					const = 0;
	virtual	void				setRepulsionCoefficient(NxReal) = 0;
	virtual	NxReal				getRepulsionCoefficient()				const = 0;

	virtual void				resetEmission(NxU32) = 0;
	virtual	NxU32 				getMaxParticles()						const = 0;
	virtual	NxU32				getNbParticlesEmitted()					const = 0;
	
	/**
	Manage the emitter flags. See the list of flags in NxFluidEmitterDesc.h.
	*/
	virtual	void				setFlags(NxU32) = 0;
	virtual	NxU32				getFlags()								const = 0;

	/**
	Get the emitter shape. See the list of methods in NxFluidEmitterDesc.h.
	*/
	virtual	NxU32				getShapes()								const = 0;

	/**
	Get the emitter type. See the list of actions in NxFluidEmitterDesc.h.
	*/
	virtual	NxU32				getTypes()								const = 0;
	};

#endif


//NVIDIACOPYRIGHTBEGIN
///////////////////////////////////////////////////////////////////////////
// Copyright (c) 2010 NVIDIA Corporation
// All rights reserved. www.nvidia.com
///////////////////////////////////////////////////////////////////////////
//NVIDIACOPYRIGHTEND


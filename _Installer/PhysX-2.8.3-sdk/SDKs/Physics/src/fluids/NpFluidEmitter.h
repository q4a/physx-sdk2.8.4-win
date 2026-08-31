#ifndef NX_FLUIDS_NPFLUIDEMITTER
#define NX_FLUIDS_NPFLUIDEMITTER
/*----------------------------------------------------------------------------*\
|
|						Public Interface to NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/
#include "Physics.h"
#include "NxPhysicsSDK.h"
#include "Allocateable.h"
#include "ReadWriteMutex.h"
#include "DebugRenderable.h"

#include "fluids/NxFluidEmitter.h"

class NxBounds3;
class NxFluid;
class NvFluidEmitter;
class NpFluid;
class NxActor;

class NpFluidEmitter: public NxFluidEmitter, public NxFoundation::NxAllocateable, public ReadWriteMutex
	{
	public:
	
	NpFluidEmitter(NvFluidEmitter* _nvFluidEmitter, NpFluid* _npFluid);
	virtual		~NpFluidEmitter();
				void				visualize(NxFoundation::DebugRenderable & dr);
	NX_INLINE	const NvFluidEmitter*	getNvFluidEmitterConst()				const	{ return nvFluidEmitter; }
	NX_INLINE	NvFluidEmitter*		getNvFluidEmitter()							{ return nvFluidEmitter; }

	virtual		NxFluid&			getFluid()							const;

	virtual		bool				loadFromDesc(const NxFluidEmitterDesc& desc);
	virtual		bool				saveToDesc(NxFluidEmitterDesc &desc)const;

	virtual		void				setGlobalPose(const NxMat34&);
	virtual		void				setGlobalPosition(const NxVec3&);
	virtual		void				setGlobalOrientation(const NxMat33&);

	virtual		NxMat34				getGlobalPoseVal()					const;
	virtual		NxVec3				getGlobalPositionVal()				const;
	virtual		NxMat33				getGlobalOrientationVal()			const;

	virtual		void				setLocalPose(const NxMat34&);
	virtual		void				setLocalPosition(const NxVec3&);
	virtual		void				setLocalOrientation(const NxMat33&);

	virtual		NxMat34				getLocalPoseVal()					const;
	virtual		NxVec3				getLocalPositionVal()				const;
	virtual		NxMat33				getLocalOrientationVal()			const;

	virtual		void 				setFrameShape(NxShape*);
	virtual		NxShape* 			getFrameShape()						const;

	virtual		NxReal 				getDimensionX()						const;
	virtual		NxReal 				getDimensionY()						const;
	virtual		void 				setRandomPos(NxVec3);
	virtual		NxVec3 				getRandomPos()						const;
	virtual		void 				setRandomAngle(NxReal);
	virtual		NxReal 				getRandomAngle()					const;
	virtual		void 				setFluidVelocityMagnitude(NxReal);
	virtual		NxReal 				getFluidVelocityMagnitude()			const;
	virtual		void 				setRate(NxReal);
	virtual		NxReal 				getRate()							const;
	virtual		void 				setParticleLifetime(NxReal);
	virtual		NxReal 				getParticleLifetime()				const;

	virtual		void				setRepulsionCoefficient(NxReal);
	virtual		NxReal				getRepulsionCoefficient()			const;

	virtual		void				resetEmission(NxU32);
	virtual		NxU32 				getMaxParticles()					const;
	virtual		NxU32				getNbParticlesEmitted()				const;

	/**
	Manage the emitter flags. See the list of flags in NxFluidEmitterDesc.h.
	*/
	virtual		void				setFlag(NxFluidEmitterFlag, bool);
	virtual		NX_BOOL				getFlag(NxFluidEmitterFlag)			const;

	/**
	Get the emitter shape. See the list of methods in NxFluidEmitterDesc.h.
	*/
	virtual		NX_BOOL				getShape(NxEmitterShape)			const;

	/**
	Get the emitter type. See the list of actions in NxFluidEmitterDesc.h.
	*/
	virtual		NX_BOOL				getType(NxEmitterType)				const;


	/**
	Sets a name string for the object that can be retrieved with getName(). This is for debugging and is not used
	by the SDK. The string is not copied by the SDK, only the pointer is stored.
	*/
	virtual	void					setName(const char*);

	/**
	retrieves the name string set with setName().
	*/
	virtual	const char*				getName()			const;
	void							removeFromVRD();
	
	private:

	NvFluidEmitter * nvFluidEmitter;
	NpFluid * npFluid;
	};
#endif


//NVIDIACOPYRIGHTBEGIN
///////////////////////////////////////////////////////////////////////////
// Copyright (c) 2010 NVIDIA Corporation
// All rights reserved. www.nvidia.com
///////////////////////////////////////////////////////////////////////////
//NVIDIACOPYRIGHTEND


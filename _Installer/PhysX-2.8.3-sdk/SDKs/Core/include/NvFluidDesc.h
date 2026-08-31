#ifndef NX_FLUID_NVFLUIDDESC
#define NX_FLUID_NVFLUIDDESC
/*----------------------------------------------------------------------------*\
|
|						Public Interface to NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "fluids/NxFluidDesc.h"
#include "fluids/NxFluidEmitterDesc.h"

// Internal descriptor for NvFluid class. 
// For information about parameters see NxFluidDesc.

class NvFluidDesc
	{
	public:
	// has no emitters, emitters need to be gerenrated above DDI
	NxParticleData				initialParticleData;
	NxU32						maxParticles;
	NxU32						numReserveParticles;
	NxReal						restParticlesPerMeter;
	NxReal						restDensity;
	NxReal						kernelRadiusMultiplier;
	NxReal						motionLimitMultiplier;
	NxReal						collisionDistanceMultiplier;
	NxU32						packetSizeMultiplierLog2;
	NxReal						stiffness;
	NxReal						viscosity;
	NxReal						surfaceTension;
	NxReal						damping;
	NxReal						fadeInTime;
	NxVec3						externalAcceleration;
	NxPlane						projectionPlane;
	NxReal						staticCollisionRestitution;
	NxReal						staticCollisionAdhesion;
	NxReal						staticCollisionAttraction;
	NxReal						dynamicCollisionRestitution;
	NxReal						dynamicCollisionAdhesion;
	NxReal						dynamicCollisionAttraction;
	NxReal						collisionResponseCoefficient;
	NxU32						simulationMethod;
	NxU32						collisionMethod;
	
	NxCollisionGroup			collisionGroup;
	NxGroupsMask				groupsMask;

	NxU16						forceFieldMaterial;

	// has no surface mesh descriptor, surface mesh is created above DDI
	NxParticleData				particlesWriteData;
	NxParticleIdData			particleDeletionIdWriteData;
	NxParticleIdData			particleCreationIdWriteData;
	NxFluidPacketData			fluidPacketData;
	NxU32						flags;

	NX_INLINE		NvFluidDesc();	
	NX_INLINE void	setToDefault();
	};

NX_INLINE NvFluidDesc::NvFluidDesc()
	{
	setToDefault();
	}

NX_INLINE void NvFluidDesc::setToDefault()
	{
	maxParticles				= 0xffff;
	numReserveParticles			= 0;
	restParticlesPerMeter		= 50.0f;
    restDensity					= 1000.0f;
    kernelRadiusMultiplier		= 1.2f;
	motionLimitMultiplier		= 3.0f*kernelRadiusMultiplier;
	collisionDistanceMultiplier = 0.1f*kernelRadiusMultiplier;
	stiffness					= 20.0f;
    viscosity					= 6.0f;
	surfaceTension				= 0.0f;
    damping						= 0.0f;
	fadeInTime					= 0.0f;
	externalAcceleration.zero();
	projectionPlane.set(NxVec3(0.0f, 0.0f, 1.0f), 0.0f);
    staticCollisionRestitution	= 1.0f;
	staticCollisionAdhesion		= 0.05f;
	staticCollisionAttraction	= 0.0f;
	dynamicCollisionRestitution	= 1.0f;
	dynamicCollisionAdhesion	= 0.5f;
	dynamicCollisionAttraction	= 0.0f;
	collisionResponseCoefficient = 0.2f;

	simulationMethod			= NX_F_SPH;
	collisionMethod				= NX_F_STATIC|NX_F_DYNAMIC;
	
	collisionGroup				= 0;
	groupsMask.bits0			= 0;
	groupsMask.bits1			= 0;
	groupsMask.bits2			= 0;
	groupsMask.bits3			= 0;

	forceFieldMaterial			= 0;

	
	particlesWriteData			.setToDefault();
	particleDeletionIdWriteData .setToDefault();
	particleCreationIdWriteData .setToDefault();
	fluidPacketData				.setToDefault();

	flags						= NX_FF_VISUALIZATION;
	}

#endif


//NVIDIACOPYRIGHTBEGIN
///////////////////////////////////////////////////////////////////////////
// Copyright (c) 2010 NVIDIA Corporation
// All rights reserved. www.nvidia.com
///////////////////////////////////////////////////////////////////////////
//NVIDIACOPYRIGHTEND


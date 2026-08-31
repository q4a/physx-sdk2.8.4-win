#ifndef NX_FLUID_NV_FLUID
#define NX_FLUID_NV_FLUID
/*----------------------------------------------------------------------------*\
|
|						Public Interface to NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "DebugRenderable.h"
#include "NvMirrorManagerClient.h"
#include "NvQueuedEvent.h"
#include "NxFluidUserNotify.h"


class NxStream;
class NxParticleData;
class NxParticleUpdateData;
class NxParticleIdData;
enum NxFluidFlag;

class NvFluidEmitter;
class NxFluidEmitterDesc;
class NvScene;
class NpFluid;
class NvFluid;
class NvShape;
class NvRawTriangleMesh;

struct NvFluidParticle
	{
		NxVec3	position;      
		NxF32	density;
		NxVec3	velocity;     
		NxU16	id;
		NxU16	_reserved0;
		NxVec3	normal;	
		NxU32	_reserved1;
		NxF32	deathTime;
		NxU32	_reserved2[3];  
	};

class NvFluidEvent : public NvQueuedEvent
	{
	public:
	NvFluid& mFluid;
	NxFluidEventType mType;
	NvFluidEvent(NvFluid& fluid, NxFluidEventType type) : NvQueuedEvent(NX_QE_FLUID), mFluid(fluid), mType(type) {}
	};

class NvFluid: public NvMirrorManagerClient
	{
	public:
	virtual		NvScene*			getOwnerScene()											const	= 0;
	virtual		void                visualize(NxFoundation::DebugRenderable & dr)					= 0;
	virtual		void				setNpFluid(NpFluid*)											= 0;
	virtual		NpFluid*			getNpFluid()													= 0;

	virtual		NvFluidEmitter*		createEmitter(const NxFluidEmitterDesc&, bool buffered)			= 0;
	virtual		void				releaseEmitter(NvFluidEmitter&)									= 0;
	virtual		void				addEmitter(NvFluidEmitter&e)									= 0;
	virtual		NxU32				getNbEmitters()											const	= 0;
	virtual		NvFluidEmitter**	getEmitters()											const	= 0;

	virtual		NxU32 				addParticles(const NxParticleData&, bool readIds, bool appendIds) = 0;
	virtual		void				removeAllParticles()											= 0;
	virtual		void 				getParticleIds(NxFoundation::NxArraySDK<NxU32>& ids, NxU32 requestedNum) = 0;
	virtual		void				appendUserParticleData(const NxParticleData& particles, bool appendIds) = 0;


	virtual		NxParticleData 		getParticlesWriteData()									const	= 0;
	virtual		void 				setParticlesWriteData(const NxParticleData&)					= 0;

	virtual		NxParticleIdData	getParticleDeletionIdWriteData()						const	= 0;
	virtual		void				setParticleDeletionIdWriteData(const NxParticleIdData&)			= 0;

	virtual		NxParticleIdData	getParticleCreationIdWriteData()						const	= 0;
	virtual		void				setParticleCreationIdWriteData(const NxParticleIdData&)			= 0;

	virtual		NxFluidPacketData	getFluidPacketData()									const	= 0;
	virtual		void				setFluidPacketData(const NxFluidPacketData&)					= 0;

	virtual		NxU32				getSimulationMethod()									const	= 0;
	virtual		void				setSimulationMethod(NxU32 simMethod)							= 0;

	virtual		NxReal				getStiffness()											const	= 0;
	virtual		void 				setStiffness(NxReal)											= 0;
	virtual		NxReal				getViscosity()											const	= 0;
	virtual		void 				setViscosity(NxReal)											= 0;
	virtual		NxReal				getSurfaceTension()										const	= 0;
	virtual		void 				setSurfaceTension(NxReal)										= 0;	
	virtual		NxReal				getDamping()											const	= 0;
	virtual		void 				setDamping(NxReal)												= 0;
	virtual		NxReal				getFadeInTime()											const	= 0;
	virtual		void 				setFadeInTime(NxReal)											= 0;
	virtual		NxVec3				getExternalAcceleration()								const	= 0;
	virtual		void 				setExternalAcceleration(const NxVec3 &)							= 0;

	virtual		NxPlane				getProjectionPlane()									const	= 0;
	virtual		void 				setProjectionPlane(const NxPlane& plane)						= 0;

	virtual		NxU32				getCollisionMethod()									const	= 0;
	virtual		void				setCollisionMethod(NxU32 collMethod)							= 0;

	virtual		NxReal				getStaticCollisionRestitution()							const	= 0;
	virtual		void 				setStaticCollisionRestitution(NxReal)							= 0;
	virtual		NxReal				getStaticCollisionAdhesion()							const	= 0;
	virtual		void 				setStaticCollisionAdhesion(NxReal)								= 0;
	virtual		NxReal				getDynamicCollisionRestitution()						const	= 0;
	virtual		void 				setDynamicCollisionRestitution(NxReal)							= 0;
	virtual		NxReal				getDynamicCollisionAdhesion()							const	= 0;
	virtual		void 				setDynamicCollisionAdhesion(NxReal)								= 0;
	virtual		void				setCollisionResponseCoefficient(NxReal coefficient)				= 0;
	virtual		NxReal				getCollisionResponseCoefficient()						const	= 0;

	virtual		void				setFlags(NxU32)													= 0;
	virtual		NxU32				getFlags()												const	= 0;

	virtual		NxU32 				getMaxParticles()										const	= 0;
	virtual		NxU32				getNumReserveParticles()								const	= 0;
	virtual		void				setNumReserveParticles(NxU32)									= 0;
	virtual     void				setCurrentParticleLimit(NxU32)									= 0;
	virtual     NxU32				getCurrentParticleLimit()								const	= 0;

	/**
	@param ud, has to reference the update buffer of the caller, which has to correspond to the 
	entire particle array, as it is ordered after the last waitStep.
	*/
	virtual		void				updateParticles(const NxParticleUpdateData&)					= 0;

	/**
	@param ud, needs to reference an update buffer which corresponds to the 
	particle subarray corresponding to a fluid packet, as it is ordered after the last waitStep.
	@param packet, needs to reference the target packet subarray of the update.

	e.g. ud.bufferForce needs to point to the first F32 force component corresponding to the 
	update for the particle at packet.firstParticleIndex.
	*/	
	virtual		void				updateParticles(const NxParticleUpdateData&, const NxFluidPacket& packet) = 0;

	virtual		NxReal				getKernelRadiusMultiplier()								const	= 0;
	virtual		NxReal				getMotionLimitMultiplier()								const	= 0;
	virtual		NxReal				getCollisionDistanceMultiplier()						const	= 0;
	virtual		NxReal				getRestParticlesPerMeter()								const	= 0;
	virtual		NxReal				getRestDensity()										const	= 0;
	virtual		NxReal				getRestParticleDistance()								const	= 0;
	virtual		NxU32				getPacketSizeMultiplier()								const	= 0;
	virtual		NxReal				getParticleMass()										const	= 0;
	virtual		void				setGroup(NxCollisionGroup collisionGroup)						= 0;
	virtual		NxCollisionGroup	getGroup()												const	= 0;

	virtual		void				setGroupsMask(const NxGroupsMask& groupsMask)					= 0;
	virtual		const NxGroupsMask*	getGroupsMask()											const	= 0;

	virtual		void				getWorldBounds(NxBounds3& dest)							const	= 0;

	virtual NxForceFieldMaterial	getForceFieldMaterial()									const	= 0;							
	virtual void 					setForceFieldMaterial(NxForceFieldMaterial)						= 0;

	/**
	For now, this method should not be called while the fluid is simuating.
	*/
	virtual		bool				getPacket(NxFluidPacket& packet, const NxBounds3& packetBound) const = 0;

	/**
	For now, this method should not be called while the fluid is simuating.
	*/
	virtual		NvFluidParticle*	getParticles() = 0;
	
	/**
	For now, this method should not be called while the fluid is simuating.
	*/
	virtual		NxU16				getParticleCount() = 0;



    virtual		NxU32			    getLaneUsec()											const	= 0;

};

#endif


//NVIDIACOPYRIGHTBEGIN
///////////////////////////////////////////////////////////////////////////
// Copyright (c) 2010 NVIDIA Corporation
// All rights reserved. www.nvidia.com
///////////////////////////////////////////////////////////////////////////
//NVIDIACOPYRIGHTEND


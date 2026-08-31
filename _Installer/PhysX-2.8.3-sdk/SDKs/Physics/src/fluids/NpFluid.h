#ifndef NP_PHYSICS_NX_FLUID
#define NP_PHYSICS_NX_FLUID
/*----------------------------------------------------------------------------*\
|
|						Public Interface to NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/
#include "fluids/NxFluid.h"
#include "DebugRenderable.h"

#ifdef CORELIB					//lame hack needed until NpFluid is pulled out of core.
#include "../ReadWriteMutex.h"
#else
#include "ReadWriteMutex.h"
#endif

class NxFluidEmitterDesc;
class NxFluidEmitter;
class NvFluidDesc;
class NvFluid;
class NpScene;
class NpFluidEmitter;

class NpFluid: public NxFluid, public NxFoundation::NxAllocateable, public ReadWriteMutex
	{

	public:

		NpFluid(NvFluid* nvFluid, NpScene* npScene,NxCompartment *);
	virtual		~NpFluid();
	void		visualize(NxFoundation::DebugRenderable & dr);
	static void convertDescriptor(const NxFluidDescBase &nxDesc, NvFluidDesc &nvDesc);

	NX_INLINE	const NvFluid *		getNvFluidConst()										const	{ return nvFluid; }
	NX_INLINE	NvFluid *			getNvFluid()													{ return nvFluid; }

	NX_INLINE	NpScene *			getNpScene()											const	{ return npScene; }

	
	virtual		bool				loadFromDesc(const NxFluidDescBase& desc);
	virtual		bool				saveToDesc(NxFluidDescBase &desc)							const;

	virtual		NxFluidEmitter*		createEmitter(const NxFluidEmitterDesc&);
	virtual		void				releaseEmitter(NxFluidEmitter&);
	virtual		NxU32				getNbEmitters()											const;
	virtual		NxFluidEmitter**	getEmitters()											const;

	void		commitChanges();

	virtual		NxU32 				addParticles(const NxParticleData&, bool appendIds);
	virtual		void				removeAllParticles();

	virtual		NxParticleData 		getParticlesWriteData()									const;
	virtual		void 				setParticlesWriteData(const NxParticleData&);
	virtual		NxParticleIdData	getParticleDeletionIdWriteData()						const;
	virtual		void				setParticleDeletionIdWriteData(const NxParticleIdData&);
	virtual		NxParticleIdData	getParticleCreationIdWriteData()						const;
	virtual		void				setParticleCreationIdWriteData(const NxParticleIdData&);
	virtual		NxFluidPacketData	getFluidPacketData()									const;
	virtual		void				setFluidPacketData(const NxFluidPacketData&);


	virtual		NxReal				getStiffness()											const;
	virtual		void 				setStiffness(NxReal);
	virtual		NxReal				getViscosity()											const;
	virtual		void 				setViscosity(NxReal);
	virtual		NxReal				getSurfaceTension()										const;
	virtual		void 				setSurfaceTension(NxReal);
	virtual		NxReal				getDamping()											const;
	virtual		void 				setDamping(NxReal);
	virtual		NxReal				getFadeInTime()											const;
	virtual		void 				setFadeInTime(NxReal);
	virtual		NxVec3				getExternalAcceleration()								const;
	virtual		void 				setExternalAcceleration(const NxVec3 &);
	virtual		NxReal				getRestitutionForStaticShapes()							const;
	virtual		void 				setRestitutionForStaticShapes(NxReal);
	virtual		NxReal				getDynamicFrictionForStaticShapes()						const;
	virtual		void 				setDynamicFrictionForStaticShapes(NxReal);
	virtual		NxReal				getStaticFrictionForStaticShapes()						const	{ return 0.0f; }
	virtual		void 				setStaticFrictionForStaticShapes(NxReal)						{}
	virtual		NxReal				getAttractionForStaticShapes()							const	{ return 0.0f; }
	virtual		void 				setAttractionForStaticShapes(NxReal)							{}
	virtual		NxReal				getRestitutionForDynamicShapes()						const;
	virtual		void 				setRestitutionForDynamicShapes(NxReal);
	virtual		NxReal				getDynamicFrictionForDynamicShapes()					const;
	virtual		void 				setDynamicFrictionForDynamicShapes(NxReal);
	virtual		NxReal				getStaticFrictionForDynamicShapes()						const	{ return 0.0f; }
	virtual		void 				setStaticFrictionForDynamicShapes(NxReal)						{}
	virtual		NxReal				getAttractionForDynamicShapes()							const	{ return 0.0f; }
	virtual		void 				setAttractionForDynamicShapes(NxReal)							{}
	virtual		void				setCollisionResponseCoefficient(NxReal coefficient); 
	virtual		NxReal				getCollisionResponseCoefficient()						const; 

	virtual		void				setFlag(NxFluidFlag flag, bool value);
	virtual		NX_BOOL				getFlag(NxFluidFlag flag)								const;

	virtual		NxScene&			getScene()												const;

	virtual		NxU32 				getMaxParticles()										const;
	virtual		NxU32				getNumReserveParticles()								const;
	virtual		void				setNumReserveParticles(NxU32);

	virtual		NxU32 				getCurrentParticleLimit()								const;
	virtual		void 				setCurrentParticleLimit(NxU32);

	virtual		void				updateParticles(const NxParticleUpdateData&);

	virtual		NxReal				getKernelRadiusMultiplier()								const;
	virtual		NxReal				getMotionLimitMultiplier()								const;
	virtual		NxReal				getCollisionDistanceMultiplier()						const;
	virtual		NxReal				getRestParticlesPerMeter()								const;
	virtual		NxU32				getPacketSizeMultiplier()								const;
	virtual		NxReal				getRestDensity()										const;
	virtual		NxReal				getRestParticleDistance()								const;
	virtual		NxReal				getParticleMass()										const;

	virtual		NxU32				getSimulationMethod()									const;
	virtual		void				setSimulationMethod(NxU32 simMethod);

	virtual		NxU32				getCollisionMethod()									const;
	virtual		void				setCollisionMethod(NxU32 collMethod);

	virtual		void				setGroup(NxCollisionGroup collisionGroup);
	virtual		NxCollisionGroup	getGroup()												const;

	virtual		void				setGroupsMask(const NxGroupsMask& groupsMask);
	virtual		const NxGroupsMask	getGroupsMask()											const;

	virtual		void				getWorldBounds(NxBounds3& dest)							const;

    virtual		NxU32			    getPPUTime()											const;

	virtual		NxForceFieldMaterial getForceFieldMaterial()					const;
	virtual		void 				setForceFieldMaterial(NxForceFieldMaterial mat);

	virtual		NxPlane				getProjectionPlane()									const;
	virtual		void 				setProjectionPlane(const NxPlane& plane);


	/**
	Sets a name string for the object that can be retrieved with getName(). This is for debugging and is not used
	by the SDK. The string is not copied by the SDK, only the pointer is stored.
	*/
	virtual	void					setName(const char*);

	/**
	retrieves the name string set with setName().
	*/
	virtual	const char*				getName()												const;
	virtual NxCompartment *			getCompartment()										const;

	void							sendToVRD(bool create);
	void							removeFromVRD();

	protected:

	NvFluid *						nvFluid;
	NpScene	*						npScene;
	NxCompartment *					compartment;

	private:

	struct BufferedParticle
	{
		NxU32  id;
		NxVec3 position;
		NxVec3 velocity;
		NxReal life;
	};

	NxFoundation::NxArraySDK<NpFluidEmitter*> 
									npEmitters;

	NxFoundation::NxArraySDK<NpFluidEmitter*> 
									npBufferedEmitters;
	NxFoundation::NxArraySDK<NpFluidEmitter*> 
									npDeadEmitters;

	//buffered 
	bool							removeAllParticlesBuffered;

	NxFoundation::NxArraySDK<BufferedParticle> 
									addParticlesBuffered;
	NxFoundation::NxArraySDK<NxU32>
									idsBuffered;
	};

#endif


//NVIDIACOPYRIGHTBEGIN
///////////////////////////////////////////////////////////////////////////
// Copyright (c) 2010 NVIDIA Corporation
// All rights reserved. www.nvidia.com
///////////////////////////////////////////////////////////////////////////
//NVIDIACOPYRIGHTEND


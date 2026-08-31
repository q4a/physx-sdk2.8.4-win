#ifndef NX_PHYSICS_FLUID
#define NX_PHYSICS_FLUID

/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Nxp.h"
#if NX_USE_FLUID_API

#include "NvFluidDesc.h"
#include "NvFluid.h"
#include "IDPool.h"
#include "NxI16Vec3.h"
#include "Scene.h"
#include "Shape.h"
#include "RbActor.h"
#include "SimplePool.h"
#include "FluidPacketShape.h"

#include "PxActor.h"
#include "px_lowlevel.h"

class FluidEmitter;
class NxParticle;
class FluidParticleQueue;
class FluidIdToIndexMap;

#define MAX_PARTICLES_PER_FLUID			PXD_FLUID_PARTICLE_LIMIT
#define MAX_PARTICLE_ID					65536

// Maximum number of new particles that can be generated per time step
#define NX_FLUID_MAX_NEW_PARTICLES_PER_UPDATE 4096

#define NX_FLUID_DEBUG_RENDERING		1
#define NX_FLUID_FORCE_PACKET_DATA		1
#define NX_FLUID_FORCE_ID_TO_INDEX_MAP	1

#define NX_FLUID_INTERACTING_SHAPE_HASH_SIZE	128	// Size of hash table for interacting rigid body shapes
													// NOTE: Needs to be power of 2

/*!
Structure to store the rigid body shapes that collide with the fluid
*/
class InteractingShapeHash
{
public:
	struct ShapeHashEntry
	{
		NvShape*			shape;		// The interacting rigid body shape
		ShapeHashEntry*		next;		// Next hash entry (used if some entries share the same hash index)
		NxU16				refCount;	// Reference counter to count number of fluid shapes that collide with the rigid body
		NxU16				prevRefCount;	// Reference counter of last simulation step
	};

	class Iterator
	{
	public:
												Iterator(const InteractingShapeHash* shapeHash);	// If you use this ctor directly you have to call reset()
																									// before accessing elements
												~Iterator();

		NX_INLINE		void					reset() { mEntry = NULL; mIndex = -1; if (mShapeHash->getNumEntries()) {next();} }

		NX_INLINE		void					operator++() { next(); }
		NX_INLINE       const ShapeHashEntry*	operator*() { return mEntry; }

	private:
												Iterator();
						void					next();

		const InteractingShapeHash*	mShapeHash;	// Shape hash the iterator is attached to
		ShapeHashEntry*				mEntry;		// Current hash entry
		NxI32						mIndex;		// Current hash index
	};

												InteractingShapeHash();
												~InteractingShapeHash();

						void					registerShape(NvShape* shape);
						void					unregisterShape(NvShape* shape);
						void					updateRefCounters();
						const InteractingShapeHash::Iterator&	getIterator();
	NX_INLINE			NxU32					getNumEntries() const { return mNumEntries; }

private:

	NX_INLINE			NxU32					getHashIndex(const NvShape* shape) const;
	NX_INLINE			ShapeHashEntry*			createEntry(NvShape* shape);
	NX_INLINE			bool					findEntry(const NvShape* shape, ShapeHashEntry*& entry, ShapeHashEntry*& prevEntry) const;


	ShapeHashEntry*						mShapeHashTable[NX_FLUID_INTERACTING_SHAPE_HASH_SIZE];	// Hash table for interacting shapes
	SimplePool<InteractingShapeHash::ShapeHashEntry>	mShapeHashPool;	// Pool of hash entries
	Iterator							mIterator;
	NxU32								mNumEntries;

	friend class Iterator;
};

NX_INLINE NxU32 InteractingShapeHash::getHashIndex(const NvShape* shape) const
{
	NxU32 index = (static_cast<const RbActor*>(shape))->getActorId();	// MS: This cast is ugly! We could use the address...
	return (index & (NX_FLUID_INTERACTING_SHAPE_HASH_SIZE - 1));	// Modulo hash size
}

NX_INLINE InteractingShapeHash::ShapeHashEntry* InteractingShapeHash::createEntry(NvShape* shape)
{
	ShapeHashEntry* entry = mShapeHashPool.construct();

	if (entry)
	{
		entry->shape = shape;
		entry->next = NULL;
		entry->refCount = 1;
		entry->prevRefCount = 0;
	}

	return entry;
}

PXD_FORCE_INLINE bool InteractingShapeHash::findEntry(const NvShape* shape, ShapeHashEntry*& entry,
													  ShapeHashEntry*& prevEntry) const
{
	NxU32 hashIndex = getHashIndex(shape);

	prevEntry = NULL;
	bool hasEntry = false;
	if (mShapeHashTable[hashIndex])
	{
		ShapeHashEntry* curEntry = mShapeHashTable[hashIndex];

		while(curEntry->next)
		{
			if (curEntry->shape == shape)
				break;

			prevEntry = curEntry;
			curEntry = curEntry->next;
		}

		entry = curEntry;
		if (curEntry->shape == shape)
		{
			// An entry already exists for the given shape
			hasEntry = true;
		}
	}
	else
	{
		entry = NULL;
	}

	return hasEntry;
}


// Reduced particle data structure to buffer particles which should be added to the fluid
// (designed such that it can be mapped to low level API particle structure easily).
struct ParticleBirthData
{
	NxF32		position[3];
	NxF32		velocity[3];
	NxF32		deathTime;
	NxU16		id;
};

typedef NxFoundation::NxArraySDK<ParticleBirthData>	ParticleBirthDataArray;


class Fluid: public CoreActor, public NvFluid, public NxFoundation::NxAllocateable
{
public:

						Fluid(const NvFluidDesc &, NxU32, Scene &, NxU8 alternator);
						~Fluid();

	void				releaseFluidPacketShapes();


	NvScene*			getOwnerScene()									const	{ return &getScene(); }
	void				visualize(NxFoundation::DebugRenderable & dr);


	/* should be NxFluid */
	void				setNpFluid(NpFluid*);
	NpFluid*			getNpFluid();

	NvFluidEmitter*		createEmitter(const NxFluidEmitterDesc&, bool buffered);
	void				releaseEmitter(NvFluidEmitter&);
	void				addEmitter(NvFluidEmitter&e);

	NxU32				getNbEmitters()									const;
	NvFluidEmitter**	getEmitters()									const;

	NxReal				getStiffness()									const;
	void 				setStiffness(NxReal);

	NxReal				getViscosity()									const;
	void 				setViscosity(NxReal);

	NxReal				getSurfaceTension()								const;
	void 				setSurfaceTension(NxReal);

	NxReal				getDamping()									const;
	void 				setDamping(NxReal);

	NxReal				getFadeInTime()									const;
	void 				setFadeInTime(NxReal);

	NxReal				getStaticCollisionRestitution()					const;
	void 				setStaticCollisionRestitution(NxReal);

	NxReal				getStaticCollisionAdhesion()					const;
	void 				setStaticCollisionAdhesion(NxReal);

	NxReal				getDynamicCollisionRestitution()				const;
	void 				setDynamicCollisionRestitution(NxReal);

	NxReal				getDynamicCollisionAdhesion()					const;
	void 				setDynamicCollisionAdhesion(NxReal);

	NxReal				getCollisionResponseCoefficient()				const;
	void				setCollisionResponseCoefficient(NxReal);

	NxReal				getStaticFrictionForStaticShapes()				const;
	void				setStaticFrictionForStaticShapes(NxReal t);

	NxReal				getStaticFrictionForDynamicShapes()				const;
	void				setStaticFrictionForDynamicShapes(NxReal t);

	NxU32				getFlags()										const;
	void				setFlags(NxU32);

	NxU32 				getMaxParticles()								const;
	NxU32				getNumReserveParticles()						const;
	void				setNumReserveParticles(NxU32);
	void				setCurrentParticleLimit(NxU32 count);
	NxU32				getCurrentParticleLimit()						const;




	/**
	@param ud, has to reference the update buffer of the caller, which has to correspond to the 
	entire particle array, as it is ordered after the last waitStep.
	*/
	void				updateParticles(const NxParticleUpdateData&);

	/**
	@param ud, needs to reference an update buffer which corresponds to the 
	particle subarray corresponding to a fluid packet, as it is ordered after the last waitStep.
	@param packet, needs to reference the target packet subarray of the update.

	e.g. ud.bufferForce needs to point to the first F32 force component corresponding to the 
	update for the particle at packet.firstParticleIndex.
	*/	
	void				updateParticles(const NxParticleUpdateData& ud, const NxFluidPacket& packet);



	NxReal				getKernelRadiusMultiplier()						const;
	NxReal				getMotionLimitMultiplier()						const;
	NxReal				getMaxMotionDistance()							const;
	NxReal				getCollisionDistanceMultiplier()				const;
	NxReal				getCollisionDistance()							const;
	NxReal				getCollisionRange()								const;
	NxReal				getRestParticlesPerMeter()						const;
	NxReal				getRestDensity()								const;
	NxU32				getPacketSizeMultiplier()						const;
	NxU32				getPacketSizeMultiplierLog()					const;
	NxReal				getRestParticleDistance()						const;
	NxReal				getParticleMass()								const;
	NxReal				getCellSize()									const;
	NxReal				getPacketSize()									const;
	
	NxU32				getSimulationMethod()							const;
	void				setSimulationMethod(NxU32 simMethod);
	NxU32				getCollisionMethod()							const;
	void				setCollisionMethod(NxU32 collMethod);

	void				setGroup(NxCollisionGroup collisionGroup)				{ mCollisionGroup = collisionGroup; }
	NxCollisionGroup	getGroup()										const	{ return mCollisionGroup; }

	void				setGroupsMask(const NxGroupsMask& groupsMask)			{ mGroupsMask = groupsMask; }
	const NxGroupsMask*	getGroupsMask()									const	{ return &mGroupsMask; }

	NxForceFieldMaterial getForceFieldMaterial()						const	{ return mForceFieldMaterial;	}			
	void				setForceFieldMaterial(NxForceFieldMaterial mat)			{ mForceFieldMaterial = mat;	}

	void				getWorldBounds(NxBounds3& dest)					const;
	bool				getPacket(NxFluidPacket& packet, const NxBounds3& packetBound) const;

	NxU32 				addParticles(const NxParticleData& pd, bool readIds, bool appendIds)	{ return addParticles(pd, false, readIds, appendIds); }	
	void				removeAllParticles();
	void 				getParticleIds(NxFoundation::NxArraySDK<NxU32>& ids, NxU32 requestedNum);
	void				appendUserParticleData(const NxParticleData& srcPtcls, bool appendIds);


	NxParticleData 		getParticlesWriteData()							const;
	void 				setParticlesWriteData(const NxParticleData&);

	NxParticleIdData	getParticleDeletionIdWriteData()				const;
	void				setParticleDeletionIdWriteData(const NxParticleIdData&);

	NxParticleIdData	getParticleCreationIdWriteData()				const;
	void				setParticleCreationIdWriteData(const NxParticleIdData&);

	NxFluidPacketData	getFluidPacketData()							const;
	void				setFluidPacketData(const NxFluidPacketData&);

	NxVec3				getExternalAcceleration()						const;
	void 				setExternalAcceleration(const NxVec3 &);

	NxPlane				getProjectionPlane()							const;
	void 				setProjectionPlane(const NxPlane& plane);

	bool				isHardware() const			{ return (mFlags & NX_FF_HARDWARE)!=0; }

	// NvMirrorManagerClient implementation
	void				addInteractionWithRbShape(NvShape& nvShape, void* userData);
	void				removeInteractionWithRbShape(NvShape& nvShape, void* userData);
	void				addStaticShapeToRegion(NvShape &,const NxBounds3&) { NX_ASSERT(0); }
	void				removeStaticShapeFromRegion(NvShape &,const NxBounds3&) { NX_ASSERT(0); }
	void				addDynamicShape(NvShape& shape) { NX_ASSERT(0); }
//	bool				canShareStaticMeshesWith(NvFluid &)				const;	//a fluid from eventually A DIFFERENT same scene is passed.  Return true if the fluids can share static meshes.
	bool				isCompatibleWithStaticMesh(NvRawTriangleMesh &) const { NX_ASSERT(0); return false; }
	void				notifyReleasePermanentlyMirroredShape(NvShape *shape);
	NxReal				getImpulseTransferSmoothingAlpha() const { return 0.1f; }
	NxU32				getClientType() const { return NV_MIRROR_MANAGER_CLIENT_TYPE_FLUID; }
	void                accessParentScene() {} // in this call back, it is safe to access the parent (sw) scene
	void				passShapeRefCountChanges();
	// ~NvMirrorManagerClient

	// The following methods are needed for fluids that run in a compartment
	NX_INLINE bool		runsInCompartment() { return mRunsInCompartment; }
	NX_INLINE void		unregisterInteractingShape(NvShape* shape) { mInteractingShapeHash.unregisterShape(shape); }
	NX_INLINE void		updateInteractingShapeList() { mInteractingShapeHash.updateRefCounters(); }

	// PxActor implementation
	virtual bool canActivate() const { return false; }
	virtual bool canDeactivate() const { return true; }
	virtual bool isIslandGenerating() const { return false; }
	// ~PxActor

public:
	// non-DDI methods:

	NxU32 				addParticles(const NxParticleData&, bool initialParticles, bool readIds, bool appendIds);

	// Core functionality
	void				startStep(NxU32 substepCount, NxU32 substep);
	void				endStep();
	void				processFluidShapesUpdate();
	NX_INLINE PxdTask	startFluidShapesUpdate();
	NX_INLINE PxdTask	startDynamicsUpdate();
	NX_INLINE PxdTask	startCollisionUpdate();

	virtual NvFluidParticle *		getParticles();
	virtual NxU16					getParticleCount();
	NxU32							getParticles(PxdFluidParticleDataDesc* particles);

	void				updateUserData(NxU32* error);
	
	NX_INLINE PxdFluid&	getLowLevelFluid() { return mLLFluid; }

	int					getID() const	{ return mID; }

	NxU8				getMixedModeAlternator()  { return mMixedModeAlternator; }
	void				swapMixedModeAlternator() { mMixedModeAlternator = 1 - mMixedModeAlternator; }

	NxU32               getLaneUsec() const       { NX_ASSERT(0); return 0; }

	NX_INLINE static NxReal	computeCollisionRange(NxReal maxMotionDist, NxReal collisionDist)
	{
		return maxMotionDist + collisionDist + PXD_FLUID_COLLISION_SLACK;
	}

	NX_INLINE static NxReal	computeCollisionRange(NxReal restParticlesPerMeter, NxReal motionLimitMult, NxReal collisionDistMult)
	{
		NxReal maxMotionDist = motionLimitMult/restParticlesPerMeter;
		NxReal collisionDist = collisionDistMult/restParticlesPerMeter;
		return computeCollisionRange(maxMotionDist, collisionDist);
	}

	NX_INLINE static NxReal	computeCellSize(NxReal restParticlesPerMeter, NxReal kernelRadiusMultiplier)
	{
		NxReal restParticleDistance = 1.0f/restParticlesPerMeter;
		return kernelRadiusMultiplier * restParticleDistance;
	}

	NX_INLINE static NxReal	computePacketSize(NxReal restParticlesPerMeter, NxReal kernelRadiusMultiplier, NxU32 packetSizeMult)
	{
		NxReal cellSize = computeCellSize(restParticlesPerMeter, kernelRadiusMultiplier);
		return cellSize * packetSizeMult;
	}	

private:

	//
	// Particle update related structures
	//
	enum ParticleUpdateFlag
	{
		PUF_NONE		= 0,		// No update.
		PUF_DELETE		= (1<<0),	// Delete particle.
		PUF_VELOCITY	= (1<<1)	// Update particle velocity.
	};

	struct ParticleUpdateData
	{
		PxdF32		deltaVelocity[3];
		//PxU32		flag;
	};

	typedef NxFoundation::NxArraySDK<NxU32>		IdArray;
	typedef NxFoundation::NxArraySDK<FluidPacketShape*>	FluidShapeArray;

	void					updateRigidBodies();

	void					updateUserParticleData(PxdFluidParticleDataDesc& srcParticles, PxU32 srcNumParticles);
	void					updateUserCreationData(NxU32* srcIds, NxU32 srcIdsByteStride, NxU32 srcNumIds, bool appendIds);
	void					updateUserDeletionData();
	void					updateUserPacketData();
	void					updateEmitters();

	// The following methods are needed for fluids that run in a compartment
	void addInteractionsWithKnownRbShapes(FluidPacketShape& fluidShape);

private:

	bool					mSimulating;
	bool					mRunsInCompartment; // Specifies whether the fluid runs in a compartment or the primary scene.

	PxdFluid				mLLFluid;

	NxFoundation::NxArraySDK<FluidEmitter*>
							mEmitters;

	NpFluid*				mNpFluid;

	// The following stuff is only needed for fluids that run in compartments
	InteractingShapeHash	mInteractingShapeHash;
	NxFoundation::NxArraySDK<const NvShape*>	mChangedRefCountShapes;
	NxFoundation::NxArraySDK<NxI32>				mChangedRefCount;

	IDPool					mParticleIDPool;
	NxU32					mNumIdsOut;
	//all accesses (r/w) to mParticleIDPool, mNumIdsOut need to be locked with that. 
	NxMutex					mParticleIDPoolMutex;

	ParticleBirthDataArray	mParticlesToAdd;
	
	IdArray					mDeletedIds;
	NxU16					mDeletedIdsMark;	// Marker to track the history of deleted particle ids over simulation substeps
												// such that deleted ids are not processed more than once.
	
	IdArray					mCreatedIds;

	FluidIdToIndexMap*		mIdToIndexMap;

	ParticleUpdateData*     mParticleUpdateBuffer;
	NxU16*					mParticleUpdateFlagBuffer;	// Buffer to set update status for each particle
														// (including deletion). Designed such that it can be passed
														// to low level easily.
	NxFoundation::NxArraySDK<PxdFluidParticleUpdateInterval> mParticleUpdateIntervalBuffer;	// Buffer with particle update intervals.
	bool					mParticleUpdate;
	bool					mParticleUpdateInitialized;

	bool					mRemoveAllParticles;
	bool					mUserDataNeedsUpdate;

	NxBounds3				mBounds;
	NxBounds3				mBoundsAPI; // buffered API data
	NxFluidPacket*			mFluidPackets;
	NxU32					mNumPackets;
	NxU16*					mPacketHashToData;

	int						mID;

	NxU32					mMaxParticles;
	NxU32					mNumReserveParticles;
	NxU32					mCurrentParticleLimit;

	NxU32					mFlags;
	NxCollisionGroup		mCollisionGroup;
	NxGroupsMask			mGroupsMask;
	NxForceFieldMaterial	mForceFieldMaterial;
	
	/* some simulation parameters not stored in the FluidControllerPPU */
	NxReal					mRestParticlesPerMeter;
	NxReal					mKernelRadiusMultiplier;
	NxReal					mMotionLimitMultiplier;
	NxReal					mCollisionDistanceMultiplier;
	NxReal					mCollisionRange;
	NxReal					mRestParticleDistance;
	NxReal					mCellSize;
	NxVec3					mExternalAcceleration;
	NxReal					mCollisionResponseCoefficient;

	// The following data members store user provided coefficients which might be adjusted internally
	NxReal					mUserStaticCollisionRestitution;
	NxReal					mUserStaticCollisionAdhesion;
	NxReal					mUserStaticCollisionStiction;
	NxReal					mUserDynamicCollisionRestitution;
	NxReal					mUserDynamicCollisionAdhesion;
	NxReal					mUserDynamicCollisionStiction;

	NxU32					mSimulationMethod;
	NxU32					mCollisionMethod;
	
	NxU32					mTotalStepsSimulated;

	NxU8					mMixedModeAlternator;

	/* user buffer */
	NxParticleData			mParticleWriteData;
	NxParticleIdData		mDeletedIdWriteData;
	NxParticleIdData		mCreatedIdWriteData;
	NxFluidPacketData		mFluidPacketData;

	// Array of fluid packet shapes
	FluidShapeArray			mFluidPacketShapes;

	// Keep track of deleted/created fluid shapes (for HSM)
	NxU32					mCreatedShapeCount;
	NxU32					mDestroyedShapeCount;
	NxFoundation::NxArraySDK<NxBounds3>		mChangedShapeBounds;
	NxFoundation::NxArraySDK<void*>	mChangedShapes;

	// Particle counters
	NxU16					mOldParticleCount;
	NxU16					mCreatedParticleCount;

	FluidParticleQueue*		mParticleQueue;
	NxReal					mClock;

	// MS: This is ugly but as long as we have to keep the hardware fluids in HL we must support the NvFluidParticle data
	//     structure as well (fluid-cloth collision and force fields rely on it)
	bool					mParticlesExternUpToDate;				// Marks whether the particles have been copied
																	// to a buffer for external access
	NxFoundation::NxArraySDK<NvFluidParticle>	mParticlesExtern;	// Used for external access to particles.

	////////////////////////////////////////////////////////

	/*!
	Compute the fluid's unit response multiplier for a given force mode.
	The unit multiplier will be used to interpret the forces of the particle update data structure correctly.
	*/
	NxReal					getUpdateParticlesUnitMult(NxForceMode forceMode) const;
	void					updateParticlesInit();
	void					updateParticlesInit(NxU32 offset, NxU32 number);
	void					updateParticles(const NxParticleUpdateData& ud, NxU32 dstParticleOffset, NxU32 dstParticleNumber);
	void					updateParticlesByIds(const NxParticleUpdateData& ud);
	
	void					initializeParticle(ParticleBirthData* p, NxU32 numParticles, bool setIds);
	NxU16					collectEmitterParticles(NxReal dt);		// Let the emitters create particles and add them to
																	// the particle insertion buffer
	NxU16					flushNewParticlesBuffer();		// Pass new particles to low level and clear particles insertion buffer.
	void					performParticleUpdate(NxReal deathTime);	// Mark old particles for kill, process and pass particle update data to low level.
	
	///////////////////////////

	void				updateWorldBounds();

public:

	void				writeParticleLifetime(NxF32& lifetime, NxF32 deathTime);
	void				writeParticleDeathTime(NxF32& deathTime, NxF32 lifeTime);

	NX_INLINE void		unlinkFluidShape(FluidPacketShape* fluidShape);

private:

	NX_INLINE void		addDeletedParicleId(NxU16 id) { NX_ASSERT(id < getMaxParticles()); mDeletedIds.pushBack(id); }
	NX_INLINE NxU16		getNewParticleID();
	NX_INLINE void		freeParticleID(NxU16 id);
	void				removeAllParticlesInternal();

	// Fluid packet handling
	NX_INLINE void		addFluidPacket(PxdShape llFluidShape);
	NX_INLINE void		removeFluidPacket(PxdShape llFluidShape);
	NX_INLINE void      packetToAABB(const NxI16Vec3& coord, NxVec3& origin, NxVec3& extent);

	// Low level fluid creation
	PxdFluid createLLFluid(const NvFluidDesc& desc);

	// Conversion methods for low level parameters
	NX_INLINE PxdFluidSimulationMethod	convertSimMethodToLL(NxU32 simMethod) const;
	NX_INLINE NxU32						convertSimMethodFromLL(PxdFluidSimulationMethod simMethod) const;

	NxReal getSelfDensity()
	{
		PxdFluidSimulationMethod simMethod = static_cast<PxdFluidSimulationMethod>(PxdFluidGetInt(mLLFluid, PXD_FLUID_SIMULATION_METHOD));
		if (simMethod == PXD_F_NO_PARTICLE_INTERACTION)
			return 0.0f;
		else
			return PxdFluidGetFloat(mLLFluid, PXD_FLUID_SELF_DENSITY);
	}

	NxReal getMaxVelocity()
	{
		if (getScene().getSubStepSize() == 0.0f)
			return 0.0f;

		return getMaxMotionDistance() / getScene().getSubStepSize();
	}

//////////////////////////////////////////////////////////////

	void	visualizeDebugParticles(NxFoundation::DebugRenderable & dr);
	void	visualizePacketHash(NxFoundation::DebugRenderable & dr);
	void	visualizeDynCollision(NxFoundation::DebugRenderable & dr);
	void	visualizeRawTriMesh(NxFoundation::DebugRenderable & dr);
	void	visualizeMeshPackets(NxFoundation::DebugRenderable & dr);
	void	visualizeFluidPacketData(NxFoundation::DebugRenderable & dr);

	//void	visualizeInteractions(NxFoundation::DebugRenderable & dr);	// MS: Might be helpful for debugging

	// DEBUG Rendering
	NxFoundation::DebugRenderable mDebugRenderable;

public:
	NxFoundation::DebugRenderable&
		getDebugRenderable() { return mDebugRenderable; }

};


NX_INLINE PxdTask Fluid::startFluidShapesUpdate()
{
	return PxdFluidShapesUpdate(mLLFluid);
}

NX_INLINE PxdTask Fluid::startDynamicsUpdate()
{
	return PxdFluidDynamicsUpdate(mLLFluid);
}

NX_INLINE PxdTask Fluid::startCollisionUpdate()
{
	return PxdFluidCollisionUpdate(mLLFluid);
}

NX_INLINE void Fluid::unlinkFluidShape(FluidPacketShape* fluidShape)
{
	// Remove the specified fluid shape from the shape list

	NX_ASSERT(fluidShape);
	NX_ASSERT(mFluidPacketShapes.size() > 0);

	NxU32 index = fluidShape->getIndex();
	NX_ASSERT(mFluidPacketShapes[index] == fluidShape);
	
	mFluidPacketShapes.replaceWithLast(index);

	if (mFluidPacketShapes.size() > 0)
		mFluidPacketShapes[index]->setIndex(index);	// Adjust index of swapped shape.
}

// MS: We could remove this method (as well as the related methods getCollisionRange(), ...)
//     since the LowLevel fluid shapes already add the collision range margin to the packet
//     bouding box.
NX_INLINE void Fluid::packetToAABB(const NxI16Vec3& coord, NxVec3& origin, NxVec3& extent)
{
	NxReal packetSize = getPacketSize();
	extent = 0.5f * NxVec3(packetSize);

	origin.x = static_cast<NxReal>(coord.x);
	origin.y = static_cast<NxReal>(coord.y);
	origin.z = static_cast<NxReal>(coord.z);

	origin *= packetSize;
	origin += extent;

	extent += NxVec3(getCollisionRange());	// Enlarge the bounding box such that a particle on the boundary could
											// travel the maximum distance and would still be inside the enlarged volume.
}

NX_INLINE PxdFluidSimulationMethod Fluid::convertSimMethodToLL(NxU32 simMethod) const
{
	PxdFluidSimulationMethod llSimMethod = PXD_F_SPH;

	if (simMethod == NX_F_MIXED_MODE)
	{
		if (mMixedModeAlternator == 0)
			llSimMethod = PXD_F_SPH;
		else
			llSimMethod = PXD_F_NO_PARTICLE_INTERACTION;
	}
	else
	{
		if (simMethod == NX_F_SPH)
			llSimMethod = PXD_F_SPH;
		else if (simMethod == NX_F_NO_PARTICLE_INTERACTION)
			llSimMethod = PXD_F_NO_PARTICLE_INTERACTION;
	}
	
	return llSimMethod;
}

NX_INLINE NxU32	Fluid::convertSimMethodFromLL(PxdFluidSimulationMethod simMethod) const
{
	if (simMethod == PXD_F_SPH)
		return NX_F_SPH;
	else if (simMethod == PXD_F_NO_PARTICLE_INTERACTION)
		return NX_F_NO_PARTICLE_INTERACTION;
	else
	{
		NX_ASSERT(0);
		return 0;
	}
}


#endif	// NX_USE_FLUID_API

#endif

//NVIDIACOPYRIGHTBEGIN
///////////////////////////////////////////////////////////////////////////
// Copyright ?2010 NVIDIA Corporation
// All rights reserved. www.nvidia.com
///////////////////////////////////////////////////////////////////////////
//NVIDIACOPYRIGHTEND

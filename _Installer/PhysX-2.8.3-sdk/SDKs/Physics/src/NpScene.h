#ifndef NX_PHYSICS_NP_SCENE
#define NX_PHYSICS_NP_SCENE
/*----------------------------------------------------------------------------*\
|
|						Public Interface to NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/
#include "NxScene.h"
#include "Allocateable.h"
#include "ReadWriteMutex.h"
#include "NxSync.h"
#include "IDPool.h"
#include "DebugRenderable.h"
#include "NpDefaultScheduler.h"
#include "HardwareAbstraction.h"
#include "NvScene.h"
#include "NvVRDReporter.h"
#include "SceneStats2.h"
#include "ForceFieldManager.h"
#include "Array.h"
#include "NxSceneEvent.h"

#if defined(WIN32)
#define MULTI_THREADED_FF 1
#endif

#define GATHER_SIMULATION_TIMES 0
#if GATHER_SIMULATION_TIMES
#include "CallTimer.h"
#endif // GATHER_SIMULATION_TIMES

class PhysicsThread;
class NxSync;
class NpActor;
class NvJoint;
class NvSpringAndDamperEffector;
class NvJointBreakEvent;
class NvFluidEmitterEvent;
class NvFluidEvent;
class NpInternalThread;
class NxForceField;
class NxForceFieldDesc;

class NpFluid;
class NpCloth;
class NpClothMesh;
class NpSoftBody;
class NpSoftBodyMesh;
class NpForceField;
class NpForceFieldKernel;
class NxForceFieldKernel;
class NpForceFieldShapeGroup;
#if MULTI_THREADED_FF
class NpForceFieldThreading;
#endif

class NpScene;
class NpSceneStats
	{
	public:
	NxU32   numActors;
	NxU32	maxActors;
	NxU32   numDynamicActors;
	NxU32	maxDynamicActors;
	NxU32	numJoints;
	NxU32	maxJoints;

	NpSceneStats(NpScene* scene = NULL);
	~NpSceneStats();

	void incActors();
	void decActors();
	void incBodies();
	void decBodies();
	void incJoints();
	void decJoints();

	void readOut(NxSceneStats & dest) const;
	NX_INLINE void setScene(NpScene* s) { m_scene = s; }
	void sendToVRD(bool create);
	void removeFromVRD();
	private:
		NpScene* m_scene;
	};

#ifdef NX_ENABLE_SCENE_STATS2
class NpSceneStats2
{
public:
	NxFoundation::NxArraySDK<NxSceneStatistic> stats;

	NpSceneStats2(NpScene* scene = NULL);

	void incStat(SceneStatisticEnum);
	void decStat(SceneStatisticEnum);
	void addToStat(SceneStatisticEnum, NxI32);
	void setStat(SceneStatisticEnum id, NxI32 number);

	void readOut(NxSceneStats2 & dest) const;
	NX_INLINE void setScene(NpScene* s) { m_scene = s; }
private:
	NpScene* m_scene;
};
#endif

enum NpProfileZones
{
	NpScene_checkResults,
	NpScene_reportContacts,
	NpScene_reportProfiling,
	NpScene_reportTriggers,
	NpScene_finishVRDFrame,
	NpScene_submitStats2,
	NpScene_stats,

	NpScene_forceFieldUpdate,
	NpForceField_updateBounds,
	NpForceField_applyForces,
	NpForceField_prepGroups,
	NpForceField_group,
	NpForceField_taskEval,
	NpForceField_taskUpdate,

	NpPrNumZones
};

struct KernelPair
{
	NxForceFieldKernel* orig;
	NxForceFieldKernel* clone;
};

class NpScene : public NxScene, public NxFoundation::NxAllocateable, public ReadWriteMutex, public NvVRDReporter, public NxProfileData
	{
	//virtual interfaces:
	public:

	virtual		bool						saveToDesc(NxSceneDesc& desc)	const;
	virtual		NxU32						getFlags() const;

	// implement NxScene:
	// Gravity
	virtual		void						setGravity(const NxVec3&);
	virtual		void						getGravity(NxVec3&);
	// Factory
	virtual		NxActor*					createActor(const NxActorDescBase&);
	virtual		void						releaseActor(NxActor&);
	virtual		NxJoint*					createJoint(const NxJointDesc&);
	virtual		void						releaseJoint(NxJoint&);
	virtual		NxEffector*					createEffector(const NxEffectorDesc& desc); 
	virtual		NxSpringAndDamperEffector*	createSpringAndDamperEffector(const NxSpringAndDamperEffectorDesc&);
	virtual		void						releaseEffector(NxEffector&);
	virtual		NxCompartment *				createCompartment(const NxCompartmentDesc&);
	virtual		NxU32						getNbCompartments() const;
	virtual		NxU32						getCompartmentArray(NxCompartment ** userBuffer, NxU32 bufferSize, NxU32 & usersIterator) const;
	virtual		NxForceField*				createForceField(const NxForceFieldDesc& forceFieldDesc);
	virtual		void						releaseForceField(NxForceField& forceField);
	virtual		NxU32						getNbForceFields()		const;
	virtual		NxForceField**				getForceFields();
	virtual		NxForceFieldLinearKernel*	createForceFieldLinearKernel(const NxForceFieldLinearKernelDesc& kernelDesc);
	virtual		void						releaseForceFieldLinearKernel(NxForceFieldLinearKernel& kernel);
	virtual		NxU32						getNbForceFieldLinearKernels() const; 
	virtual		void						resetForceFieldLinearKernelsIterator(); 
	virtual		NxForceFieldLinearKernel*	getNextForceFieldLinearKernel(); 
	virtual		NxForceFieldShapeGroup*		createForceFieldShapeGroup(const NxForceFieldShapeGroupDesc& desc);
	virtual		void						releaseForceFieldShapeGroup(NxForceFieldShapeGroup &);
	virtual		NxU32						getNbForceFieldShapeGroups() const;	
	virtual		void						resetForceFieldShapeGroupsIterator();
	virtual		NxForceFieldShapeGroup*		getNextForceFieldShapeGroup();	
	virtual		NxForceFieldVariety			createForceFieldVariety();
	virtual		NxForceFieldVariety			getHighestForceFieldVariety() const;
	virtual		void						releaseForceFieldVariety(NxForceFieldVariety var);
	virtual		NxForceFieldMaterial		createForceFieldMaterial();
	virtual		NxForceFieldMaterial		getHighestForceFieldMaterial() const;
	virtual		void						releaseForceFieldMaterial(NxForceFieldMaterial mat);
	virtual		NxReal						getForceFieldScale(NxForceFieldVariety var, NxForceFieldMaterial mat);
	virtual		void						setForceFieldScale(NxForceFieldVariety var, NxForceFieldMaterial mat, NxReal val);

	// Pairs
	virtual		void						setActorPairFlags(NxActor&, NxActor&, NxU32 nxContactPairFlag);
	virtual		NxU32						getActorPairFlags(NxActor&, NxActor&) const;
	virtual		void						setShapePairFlags(NxShape&, NxShape&, NxU32 nxContactPairFlag);
	virtual		NxU32						getShapePairFlags(NxShape&, NxShape&) const;
	virtual		NxU32						getNbPairs() const;
	virtual		NxU32						getPairFlagArray(NxPairFlag* userArray, NxU32 numPairs) const;

	// Groups
	virtual		void						setGroupCollisionFlag(NxCollisionGroup group1, NxCollisionGroup group2, bool enable);
	virtual		bool						getGroupCollisionFlag(NxCollisionGroup group1, NxCollisionGroup group2) const;

	virtual		void						setDominanceGroupPair(NxDominanceGroup group1, NxDominanceGroup group2, NxConstraintDominance & dominance);
	virtual		NxConstraintDominance		getDominanceGroupPair(NxDominanceGroup group1, NxDominanceGroup group2) const;

	virtual		void						setActorGroupPairFlags(NxActorGroup group1, NxActorGroup group2, NxU32 flags);
	virtual		NxU32 						getActorGroupPairFlags(NxActorGroup group1, NxActorGroup group2) const;
	virtual		NxU32						getNbActorGroupPairs() const;
	virtual		NxU32						getActorGroupPairArray(NxActorGroupPair * userBuffer, NxU32 bufferSize, NxU32 & userIterator) const;

	virtual		void						setFilterOps(NxFilterOp op0, NxFilterOp op1, NxFilterOp op2);
	virtual		void						setFilterBool(bool flag);
	virtual		void						setFilterConstant0(const NxGroupsMask& mask);
	virtual		void						setFilterConstant1(const NxGroupsMask& mask);

	virtual		void						getFilterOps(NxFilterOp& op0, NxFilterOp& op1, NxFilterOp& op2) const;
	virtual		bool 						getFilterBool() const;
	virtual		NxGroupsMask				getFilterConstant0() const;
	virtual		NxGroupsMask				getFilterConstant1() const;

	// Actors
	virtual		NxU32						getNbActors()			const;
	virtual		NxActor**					getActors();
//	virtual		NxU32						getActorArray(NxActor ** userBuffer, NxU32 bufferSize, NxU32 & usersIterator);
	virtual		NxActiveTransform*			getActiveTransforms(NxU32 &nbTransformsOut);
	virtual		NxU32						getNbStaticShapes()		const;
	virtual		NxU32						getNbDynamicShapes()	const;
	virtual		NxU32						getTotalNbShapes()		const;
	// Joint and Effector iterators 
	virtual		NxU32						getNbJoints()			const;
	virtual		void						resetJointIterator();
	virtual		NxJoint *					getNextJoint();
	virtual		NxU32						getNbEffectors()		const;
	virtual		void						resetEffectorIterator();
	virtual		NxEffector *				getNextEffector();
	// materials
	virtual		NxMaterial *				createMaterial(const NxMaterialDesc &);
	virtual		void						releaseMaterial(NxMaterial &);
	virtual		NxU32						getNbMaterials() const;
	virtual		NxU32						getMaterialArray(NxMaterial ** userBuffer, NxU32 bufferSize, NxU32 & usersIterator);
	virtual		NxMaterialIndex				getHighestMaterialIndex() const;
	virtual		NxMaterial *				getMaterialFromIndex(NxMaterialIndex);


	// Run
	virtual		void						flushStream();
	virtual		void						setTiming(NxReal _maxTimestep, NxU32 _maxIter, NxTimeStepMethod _method);
	virtual		void						getTiming(NxReal & _maxTimestep, NxU32 & _maxIter, NxTimeStepMethod & _method, NxU32* numSubSteps) const;
	virtual		void						getStats(NxSceneStats & s) const;
#ifdef NX_ENABLE_SCENE_STATS2
	virtual		NxSceneStats2				*getStats2() const;
#endif
	virtual		void						getLimits(NxSceneLimits& limits) const;

	// User-notify
	virtual		void						setUserNotify(NxUserNotify* callback);
	virtual		NxUserNotify*				getUserNotify() const;
	// Fluid user-notify
#if NX_USE_FLUID_API
	virtual		void						setFluidUserNotify(NxFluidUserNotify* callback);
	virtual		NxFluidUserNotify*			getFluidUserNotify() const;
#endif
#if NX_USE_CLOTH_API
	virtual		void						setClothUserNotify(NxClothUserNotify* callback);
	virtual		NxClothUserNotify*			getClothUserNotify() const;
#endif
#if NX_USE_SOFTBODY_API
	virtual		void						setSoftBodyUserNotify(NxSoftBodyUserNotify* callback);
	virtual		NxSoftBodyUserNotify*		getSoftBodyUserNotify() const;
#endif
	// User contact modification callback
	virtual		void						setUserContactModify(NxUserContactModify* callback);
	virtual		NxUserContactModify*		getUserContactModify() const;
	// User trigger report
	virtual		void						setUserTriggerReport(NxUserTriggerReport* callback);
	virtual		NxUserTriggerReport*		getUserTriggerReport() const;
	// User contact report
	virtual		void						setUserContactReport(NxUserContactReport* callback);
	virtual		NxUserContactReport*		getUserContactReport() const;
	// User contact filtering
	virtual		void						setUserActorPairFiltering(NxUserActorPairFiltering* callback);
	virtual		NxUserActorPairFiltering*	getUserActorPairFiltering() const;

	// Get NxSceneDesc information to import to VRD
	virtual		NxU32						getSubdivisionLevel() const;
	virtual		NxU32						getStaticStructure() const;
	virtual		NxU32						getDynamicStructure() const;
	virtual		NxU32						getUpAxis() const;
	virtual		NxU32						getBackgroundThreadCount() const;
	virtual		NxU32						getThreadMask() const;
	virtual		NxU32						getInternalThreadCount() const;

	// Get Physics SDK
	virtual		NxPhysicsSDK&				getPhysicsSDK();

	// Raycasting
	virtual		bool						raycastAnyBounds		(const NxRay& worldRay, NxShapesType shapesType, NxU32 groups=0xffffffff, NxReal maxDist=NX_MAX_F32, const NxGroupsMask* groupsMask=NULL) const;
	virtual		bool						raycastAnyShape			(const NxRay& worldRay, NxShapesType shapesType, NxU32 groups=0xffffffff, NxReal maxDist=NX_MAX_F32, const NxGroupsMask* groupsMask=NULL, NxShape** cache=NULL) const;
	virtual		NxU32						raycastAllBounds		(const NxRay& worldRay, NxUserRaycastReport& report, NxShapesType shapesType, NxU32 groups=0xffffffff, NxReal maxDist=NX_MAX_F32, NxU32 hintFlags=0xffffffff, const NxGroupsMask* groupsMask=NULL) const;
	virtual		NxU32						raycastAllShapes		(const NxRay& worldRay, NxUserRaycastReport& report, NxShapesType shapesType, NxU32 groups=0xffffffff, NxReal maxDist=NX_MAX_F32, NxU32 hintFlags=0xffffffff, const NxGroupsMask* groupsMask=NULL) const;
	virtual		NxShape*					raycastClosestBounds	(const NxRay& worldRay, NxShapesType shapeType, NxRaycastHit& hit, NxU32 groups=0xffffffff, NxReal maxDist=NX_MAX_F32, NxU32 hintFlags=0xffffffff, const NxGroupsMask* groupsMask=NULL) const;
	virtual		NxShape*					raycastClosestShape		(const NxRay& worldRay, NxShapesType shapeType, NxRaycastHit& hit, NxU32 groups=0xffffffff, NxReal maxDist=NX_MAX_F32, NxU32 hintFlags=0xffffffff, const NxGroupsMask* groupsMask=NULL, NxShape** cache=NULL) const;

	// Collision detection
	virtual		NxU32						overlapAABBTriangles	(const NxBounds3& worldBounds, NxFoundation::NxArraySDK<NxTriangle>& worldTriangles);

	virtual		NxU32						overlapSphereShapes		(const NxSphere& worldSphere, NxShapesType shapeType, NxU32 nbShapes, NxShape** shapes, NxUserEntityReport<NxShape*>* callback, NxU32 activeGroups=0xffffffff, const NxGroupsMask* groupsMask=NULL, bool accurate_collision=false);
	virtual		NxU32						overlapAABBShapes		(const NxBounds3& worldBounds, NxShapesType shapeType, NxU32 nbShapes, NxShape** shapes, NxUserEntityReport<NxShape*>* callback, NxU32 activeGroups=0xffffffff, const NxGroupsMask* groupsMask=NULL, bool accurate_collision=false);
	virtual		NxU32						overlapOBBShapes		(const NxBox& worldBox, NxShapesType shapeType, NxU32 nbShapes, NxShape** shapes, NxUserEntityReport<NxShape*>* callback, NxU32 activeGroups=0xffffffff, const NxGroupsMask* groupsMask=NULL, bool accurate_collision=false);
	virtual		NxU32						overlapCapsuleShapes	(const NxCapsule& worldCapsule, NxShapesType shapeType, NxU32 nbShapes, NxShape** shapes, NxUserEntityReport<NxShape*>* callback, NxU32 activeGroups=0xffffffff, const NxGroupsMask* groupsMask=NULL, bool accurate_collision=false);
	virtual		NxU32						cullShapes				(NxU32 nbPlanes, const NxPlane* worldPlanes, NxShapesType shapeType, NxU32 nbShapes, NxShape** shapes, NxUserEntityReport<NxShape*>* callback, NxU32 activeGroups=0xffffffff, const NxGroupsMask* groupsMask=NULL);
	virtual		bool						checkOverlapSphere		(const NxSphere& worldSphere, NxShapesType shapeType, NxU32 activeGroups=0xffffffff, const NxGroupsMask* groupsMask=NULL);
	virtual		bool						checkOverlapAABB		(const NxBounds3& worldBounds, NxShapesType shapeType, NxU32 activeGroups=0xffffffff, const NxGroupsMask* groupsMask=NULL);
	virtual		bool						checkOverlapOBB			(const NxBox& worldBox, NxShapesType shapeType, NxU32 activeGroups=0xffffffff, const NxGroupsMask* groupsMask=NULL);
	virtual		bool						checkOverlapCapsule		(const NxCapsule& worldCapsule, NxShapesType shapeType, NxU32 activeGroups=0xffffffff, const NxGroupsMask* groupsMask=NULL);
#if NX_SUPPORT_SWEEP_API
	virtual		NxSweepCache*				createSweepCache();
    virtual		void						releaseSweepCache(NxSweepCache* cache);

	virtual		NxU32						linearOBBSweep			(const NxBox& worldBox, const NxVec3& motion, NxU32 flags, void* userData, NxU32 nbShapes, NxSweepQueryHit* shapes, NxUserEntityReport<NxSweepQueryHit>* callback, NxU32 activeGroups=0xffffffff, const NxGroupsMask* groupsMask=NULL);
	virtual		NxU32						linearCapsuleSweep		(const NxCapsule& worldCapsule, const NxVec3& motion, NxU32 flags, void* userData, NxU32 nbShapes, NxSweepQueryHit* shapes, NxUserEntityReport<NxSweepQueryHit>* callback, NxU32 activeGroups=0xffffffff, const NxGroupsMask* groupsMask=NULL);
#endif

	// new API methods
	virtual		bool						isWritable();
	virtual		void						simulate(NxReal _elapsedTime);
	virtual		bool						checkResults(NxSimulationStatus, bool block = false);
	virtual		bool						fetchResults(NxSimulationStatus, bool block = false, NxU32 *errorState = 0);
	virtual		const NxDebugRenderable *	getDebugRenderable();
	virtual		const NxProfileData *		readProfileData(bool clearData);

	virtual		NxSimulationType			getSimType() const;		
	virtual		void *						getInternal();

	virtual		void						flushCaches();

	// Threading interface methods
	virtual		NxThreadPollResult			pollForWork(NxThreadWait waitType);
	virtual		NxThreadPollResult			pollForBackgroundWork(NxThreadWait waitType);
	virtual		void						resetPollForWork();
	virtual		void						shutdownWorkerThreads();

	virtual		void						lockQueries();
	virtual		void						unlockQueries();

	// BATCHED_RAYCASTS
	virtual		NxSceneQuery*				createSceneQuery(const NxSceneQueryDesc&);
	virtual		bool						releaseSceneQuery(NxSceneQuery&);
	//~ BATCHED_RAYCASTS

	virtual		void						setDynamicTreeRebuildRateHint(NxU32 dynamicTreeRebuildRateHint);
	virtual		NxU32						getDynamicTreeRebuildRateHint() const;

	virtual		void						setSolverBatchSize(NxU32 solverBatchSize);
	virtual		NxU32						getSolverBatchSize(void) const;

	virtual		NxU32						getBoundForIslandSize(NxActor& actor);
	virtual		NxU32						getIslandArrayFromActor(NxActor& actor, NxActor** userBuffer, NxU32 bufferSize, NxU32& userIterator);

	// fluids
#if NX_USE_FLUID_API
	virtual		NxFluid*					createFluid(const NxFluidDescBase&);
	virtual		void						releaseFluid(NxFluid&);
	virtual		NxU32						getNbFluids()		const;
	virtual		NxFluid**					getFluids();
	virtual		bool						cookFluidMeshHotspot(const NxBounds3& bounds, NxU32 packetSizeMultiplier, NxReal restParticlesPerMeter, NxReal kernelRadiusMultiplier, NxReal motionLimitMultiplier, NxReal collisionDistanceMultiplier, NxCompartment* compartment, bool forceStrictCookingFormat);
#endif

	// cloths
#if NX_USE_CLOTH_API
	virtual		NxCloth*					createCloth(const NxClothDesc&);
	virtual		void						releaseCloth(NxCloth&);
	virtual		NxU32						getNbCloths() const;
	virtual		NxCloth**					getCloths();
#endif

	// soft bodies
#if NX_USE_SOFTBODY_API
	virtual		NxSoftBody*					createSoftBody(const NxSoftBodyDesc&);
	virtual		void						releaseSoftBody(NxSoftBody&);
	virtual		NxU32						getNbSoftBodies() const;
	virtual		NxSoftBody**				getSoftBodies();
#endif

	NX_INLINE	bool						isPhysicsRunning()	const	{ BEGINREAD return mPhysicsRunning; }

	NX_INLINE	NxFoundation::NxArraySDK<NxU32>& getOverlapAABBTrianglesBuffer() { return overlapAABBTrianglesBuffer;}
	// Debugger reporter
				void						reportContacts(NvVRDContact *c, NxU32 count);
				void						reportProfiling(NxProfilerData *profileData);
				void						reportTriggers(NvVRDTrigger *t, NxU32 count);

	//implement NxProfileData
	virtual const NxProfileZone *			getNamedZone(NxProfileZoneName) const;

	virtual void							setMaxCPUForLoadBalancing(NxReal cpuFraction);
	virtual NxReal							getMaxCPUForLoadBalancing();
#ifdef RRB_SUPPORTED_PLATFORM
	virtual NxArray<NxSceneEvent> *			createSceneEventStream();
	virtual void							releaseSceneEventStream(NxArray<NxSceneEvent> * sceneEventStream);
	virtual void							eventStreamsSend(const NxSceneEvent & sceneEvent);
#endif
	//internal public methods:
	public:
											NpScene();
											~NpScene();
				void						setScene(NvScene* scene);
	NX_INLINE	NvScene*					getScene()					{ return mScene;		}
	NX_INLINE	ForceFieldManager*			getForceFieldManager()		{ return &forceFieldManager; }

		static	void						computeTiming(NxReal elapsedTime, NxReal maxTimestep, NxU32 maxIter, NxTimeStepMethod timeStepMethod, NxReal & subStepSize, NxU32 & numSubSteps, NxReal & remain);
				//void						setSubHWFluidScene(NvScene* subScene);
				//NvScene*					getSubHWFluidScene();
				void						executeScene();
				// MSS HACK EARLY BROADPHASE UPDATE
				void						earlyUpdateBroadphase();
				void						jointBreakEventNotify(const NvJointBreakEvent*);
				void						fluidEmitterNotify(const NvFluidEmitterEvent*);
				void						fluidNotify(const NvFluidEvent*);
	NX_INLINE	NxU32						getSimulateCounter()		{ return simulateCounter; }

	// Creation
				bool						loadFromDesc(const NxSceneDesc&);
	// Actors
//				NxU32						getNbActors()		const	{ return npActorArray.size(); }
				NpActor**					getNpActors()				{ return npActorArray.begin();}
				void						purgeActors();
				bool						containsActor(NxActor* actor);
				//void						removeFrameActors(NxActor* npActor);

	NX_INLINE	NpDefaultScheduler*			getDefaultScheduler()		{ return &defaultScheduler;	}
	NX_INLINE	NxUserScheduler*			getCurrentScheduler()
											{
											if(mDescSave.customScheduler!=NULL)
												return mDescSave.customScheduler;
											else
												return getDefaultScheduler();
											}
	NX_INLINE	NxU32						getSimStackSize()			const	{ return mDescSave.simThreadStackSize;		}
	NX_INLINE	NxU32						getWorkerStackSize()		const	{ return mDescSave.workerThreadStackSize;	}
	NX_INLINE	NxU32						getFlagsFast()				const	{ return mDescSave.flags;					}

	NX_INLINE	NxU32						getNbBackgroundThreads()	const	{ return backgroundThreads.size();			}

#if NX_USE_FLUID_API
				NpFluid**					getNpFluids()				{ return npFluidArray.begin();}
#endif
				typedef		NxFoundation::NxArraySDK<NxReal>					NpForceFieldMaterialRow;
				typedef		NxFoundation::NxArraySDK<NpForceFieldMaterialRow*>	NpForceFieldMaterialTable;
				NxForceFieldKernel*			getForceFieldKernelClone(NxForceFieldKernel* orig);
				void						onReleaseForceFieldKernel(NxForceFieldKernel* orig);
				NpForceFieldMaterialRow*	getForceFieldMaterialRow(NxForceFieldVariety ffid) { return mForceFieldMaterialTable[ffid]; }
				//SDS: These seem to be needed for NpFluid::loadFromDesc()
				//SDS: These seem to be needed for NpFluid::loadFromDesc()
				NvFluid*					createFluidInternal(NxCompartment * c, const NvFluidDesc& desc);
				void						releaseFluidInternal(NvFluid* nvFluid);
				
				NxU32						getNewActorId();
				void						freeActorId(NxU32);

				void						sendSceneToVRD();
				void						removeSceneFromVRD();
				NxFoundation::ProfilerManager* getProfilerManager();
private:
				void						finishVRDFrame();
				void						sendToVRD(bool create, bool fromCtor);
				void						removeFromVRD();
				NxU8*						getVRDBuffer(NxU32 size);
				void						visualize();
				void                        fireSleepCallbacks();
				void						buildActiveTransforms();
				void						updateForceFieldKernelClones();

				void						createSceneThread(NxU32 stackSize, NxThreadPriority priority);
				void						destroySceneThread();

				void						createInternalThreads(NxU32 internalThreadCount,NxU32 backgroundThreadCount,
																  NxU32 stackSize, NxThreadPriority priority, NxThreadPriority bktPriority);
				void						destroyInternalThreads();
#ifdef NX_ENABLE_SCENE_STATS2
				void						sendSceneStats2ToVRD(bool create);
#ifdef AG_PERFMON 
                void                        submitStats2();
#endif //AG_PERFMON 
#endif // NX_ENABLE_SCENE_STATS2
				
public:

	typedef		NxFoundation::NxArraySDK<NpActor*>		NpActorArray;

				NpSceneStats				stats;
#ifdef NX_ENABLE_SCENE_STATS2
				NpSceneStats2				stats2;
#ifdef AG_PERFMON
                mutable NxU32*              mStatEventIDs; //Event IDs for submitting stats to AgPerfMon
				mutable NxU32*				mSubsetStatIDs; //Event IDs for a select set of stats
				mutable NxU32				mNumSubsetStats;
#endif //AG_PERFMON
#endif
				HardwareAbstraction			hardwareAbstraction;
private:
				ForceFieldManager			forceFieldManager;
				NpActorArray				npActorArray;	//  for NpScene::getActors()
				NvJoint*					userJointIterator;
				NvSpringAndDamperEffector*	userEffectorIterator;
				NxFoundation::DebugRenderable	debugVisualization;
				NxDebugRenderable			debugRenderable;
//				NxFoundation::Profiler		profiler;

#if NX_USE_FLUID_API
	typedef		NxFoundation::NxArraySDK<NpFluid*>		NpFluidArray;
				NpFluidArray				npFluidArray;	//  for NpScene::getFluids()
#endif

#if NX_USE_CLOTH_API
	typedef		NxFoundation::NxArraySDK<NpCloth*>		NpClothArray;
				NpClothArray				npClothArray;	//  for NpScene::getCloths()
#endif

#if NX_USE_SOFTBODY_API
	typedef		NxFoundation::NxArraySDK<NpSoftBody*>		NpSoftBodyArray;
				NpSoftBodyArray				npSoftBodyArray;	//  for NpScene::getSoftBodies()
#endif

#ifndef NX_DISABLE_REMOTE_DEBUG
				NxU8*						vrdBuffer;
				NxU32						vrdBufferSize;
#endif

	public:
	NX_INLINE	NxReal						getElapsedTime()	const	{ return elapsedTime;	}
	NX_INLINE	NxReal						getSubstepTime()	const	{ return mSubStepSize;	}
	typedef		NxFoundation::NxArraySDK<NpForceField*>			NpForceFieldArray;
				NpForceFieldArray			npForceFieldArray;
				NxFoundation::NxArraySDK<KernelPair>
											kernelPairs;


	private:
	typedef		NxFoundation::NxArraySDK<NxForceFieldLinearKernel*>	NpForceFieldLinearKernelArray;
	typedef		NxFoundation::NxArraySDK<NpForceFieldShapeGroup*>	NpForceFieldShapeGroups;
				NpForceFieldLinearKernelArray	npForceFieldLinearKernelArray;
				NpForceFieldShapeGroups			npFFShapeGroups;
				NxI32							mFFShapeGroupsIter;
				NxI32							mFFLinearKernelIter;

				IDPool						mFFMaterialIdPool;
				IDPool						mShapeFFMaterialIDPool;
				NpForceFieldMaterialTable	mForceFieldMaterialTable;
#if MULTI_THREADED_FF
				NpForceFieldThreading*		mForceFieldThreading;
#endif

	static		bool						computePlanes(const NxBounds3& bounds, NxPlane* planes);

				NvScene*					mScene;
				//NvScene*					mSubHWFluidScene;

				NxSync						mPhysicsDone;		// physics thread signals this when update ready
				NxSync						mPrimaryDone;		// physics thread signals this when update ready
				NxSync						mPhysicsStart;		// we signal this to start physics update
				PhysicsThread*				mPhysicsThread;		// thread where the physics runs

				NpDefaultScheduler			defaultScheduler;	// Default Scheduler/SDK managed work queue
				NxFoundation::NxArraySDK<NpInternalThread*>	internalThreads;	// Array of SDK created threads
				NxFoundation::NxArraySDK<NpInternalThread*>	backgroundThreads;	// Array of threads for processing background work items
				

#if GATHER_SIMULATION_TIMES
				CallTimer					simulationTimer_;
#endif // GATHER_SIMULATION_TIMES

				bool						mPhysicsRunning;	// is true between simulate() and fetchResults(ALL)
				bool						mPrimaryRunning;	// is true between simulate() and fetchResults(PRIMARY)
				bool						bIteratingSWJoints;	// used for HSM joint iterator.
				NxU32						numMaterials;
				NxU32						simulateCounter;	// incremented with every simulate call.
				NxU32						mNumSubSteps;       // stores number of sub steps in last simulate call
				NxReal						mSubStepSize;		// stores the size of the sub steps in last simulate
				IDPool						actorIdPool;
		//legacy timing settings:
				NxReal						remain;				//Remaining time. Elapsed user time less than a time substep (maxTimeStep) is accumulated here.
				NxReal						elapsedTime;		//needed to transfer the elapsed time param from the user to the sim thread.
				NxReal						accumulatedTime;	// Time past this far. Used by debugger only

				NxSceneDesc					mDescSave;			// copy of our scene description
				NxBounds3					mDescSaveMaxBounds;	// local copy of scene max bounds
	mutable     NxSceneLimits				mDescSaveLimits;	// local copy of scene limits
				bool						mHasLimits;			// did user specify limits?

				NxActor*					pickedActor;		// Remote debugger picking
				NxActor*					pickingAnchor;		// Anchor that picked body is attached to
				NxJoint*					pickingSpring;		// Spring for picking
				NxFoundation::NxArraySDK<const NpShape **>	contactCache;		// Contact list for debugger
				unsigned int				contactCount;		// Contact counter for debugger
				NxFoundation::NxArraySDK<const char*>	profilingList;		// Profiling list for debugger
#ifdef NX_ENABLE_SCENE_STATS2
				NxFoundation::NxArraySDK<NxSceneStatistic>	statCache;		// Stats2 cache for debugger
#endif
#if NX_SUPPORT_SWEEP_API
				typedef		NxFoundation::NxArraySDK<NxSweepCache*>		NpSweepCacheArray;
				NpSweepCacheArray			npSweepCacheArray;	// list of all active sweep cache objects
#endif
				//buffer which is used to emulate the non callback version of overlapAABBTRiangles()
				NxFoundation::NxArraySDK<NxU32> overlapAABBTrianglesBuffer; 

#ifdef NX_ENABLE_PROFILER
				NxFoundation::ProfilerManager	mProfiler;
#endif
#ifdef RRB_SUPPORTED_PLATFORM
				NxArray< NxArray<NxSceneEvent> >	eventStreams;
#endif
	};

#endif

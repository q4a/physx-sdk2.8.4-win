#ifndef PXS_CONTEXT_H
#define PXS_CONTEXT_H

#include "PxnContext.h"
#include "PxnMemory.h"
#include "PxnTask.h"

#include "PxcPool.h"
#include "PxcThreadCoherantCache.h"
#include "PxcProfiler.h"
#include "PxcDebugData.h"

#include "PxsContactCache.h"
#include "PxsSubTask.h"
#include "PxsTask.h"
#include "PxsContactCallback.h"
#include "PxsContactCallbackPatch.h"
#include "PxsContactCallbackQuat.h"
#include "PxsContactManager.h"
#include "PxsShapeTransformHash.h"

#include "PxsFluidConfig.h"
#ifdef PXS_ENABLE_FLUIDS
#include "PxsFluidSpatialHash.h"
#endif

class PxsDynamicsContext;
class PxsShape;
class PxsBodyShape;
#ifdef PXS_ENABLE_FLUIDS
class PxsFluid;
class PxsFluidShape;
class PxsFluidBodyContactManager;
#endif
//class PxsContactManager;

enum PxsProfilerZoneDefs
{
	PXS_PROFILE_ZONE_BP = 0,
	PXS_PROFILE_ZONE_CM,
	
	PXS_PROFILE_ZONE_DYNAMICS,
	PXS_PROFILE_ZONE_ISLANDGEN,

#ifdef PXS_ENABLE_FLUIDS
	PXS_PROFILE_ZONE_FL_PU,		// Fluid packet update
	PXS_PROFILE_ZONE_FL_DYN,	// Fluid dynamics
	PXS_PROFILE_ZONE_FL_COLL,	// Fluid collision
#endif
	
	PXS_PROFILE_ZONE_COUNT
};



class PxsContext : public PxnContext
{
public:
	static				PxsContext*				create(PxU32 index, 
													   PxdBroadPhaseImplementation bp, 
													   bool useFastMassiveBPVolumeDeletion, 
													   bool useImprovedSpringSolver,
													   bool useLegacyJointDrive);

	// Implements PxnContext
	virtual				PxdContextType			getContextType() { return PXD_CPU; }
	virtual				PxnShape*				createShapeV(const PxdShapeDesc& desc);
	virtual				PxnBodyShape*			createBodyShapeV(const PxdShapeDesc& desc);
	virtual				PxnBodyAtom*			createBodyAtomV(const PxdAtomDesc& desc);
	virtual				PxnFluid*				createFluidV(const PxdFluidDesc& desc);
	virtual				PxnVolume*				createVolumeV(const PxdVolumeDesc& desc);
	virtual				PxnContactManager*		createContactManagerV(const PxdManagerDesc& desc);
	virtual				PxnFluidBodyContactManager*	createContactManagerV(const PxdManagerDescFluidBody& desc);
	virtual				PxnCustomConstraint*	createCustomConstraintV(const PxdConstraintDesc& desc);
	virtual				PxnContactConstraint*	createContactConstraintV(const PxdContactConstraintDesc& desc);
	virtual				PxnD6Joint*				createD6JointV(const PxdD6JointDesc& desc);

	virtual				PxnShape*				findShapeV(PxdHandle handle)				const;
	virtual				PxnBodyShape*			findBodyShapeV(PxdHandle handle)			const;
	virtual				PxnFluidShape*			findFluidShapeV(PxdHandle handle)			const;
	virtual				PxnBodyAtom*			findBodyAtomV(PxdHandle handle)				const;
	virtual				PxnFluid*				findFluidV(PxdHandle handle)				const;
	virtual				PxnVolume*				findVolumeV(PxdHandle handle)				const;
	virtual				PxnContactManager*		findContactManagerV(PxdHandle handle)		const;
	virtual				PxnFluidBodyContactManager*	findFluidBodyContactManagerV(PxdHandle handle)	const;
	virtual				PxnCustomConstraint*	findCustomConstraintV(PxdHandle handle)		const;
	virtual				PxnContactConstraint*	findContactConstraintV(PxdHandle handle)	const;
	virtual				PxnD6Joint*				findD6JointV(PxdHandle handle)				const;

	virtual				PxdHandle				getHandleV()								const;
	virtual				void					destroyV();

	// Broad phase related
	virtual				PxnBroadPhase*			getBroadPhaseV();

	// Dynamics properties
	virtual				PxFloat					getBounceThresholdV()						const;
	virtual				void					setBounceThresholdV(PxFloat f);
	virtual				PxInt					getSolverBatchSizeV()						const;
	virtual				void					setSolverBatchSizeV(PxInt i);



	// Task-related
	virtual				PxnTask*				findTaskV(PxdHandle handle)					const;
	virtual				PxdTask					updateBroadPhaseV();
	virtual				PxdTask					updateContactManagerV(PxFloat dt);
	virtual				PxdTask					updateDynamicsV(PxFloat dt);

	virtual				PxUInt					waitTaskAnyV(PxdHandle* tasks, PxUInt taskCount) const;

	virtual				void					setSubTaskCallbackV(PxdUserSubTaskCallback userSubTask, void* userParam);

	// Manager status change
	virtual				bool					getManagerStatusChangeCountV(PxInt* newTouch, PxInt* lostTouch)		const;
	virtual				bool					fillManagerStatusChangeV(PxdContactManagerStatusChange* newTouch, PxdContactManagerStatusChange* lostTouch);

	// body pair reporting
	virtual				PxUInt					getAtomReportPairCountV()					const;
	virtual				bool					fillAtomReportPairBufferV(PxdAtomPair*)		const;

	// Profiling related
	virtual				PxInt					getProfileZoneCountV()						const;
	virtual				void					getProfileZoneDataV(PxdProfileZone* data);
	//~Implements PxnContext

#ifdef PXD_SUPPORT_DEBUG_RENDERER
	virtual				PxcDebugData*			getDebugDataV()								{ return &mDebugData;	}
#endif

#ifdef PXS_ENABLE_FLUIDS
						PxsFluidShape*			createFluidShape(PxsFluid* fluid, const PxsFluidPacket* packet);
#endif

	// Contact manager related
						void					markActive(PxsContactManager* cm);	
						void					markInactive(PxsContactManager* cm);

						void					updateContactManagerMulti(PxFloat dt, PxsTask* task);

#ifdef PXS_ENABLE_FLUIDS
						void					releaseFluid(PxsFluid* fluid);
#endif

						void					releaseShape(PxsShape* shape);
						void					releaseBodyShape(PxsBodyShape* shape);
#ifdef PXS_ENABLE_FLUIDS
						void					releaseFluidShape(PxsFluidShape* shape);
#endif

						void					releaseContactManager(PxsContactManager* cm);
#ifdef PXS_ENABLE_FLUIDS
						void					releaseContactManager(PxsFluidBodyContactManager* cm);
#endif

	PXD_FORCE_INLINE	PxsBodyShape*			findBodyShapeFromIndex(PxU32 index) const
	{
		return mBodyShapePool.findByIndexFast(index);
	}

#ifdef PXS_ENABLE_FLUIDS
	PXD_FORCE_INLINE	void					markDeletedShapesMapForReset() { mClearDeletedShapesMap = true; }
	PXD_FORCE_INLINE	PxcBitMap&				getDeletedShapesMap() { return mDeletedShapesMap; }

	PXD_FORCE_INLINE	void					increaseNbActiveFluids() { mNbActiveFluids++; }
	PXD_FORCE_INLINE	void					decreaseNbActiveFluids() { mNbActiveFluids--; }
	PXD_FORCE_INLINE	PxU32					getNbActiveFluids() { return mNbActiveFluids; }

	PXD_FORCE_INLINE	void					resetNbFluidCollisions() { mNbFluidCollProcessed = 0; }
	PXD_FORCE_INLINE	void					increaseNbFluidCollisions() { mNbFluidCollProcessed++; }
	PXD_FORCE_INLINE	void					decreaseNbFluidCollisions() { mNbFluidCollProcessed--; }
	PXD_FORCE_INLINE	PxU32					getNbFluidCollisions() { return mNbFluidCollProcessed; }

	PXD_FORCE_INLINE	const PxsShapeTransformHash&	getShapeTransformHash() { return mShapeTransformHash; }
	PXD_FORCE_INLINE	void					addShapeTransform(const PxsBodyShape& bodyShape) { mShapeTransformHash.addShape(bodyShape); }
	PXD_FORCE_INLINE	void					removeShapeTransform(const PxsBodyShape& bodyShape) { mShapeTransformHash.removeShape(bodyShape); }
	PXD_FORCE_INLINE	void					updateShapeTransforms() { mShapeTransformHash.update(); }
#endif	// PXS_ENABLE_FLUIDS

	PXD_FORCE_INLINE	PxU32					getIndex()									const	{ return mIndex;						}
	PXD_FORCE_INLINE	PxsDynamicsContext*		getDynamicsContext()						const	{ return mDynamicsContext;				}
	PXD_FORCE_INLINE	PxU32					getDynamicsTimestamp()						const	{ return mDynamicsTimestamp;			}
	PXD_FORCE_INLINE	PxU32					getCMTimestamp()							const	{ return mContactGenerationTimestamp;	}

	PXD_FORCE_INLINE	PxcThreadCoherantCache<PxsContactCacheObject>&		getContactCache()
	{
		return mContactCache;
	}

	PXD_INLINE			PxsContactCacheObject*	getContactCacheObject()
	{
		// We may want to conditional compile to exclude this on single threaded implimentations
		// if it is determined to be a performance hit.
		return mContactCache.get();
	}
	PXD_INLINE			void					putContactCacheObject(PxsContactCacheObject* cacheObject)
	{
		mContactCache.put(cacheObject);
	}

	PXD_INLINE		PxsContactCallbackPatch*	createContactCallbackPatch(PxsContactManager* cm)
	{
		return mContactCallbackPatchPool.construct(cm);
	}

	PXD_INLINE			PxsContactCallbackQuat*	createContactCallbackQuat(PxsContactManager* cm)
	{
		return mContactCallbackQuatPool.construct(cm);
	}

	PXD_INLINE			void					destroyContactCallbackPatch(PxsContactCallbackPatch* cc)
	{
		mContactCallbackPatchPool.destroy(cc);
	}

	PXD_INLINE			void					destroyContactCallbackQuat(PxsContactCallbackQuat* cc)
	{
		mContactCallbackQuatPool.destroy(cc);
	}

	PXD_INLINE			const PxcBitMapIterator	getActiveContactManagerIterator() const
	{
		return PxcBitMapIterator(mActiveContactManager);
	}

	PXD_INLINE		const PxsContactManager*	getContactManager(PxU32 index) const
	{
		return mContactManagerPool.findByIndexFast(index);
	}

	// task functions

	PXD_FORCE_INLINE	PxsTask*				addTask()
	{
		if (mTaskProxies)
			return &(mTaskProxies->addTask());
		else
			return NULL;
	}

	// sub task functions

	PXD_INLINE			void					destroyCMUpdateSubTask(PxsCMUpdateSubTask* subTask)
	{
		mCMUpdatePool.destroy(subTask);
	}

	PXD_FORCE_INLINE	bool					isThreaded()	const
	{
		return mUserSubtaskCallback!=defaultUserSubTask;
	}

	PXD_FORCE_INLINE	void					addSubTask(PxdSubTaskExecuteFunc executeFunc, void* subTaskData)
	{
		mUserSubtaskCallback(PXD_SUBTASK_NORMAL, executeFunc, subTaskData, mUserSubtaskParam);
	}

	PXD_FORCE_INLINE	void					waitSubTasks()	const
	{
		mUserSubtaskCallback(PXD_SUBTASK_WAIT, NULL, NULL, mUserSubtaskParam);
	}


	// Automatic Volume-Shape path stuff

	PXD_FORCE_INLINE	bool					areBoundsValid(PxU32 volumeIndex) const
	{
		return mChangedVolumes.check(volumeIndex);
	}

	PXD_FORCE_INLINE	void					markAllVolumesValid()
	{
		mChangedVolumes.reset();
	}

	PXD_FORCE_INLINE	void					markVolume(PxU32 idx)
	{
		mChangedVolumes.set(idx);
	}

	PXD_FORCE_INLINE	void					unMarkVolume(PxU32 idx)
	{
		mChangedVolumes.clear(idx);
	}

	PXD_FORCE_INLINE	PxcBitMap&				getChangedVolumeMap()
	{
		return mChangedVolumes;
	}

#ifdef PXS_ENABLE_OPT_BP_VOLUME_DELETION
	PXD_FORCE_INLINE	bool					useFastMassiveBPVolumeDeletion() const { return mFastMassiveBPVolumeDeletion; }
#endif

	PXD_FORCE_INLINE	PxcArray<PxsBodyPair>&	getReportPairArray()	{	return mReportPairs; }
	//Profiler
						PxcProfiler				mContextProfiler;

private:
												PxsContext(PxU32 index, 
														   PxdBroadPhaseImplementation bp, 
														   bool useFastMassiveBPVolumeDeletion, 
														   bool useImprovedSpringSolver,
														   bool useLegacyJointDrive);
	virtual										~PxsContext();

	static				void					defaultUserSubTask(PxdSubTaskType taskType, PxdSubTaskExecuteFunc executeFunc, void* subTaskData, void* userParam);
						void					invalideAllTransformCaches();

						PxU32					mIndex;

						// PT: we want to support multiple implementations at the same time so we need to
						// switch to a real PxnBroadPhase interface. It means some methods will become virtual
						// (slower), but at least the design finaly makes some kind of sense...
//						PxsBroadPhaseContext*	mBPContext;
						PxnBroadPhase*			mBPContext;
						PxsDynamicsContext*		mDynamicsContext;

	// Threading
						PxdUserSubTaskCallback	mUserSubtaskCallback;
						void*					mUserSubtaskParam;

	PxsSubTaskPool<PxsCMUpdateSubTask>			mCMUpdatePool;
	PxcThreadCoherantCache<PxsContactCacheObject> mContactCache;

	// Shapes
	PxcPoolList<PxsShape, PxsContext>			mShapePool;
	PxcPoolList<PxsBodyShape, PxsContext>		mBodyShapePool;
#ifdef PXS_ENABLE_FLUIDS
	PxcBitMap									mDeletedShapesMap;
	bool										mClearDeletedShapesMap;
	PxsShapeTransformHash						mShapeTransformHash;	// Hash table to store last frames world to shape transforms.

	PxcPoolList<PxsFluidShape, PxsContext>		mFluidShapePool;

	// Fluids
	PxcPoolList<PxsFluid, PxsContext>			mFluidPool;
	PxU32										mNbActiveFluids;
	PxU32										mNbFluidCollProcessed;	// Track the number of fluids for which collision was
																		// processed. This is needed to know when the first
																		// fluid starts collision to do some preprocessing.
#endif	// PXS_ENABLE_FLUIDS

	// Contact managers
	PxcPoolList<PxsContactManager, PxsContext>	mContactManagerPool;
						PxcBitMap				mActiveContactManager;
						PxcBitMap				mChangeTouchContactManager;	
						PxU32					mNewTouchCMCount;
						PxU32					mLostTouchCMCount;

#ifdef PXS_ENABLE_FLUIDS
	PxcPoolList<PxsFluidBodyContactManager, PxsContext>	mFluidBodyContactManagerPool;
#endif

	// Contact callbacks

	PxcSimplePool<PxsContactCallbackPatch>		mContactCallbackPatchPool;
	PxcSimplePool<PxsContactCallbackQuat>		mContactCallbackQuatPool;	

	// Automatic Volume-Shape path stuff

						PxcBitMap				mChangedVolumes;

	// array for body pairs which pass contact force reporting threshold.
				PxcArray<PxsBodyPair>			mReportPairs;

	//timestmps used only to decide if something has moved for contact detectors.
	//It would be better to use bitmaps, as we dont have to access memory for all shapes/body shapes/atoms
	//to decide this.

						PxU32					mContactGenerationTimestamp;
						PxU32					mDynamicsTimestamp;
#ifdef PXD_SUPPORT_DEBUG_RENDERER
						PxcDebugData			mDebugData;
#endif

						PxsTaskList*			mTaskProxies;

#ifdef PXS_ENABLE_OPT_BP_VOLUME_DELETION
						bool					mFastMassiveBPVolumeDeletion;
#endif
};

#endif

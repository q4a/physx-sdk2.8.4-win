#ifndef NX_PHYSICS_NP_FORCEFIELDTHREADING
#define NX_PHYSICS_NP_FORCEFIELDTHREADING

/*----------------------------------------------------------------------------*\
|
|						Public Interface to NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "NpScene.h"

#if MULTI_THREADED_FF

#define NP_FF_SUBTASK_SIZE 1024
#define NP_FF_TASK_SIZE (4 * NP_FF_SUBTASK_SIZE)

class NpForceField;
class NpScene;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class NpForceFieldSubTask: public NxFoundation::NxAllocateable
{
public:
	NX_INLINE void init(const NpForceField* ff, const NxForceFieldKernel* kernel, NvFluid* fluid, NxU32 particleOfs, NxU32 numParticles,
						const NxBounds3* bounds, NxU32 groupIndex, NxReal simTimeFluid, NxReal scaleTableValue, bool divByDensity, 
						NxU32 coordSystem, const NxMat34* globalPose, NxForceMode forceMode);
	NX_INLINE void execute(NxVec3* forces, NxU32& overlapCount);
	NX_INLINE void updateFluidParticles(NxVec3* forces);

private:
	const NpForceField*			mNpForceField;
	const NxForceFieldKernel*	mKernel;
	NvFluid*					mNvFluid;
	NxU32						mParticleOfs;
	NxU32						mNumParticles;

	NxBounds3					mBounds;
	NxU32						mGroupIndex;
	NxReal						mSimTimeFluid;
	NxReal						mScaleTableValue;
	bool						mDivByDensity;
	NxU32						mCoordSystem;
	NxMat34						mGlobalPose;
	NxForceMode					mForceMode;

	friend class NpForceFieldThreading;
	friend struct NpForceFieldTask;
};


NX_INLINE void NpForceFieldSubTask::init(const NpForceField* ff, const NxForceFieldKernel* kernel, NvFluid* fluid, NxU32 particleOfs, NxU32 numParticles,
		const NxBounds3* bounds, NxU32 groupIndex, NxReal simTimeFluid, NxReal scaleTableValue, 
		bool divByDensity,  NxU32 coordSystem, const NxMat34* globalPose, NxForceMode forceMode)
{
	mNpForceField = ff;
	mKernel = kernel;
	mNvFluid = fluid;
	mParticleOfs = particleOfs;
	mNumParticles = numParticles;

	mBounds = *bounds;
	mGroupIndex  = groupIndex;
	mSimTimeFluid = simTimeFluid;
	mScaleTableValue = scaleTableValue;
	mDivByDensity = divByDensity;
	mCoordSystem = coordSystem;
	mGlobalPose = *globalPose;
	mForceMode = forceMode;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct NpForceFieldTask: public NxTask, public NxFoundation::NxAllocateable
{
	NX_INLINE	NpForceFieldTask(NpForceFieldThreading& owner)
		: mOwner(owner) {}

				void	execute();
	NX_INLINE	bool	addSubTask(NpForceFieldSubTask*);
	NX_INLINE	void	init(NxMutex* update, NxMutex* init);

public:
	NxFoundation::NxArraySDK<NpForceFieldSubTask*>	mSubTasks;
	NxFoundation::NxArraySDK<NxVec3>				mForceUpdates;
	NxU32											mNumParticles;
	NxMutex*										mFluidUpdateMutex;
	NxMutex*										mFluidInitMutex;
	NpForceFieldThreading&							mOwner;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct NpFFTaskList: public NxFoundation::NxAllocateable
{
	NX_INLINE NpFFTaskList(NvFluid* f)
		: fluid(f)
		, curTask(0)
		, curSubTask(0) {}
	
	NvFluid*	fluid;
	NxMutex		fluidUpdateMutex;
	NxMutex		fluidInitMutex;
	NxFoundation::NxArraySDK<NpForceFieldSubTask*> 
				subTaskList;
	NxFoundation::NxArraySDK<NpForceFieldTask*> 
				taskList;
	NxU32		curTask;
	NxU32		curSubTask;
	// maybe list of force vectors per fluid goes also here.
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class NpForceFieldThreading: public NxFoundation::NxAllocateable
{
public:
	NpForceFieldThreading(NpScene* nps);
	~NpForceFieldThreading();

				void					applyForces(NpForceField** ff, NxU32 numFF, NxReal simulatedTimeFluid, NxReal simulatedTimeCloth);
				NpForceFieldSubTask*	popSubTask();
				void					submitTask(NpForceFieldSubTask*);
	NX_INLINE	NpScene*				getNpScene()						{ return mNpScene; }
		
private:
	void								submitTasks();
	void								resetLists();
	NX_INLINE	void					pushSubTask(NpForceFieldSubTask* task);
	NX_INLINE	NpForceFieldTask*		popTask();
	NX_INLINE	void					pushTask(NpForceFieldTask* task);
	NX_INLINE	NpFFTaskList*			getTaskList(NvFluid* nvf);				

private:
	NxFoundation::NxArraySDK<NpFFTaskList*>			
					mTaskPerFluidLists;
	NxFoundation::NxArraySDK<NpForceFieldSubTask*>
					mSubTaskPool;
	NxFoundation::NxArraySDK<NpForceFieldTask*>
					mTaskPool;
	NxU32			mNumCreatedTasks;
	NxU32			mNumCreatedSubTasks;
	NpScene*		mNpScene;
};

#endif

#endif

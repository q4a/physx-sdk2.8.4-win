#ifndef PXS_DYNAMICS_H
#define PXS_DYNAMICS_H

#include "px_config.h"
#include "px_dynamics.h"

#include "PxnBodyAtom.h"
#include "PxsSolverAtom.h"
#include "PxsSolverConstraint.h"
#include "PxsContactCache.h"

#include "PxcNoCopy.h"
#include "PxcBitMap.h"
#include "PxcPool.h"
#include "PxcUnionFind.h"
#include "PxcThreadCoherantCache.h"

#include "PxsSubTask.h"

class PxsContext;
class PxsBodyAtom;
class PxsSolverCore;
class PxsCustomConstraint;
class PxsContactManager;
class PxsSolverConstraintPool;
class PxsContactConstraint;
class PxsD6Joint;

class PxsConstraintBase
{
public:
	PxsConstraintBase()  :
		mAtom0(NULL), mAtom1(NULL), mDominance0(1.0f), mDominance1(1.0f)
	{}

	virtual							~PxsConstraintBase()
	{}

	virtual	PxU32					setupSolverConstraint(PxsSolverConstraintPool& scpool, PxFloat dt, PxU32& thresholdPairCount) = 0;

	PXD_FORCE_INLINE PxU8			getDominanceByte() const
		{
		return 
			(mDominance0 == 0 ? 0 : PxsSolverConstraint::PXS_SC_FLAG_0_IS_PUSHABLE)
		+	(mDominance1 == 0 ? 0 : PxsSolverConstraint::PXS_SC_FLAG_1_IS_PUSHABLE);
		}
	//General properties
			PxsBodyAtom*			mAtom0;
			PxsBodyAtom*			mAtom1;

			PxFloat					mDominance0;
			PxFloat					mDominance1;


};


class PxsDynamicsContext : private PxcNoCopy
{
public:
	static PxsDynamicsContext*	create(PxsContext*, 
									   bool useImprovedSpringSolver,
									   bool useLegacyJointDrive);
	void						destroy();
	PXD_FORCE_INLINE PxsContext*	getContext()							const
	{
		return mContext;
	}

	bool						update(PxFloat dt);

	// Atom handling
	PxsBodyAtom*				createBodyAtom(const PxdAtomDesc& desc);
	void						releaseBodyAtom(PxsBodyAtom* atom);
	PxsBodyAtom*				findBodyAtom(PxdAtom handle)			const;
	
	void						markActive(PxsBodyAtom* atom);
	void						markInactive(PxsBodyAtom* atom);

	// Constraint handling
	PxsCustomConstraint*		createCustomConstraint(const PxdConstraintDesc& desc);
	void						releaseCustomConstraint(PxsCustomConstraint* constraint);
	PxnCustomConstraint*		findCustomConstraint(PxdConstraint handle) const;

	PxsContactConstraint*		createContactConstraint(const PxdContactConstraintDesc& desc);
	void						releaseContactConstraint(PxsContactConstraint* constraint);
	PxnContactConstraint*		findContactConstraint(PxdContactConstraint handle) const;

	PxsD6Joint*					createD6Joint(const PxdD6JointDesc& desc);
	void						releaseD6Joint(PxsD6Joint* joint);
	PxnD6Joint*					findD6Joint(PxdD6Joint handle)			const;

	PXD_FORCE_INLINE PxFloat	getBounceThreshold()					const
	{
		return mBounceThreshold;
	}

	PXD_FORCE_INLINE void		setBounceThreshold(PxFloat f)
	{
		mBounceThreshold = f;
	}

	PXD_FORCE_INLINE PxInt	getSolverBatchSize()					const
	{
		return mSolverBatchSize;
	}

	PXD_FORCE_INLINE void		setSolverBatchSize(PxInt f)
	{
		mSolverBatchSize = f;
	}

	PXD_FORCE_INLINE bool		useImprovedSpringSolver()			const
	{
		return mUseImprovedSpringSolver;
	}

	PXD_FORCE_INLINE bool		useLegacyJointDrive()				const
	{
		return mUseLegacyJointDrive;
	}


	// Used by contact managers to get external IDs. Only valid one frame
	PxdHandle					allocConstraintHandle(PxsContactManager* manager, PxU32 index);
	// Release all external IDs
	void						releaseAllConstraintHandles();

	// Solving interface
	void						solveDynamics(); //Main solver method


	PXD_FORCE_INLINE PxFloat	getDt()								const
	{
		return mDt;
	}

	PXD_FORCE_INLINE PxFloat	getInvDt()							const
	{
		return mInvDt;
	}

	//called by sub tasks during parallel solving.
	void						solveGroup(PxsBodyAtom* groupRoot, PxsContactCacheObject& cacheObject, PxcBitMap &volumeChangedMap);

	PXD_INLINE void				destroyIslandSubTask(PxsIslandSubTask* subTask)
	{
		mIslandSubTaskPool.destroy(subTask);
	}

	PXD_INLINE PxsContext*		getContext()
	{
		return mContext;
	}

protected:
								PxsDynamicsContext(PxsContext*, 
												   bool useImprovedSpringSolver,
												   bool useLegacyJointDrive);
								~PxsDynamicsContext();

	// Solver helper-methods
	void						integrateAtomAcceleration(PxsBodyAtom* atom)	const;
	void						integrateAtomPose(PxsBodyAtom* atom, PxcBitMap &volumeChangedMap) const;

	// General data
	PxsContext*					mContext;

	// All created atoms
	PxcPoolList<PxsBodyAtom, PxsDynamicsContext> mBodyAtomPool;
	
	// Bitmap of active atoms
	PxcBitMap					mActiveAtomMap;

	// Helper for solver island building
	PxcUnionFind				mUnionFindHelper;

	// "Constants" during solving
	PxFloat						mDt, mInvDt;

	// Solver to use
	PxsSolverCore*				mSolverCore;

	// Constraint-pool, all allocated constraints
	PxcPoolList<PxsCustomConstraint, PxsDynamicsContext>	mConstraintPool;
	// Contact constraint pool
	PxcPoolList<PxsContactConstraint, PxsDynamicsContext>	mContactConstraintPool;
	// D6 pool
	PxcPoolList<PxsD6Joint, PxsDynamicsContext>				mD6JointPool;

	// External constraint handle mapping stuff
	struct ExternalConstraintEntry
	{
		PxsContactManager*	mOwningManager;
		PxU32				mIndex;
	};
	ExternalConstraintEntry*	mExternalConstraintList;
	PxU32						mExternalConstraintSize;
	PxU32						mExternalConstraintCapacity;

	// Threading
	PxsSubTaskPool<PxsIslandSubTask>		mIslandSubTaskPool;

	// Dynamics properties
	PxFloat						mBounceThreshold;
	PxInt						mSolverBatchSize;

	// SIMD-related
	bool						mSupportSIMD;

	// Joint related
	bool						mUseImprovedSpringSolver;
	bool						mUseLegacyJointDrive;
};


#endif

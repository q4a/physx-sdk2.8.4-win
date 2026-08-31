#ifndef PXS_CONTACTCALLBACK_H
#define PXS_CONTACTCALLBACK_H

//#include "px_dynamics.h"

#include "PxnCustomConstraint.h"

//#include "PxsSolverConstraint.h"
#include "PxsContactCache.h"

#include "PxcContactMethod.h"
#include "PxcArray.h"
#include "PxcMatrix34.h"
#include "PxcCorrelateContacts.h"

class PxsBodyShape;
class PxsSolverConstraintPool;
class PxsContactManager;
//#include "PxsContactManager.h"


// Baseclass for contact callbacks in software
class PxsContactCallback : public PxcContactCallback
{
public:
	PxsContactCallback(PxsContactManager* manager);
	virtual ~PxsContactCallback()
	{}

	/*!
	Initialize on contact manager initialization
	*/
	virtual void		init();

	/*!
	Release any external constraint handles
	*/
	virtual void releaseConstraintHandles()
	{}
	
	/*!
	Execute the contact method and run begin/finish contacts(avoid a virtual call)
	*/
	virtual bool executeContactMethod(PxcContactMethod conMethod, 
		PxsContactCacheObject& threadCache, PxcPairContactCache& pairCache) = 0;

	/*!
	Process contacts to solver constraints
	*/
	virtual PxU32		setupSolverConstraints(PxsSolverConstraintPool& scpool, PxFloat dt,
							const PxcCachedTransforms& cTrans0, const PxcCachedTransforms& cTrans1,
							PxsBodyAtom* prefetchAtom0, PxsBodyAtom* prefetchAtom1) = 0;

	/*!
	Get number of externally modifiable constraints
	*/
	virtual PxUInt		getConstraintCount() const
	{
		return 0;
	}

	/*!
	Get constraints for external modification
	*/
	virtual void		getConstraintList(PxdConstraint* clist)
	{}

	/*!
	Get a specific constraint
	*/
	virtual PxnContactConstraint*	getConstraint(PxU32 index)
	{
		return 0;
	}

	/*!
	Clean up a callback on manager deactivation
	*/
	virtual void		onManagerDeactivation()
	{}

protected:
	PxsContactManager&				mManager;

	// The two body-shapes we're connected to
	PxsBodyShape*					mBodyShape0;		// PT: hmmm, can't we fetch those from mManager?
	PxsBodyShape*					mBodyShape1;		// PT: hmmm, can't we fetch those from mManager?
	PxFloat							mBounceThreshold;

	PXD_FORCE_INLINE	PxsBodyShape*	getBodyShape0()	{ return mBodyShape0;	}
	PXD_FORCE_INLINE	PxsBodyShape*	getBodyShape1()	{ return mBodyShape1;	}
	// PT: TODO: doesn't work because of include dependency problems.
//	PXD_FORCE_INLINE	PxsBodyShape*	getBodyShape0()	{ return mManager.mBodyShape0;	}
//	PXD_FORCE_INLINE	PxsBodyShape*	getBodyShape1()	{ return mManager.mBodyShape1;	}
};

#endif

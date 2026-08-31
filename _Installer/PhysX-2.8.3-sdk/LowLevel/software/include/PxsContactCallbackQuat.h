#ifndef PXS_CONTACTCALLBACKQUAT_H
#define PXS_CONTACTCALLBACKQUAT_H


#include "PxsContactCallback.h"

class PxsContactManager;

/*!
Contact callback generating quat-based contact constraints.
Used whenever contacts needs to be modifiable
*/
class PxsContactCallbackQuat : public PxsContactCallback
{
public:
	PxsContactCallbackQuat(PxsContactManager* manager);

	/*!
	Notify of a new contact point
	*/
	void		contact(const PxcVector& worldPoint,
		const PxcVector& worldNormal,
		PxFloat separation,
		PxU32 featureIndex0,
		PxU32 featureIndex1);

	/*!
	Initialize on contact manager initialization
	*/
	void		init();

	/*!
	Release any external constraint handles
	*/
	virtual void releaseConstraintHandles();

	/*!
	Execute the contact method and run begin/finish contacts(avoid a virtual call)
	*/
	virtual bool executeContactMethod(PxcContactMethod conMethod, 
		PxsContactCacheObject& threadCache, PxcPairContactCache& pairCache);



	/*!
	Process contacts to solver constraints
	*/
	PxU32		setupSolverConstraints(PxsSolverConstraintPool& scpool, PxFloat dt,
		const PxcCachedTransforms& cTrans0, const PxcCachedTransforms& cTrans1,
		PxsBodyAtom* prefetchAtom0, PxsBodyAtom* prefetchAtom1);

	/*!
	Get number of externally accessible constraints
	*/
	PxUInt		getConstraintCount() const;

	/*!
	Get constraints for external access
	*/
	void		getConstraintList(PxdConstraint* clist);

	/*!
	Get a specific constraint
	*/
	PxnContactConstraint*	getConstraint(PxU32 index);

	/*!
	Clean up a callback on manager deactivation
	*/
	void		onManagerDeactivation();

private:
		/*!
	Prepare for new pass of contacts.
	*/
	void beginContacts(const PxcCachedTransforms& cTrans0, 
								const PxcCachedTransforms& cTrans1);

	/*!
	Finalize processing of contacts for a single pair.
	*/
	void finishContacts(const PxcCachedTransforms& cTrans0, const PxcCachedTransforms& cTrans1);

	// Persistent data
	PxcArray<PxcPersistentContact>		mPersistentContacts;

	// Temporary data
	PxcArray<PxcTemporaryContact>		mTemporaryContacts;

	//Accumulators to use if we want contact reports
    PxcArray<PxsSolverImpulseAccumulatorQuat>	mAccumulators;

	// Helpers
	PxcQuaternion		computeQuatFromNormal(const PxcVector& n) const;

	// Go through all temporal contacts and set them up as persistent
	void				setupNewPersistentConstraints(const PxcMatrix34& bodyFrame0, const PxcMatrix34& bodyFrame1);

	// Prune untouched contacts
	void				pruneOldContacts();

	// Copy of externally modified contacts
	struct ModifiedContactPoint
	{
		PxcTransform					mFrame0;
		PxcTransform					mFrame1;
		PxcVector						mError;
		PxcVector						mTargetVelocity;
		PxFloat							mStaticFriction0;
		PxFloat							mStaticFriction1;
		PxFloat							mDynamicFriction0;
		PxFloat							mDynamicFriction1;
		PxFloat							mRestitution;
		PxFloat							mMaxMagnitude;
		PxFloat							mMinMagnitude;
		PxFloat							mDominance0;
		PxFloat							mDominance1;
		bool							mAnisotropic;
		bool							mIgnoreContact; 
		PxsSolverImpulseAccumulatorQuat	mAccumulator;
	};
	PxcArray<ModifiedContactPoint>	mModifiedContacts;

	class ExternalContactProxy : public PxnContactConstraint
	{
	public:
		ExternalContactProxy()
			: mCopyIndex(-1)
		{}

		ExternalContactProxy(PxsContactCallbackQuat* owner, PxdHandle handle, PxcPersistentContact* pcp);

		PxdHandle				getHandleV()									const
		{
			return mHandle;
		}

		void					destroyV();

		void					getAtomsV(PxnBodyAtom*& a0, PxnBodyAtom*& a1)	const;

		PxdTransform			getFrame0V()									const;
		void					setFrame0V(const PxdTransform& t);

		PxdTransform			getFrame1V()									const;
		void					setFrame1V(const PxdTransform& t);

		PxdVector				getErrorV()										const;
		void					setErrorV(const PxdVector& v);

		PxdVector				getTargetVelocityV()							const;
		void					setTargetVelocityV(const PxdVector& v);

		PxdVector				getImpulseV()									const;

		PxFloat					getMinImpulseV()								const;
		void					setMinImpulseV(PxFloat f);

		PxFloat					getMaxImpulseV()								const;
		void					setMaxImpulseV(PxFloat f);

		PxFloat					getStaticFriction0V()							const;
		void					setStaticFriction0V(PxFloat f);

		PxFloat					getStaticFriction1V()							const;
		void					setStaticFriction1V(PxFloat f);

		PxFloat					getDynamicFriction0V()							const;
		void					setDynamicFriction0V(PxFloat f);

		PxFloat					getDynamicFriction1V()							const;
		void					setDynamicFriction1V(PxFloat f);

		PxFloat					getRestitutionV()								const;
		void					setRestitutionV(PxFloat f);

		bool					getAnisotropicV()								const;
		void					setAnisotropicV(bool b);

		bool					getImpulseReportV()								const;
		void					setImpulseReportV(bool b);

		PxInt					getFeature0V()									const;
		PxInt					getFeature1V()									const;

		PxFloat					getDominance0V()								const;
		void					setDominance0V(PxFloat);

		PxFloat					getDominance1V()								const;
		void					setDominance1V(PxFloat);
	protected:

		PxsContactCallbackQuat* mOwner;
		PxdHandle				mHandle;

		PxInt					mCopyIndex;			// copy made, use this index. -1 if no copy exists
		PxcPersistentContact*	mPcp;				// original persistent contact

		friend class PxsContactCallbackQuat;
	};

	PxInt makeContactModifiable(PxcPersistentContact& pcp);

	PxcArray<ExternalContactProxy>		mExternalProxies;

	bool								mSwapBodies;	// PT: bodies in base class and swap bodies here??

	friend class PxsContactManager;
};


#endif

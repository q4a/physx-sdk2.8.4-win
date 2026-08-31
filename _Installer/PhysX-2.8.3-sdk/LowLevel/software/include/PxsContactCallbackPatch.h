#ifndef PXS_CONTACTCALLBACKPATCH_H
#define PXS_CONTACTCALLBACKPATCH_H

#include "PxsContactCallback.h"

class PxsContactManager;


/*!
Contact callback using patch-correlation and generating per patch
friction constraints
*/
class PxsContactCallbackPatch : public PxsContactCallback
{
public:
	PxsContactCallbackPatch(PxsContactManager* manager);
	virtual ~PxsContactCallbackPatch();
	/*!
	Initialize on contact manager initialization
	*/
	virtual void		init();

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
	Notify of a new contact point

	This has to be virtual due to contact routines only understanding PxsContactCallback:-(
	*/
	virtual void		contact(const PxcVector& worldPoint,
		const PxcVector& worldNormal,
		PxFloat separation,
		PxU32 featureIndex0,
		PxU32 featureIndex1);



	/*!
	Process contacts to solver constraints

	\NOTE: Transforms are for bodyShape0 and bodyShape1
	*/
	virtual PxU32		setupSolverConstraints(PxsSolverConstraintPool& scpool, PxFloat dt,
		const PxcCachedTransforms& cTrans0, const PxcCachedTransforms& cTrans1,
		PxsBodyAtom* prefetchAtom0, PxsBodyAtom* prefetchAtom1);

	/*!
	Get number of externally accessible constraints
	*/
	virtual PxUInt		getConstraintCount() const
	{
		return mContactPoints;
	}

	/*!
	Get constraints for external access
	*/
	virtual void		getConstraintList(PxdConstraint* clist);

	/*!
	Get a specific constraint
	*/
	virtual PxnContactConstraint*	getConstraint(PxU32 index);


	/*!
	Clean up a callback on manager deactivation
	*/
	virtual void		onManagerDeactivation();

protected:

	/*!
	Prepare for new pass of contacts.

	\NOTE: Transforms are for bodyShape0 and bodyShape1
	*/
	PXD_FORCE_INLINE void beginContacts(const PxcCachedTransforms& cTrans0, const PxcCachedTransforms& cTrans1);
	/*!
	Finalize processing of contacts for a single pair.

	\NOTE: Transforms are for bodyShape0 and bodyShape1
	*/
	PXD_FORCE_INLINE void finishContacts(const PxcCachedTransforms& cTrans0, const PxcCachedTransforms& cTrans1);


	//keep the old constraint setup as it may ease implimentation of future contact constraint systems.
	PxU32 setupSolverConstraintsOld(PxsSolverConstraintPool& scpool, PxFloat dt,
					const PxcCachedTransforms& cTrans0, const PxcCachedTransforms& cTrans1,
					PxsBodyAtom* prefetchAtom0, PxsBodyAtom* prefetchAtom1);

	PXD_FORCE_INLINE void computeFrictionTangents(
					const PxsSolverBodyAtom* solverAtom0, const PxsSolverBodyAtom* solverAtom1, 
					const PxcVector& worldNormal, PxcVector& t0, PxcVector& t1);

	struct TemporalPatch;
	// Persistent data
	struct PersistentPatch : public PxsSolverConstraintCallback
	{
		PxcVector			mLocalAnchor[2][2];			//Anchor-points indexed by [pointIndex][bodyIndex] Local-space
		PxcVector			mLocalNormal[2];			//Local-space normal [bodyIndex]
		PxcVector			mWorldNormal;				//One of the normals cached in worldspace
		TemporalPatch*		mTemporalPatch;				//Pointer to first temporal patch, used during correlation and constraint setup
		PxU8				mNumPoints;					//Number of points the patch spans (1 or 2 encoded as 0 or 1)
		PxU8				mFreshened;
		bool				mBroken;

		// Implements PxsSolverConstraintCallback
		virtual void lastIterationCallback(const PxsSolverConstraint& sc)
		{
			if(sc.mFlags.mType == PxsSolverConstraint::PXS_SC_TYPE_LINEAR_FRICTION && 
				sc.mFlags.mFlags & PxsSolverConstraint::PXS_SC_FLAG_BROKEN)
				mBroken = true;
		}
		virtual void lastIterationCallback(PxFloat vForce, PxFloat force) {}

		virtual void frictionBrokenCallback()
		{
			mBroken = true;
		}

		//~Implements PxsSolverConstraintCallback
	};
	PxcArray<PersistentPatch>	mFrictionPatches;

	//Temporal data
	static const int TEMPORAL_PPP = 4;
	struct TemporalPatch
	{
		PxcVector					mWorldNormal;				//12
		TemporalPatch*				mNext;						//16
		PxU32						mNumPoints;					//20
		PxcVector					mWorldPoint[TEMPORAL_PPP];				//+72->92
		PxFloat						mSeparation[TEMPORAL_PPP];				//116
		PxU32						mFeature0[TEMPORAL_PPP];				//140
		PxU32						mFeature1[TEMPORAL_PPP];				//164
		PxU32 mPad[3];
	};
	PxcArray<TemporalPatch>		mContactPatches;
	PxU32						mContactPoints;

	struct TemporalAccumulator
	{
		PxsSolverImpulseAccumulator	mAcc[6];
	};
	PxcArray<TemporalAccumulator>	mAccumulators;

	bool						mSwapBodies;	// PT: bodies in base class and swap bodies here??

	// Helpers
	void				finalizeFrictionPatch(PersistentPatch& patch, const PxcVector** points, PxUInt numPoints,
		const PxcMatrix34& bodyFrame0, const PxcMatrix34& bodyFrame1);

	PersistentPatch* 	findFrictionPatchForNormal(const PxcVector& normal,
		const PxcMatrix34& bodyFrame0, const PxcMatrix34& bodyFrame1);


	//For external constraint access
	class ExternalContactProxy : public PxnContactConstraint
	{
	public:
		ExternalContactProxy()
		{}

		ExternalContactProxy(PxsContactCallbackPatch* owner, PxdHandle handle, PxU32 cIdx)
			: mOwner(owner), mHandle(handle), mCpIndex(cIdx)
		{
		}

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

		PxsContactCallbackPatch*	mOwner;
		PxdHandle					mHandle;

		PxUInt						mCpIndex;
		friend class PxsContactCallbackPatch;
	};
	PxcArray<ExternalContactProxy>	mExternalProxies;
	friend class ExternalContactProxy;
	friend class PxsContactManager;
};


#endif

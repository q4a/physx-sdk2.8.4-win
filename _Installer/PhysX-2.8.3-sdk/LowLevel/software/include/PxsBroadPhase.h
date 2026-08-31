#ifndef PXS_BROADPHASE_H
#define PXS_BROADPHASE_H

#include "PxnBroadPhase.h"
#include "PxnVolume.h"

#include "PxcPool.h"
#include "PxcNoCopy.h"
#include "PxcBitMap.h"
#include "PxcHelperFunction.h"
#include "PxcMath.h"
#include "PxcVector.h"
#include "PxcArray.h"
#include "PxcPlatformInclude.h"


class PxsVolume;
class PxsContext;
class PxsBroadPhaseContext;

#define PXS_INT_BP

// We must pad min/max value by a small amount so that the min endpoint is not equal to the max endpoint
// after rounding errors. Should also help stability in general(eg without two aligned boxes could touch
// and oscilate in and out of BP overlap).
static const PxFloat PXS_BP_MINMAX_EPSILON = 1.0e-4f;

#ifdef PXS_INT_BP
	typedef PxU32 PxsBpValue;
#else
	typedef float PxsBpValue;
#endif

#ifdef PXS_INT_BP
	PXD_FORCE_INLINE PxsBpValue PxsBpEncodeFloat(const PxFloat &newPos)
	{
		//we may need to check on -0 and 0
		//But it should make no practical difference.
		PxU32 ir = ((PxU32&)(newPos));

		if(ir & 0x80000000) //negative?
			ir = ~ir;//reverse sequence of negative numbers
		else
			ir |= 0x80000000; // flip sign

		return ir;
	}

	PXD_FORCE_INLINE PxFloat PxsBpDecodeFloat(PxsBpValue ir)
	{
		PxU32 rv;

		if(ir & 0x80000000) //positive?
			rv = ir & ~0x80000000; //flip sign
		else
			rv = ~ir; //undo reversal

		return ((PxFloat&)(rv));
	}
#else

	PXD_FORCE_INLINE PxsBpValue PxsBpEncodeFloat(const PxFloat &newPos)
	{
		return newPos;
	}

	PXD_FORCE_INLINE PxFloat PxsBpDecodeFloat(PxsBpValue ir)
	{
		return ir;
	}
#endif

//we keep the critical info in a cut down structure for less cache pollution.
//Should we duplicate the endpoint values in here so we dont have to access the arrays for intersection test?

struct PxsBpSmallVolume
{
	PxU32 mMinEPs[3];
	PxU32 mMaxEPs[3];
//	PxU16 mMinEPs[3];
//	PxU16 mMaxEPs[3];

	PxU32 mGroup;

	PxU16 mPairBucketIndex;
	PxU16 mOwnerIndex; //32 bytes
};

#include "PxsBroadPhasePairMap.h"
#include "PxsBroadPhaseEndPointArray.h"


class PxsVolume : public PxnVolume
{
public:
	// Public interface
	virtual void				setOriginExtentV(const PxdVector& o, const PxdVector& e);

	virtual const PxdVector &	getOriginV()					const;
	virtual void				setOriginV(const PxdVector &v);

	virtual const PxdVector &	getExtentV()					const;
	virtual void				setExtentV(const PxdVector &v);

	virtual PxU32				getGroupV()						const;
	virtual void				setGroupV(PxU32 group);

	virtual void*				getUserDataV()					const;
	virtual void				setUserDataV(void* data);

	virtual PxnBodyShape *		getShapeV()						const;
	virtual PxdVolumeType		getTypeV()						const;

	virtual PxdHandle			getHandleV()					const;
	virtual void				destroyV();

	PXD_FORCE_INLINE PxU32		getIndex()						const
	{
		return mIndex;
	}

	PxsVolume(PxsBroadPhaseContext *context, PxU32 index);

	~PxsVolume();

	PXD_FORCE_INLINE void		removeDependantVolume()
	{
		if(mBodyShape != NULL)
			mBodyShape->removeDependantVolume(mIndex);
	}
private:
	PxsBroadPhaseContext*		mBpContext;
	PxU32						mIndex;

	PxU32						mSmallVolIndex;//OPT: only need 16bits for mIndex and mSmallVolIndex
	PxsBodyShape*				mBodyShape;

	PxU32						mGroup;
	void*						mUserData;

	//OPT: we probably only need mOrigin+mExtent or mShape(mPaddedMin/Max can be updated directly)
	PxcVector					mOrigin;
	PxcVector					mExtent;


	PXD_FORCE_INLINE void getPaddedMinMax(PxcVector &paddedMin, PxcVector &paddedMax) const
	{

		//pad values a little to avoid problems with very thin volumes.
		paddedMin = (mOrigin - mExtent);
		paddedMax = (mOrigin + mExtent);
		
		//No need to pad values anymore as we should now correctly handle equal min/max.
	}


	friend class PxsBroadPhaseContext;
};


//TODO: try lazy compacting of the endpoint arrays and possibly adding gaps delibratly
//then binary search for insertion points into the array. Is insertion going to be expensive?
class PxsBroadPhaseContext : public PxnBroadPhase, private PxcNoCopy
{
public:
	static PxsBroadPhaseContext* create(PxsContext*);
	void						destroy();

	PxUInt						getCreatedOverlapsCountV()		const;
	PxUInt						getDestroyedOverlapsCountV()	const;

	bool						fillCreatedOverlapsV(PxdBroadPhaseOverlap *overlaps);
	bool						fillDestroyedOverlapsV(PxdBroadPhaseOverlap *overlaps);

	void						setReportCreatedOverlapsV(bool);
	void						setReportDestroyedOverlapsV(bool);
	void						setReportIdentifierV(bool);
	void						setReportUserDataV(bool);

	bool						getReportCreatedOverlapsV()		const;
	bool						getReportDestroyedOverlapsV()	const;
	bool						getReportIdentifierV()			const;
	bool						getReportUserDataV()			const;

	// PT: hooks
	virtual	void				destroyV()									{ destroy();					}
	virtual	PxnVolume*			createVolumeV(const PxdVolumeDesc& desc)	{ return createVolume(desc);	}
	virtual	void				releaseVolumeV(PxnVolume* volume)			{ releaseVolume(volume);		}
	virtual	PxnVolume*			findVolumeV(PxdHandle handle)				{ return findVolume(handle);	}
	virtual	bool				updateBroadPhaseV()							{ return updateBroadPhase();	}

	bool						updateBroadPhase();

	// Volume interaction
	PxnVolume*					createVolume(const PxdVolumeDesc &desc);
	void						releaseVolume(PxnVolume *volume);
	PxnVolume*					findVolume(PxdHandle handle);


	// Interface for PxsVolume
	PxsContext*					getContext() const
	{
		return mContext;
	}

//private:
	PxsBroadPhaseContext(PxsContext*);
	~PxsBroadPhaseContext();
private:

	// Data
	bool							mReportCreatedOverlaps;
	bool							mReportDestroyedOverlaps;
	bool							mReportIdentifier;
	bool							mReportUserData;

	PxsContext*						mContext;
	PxcPoolList<PxsVolume, PxsBroadPhaseContext>	mVolumePool;

	PxcBitMap						mRemovedVolumes;
	PxcBitMap						mCreatedVolumes;
	
	PxsBroadPhasePairMap			mPairMap;
	PxsBroadPhaseEndPointArray		mEndPointArray;

	// small volumes
	PxcArray<PxsBpSmallVolume>		mSmallVolumes;
	PxU16							mFreeSmallVolume;


	//temp arrays for insert/remove
	//TODO: try and share these arrays.
	PxcArray<PxsBpEndPoint>			mEpForInsert;
	PxcArray<PxU32>					mEpForRemoval;

	// Utility stuff
	////////////////////////////////////////

	bool						isConsistent();

	void						purgeVolumes();
	void						createVolumes();
	void						updateVolumes();
	void						updateEndPoints();
	void						dumpEndPoints();



	PXD_FORCE_INLINE PxU32 newSmallVolume(PxsVolume *owner, PxU32 group)
	{
		PxU32 idx;

		if(mFreeSmallVolume != 0xffFF)
		{
			idx = mFreeSmallVolume;
			mFreeSmallVolume = mSmallVolumes[idx].mGroup;
		}
		else
		{
			mSmallVolumes.insert();
			idx = mSmallVolumes.getSize()-1;
		}

		PXN_ASSERT(owner->getIndex() < 0xffFF);

		mSmallVolumes[idx].mOwnerIndex = (PxU16)owner->getIndex();
		mSmallVolumes[idx].mGroup = group;
		mSmallVolumes[idx].mPairBucketIndex = 0;

#ifdef _DEBUG

		for(PxU32 i=0; i<3; i++)
		{
			mSmallVolumes[idx].mMinEPs[i] = 0xcdcdcdcd;
			mSmallVolumes[idx].mMaxEPs[i] = 0xcdcdcdcd;
		}
#endif
		return idx;
	}

	PXD_FORCE_INLINE void deleteSmallVolume(PxU32 idx)
	{
#ifdef _DEBUG
		PxcMemset(&(mSmallVolumes[idx]), 0xcd, sizeof(PxsBpSmallVolume));
#endif
		mSmallVolumes[idx].mGroup = mFreeSmallVolume;
		mFreeSmallVolume = idx;
	}
};





// A version without the shared context
class PxsBroadPhaseContextCell;
class PxsVolumeBpCell
{
public:
	void	setOriginExtent(PxsBroadPhaseContextCell& context, const PxdVector& o, const PxdVector& e);

	PXD_FORCE_INLINE PxU32		getIndex()						const
	{
		return mIndex;
	}
	PXD_FORCE_INLINE PxU32		getID()							const
	{
		return mID;
	}

	PxsVolumeBpCell(void*, PxU32 index);	// PT: couldn't get rid of useless first arg because of insane PxcPool template. Don't ask me.
	~PxsVolumeBpCell();

private:
//	PxU32						mIndex;
//	PxU32						mSmallVolIndex;//OPT: only need 16bits for mIndex and mSmallVolIndex
	PxU16						mIndex;
	PxU16						mSmallVolIndex;

	PxU32						mGroup;
	PxU32						mID;	// Mapping to owner volume of the multi SAP (owner volume index)

	PxcVector					mOrigin;
	PxcVector					mExtent;

	PXD_FORCE_INLINE void getPaddedMinMax(PxcVector &paddedMin, PxcVector &paddedMax) const
	{
		//pad values a little to avoid problems with very thin volumes.
		paddedMin = (mOrigin - mExtent);
		paddedMax = (mOrigin + mExtent);

		//No need to pad values anymore as we should now correctly handle equal min/max.
	}
	friend class PxsBroadPhaseContextCell;
};

struct PxdVolumeDescBpCell
{
	PxInt		group;
	PxU32		mID;
	PxdVector	origin;
	PxdVector	extent;
};

// PT: this version is roughly the same as PxsBroadPhaseContext, except:
// - it stores changed volumes inside the class, and not in the PxsContext
// - it is only used internally by the MultiSAP so we don't need the virtual interface
// - in the same way it only returns whatever is needed for us, using dedicated structs
// - various report control variables have been removed (useless here, handled by owner)
// - fillCreatedOverlapsV / fillDestroyedOverlapsV are directly performed by the owner MultiSAP
class PxsBroadPhaseContextCell : private PxcNoCopy
{
	public:
	static PxsBroadPhaseContextCell*				create(PxsContext*);
						void						destroy();

						PxUInt						getCreatedOverlapsCount()	const;
						PxUInt						getDestroyedOverlapsCount()	const;

						bool						updateBroadPhase();

	// Volume interaction
						PxsVolumeBpCell*			createVolume(const PxdVolumeDescBpCell& desc);
						void						releaseVolume(PxsVolumeBpCell* volume);
						PxsVolumeBpCell*			findVolume(PxdHandle handle);
//private:
													PxsBroadPhaseContextCell(PxsContext*);
													~PxsBroadPhaseContextCell();
	// Interface for PxsVolume
	PXD_FORCE_INLINE	PxsContext*					getContext_() const		{ return mContext_;				}

// Previously in PxsContext
	PXD_FORCE_INLINE	void						markAllVolumesValid()	{ mChangedVolumes.reset();		}
	PXD_FORCE_INLINE	void						markVolume(PxU32 idx)	{ mChangedVolumes.set(idx);		}
	PXD_FORCE_INLINE	void						unMarkVolume(PxU32 idx)	{ mChangedVolumes.clear(idx);	}
	PXD_FORCE_INLINE	PxcBitMap&					getChangedVolumeMap()	{ return mChangedVolumes;		}
						PxcBitMap					mChangedVolumes;
//private:

	// Data
						PxsContext*					mContext_;
				PxcPoolList<PxsVolumeBpCell, void>	mVolumePool;

						PxcBitMap					mRemovedVolumes;
						PxcBitMap					mCreatedVolumes;

						PxcBitMap					mPreviouslyRemovedVolumes;
						
						PxsBroadPhasePairMapCell	mPairMap;
						PxsBroadPhaseEndPointArray	mEndPointArray;
						
	// small volumes
						PxcArray<PxsBpSmallVolume>	mSmallVolumes;
						PxU16						mFreeSmallVolume;

	//temp arrays for insert/remove
	//TODO: try and share these arrays.
						PxcArray<PxsBpEndPoint>		mEpForInsert;
						PxcArray<PxU32>				mEpForRemoval;

	// Utility stuff
	////////////////////////////////////////

						bool						isConsistent();

						void						purgeVolumes();
						void						createVolumes();
						void						updateVolumes();
						void						updateEndPoints();

	PXD_FORCE_INLINE PxU32 newSmallVolume(PxsVolumeBpCell* owner, PxU32 group)
	{
		PxU32 idx;

		if(mFreeSmallVolume != 0xffFF)
		{
			idx = mFreeSmallVolume;
			mFreeSmallVolume = mSmallVolumes[idx].mGroup;
		}
		else
		{
			mSmallVolumes.insert();
			idx = mSmallVolumes.getSize()-1;
		}

		PXN_ASSERT(owner->getIndex() < 0xffFF);

		mSmallVolumes[idx].mOwnerIndex = (PxU16)owner->getIndex();
		mSmallVolumes[idx].mGroup = group;
		mSmallVolumes[idx].mPairBucketIndex = 0;

#ifdef _DEBUG
		for(PxU32 i=0; i<3; i++)
		{
			mSmallVolumes[idx].mMinEPs[i] = 0xcdcdcdcd;
			mSmallVolumes[idx].mMaxEPs[i] = 0xcdcdcdcd;
		}
#endif
		return idx;
	}

	PXD_FORCE_INLINE void deleteSmallVolume(PxU32 idx)
	{
#ifdef _DEBUG
		PxcMemset(&(mSmallVolumes[idx]), 0xcd, sizeof(PxsBpSmallVolume));
#endif
		mSmallVolumes[idx].mGroup = mFreeSmallVolume;
		mFreeSmallVolume = idx;
	}
};


#endif

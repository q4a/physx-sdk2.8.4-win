#ifndef PXS_BROADPHASE_MULTI_H
#define PXS_BROADPHASE_MULTI_H

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

class PxsVolumeArray;
class PxsContext;
class PxsBroadPhaseContext;

	// PT: we want to reuse the PxsBroadPhasePairMap object to store our pairs, unfortunately it's closely coupled to the
	// PxsBpSmallVolume class, which is itself part of the PhysX SAP implementation. That's bad design, the "pair manager"
	// object should be independent and reusable. Since I do NOT need the full PxsBpSmallVolume here, the following #defines
	// are used to reimplement PxsBroadPhasePairMap with a smaller class (PxsBpSmallVolumeMulti). I know, it looks ugly.
	// Blame whoever wrote PxsBroadPhasePairMap.
	// MS: The pair mananger is now designed as a template to avoid this hack (although Pierre with his "love" for templates
	//     would not agree... but he will never know I hope...)
	struct PxsBpSmallVolumeMulti
	{
		PxU16 mPairBucketIndex;
	};

#include "PxsBroadPhasePairMap.h"

	// PT: this is the max number of handles stored in each object, i.e. an object in the multi-SAP can be stored in
	// MAX_NB_HANDLES cells at most. For dynamic objects 4 cells is usually enough (if cell size is carefully chosen).
	// Unfortunately big static objects like an infinite plane end up in *all* cells! So until we handle static objects
	// in a different way, MAX_NB_HANDLES must be the total number of SAP cells :(
	// TODO: optimize this! It's not very memory friendly.
//	#define MAX_NB_HANDLES	8
//	#define MAX_NB_HANDLES	16
	#define MAX_NB_HANDLES	64

	//! (Handle, Index) pair.
	struct HandleIndex
	{
		PxI32				mIndex;		//!< Grid-cell index
		PxsVolumeBpCell*	mHandle;	//!< Handle to the SAP in indexed grid-cell
	};

	struct SAP_FatHandle
	{
		PxdVolumeDescBpCell	mDesc;			// ### optimize size here?
		PxU32				mNbHandles;
		HandleIndex			mBundles[MAX_NB_HANDLES];
	};

class PxsBroadPhaseContextMulti;
// PT: we need our own "volume" here, since it must contains the fat SAP handle
class PxsVolumeMulti : public PxnVolume
{
public:
	// Public interface
	virtual void				setOriginExtentV(const PxdVector& o, const PxdVector& e);

	virtual const PxdVector&	getOriginV()					const;
	virtual void				setOriginV(const PxdVector& v);

	virtual const PxdVector&	getExtentV()					const;
	virtual void				setExtentV(const PxdVector& v);

	virtual PxU32				getGroupV()						const;
	virtual void				setGroupV(PxU32 group);

	virtual void*				getUserDataV()					const;
	virtual void				setUserDataV(void* data);

	virtual PxnBodyShape*		getShapeV()						const;
	virtual PxdVolumeType		getTypeV()						const;

	virtual PxdHandle			getHandleV()					const;
	virtual void				destroyV();

	PXD_FORCE_INLINE PxU32		getIndex()						const
	{
		return mIndex;
	}

	PxsVolumeMulti(PxsBroadPhaseContextMulti* context, PxU32 index);

	~PxsVolumeMulti();

	PXD_FORCE_INLINE void		removeDependantVolume()
	{
		if(mBodyShape != NULL)
			mBodyShape->removeDependantVolume(mIndex);
	}
private:
	PxsBroadPhaseContextMulti*	mBpContext;
	PxU32						mIndex;

	PxsBodyShape*				mBodyShape;

	PxU32						mGroup;
	void*						mUserData;

	PxcVector					mOrigin;
	PxcVector					mExtent;

	SAP_FatHandle				mSAPHandle;

	friend class PxsBroadPhaseContextMulti;
};


class PxsBroadPhaseContextMulti : public PxnBroadPhase, private PxcNoCopy
{
	public:
	static PxsBroadPhaseContextMulti*		create(PxsContext*);

						void				destroy();

	// PxnBroadPhase
	virtual				PxUInt				getCreatedOverlapsCountV()			const;
	virtual				PxUInt				getDestroyedOverlapsCountV()		const;

	virtual				bool				fillCreatedOverlapsV(PxdBroadPhaseOverlap* overlaps);
	virtual				bool				fillDestroyedOverlapsV(PxdBroadPhaseOverlap* overlaps);

	virtual				void				setReportCreatedOverlapsV(bool report)		{ mReportCreatedOverlaps = report;		}
	virtual				void				setReportDestroyedOverlapsV(bool report)	{ mReportDestroyedOverlaps = report;	}
	virtual				void				setReportIdentifierV(bool report)			{ mReportIdentifier = report;			}
	virtual				void				setReportUserDataV(bool report)				{ mReportUserData = report;				}

	virtual				bool				getReportCreatedOverlapsV()			const	{ return mReportCreatedOverlaps;		}
	virtual				bool				getReportDestroyedOverlapsV()		const	{ return mReportDestroyedOverlaps;		}
	virtual				bool				getReportIdentifierV()				const	{ return mReportIdentifier;				}
	virtual				bool				getReportUserDataV()				const	{ return mReportUserData;				}

	virtual				void				setUpAxis(PxU32 axis);
	virtual				PxU32				getUpAxis()							const;
	virtual				void				setNbX(PxU32 nb);
	virtual				PxU32				getNbX()							const;
	virtual				void				setNbY(PxU32 nb);
	virtual				PxU32				getNbY()							const;
	virtual				void				setSizeX(PxFloat val);
	virtual				PxFloat				getSizeX()							const;
	virtual				void				setSizeY(PxFloat val);
	virtual				PxFloat				getSizeY()							const;

	// PT: hooks
	virtual				void				destroyV()									{ destroy();					}
	virtual				PxnVolume*			createVolumeV(const PxdVolumeDesc& desc)	{ return createVolume(desc);	}
	virtual				void				releaseVolumeV(PxnVolume* volume)			{ releaseVolume(volume);		}
	virtual				PxnVolume*			findVolumeV(PxdHandle handle)				{ return findVolume(handle);	}
	virtual				bool				updateBroadPhaseV()							{ return updateBroadPhase();	}
	//~PxnBroadPhase

						bool				updateBroadPhase();

	// Volume interaction
						SAP_FatHandle		createSAPVolume(const PxdVolumeDescBpCell& desc);
						bool				updateObject(SAP_FatHandle& handle, const PxdVector& origin, const PxdVector& extent);
	// PT: it's a mystery why the volume themselves are not in the interface. PxnBroadPhase
	// looks like a broken interface if it doesn't even let clients create volumes...
						PxnVolume*			createVolume(const PxdVolumeDesc& desc);
						void				releaseVolume(PxnVolume* volume);
						PxnVolume*			findVolume(PxdHandle handle);

	// Interface for PxsVolume
						PxsContext*			getContext()			const	{	return mContext;	}
	private:
											PxsBroadPhaseContextMulti(PxsContext*);
											~PxsBroadPhaseContextMulti();

						PxsContext*			mContext;

PxcPoolList<PxsVolumeMulti, PxsBroadPhaseContextMulti>	mVolumePool;

// PT: the mSmallVolumes array is not really needed, only one member of the struct is used in the pair map. But the pairmap code requires
// this array in the ctor (BAD useless coupling, sigh). So, memory is wasted here, and the overall design is dubious (pair map should be an
// independent class, obviously). Still, since we may remove this code completely in the end (depends on how we synchronize), not worth
// optimizing yet.
PxsBroadPhasePairMapMulti		mPairMap;
PxcArray<PxsBpSmallVolumeMulti>	mSmallVolumes;
ContainerSizeT					mDeadVolumes;	// Hack - Only added because object deletion doesn't remove necessary pairs (...)

						PxcBitMap			mRemovedVolumesMap;

						PxFloat				mSizeX;
						PxFloat				mSizeY;
						PxFloat				mInvCellSizeX;
						PxFloat				mInvCellSizeY;
						PxU32				mUpAxis;			// 0,1,2
						PxU32				mNbX;
						PxU32				mNbY;
			PxsBroadPhaseContextCell*		mCells;
						bool*				mFlags;

						bool				mReportCreatedOverlaps;
						bool				mReportDestroyedOverlaps;
						bool				mReportIdentifier;
						bool				mReportUserData;

						bool				mInitialized;

	PXD_FORCE_INLINE	void				computeCoveredCells(PxI32& ix, PxI32& iy, PxI32& jx, PxI32& jy, const PxcVector& min, const PxcVector& max)	const;
						void				synchronize();
						void				initialize();
};

#endif

#ifndef PXS_SHAPE_H
#define PXS_SHAPE_H

#include "PxnShape.h"
#include "PxcVector.h"
#include "PxcTransform.h"
#include "PxcShapeData.h"
#include "PxcSmallArray.h"

class PxsContext;

class PxsShape : public PxnShape
{
public:
											PxsShape(PxsContext* context, PxU32 index);
	virtual									~PxsShape();
	
						void				init(const PxdShapeDesc& desc);
	// Implements PxnShape
	virtual				void				setRadiusV(PxFloat f);
	virtual				PxFloat				getRadiusV()								const;

	//Capsule
	virtual				void				setHalfExtentV(PxFloat f);
	virtual				PxFloat				getHalfExtentV()							const;

	//Box
	virtual				void				setDimensionsV(const PxdVector& v);
	virtual				const PxdVector&	getDimensionsV()							const;

	//Meshes
	virtual				void				setMeshPtrV(void* ptr);
	virtual				void*				getMeshPtrV()								const;
		
	virtual				void				setSmoothNormalsV(bool b);
	virtual				bool				getSmoothNormalsV()							const;

	//Heightfield
	virtual				PxU32				getRowsV()									const;
	virtual				PxU32				getColumnsV()								const;
	virtual				PxU32				getStrideV()								const;
	virtual				void*				getSamplesV()								const;
	virtual				PxFloat				getRowScaleV()								const;
	virtual				PxFloat				getColumnScaleV()							const;
	virtual				PxFloat				getHeightScaleV()							const;
	virtual				PxFloat				getVerticalExtentV()						const;
	virtual				PxFloat				getThicknessV()								const;
	virtual				PxU32				getFlagsV()									const;
	virtual				PxU32				getHoleMaterialV()							const;
	virtual				PxFloat				getConvexEdgeThresholdV()					const;
	virtual				void				setRowsV(PxU32 rows);
	virtual				void				setColumnsV(PxU32 columns);
	virtual				void				setStrideV(PxU32 stride);
	virtual				void				setSamplesV(void* samples);
	virtual				void				setRowScaleV(PxFloat rowScale);
	virtual				void				setColumnScaleV(PxFloat columnScale);
	virtual				void				setHeightScaleV(PxFloat heightScale);
	virtual				void				setVerticalExtentV(PxFloat verticalExtent);
	virtual				void				setThicknessV(PxFloat thickness);
	virtual				void				setFlagsV(PxU32 flags);
	virtual				void				setHoleMaterialV(PxU32 holeMaterial);
	virtual				void				setConvexEdgeThresholdV(PxFloat convexEdgeThreshold);

	virtual				PxdShapeType		getTypeV()									const;

	virtual				void				setUserDataV(void* data);
	virtual				void*				getUserDataV()								const;

	virtual				PxdHandle			getHandleV()								const;
	virtual				void				destroyV();
	//~Implements PxnShape

	// Fast-accessors
	PXD_FORCE_INLINE	PxU32				getIndex()									const	{ return mIndex;			}
	PXD_FORCE_INLINE	PxcShapeData&		getShapeData()										{ return mShapeInfo;		}
	PXD_FORCE_INLINE	const PxcShapeData&	getShapeData()								const	{ return mShapeInfo;		}
	PXD_FORCE_INLINE	PxU32				getChangeTimestamp()						const	{ return mChangeTimestamp;	}

						void				computeBounds(const PxcTransform& transform, PxcVector& origin, PxcVector& extent);

						void				addDependantVolume(PxU16 volumeIndex);
						void				addDependantVolumes(PxcSmallIndexArray &array);
						void				removeDependantVolume(PxU16 volumeIndex);
						void				removeDependantVolumes(PxcSmallIndexArray &array);

private:
						void				markDependantVolumes();

						PxsContext*			mContext;
						PxU32				mIndex;
						PxU32				mChangeTimestamp;
						void*				mUserData;
						PxcShapeData		mShapeInfo;

						PxcSmallIndexArray	mDependantVolumes;
};

#endif

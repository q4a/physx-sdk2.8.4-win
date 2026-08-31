#ifndef PXN_SHAPE_H
#define PXN_SHAPE_H

#include "px_config.h"
#include "px_geometry.h"

class PxnShape
{
public:
	//Sphere and capsule
	virtual void				setRadiusV(PxFloat f)									= 0;
	virtual PxFloat				getRadiusV()								const		= 0;

	//Capsule
    virtual void				setHalfExtentV(PxFloat f)								= 0;
	virtual PxFloat				getHalfExtentV()							const		= 0;
	
	//Box
	virtual void				setDimensionsV(const PxdVector& v)						= 0;
	virtual const PxdVector&	getDimensionsV()							const		= 0;

	//Meshes
	virtual void				setMeshPtrV(void* ptr)									= 0;
	virtual void*				getMeshPtrV()								const		= 0;

	virtual void				setSmoothNormalsV(bool b)								= 0;
	virtual bool				getSmoothNormalsV()							const		= 0;

	//Heightfield
	virtual PxU32				getRowsV()									const		= 0;
	virtual PxU32				getColumnsV()								const		= 0;
	virtual PxU32				getStrideV()								const		= 0;
	virtual void*				getSamplesV()								const		= 0;
	virtual PxFloat				getRowScaleV()								const		= 0;
	virtual PxFloat				getColumnScaleV()							const		= 0;
	virtual PxFloat				getHeightScaleV()							const		= 0;
	virtual PxFloat				getVerticalExtentV()						const		= 0;
	virtual PxFloat				getThicknessV()								const		= 0;
	virtual PxU32				getFlagsV()									const		= 0;
	virtual PxU32				getHoleMaterialV()							const		= 0;
	virtual PxFloat				getConvexEdgeThresholdV()					const		= 0;
	virtual void				setRowsV(PxU32 rows)									= 0;
	virtual void				setColumnsV(PxU32 columns)								= 0;
	virtual void				setStrideV(PxU32 stride)								= 0;
	virtual void				setSamplesV(void* samples)								= 0;
	virtual void				setRowScaleV(PxFloat rowScale)							= 0;
	virtual void				setColumnScaleV(PxFloat columnScale)					= 0;
	virtual void				setHeightScaleV(PxFloat heightScale)					= 0;
	virtual void				setVerticalExtentV(PxFloat verticalExtent)				= 0;
	virtual void				setThicknessV(PxFloat thickness)						= 0;
	virtual void				setFlagsV(PxU32 flags)									= 0;
	virtual void				setHoleMaterialV(PxU32 holeMaterial)					= 0;
	virtual void				setConvexEdgeThresholdV(PxFloat convexEdgeThreshold)	= 0;

	virtual PxdShapeType		getTypeV()									const		= 0;

	virtual void				setUserDataV(void* data)								= 0;
	virtual void*				getUserDataV()								const		= 0;

	virtual PxdHandle			getHandleV()								const		= 0;
	virtual void				destroyV()												= 0;

protected:
	virtual						~PxnShape()												{}
};



#endif

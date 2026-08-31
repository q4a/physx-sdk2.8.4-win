#ifndef PXC_SHAPEDATA_H
#define PXC_SHAPEDATA_H

#include "px_config.h"

#include "px_convexmesh.h"
#include "px_trianglemesh.h"

#include "PxcVector.h"

typedef enum PxdShapeType_ PxdShapeType;
class PxcHeightField;


// Macro to cast to a specific shape
#define PXC_GET_SPECIFIC_SHAPE(src, dest, type)		\
	type& dest = reinterpret_cast<type&>(src);

/*!
Data-definition for a sphere shape
*/
struct PxcShapeSphere
{
	PxdShapeType		mType;
	bool				mSmoothNormals;

	PxFloat				mRadius;
};

/*!
Data-definition for a capsule shape
*/
struct PxcShapeCapsule
{
	PxdShapeType		mType;
	bool				mSmoothNormals;

	PxFloat				mRadius;
	PxFloat				mHalfExtent;
};

/*!
Data-definition for a box shape
*/
struct PxcShapeBox
{
	PxdShapeType		mType;
	bool				mSmoothNormals;

	PxcVector			mHalfSide;
};

/*!
Data-definition for a plane shape
*/
struct PxcShapePlane
{
	PxdShapeType		mType;
	bool				mSmoothNormals;

	// No other properties, we have normal=<1,0,0> D = 0
};

/*!
Data-definition for a convex mesh shape
*/
struct PxcShapeConvex
{
	PxdShapeType		mType;
	bool				mSmoothNormals;

	PxConvexMeshData*	mConvexMesh;
};

/*!
Data-definition for a triangle mesh shape
*/
struct PxcShapeMesh
{
	PxdShapeType		mType;
	bool				mSmoothNormals;

	PxInternalTriangleMeshData* mMesh;
};

/*!
Data-definition for a height field shape
*/
struct PxcShapeHeightField
{
	PxdShapeType		mType;
	bool				mSmoothNormals;

	PxcHeightField*		mHeightField;
};

// Macro to find maximum of two values
#ifndef PX_MAX
#define PX_MAX(x,y) (((x)<(y))?(y):(x))
#endif

// Get the size of the largest shape descriptor type. This will be used to generate a base shape descriptor with
// the same size. This base shape descriptor structure will act as a "super type"
#define MAX_PXC_SHAPE_DATA_SIZE PX_MAX(PX_MAX(PX_MAX(PX_MAX(PX_MAX(PX_MAX(sizeof(PxcShapeSphere), sizeof(PxcShapeCapsule)),	\
sizeof(PxcShapeBox)), sizeof(PxcShapePlane)), sizeof(PxcShapeConvex)), sizeof(PxcShapeMesh)), sizeof(PxcShapeHeightField))

/*!
Data-definition for a basic shape
*/
struct PxcShapeData
{
	PxdShapeType		mType;
	bool				mSmoothNormals;

	PxU8				pad[MAX_PXC_SHAPE_DATA_SIZE - sizeof(PxdShapeType) - sizeof(bool)];
};


#endif

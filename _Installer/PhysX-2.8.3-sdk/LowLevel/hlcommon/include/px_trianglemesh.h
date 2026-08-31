#ifndef PX_TRIANGLEMESH_H
#define PX_TRIANGLEMESH_H

#include "px_config.h"

namespace Opcode
{
	class HybridModel;
}

struct PxEdgeListData;

enum PxExtraTrigDataFlag
{
	ETD_CONVEX_EDGE_01	= (1<<3),	// PT: important value, don't change
	ETD_CONVEX_EDGE_12	= (1<<4),	// PT: important value, don't change
	ETD_CONVEX_EDGE_20	= (1<<5),	// PT: important value, don't change
};

//Data
#ifndef __SPU__
#pragma PXD_PUSH_PACK(1)
#endif
struct PxTriangle32Data
{
	PxU32	v[3];
};

typedef struct PxInternalTriangleMeshData_
{
	// [TomasJ] We don't rebuild the PhysX lib yet, so member order needs to stay the same.
#ifndef _XBOX
	// NB: moved to top of structure to unify with other shape subdata
	// this allows SPU code to just load the first 24B
	PxdVector				mAABBMin;
	PxdVector				mAABBMax;
#endif

	PxU32					mNumVertices;
	PxU32					mNumTriangles;

	PxdVector*				mVertices;
	PxTriangle32Data*		mTriangles;

	PxdVector*				mLocalVertexNormals;
	PxEdgeListData*			mEdgeListData;

	// For convex-vs-mesh
	PxU32					mNumConvexParts;		//!< Number of convex patches in the mesh
	PxU32					mNumFlatParts;			//!< Number of flat polygons in the mesh
	PxU16*					mConvexParts;			//!< Array of convex IDs
	union
	{
		PxU8*				mFlatParts8;			//!< Array of flat IDs
		PxU16*				mFlatParts16;			//!< Array of flat IDs
	};

	PxU8*					mExtraTrigData;	//one per trig
	/*
	low 3 bits (mask: 7) are the edge flags:
	b001 = 1 = ignore edge 0 = edge v0-->v1
	b010 = 2 = ignore edge 1 = edge v0-->v2
	b100 = 4 = ignore edge 2 = edge v1-->v2
	*/

	Opcode::HybridModel*	mOpcodeModel;
	PxFloat					mGeomEpsilon;

#ifdef _XBOX
	PxdVector				mAABBMin;
	PxdVector				mAABBMax;
#endif

} PxInternalTriangleMeshData;

#ifndef __SPU__
#pragma PXD_POP_PACK
#endif

#endif

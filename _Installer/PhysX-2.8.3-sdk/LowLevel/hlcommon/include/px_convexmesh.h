#ifndef PX_CONVEXMESH_H
#define PX_CONVEXMESH_H

#include "px_config.h"
#include "Opcode.h"

struct PxEdgeListData;
struct PxCollisionHullData;
struct PxSupportVertexMapData;

//Data
#pragma PXD_PUSH_PACK(1)

typedef struct PxConvexMeshData_
{
	// [TomasJ] We don't rebuild the PhysX lib yet, so member order needs to stay the same.
#ifndef _XBOX
	// NB: moved to top of structure to unify with other shape subdata
	// this allows SPU code to just load the first 24B
	PxdVector					aabbMin, aabbMax; //local bounding box
#endif

	PxEdgeListData*				edgeListData;
// PXD_SUPPORT_CONVEX_OPCODE_MODEL
	Opcode::HybridModel			mOpcodeModel;
//~PXD_SUPPORT_CONVEX_OPCODE_MODEL
	PxCollisionHullData*		hullData;
	PxSupportVertexMapData*		vertexMapData;
	PxFloat						geomEpsilon;

	// [TomasJ] We don't rebuild the PhysX lib yet, so member order needs to stay the same.
#ifdef _XBOX
	PxdVector					aabbMin, aabbMax; //local bounding box
#endif
} PxConvexMeshData;

#pragma PXD_POP_PACK

#endif

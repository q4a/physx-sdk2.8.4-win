#ifndef PX_VALENCY_H
#define PX_VALENCY_H

#include "px_config.h"

#pragma PXD_PUSH_PACK(1)

struct PxValency
{
	PxU16		mCount;
	PxU16		mOffset;
};

struct PxValenciesData
{
	PxU32			mNbVerts;		//!< Number of vertices
	PxU32			mNbAdjVerts;	//!< Total number of adjacent vertices
	PxValency*		mValencies;		//!< A list of mNbVerts valencies (= number of neighbors)
	PxU8*			mAdjacentVerts;	//!< List of adjacent vertices
};

#pragma PXD_POP_PACK

#endif

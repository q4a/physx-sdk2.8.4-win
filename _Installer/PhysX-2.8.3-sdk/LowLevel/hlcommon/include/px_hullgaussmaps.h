#ifndef PX_HULLGAUSSMAPS_H
#define PX_HULLGAUSSMAPS_H

#include "px_config.h"

// Data
#pragma PXD_PUSH_PACK(1)

struct PxHullGaussMapData
{
	PxU32	mSubdiv;
	PxU32	mNbSamples;
};

struct PxSupportVertexMapData
{
	PxHullGaussMapData	mGaussMap;

	PxU8*				mSamples;
	PxU8*				mSamples2;
};

#pragma PXD_POP_PACK

#endif

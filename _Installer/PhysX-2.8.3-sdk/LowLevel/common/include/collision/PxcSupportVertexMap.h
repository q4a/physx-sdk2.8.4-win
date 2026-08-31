#ifndef PXC_SUPPORTVERTEXMAP_H
#define PXC_SUPPORTVERTEXMAP_H

#include "px_config.h"

struct PxSupportVertexMapData;
class PxcVector;

/*!
Helper for support vertex map
*/
class PxcSupportVertexMap
{
public:
	static PxU32 ComputeNearestOffset(PxSupportVertexMapData* data, const PxcVector& dir);
};

#endif

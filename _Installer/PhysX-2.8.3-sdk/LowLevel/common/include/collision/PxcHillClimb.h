#ifndef PXC_HILLCLIMB_H
#define PXC_HILLCLIMB_H

#include "px_config.h"

class PxcVector;
struct PxValenciesData;

class PxcHillClimb
{
public:
	static bool localSearchStamps(PxU32& id, const PxcVector& dir, const PxcVector* verts, const PxValenciesData* valenciesData, PxU32 stamp, PxU32* stampsData);
};

#endif

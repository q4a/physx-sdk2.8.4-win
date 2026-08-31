#ifndef NX_NV_VRDREPORTER
#define NX_NV_VRDREPORTER
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Physics.h"

class NpShape;

struct NvVRDContact
{
	const NpShape *shape0;
	const NpShape *shape1;
	NxVec3 point;
	NxVec3 axis;
	NxReal separation;
	NxReal normalForce;
	bool normalForceAvailable;
};

struct NvVRDTrigger
{
	const NpShape *triggerShape;
	const NpShape *shape;
	NxU32 state;
};

class NvVRDReporter
{
public:
	virtual void reportContacts(NvVRDContact *contacts, NxU32 count) = 0;

	virtual void reportTriggers(NvVRDTrigger *triggers, NxU32 count) = 0;
	
	virtual void reportProfiling(NxProfilerData *profilerData) = 0;
};

#endif

#ifndef NX_PHYSICS_QUEUEDEVENT
#define NX_PHYSICS_QUEUEDEVENT
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/
#include "Nxp.h"
#include "Allocateable.h"

enum NvQueuedEventType
	{
	NX_QE_JOINT_BREAK				= 0,
	NX_QE_FLUID_EMITTER				= 1,
	NX_QE_FLUID						= 2,
	NX_QE_CLOTH_TEARING				= 3,
	NX_QE_CLOTH_DETACHING			= 4,
	NX_QE_SOFTBODY_TEARING			= 5,
	NX_QE_SOFTBODY_DETACHING		= 6,
	};

class NvQueuedEvent : public NxFoundation::NxAllocateable
	{
	public:
	NvQueuedEvent(NxU32 _type):next(NULL), type(_type) {}
	
	NvQueuedEvent * next;
	//virtual void notify() = 0;
	NxU32 type;
	};
#endif

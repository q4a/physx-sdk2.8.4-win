#ifndef NX_FRAMEWORK_PX_H
#define NX_FRAMEWORK_PX_H
/*----------------------------------------------------------------------------*\
|
|							  NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/


// Actor types
enum PxActorType
{
	PX_ACTOR_TYPE_DYNAMIC = 0,
	PX_ACTOR_TYPE_STATIC,
	PX_ACTOR_TYPE_FLUID,
	PX_ACTOR_TYPE_INACTIVE,
	PX_ACTOR_TYPE_COUNT,
};

// Volume types
enum PxVolumeType
{
	PX_VOLUME_TYPE_SHAPE = 0,
	PX_VOLUME_TYPE_FLUID_PACKET,
	PX_VOLUME_TYPE_RAWBOUNDS,
	PX_VOLUME_TYPE_PAGEBOUNDS,
	PX_VOLUME_TYPE_COUNT,
};

// Interaction types
enum PxInteractionType
{
	PX_INTERACTION_TYPE_OVERLAP_HL = 0,
	PX_INTERACTION_TYPE_OVERLAP_LL,
	PX_INTERACTION_TYPE_PAGEBOUNDS,
	PX_INTERACTION_TYPE_TRIGGER,
	PX_INTERACTION_TYPE_JOINT,
	PX_INTERACTION_TYPE_MARKER,
	PX_INTERACTION_TYPE_FLUID_BODY,
	PX_INTERACTION_TYPE_COUNT,
};


#endif

#ifndef PXN_PROPERTY_TYPES_H
#define PXN_PROPERTY_TYPES_H
#include "px_config.h"

enum PxnPropertyType
{
	PXN_PROPERTY_TYPE_INT		= 1<<0,
	PXN_PROPERTY_TYPE_FLOAT		= 1<<1,
	PXN_PROPERTY_TYPE_VEC		= 1<<2,
	PXN_PROPERTY_TYPE_QUAT		= 1<<3,
	PXN_PROPERTY_TYPE_TRANSFORM	= 1<<4,
	PXN_PROPERTY_TYPE_SHAPE		= 1<<5,
	PXN_PROPERTY_TYPE_ATOM		= 1<<6,
	PXN_PROPERTY_TYPE_MISC		= 1<<7,
	PXN_PROPERTY_TYPE_ENUMFLOAT = 1<<8,
	PXN_PROPERTY_TYPE_FLUID		= 1<<9
};

extern const PxU32 gShapePropertyType[][2];
extern const PxU32 gAtomPropertyType[][2];
extern const PxU32 gFluidPropertyType[][2];
extern const PxU32 gVolumePropertyType[][2];
extern const PxU32 gContactManagerPropertyType[][2];
extern const PxU32 gBroadPhasePropertyType[][2];
extern const PxU32 gCustomConstraintPropertyType[][2];
extern const PxU32 gContactConstraintPropertyType[][2];
extern const PxU32 gD6JointPropertyType[][2];

#endif

#ifndef PX_TYPES_H
#define PX_TYPES_H

#ifdef __cplusplus
extern "C" 
{
#endif

/************************************************************************/
/* Basic type definitions                                               */
/************************************************************************/

// Types without specific size.
typedef char PxBool;	// PT: "char" used instead of bool, for the C API. Creates a few performance warnings.
typedef float PxFloat;
typedef char PxChar;
typedef unsigned char PxUChar;
typedef char PxByte;
typedef unsigned char PxUByte;
typedef short int PxShort;
typedef unsigned short int PxUShort;
typedef int PxInt;
typedef unsigned int PxUInt;
typedef int PxIBool;	// PT: fast "integer bool" similar to BOOL
typedef char PxI8;
typedef unsigned char PxU8;
typedef short PxI16;
typedef unsigned short PxU16;
typedef int PxI32;
typedef unsigned int PxU32;
typedef unsigned long long PxU64;
typedef float PxdF32;
typedef double PxdF64;

//! Generic opaque handle
typedef PxU32 PxdHandle;

#ifndef __SPU__
#pragma PXD_PUSH_PACK(1)
#endif

// Generic math types

/*!
Basic vector in R3.
*/
typedef struct PxdVector_
{
	PxFloat x, y, z;
} PxdVector;

/*!
Combination of two vectors in R3
*/
typedef struct PxdSpatialVector_
{
	PxdVector linear, angular;
} PxdSpatialVector;

/*!
Quaternion.
Defined as q = w + ix + jy + kz = [w (x,y,z)] 
*/
typedef struct PxdQuaternion_
{
	PxFloat x, y, z, w;
} PxdQuaternion;

/*!
Transform
Describes the transition from one coordinate system to another.
*/
typedef struct PxdTransform_
{
	PxdVector translation;
	PxdQuaternion rotation;
} PxdTransform;

/*!
Plane
Describes a plane in 3D space: (n o x) + d = 0
*/
typedef struct PxdPlane_
{
	PxdVector n;
	PxFloat d;
} PxdPlane;

#ifndef __SPU__
#pragma PXD_POP_PACK
#endif


#ifdef __cplusplus
}
#endif

#include <limits.h>

#endif

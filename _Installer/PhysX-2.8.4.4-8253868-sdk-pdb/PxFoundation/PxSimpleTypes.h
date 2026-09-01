// This code contains NVIDIA Confidential Information and is disclosed 
// under the Mutual Non-Disclosure Agreement.
//
// Notice
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES 
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO 
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT, 
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable. 
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such 
// information or for any infringement of patents or other rights of third parties that may 
// result from its use. No license is granted by implication or otherwise under any patent 
// or patent rights of NVIDIA Corporation. Details are subject to change without notice. 
// This code supersedes and replaces all information previously supplied. 
// NVIDIA Corporation products are not authorized for use as critical 
// components in life support devices or systems without express written approval of 
// NVIDIA Corporation.
//
// Copyright © 2009 NVIDIA Corporation. All rights reserved.
// Copyright © 2002-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright © 2001-2006 NovodeX. All rights reserved.   


#ifndef PX_FOUNDATION_PXSIMPLETYPES_H
#define PX_FOUNDATION_PXSIMPLETYPES_H

/** \addtogroup foundation
  @{
*/

// Platform specific types:
//Design note: Its OK to use int for general loop variables and temps.

#include "PxPreprocessor.h"

namespace physx
{
namespace pubfnd2
{

#ifdef PX_WINDOWS
	typedef signed __int64		PxI64;
	typedef signed int			PxI32;
	typedef signed short		PxI16;
	typedef signed char			PxI8;

	typedef unsigned __int64	PxU64;
	typedef unsigned int		PxU32;
	typedef unsigned short		PxU16;
	typedef unsigned char		PxU8;

	typedef float				PxF32;
	typedef double				PxF64;
		
#elif defined(PX_LINUX)
	typedef signed long long	PxI64;
	typedef signed int			PxI32;
	typedef signed short		PxI16;
	typedef signed char			PxI8;

	typedef unsigned long long	PxU64;
	typedef unsigned int		PxU32;
	typedef unsigned short		PxU16;
	typedef unsigned char		PxU8;

	typedef float				PxF32;
	typedef double				PxF64;

#elif defined(PX_APPLE)
	typedef signed long long	PxI64;
	typedef signed int			PxI32;
	typedef signed short		PxI16;
	typedef signed char			PxI8;

	typedef unsigned long long	PxU64;
	typedef unsigned int		PxU32;
	typedef unsigned short		PxU16;
	typedef unsigned char		PxU8;

	typedef float				PxF32;
	typedef double				PxF64;

#elif defined(PX_PS3)
	typedef signed long long	PxI64;
	typedef signed int			PxI32;
	typedef signed short		PxI16;
	typedef signed char			PxI8;

	typedef unsigned long long	PxU64;
	typedef unsigned int		PxU32;
	typedef unsigned short		PxU16;
	typedef unsigned char		PxU8;

	typedef float				PxF32;
	typedef double				PxF64;

#elif defined(PX_X360)
	typedef signed __int64		PxI64;
	typedef signed int			PxI32;
	typedef signed short		PxI16;
	typedef signed char			PxI8;

	typedef unsigned __int64	PxU64;
	typedef unsigned int		PxU32;
	typedef unsigned short		PxU16;
	typedef unsigned char		PxU8;

	typedef float				PxF32;
	typedef double				PxF64;
    
#elif defined(PX_WII)
	typedef signed long long	PxI64;
	typedef signed int			PxI32;
	typedef signed short		PxI16;
	typedef signed char			PxI8;

	typedef unsigned long long	PxU64;
	typedef unsigned int		PxU32;
	typedef unsigned short		PxU16;
	typedef unsigned char		PxU8;

	typedef float				PxF32;
	typedef double				PxF64;

#else
	#error Unknown platform!

#endif

	PX_COMPILE_TIME_ASSERT(sizeof(PxI8)==1);
	PX_COMPILE_TIME_ASSERT(sizeof(PxU8)==1);
	PX_COMPILE_TIME_ASSERT(sizeof(PxI16)==2);
	PX_COMPILE_TIME_ASSERT(sizeof(PxU16)==2);
	PX_COMPILE_TIME_ASSERT(sizeof(PxI32)==4);
	PX_COMPILE_TIME_ASSERT(sizeof(PxU32)==4);
	PX_COMPILE_TIME_ASSERT(sizeof(PxI64)==8);
	PX_COMPILE_TIME_ASSERT(sizeof(PxU64)==8);

	// Type ranges
#define	PX_MAX_I8			0x7f			//maximum possible sbyte value
#define	PX_MIN_I8			0x80			//minimum possible sbyte value
#define	PX_MAX_U8			0xff			//maximum possible ubyte value
#define	PX_MIN_U8			0x00			//minimum possible ubyte value
#define	PX_MAX_I16			0x7fff			//maximum possible sword value
#define	PX_MIN_I16			0x8000			//minimum possible sword value
#define	PX_MAX_U16			0xffff			//maximum possible uword value
#define	PX_MIN_U16			0x0000			//minimum possible uword value
#define	PX_MAX_I32			0x7fffffff		//maximum possible sdword value
#define	PX_MIN_I32			0x80000000		//minimum possible sdword value
#define	PX_MAX_U32			0xffffffff		//maximum possible udword value
#define	PX_MIN_U32			0x00000000		//minimum possible udword value
#define	PX_MAX_F32			FLT_MAX			//maximum possible float value
#define	PX_MIN_F32			(-FLT_MAX)		//minimum possible float value
#define	PX_MAX_F64			DBL_MAX			//maximum possible double value
#define	PX_MIN_F64			(-DBL_MAX)		//minimum possible double value

#define PX_EPS_F32			FLT_EPSILON		//maximum relative error of float rounding
#define PX_EPS_F64			DBL_EPSILON		//maximum relative error of double rounding

#ifndef PX_FOUNDATION_USE_F64

	typedef PxF32			PxReal;

	#define	PX_MAX_REAL		PX_MAX_F32
	#define	PX_MIN_REAL		PX_MIN_F32
	#define PX_EPS_REAL		PX_EPS_F32

#else

	typedef PxF64			PxReal;

	#define	PX_MAX_REAL		PX_MAX_F64
	#define	PX_MIN_REAL		PX_MIN_F64
	#define PX_EPS_REAL		PX_EPS_F64

#endif

} // namespace pubfnd2
namespace pubfnd = pubfnd2;
} // namespace physx


 /** @} */

#endif

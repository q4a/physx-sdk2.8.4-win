#ifndef PXC_MATH_H
#define PXC_MATH_H

#include "PxcPlatformInclude.h"

#if defined(PXD_PLATFORM_WINDOWS)
	#include "win/PxcMath_WIN.h"
#elif defined(PXD_PLATFORM_XBOX360)
	#include "xbox360/PxcMath_XBOX.h"
#elif defined(PXD_PLATFORM_WII)
	#include "wii/PxcMath_wii.h"
#elif defined(PXD_PLATFORM_WIIU)
	#include "wiiu/PxcMath_wiiu.h"
#elif defined(PXD_PLATFORM_CELL)
	#include "ps3/PxcMath_PS3.h"
#elif defined(PXD_PLATFORM_LINUX) || defined(PXD_PLATFORM_OSX) || defined(PXD_PLATFORM_NATIVE_CLIENT) || defined(PXD_PLATFORM_ANDROID) || defined(PXD_PLATFORM_FLASH) || defined(PXD_PLATFORM_WEBGL)
	#include "linux/PxcMath_LINUX.h"
#endif



#ifdef PXD_PLATFORM_X86

	// Some float optimizations ported over from novodex.

	//returns non zero if the value is negative.
	#define PXC_IS_NEGATIVE(x) (((PxU32&)(x)) & 0x80000000)

#elif !defined (PXD_PLATFORM_WII)

	//On most platforms using the integer rep is worse(produces LHSs) since the CPU has more registers.

	//returns non zero if the value is negative.
	#define PXC_IS_NEGATIVE(x) ((x) < 0.0f)

#endif

#endif

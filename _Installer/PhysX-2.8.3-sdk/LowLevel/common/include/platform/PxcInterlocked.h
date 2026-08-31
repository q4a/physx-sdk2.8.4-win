#ifndef PXC_INTERLOCKED_H
#define PXC_INTERLOCKED_H

#include "px_config.h"
#include "PxcNoCopy.h"
#include "PxcPlatformInclude.h"

#if defined(PXD_PLATFORM_WINDOWS)
#include "win/PxcInterlocked_WIN.h"
#elif defined(PXD_PLATFORM_XBOX360)
#include "xbox360/PxcInterlocked_XBOX.h"
#elif defined(PXD_PLATFORM_LINUX) || defined(PXD_PLATFORM_OSX) || defined(PXD_PLATFORM_NATIVE_CLIENT) || defined(PXD_PLATFORM_ANDROID) || defined(PXD_PLATFORM_FLASH) || defined(PXD_PLATFORM_WEBGL)
#include "linux/PxcInterlocked_LINUX.h"
#elif defined(PXD_PLATFORM_CELL)
#include "ps3/PxcInterlocked_PS3.h"
#elif defined(PXD_PLATFORM_WII)
#include "wii/PxcInterlocked_WII.h"
#elif defined(PXD_PLATFORM_WIIU)
#include "wiiu/PxcInterlocked_WIIU.h"
#else
	#error "Define interlocked primitives for your platform"
#endif 

#endif

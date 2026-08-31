#ifndef PXC_SIMPLEMUTEX_H
#define PXC_SIMPLEMUTEX_H

#include "px_config.h"
#include "PxcNoCopy.h"
#include "PxcPlatformInclude.h"

#if defined(PXD_PLATFORM_WINDOWS)
#include "win/PxcSimpleMutex_WIN.h"
#elif defined(PXD_PLATFORM_XBOX360)
#include "xbox360/PxcSimpleMutex_XBOX.h"
#elif defined(PXD_PLATFORM_LINUX) || defined(PXD_PLATFORM_OSX) || defined(PXD_PLATFORM_NATIVE_CLIENT) || defined(PXD_PLATFORM_ANDROID) || defined(PXD_PLATFORM_FLASH) || defined(PXD_PLATFORM_WEBGL)
#include "linux/PxcSimpleMutex_LINUX.h"
#elif defined(PXD_PLATFORM_CELL)
#include "ps3/PxcSimpleMutex_PS3.h"
#elif defined(PXD_PLATFORM_WII)
#include "wii/PxcSimpleMutex_WII.h"
#elif defined(PXD_PLATFORM_WIIU)
#include "wiiu/PxcSimpleMutex_WIIU.h"
#else
	#error "Define mutex for your platform"
#endif

#endif

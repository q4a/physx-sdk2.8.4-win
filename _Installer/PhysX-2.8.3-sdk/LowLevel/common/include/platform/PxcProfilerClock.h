#ifndef PXC_PROFILERCLOCK_H
#define PXC_PROFILERCLOCK_H

#include "PxcPlatformInclude.h"

#if defined(PXD_PLATFORM_WINDOWS)
#include "win/PxcProfilerClock_WIN.h"
#elif defined(PXD_PLATFORM_XBOX360)
#include "xbox360/PxcProfilerClock_XBOX.h"
#elif defined(PXD_PLATFORM_LINUX) || defined(PXD_PLATFORM_OSX) || defined(PXD_PLATFORM_NATIVE_CLIENT) || defined(PXD_PLATFORM_ANDROID) || defined(PXD_PLATFORM_FLASH) || defined(PXD_PLATFORM_WEBGL)
#include "linux/PxcProfilerClock_LINUX.h"
#elif defined(PXD_PLATFORM_CELL)
#include "ps3/PxcProfilerClock_PS3.h"
#elif defined(PXD_PLATFORM_WII)
#include "wii/PxcProfilerClock_WII.h"
#elif defined(PXD_PLATFORM_WIIU)
#include "wiiu/PxcProfilerClock_WIIU.h"
#else
#error Write profiler clock for your platform!
#endif

#endif

#ifndef PXC_PLATFORMINCCLUDE_H
#define PXC_PLATFORMINCCLUDE_H

#include "px_config.h"

//turn off as much as we can for windows. All we really need is the thread functions(critical sections/Interlocked* etc)
#if defined(PXD_PLATFORM_WINDOWS)
#include "win/PxcPlatformInclude_WIN.h"
#elif defined(PXD_PLATFORM_XBOX360)
#include "xbox360/PxcPlatformInclude_XBOX.h"
#elif defined(PXD_PLATFORM_LINUX)
#include "linux/PxcPlatformInclude_LINUX.h"
#elif defined(PXD_PLATFORM_WII)
#include "wii/PxcPlatformInclude_WII.h"
#elif defined(PXD_PLATFORM_WIIU)
#include "wiiu/PxcPlatformInclude_WIIU.h"
#endif

#endif

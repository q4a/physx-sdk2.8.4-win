#ifndef PXC_SEMAPHORE_H
#define PXC_SEMAPHORE_H

#include "px_config.h"
#include "PxcPlatformInclude.h"

#ifdef PXD_PLATFORM_WINDOWS
#include "win/PxcSemaphore_WIN.h"
#else
#error "implement PxcSemaphore for your platform"
#endif

#endif

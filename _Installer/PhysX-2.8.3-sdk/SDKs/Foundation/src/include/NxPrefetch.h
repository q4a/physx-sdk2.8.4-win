#ifndef NX_FOUNDATION_PREFETCH
#define NX_FOUNDATION_PREFETCH

#if defined(WIN32)
#include "win/NxPrefetch_WIN.h"
#elif defined(LINUX) || defined(__APPLE__) || defined(__native_client__) || defined(__flash__) || defined(__webgl__)
#include "linux/NxPrefetch_LINUX.h"
#elif defined(_XBOX)
#include "xbox360/NxPrefetch_XBOX.h"
#elif defined(__CELLOS_LV2__)
#include "ps3/NxPrefetch_PS3.h"
#elif defined(__PPCGEKKO__)
#include "wii/NxPrefetch_WII.h"
#elif defined(CAFE)
#include "wiiu/NxPrefetch_WIIU.h"
#endif

#endif

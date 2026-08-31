#ifndef NX_FOUNDATION_PROFILER_CLOCK
#define NX_FOUNDATION_PROFILER_CLOCK

#if defined(WIN32)
#include "win/ProfilerClock_WIN.h"
#elif defined(LINUX) || defined(__APPLE__) || defined(__native_client__) || defined(__flash__) || defined(__webgl__)
#include "linux/ProfilerClock_LINUX.h"
#elif defined(_XBOX)
#include "xbox360/ProfilerClock_XBOX.h"
#elif defined(__CELLOS_LV2__)
#include "ps3/ProfilerClock_PS3.h"
#elif defined(__PPCGEKKO__)
#include "wii/ProfilerClock_WII.h"
#elif defined(CAFE)
#include "wiiu/ProfilerClock_WIIU.h"
#endif

#endif

#ifndef NX_FOUNDATION_NXINTERLOCKED_H
#define NX_FOUNDATION_NXINTERLOCKED_H

#include "Nx.h"

#if defined(WIN32)
#include "win/NxInterlocked_WIN.h"
#elif defined(LINUX) || defined(__APPLE__) || defined(__native_client__) || defined(__flash__) || defined(__webgl__)
#include "linux/NxInterlocked_LINUX.h"
#elif defined(_XBOX)
#include "xbox360/NxInterlocked_XBOX.h"
#elif defined(__CELLOS_LV2__)
#include "ps3/NxInterlocked_PS3.h"
#elif defined(__PPCGEKKO__)
#include "wii/NxInterlocked_WII.h"
#elif defined(CAFE)
#include "wiiu/NxInterlocked_WIIU.h"
#endif

#endif

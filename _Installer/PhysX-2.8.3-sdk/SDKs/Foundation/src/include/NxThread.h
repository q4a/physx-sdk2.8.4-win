#ifndef NX_FOUNDATION_NXTHREAD_H
#define NX_FOUNDATION_NXTHREAD_H
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Nx.h"

#if defined(WIN32)
#include "win/NxThread_WIN.h"
#elif defined(LINUX) || defined(__APPLE__) || defined(__native_client__)
#include "linux/NxThread_LINUX.h"
#elif defined(_XBOX)
#include "xbox360/NxThread_XBOX.h"
#elif defined(__CELLOS_LV2__)
#include "ps3/NxThread_PS3.h"
#elif defined(__PPCGEKKO__)
#include "wii/NxThread_WII.h"
#elif defined(CAFE)
#include "wiiu/NxThread_WIIU.h"
#elif defined(__flash__)
#include "flash/NxThread_Dummy.h"
#elif defined(__webgl__)
#include "webgl/NxThread_Dummy.h"
#endif

#endif 

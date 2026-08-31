/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/
#ifndef NX_PHYSICS_NXMUTEX_H
#define NX_PHYSICS_NXMUTEX_H

#if defined(WIN32)
#include "win/NxMutex_WIN.h"
#elif defined(__flash__) || defined(__webgl__)
#include "flash/NxMutex_Dummy.h"
#elif defined(LINUX) || defined(__APPLE__) || defined(__native_client__)
#include "linux/NxMutex_LINUX.h"
#elif defined(_XBOX)
#include "xbox360/NxMutex_XBOX.h"
#elif defined(__CELLOS_LV2__)
#include "ps3/NxMutex_PS3.h"
#elif defined(__PPCGEKKO__)
#include "wii/NxMutex_WII.h"
#elif defined(CAFE)
#include "wiiu/NxMutex_WIIU.h"
#endif

#endif 

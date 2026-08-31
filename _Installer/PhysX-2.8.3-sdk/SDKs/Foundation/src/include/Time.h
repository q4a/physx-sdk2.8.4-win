#ifndef NX_FOUNDATION_TIME
#define NX_FOUNDATION_TIME
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

/**
Time class.
*/

#if defined(WIN32)
#include "win/Time_WIN.h"
#elif defined(LINUX) || defined(__APPLE__) || defined(__native_client__)
#include "linux/Time_LINUX.h"
#elif defined(__flash__)
#include "flash/Time_Flash.h"
#elif defined(__webgl__)
#include "webgl/Time_WebGL.h"
#elif defined(_XBOX)
#include "xbox360/Time_XBOX.h"
#elif defined(__CELLOS_LV2__)
#include "ps3/Time_PS3.h"
#elif defined(__PPCGEKKO__)
#include "wii/Time_WII.h"
#elif defined(CAFE)
#include "wiiu/Time_WIIU.h"
#endif

#endif

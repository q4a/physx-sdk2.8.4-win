/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#ifndef MEQ_SOCKET_WIN32_IMPL_H
#define MEQ_SOCKET_WIN32_IMPL_H

#if defined(WIN32)
#include "win/meq_socket_win32.h"
#elif defined(_XBOX)
#include "xbox360/meq_socket_XBOX.h"
#elif defined(__CELLOS_LV2__)
#include "ps3/socket_cell.h"
#elif defined(LINUX) || defined(__APPLE__) || defined(__native_client__)
#include "linux/socket_linux.h"
#elif defined(__PPCGEKKO__)
#include "wii/socket_wii.h"
#elif defined(CAFE)
#include "wiiu/socket_cafe.h"
#else
#error This platform needs a socket header for the VRD
#endif

#endif

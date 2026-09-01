#ifndef NXXBOX_H
#define NXXBOX_H
/*----------------------------------------------------------------------------*\
|
|					Public Interface to NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#define DEFAULT_STACK_SIZE 262144
#define DEFAULT_OS_STACK_SIZE 65536

#define NOMINMAX
#ifndef _XBOX_MINMAX
	#define _XBOX_MINMAX
	// Xbox 360 XDK 3215 (June 06) requires that this version of min and max
	// be defined in order for xmconvert.inl to build
	inline unsigned int max(unsigned int a, unsigned int b) { (((a) > (b)) ? (a) : (b)); }
	inline unsigned int min(unsigned int a, unsigned int b) { (((a) < (b)) ? (a) : (b)); }
#endif
#include<xtl.h>
#include <Xbdm.h>


#endif
//NVIDIACOPYRIGHTBEGIN
///////////////////////////////////////////////////////////////////////////
// Copyright (c) 2010 NVIDIA Corporation
// All rights reserved. www.nvidia.com
///////////////////////////////////////////////////////////////////////////
//NVIDIACOPYRIGHTEND

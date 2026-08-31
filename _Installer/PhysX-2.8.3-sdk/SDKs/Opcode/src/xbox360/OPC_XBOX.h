#ifndef OPC_XBOX_H
#define OPC_XBOX_H

#include <malloc.h>

#ifndef NOMINMAX
	#define NOMINMAX
#endif

#ifndef _XBOX_MINMAX
	#define _XBOX_MINMAX
	// Xbox 360 XDK 3215 (June 06) requires that this version of min and max
	// be defined in order for xmconvert.inl to build
	inline unsigned int max(unsigned int a, unsigned int b) { (((a) > (b)) ? (a) : (b)); }
	inline unsigned int min(unsigned int a, unsigned int b) { (((a) < (b)) ? (a) : (b)); }
#endif

#include <xtl.h>
#include <xboxmath.h>

#endif

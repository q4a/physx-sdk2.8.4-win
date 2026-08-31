#ifndef PXC_PLATFORMINCCLUDE_XBOX_H
#define PXC_PLATFORMINCCLUDE_XBOX_H

//we could probably exclude a bunch of stuff for xbox360 as well.
#define NOMINMAX
#ifndef _XBOX_MINMAX
	#define _XBOX_MINMAX
	// Xbox 360 XDK 3215 (June 06) requires that this version of min and max
	// be defined in order for xmconvert.inl to build
	inline unsigned int max(unsigned int a, unsigned int b) { (((a) > (b)) ? (a) : (b)); }
	inline unsigned int min(unsigned int a, unsigned int b) { (((a) < (b)) ? (a) : (b)); }
#endif
#include <xtl.h>
#include <ppcintrinsics.h>
#include <xboxmath.h>

#endif

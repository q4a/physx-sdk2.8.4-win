#ifndef PHYSX_PREFIX_H
#define PHYSX_PREFIX_H

#if defined (_AMD64_) || defined (__LP64__)
#define NX64 1
#else
#define NX32 1
#endif

// Android NOTE: these defines are hardcoded in compiler/Android.mk

#define NX_DISABLE_FLUIDS 1
#define NX_DISABLE_SOFTBODY 1
#define NX_PHYSICS_STATICLIB 1
#define NX_FOUNDATION_STATICLIB 1
#define NXCHARACTER_STATIC 1
#define CORELIB 1
#define NX_USE_SDK_STATICLIBS 1
#define NX_DISABLE_HARDWARE 1
#define NX_COOKING_STATICLIB 1

// wii or flash
#if defined(__PPCGEKKO__) || defined(__flash__) || defined(__webgl__)
	#define NX_DISABLE_CLOTH
	#undef NX_ENABLE_VISUALIZE
	#define NX_DISABLE_REMOTE_DEBUG 1
#endif

// needed for u_intptr in pepper 15 sdk.
#if defined(__native_client__)
#include <stdint.h>
#endif

#endif

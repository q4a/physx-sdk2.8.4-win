/* NVIDIA/SCE CONFIDENTIAL
   NVIDIA PhysX(TM) SDK for PlayStation(R)3 Version PS3_PhysX_283_B2_forSDK330_hlsrc
*/
// SCE CONFIDENTIAL
// Copyright (C) Sony Computer Entertainment Inc.
// All Rights Reserved.

#ifndef __CELL_UTILITY_MACROS_H
#define __CELL_UTILITY_MACROS_H

#include <sys/integertypes.h>

#ifndef __CELLOS_LV2__
typedef unsigned int uint32_t;
#endif

#ifdef __CELLOS_LV2__
#ifndef NX_COMPILE_TIME_ASSERT
#define NX_COMPILE_TIME_ASSERT(exp)	extern char _CELL_NX_COMPILE_TIME_NAME(__LINE__)[ (exp) ? 1 : -1 ]
#endif
#define _CELL_NX_COMPILE_TIME_NAME(x) _CELL_NX_COMPILE_TIME_NAME_(x)
#define _CELL_NX_COMPILE_TIME_NAME_(x) NX_CompileTimeAssert ## x
#else
#define NX_COMPILE_TIME_ASSERT(exp)	extern char NX_CompileTimeAssert[ (exp) ? 1 : -1 ]
#endif


// This type pretends to be a pointer to a type on the PPU, and
// just an unsigned int on the SPU.
#ifndef CELL_PPU_POINTER
#define CELL_PPU_PTR_TYPE uint32_t

#ifdef __SPU__
#define CELL_PPU_POINTER(x) CELL_PPU_PTR_TYPE
#else // __SPU__
#define CELL_PPU_POINTER(x) x *
//#include <Nx.h>
//#include <NxAssert.h>
// Hope we never switch away from 32 bits...
// But this is here just in case...
NX_COMPILE_TIME_ASSERT(sizeof(void *)==sizeof(CELL_PPU_PTR_TYPE));
#endif // __SPU__
#endif // CELL_PPU_POINTER



// Make windows/MSVC ignore GCC specific keywords.
#ifdef WIN32
#ifndef __attribute__
#define __attribute__(x) 
#endif // __attribute__

#ifndef __builtin_prefetch
#define __builtin_prefetch(x, y) do {} while (0)
#endif // __builtin_prefetch

#ifndef __builtin_expect
#define __builtin_expect(cond, exp) (cond)
#endif // __builtin_expect
#endif // WIN32

#ifndef WIN32
#include <stdint.h>
extern bool _g_bVerbosePrintf;
#ifdef __SPU__
#define _dprintf(...) do { if (_g_bVerbosePrintf) { spu_printf( __VA_ARGS__ ); } } while (0)
#else
#define _dprintf(...) do { if (_g_bVerbosePrintf) { printf( __VA_ARGS__ ); } } while (0)
#endif
#endif // WIN32

#ifdef WIN32
#define EXPORT_SYM __declspec( dllexport )
#else
#define EXPORT_SYM 
#endif

#ifdef __CELLOS_LV2__
#define SCE_PREFETCH_128(x)  do { const char *pBase=(const char *) (x); __builtin_prefetch(pBase, 0); } while (0)
#define SCE_PREFETCH_256(x)  do { const char *pBase=(const char *) (x); __builtin_prefetch(pBase, 0); __builtin_prefetch(pBase+128, 0); } while (0)
#define SCE_PREFETCH_384(x)  do { const char *pBase=(const char *) (x); __builtin_prefetch(pBase, 0); __builtin_prefetch(pBase+128, 0); __builtin_prefetch(pBase+256, 0); } while (0)
#define SCE_PREFETCH_512(x)  do { const char *pBase=(const char *) (x); __builtin_prefetch(pBase, 0); __builtin_prefetch(pBase+128, 0); __builtin_prefetch(pBase+256, 0); __builtin_prefetch(pBase+384, 0); } while (0)
#define SCE_PREFETCH_640(x)  do { const char *pBase=(const char *) (x); __builtin_prefetch(pBase, 0); __builtin_prefetch(pBase+128, 0); __builtin_prefetch(pBase+256, 0); __builtin_prefetch(pBase+384, 0); __builtin_prefetch(pBase+512, 0); } while (0)
#define SCE_PREFETCH_768(x)  do { const char *pBase=(const char *) (x); __builtin_prefetch(pBase, 0); __builtin_prefetch(pBase+128, 0); __builtin_prefetch(pBase+256, 0); __builtin_prefetch(pBase+384, 0); __builtin_prefetch(pBase+512, 0); __builtin_prefetch(pBase+640, 0); } while (0)
#define SCE_PREFETCH_896(x)  do { const char *pBase=(const char *) (x); __builtin_prefetch(pBase, 0); __builtin_prefetch(pBase+128, 0); __builtin_prefetch(pBase+256, 0); __builtin_prefetch(pBase+384, 0); __builtin_prefetch(pBase+512, 0); __builtin_prefetch(pBase+640, 0); __builtin_prefetch(pBase+768, 0); } while (0)
#define SCE_PREFETCH_1024(x) do { const char *pBase=(const char *) (x); __builtin_prefetch(pBase, 0); __builtin_prefetch(pBase+128, 0); __builtin_prefetch(pBase+256, 0); __builtin_prefetch(pBase+384, 0); __builtin_prefetch(pBase+512, 0); __builtin_prefetch(pBase+640, 0); __builtin_prefetch(pBase+768, 0); __builtin_prefetch(pBase+896, 0); } while (0)

#define SCE_PREFETCH_WRITE_128(x)  do { const char *pBase=(const char *) (x); __builtin_prefetch(pBase, 1); } while (0)
#define SCE_PREFETCH_WRITE_256(x)  do { const char *pBase=(const char *) (x); __builtin_prefetch(pBase, 1); __builtin_prefetch(pBase+128, 1); } while (0)
#define SCE_PREFETCH_WRITE_384(x)  do { const char *pBase=(const char *) (x); __builtin_prefetch(pBase, 1); __builtin_prefetch(pBase+128, 1); __builtin_prefetch(pBase+256, 1); } while (0)
#define SCE_PREFETCH_WRITE_512(x)  do { const char *pBase=(const char *) (x); __builtin_prefetch(pBase, 1); __builtin_prefetch(pBase+128, 1); __builtin_prefetch(pBase+256, 1); __builtin_prefetch(pBase+384, 1); } while (0)
#define SCE_PREFETCH_WRITE_640(x)  do { const char *pBase=(const char *) (x); __builtin_prefetch(pBase, 1); __builtin_prefetch(pBase+128, 1); __builtin_prefetch(pBase+256, 1); __builtin_prefetch(pBase+384, 1); __builtin_prefetch(pBase+512, 1); } while (0)
#define SCE_PREFETCH_WRITE_768(x)  do { const char *pBase=(const char *) (x); __builtin_prefetch(pBase, 1); __builtin_prefetch(pBase+128, 1); __builtin_prefetch(pBase+256, 1); __builtin_prefetch(pBase+384, 1); __builtin_prefetch(pBase+512, 1); __builtin_prefetch(pBase+640, 1); } while (0)
#define SCE_PREFETCH_WRITE_896(x)  do { const char *pBase=(const char *) (x); __builtin_prefetch(pBase, 1); __builtin_prefetch(pBase+128, 1); __builtin_prefetch(pBase+256, 1); __builtin_prefetch(pBase+384, 1); __builtin_prefetch(pBase+512, 1); __builtin_prefetch(pBase+640, 1); __builtin_prefetch(pBase+768, 1); } while (0)
#define SCE_PREFETCH_WRITE_1024(x) do { const char *pBase=(const char *) (x); __builtin_prefetch(pBase, 1); __builtin_prefetch(pBase+128, 1); __builtin_prefetch(pBase+256, 1); __builtin_prefetch(pBase+384, 1); __builtin_prefetch(pBase+512, 1); __builtin_prefetch(pBase+640, 1); __builtin_prefetch(pBase+768, 1); __builtin_prefetch(pBase+896, 1); } while (0)

#else // __CELLOS_LV2__
// Todo: Implement prefetching on other platforms.
#define SCE_PREFETCH_128(x)  do {  } while (0)
#define SCE_PREFETCH_256(x)  do {  } while (0)
#define SCE_PREFETCH_384(x)  do {  } while (0)
#define SCE_PREFETCH_512(x)  do {  } while (0)
#define SCE_PREFETCH_640(x)  do {  } while (0)
#define SCE_PREFETCH_768(x)  do {  } while (0)
#define SCE_PREFETCH_896(x)  do {  } while (0)
#define SCE_PREFETCH_1024(x) do {  } while (0)

#define SCE_PREFETCH_WRITE_128(x)  do {  } while (0)
#define SCE_PREFETCH_WRITE_256(x)  do {  } while (0)
#define SCE_PREFETCH_WRITE_384(x)  do {  } while (0)
#define SCE_PREFETCH_WRITE_512(x)  do {  } while (0)
#define SCE_PREFETCH_WRITE_640(x)  do {  } while (0)
#define SCE_PREFETCH_WRITE_768(x)  do {  } while (0)
#define SCE_PREFETCH_WRITE_896(x)  do {  } while (0)
#define SCE_PREFETCH_WRITE_1024(x) do {  } while (0)
#endif // __CELLOS_LV2__
#endif



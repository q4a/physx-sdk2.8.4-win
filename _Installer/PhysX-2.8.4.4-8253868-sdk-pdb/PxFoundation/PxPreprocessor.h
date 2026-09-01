// This code contains NVIDIA Confidential Information and is disclosed 
// under the Mutual Non-Disclosure Agreement.
//
// Notice
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES 
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO 
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT, 
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable. 
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such 
// information or for any infringement of patents or other rights of third parties that may 
// result from its use. No license is granted by implication or otherwise under any patent 
// or patent rights of NVIDIA Corporation. Details are subject to change without notice. 
// This code supersedes and replaces all information previously supplied. 
// NVIDIA Corporation products are not authorized for use as critical 
// components in life support devices or systems without express written approval of 
// NVIDIA Corporation.
//
// Copyright © 2009 NVIDIA Corporation. All rights reserved.
// Copyright © 2002-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright © 2001-2006 NovodeX. All rights reserved.   

#ifndef PX_FOUNDATION_PXPREPROCESSOR_H
#define PX_FOUNDATION_PXPREPROCESSOR_H

/*! \file external top level include file for all PhysX projects */

/** \addtogroup foundation
  @{
*/

/**
List of preprocessor defines used to configure the SDK
- NDEBUG/_DEBUG: enable asserts (exactly one needs to be defined)
- PX_CHECKED: enable run time checks, mostly unused or equiv. to _DEBUG
- PX_ENABLE_PROFILER: ...
- PX_SUPPORT_VISUAL_DEBUGGER: ...
- AG_PERFMON: ...
*/

/**
Compiler define
*/
#ifdef _MSC_VER 
#	define PX_VC
#   if _MSC_VER >= 1500
#       define PX_VC9
#	elif _MSC_VER >= 1400
#		define PX_VC8
#	elif _MSC_VER >= 1300
#		define PX_VC7
#	else
#		define PX_VC6
#	endif
#elif __GNUC__
#	define PX_GNUC
#elif defined(__MWERKS__)
#	define	PX_CW
#else
#	error "Unknown compiler"
#endif

/**
Platform define
*/
#ifdef PX_VC
#	ifdef _M_IX86
#		define PX_X86
#		define PX_WINDOWS
#   elif defined(_M_X64)
#       define PX_X64
#       define PX_WINDOWS
#	elif defined(_M_PPC)
#		define PX_PPC
#		define PX_X360
#		define PX_VMX
#	else
#		error "Unknown platform"
#	endif
#elif defined PX_GNUC
#	ifdef __CELLOS_LV2__
#		define PX_PS3
#	elif defined(LINUX)
#		define PX_X86
#		define PX_LINUX
#   elif defined __APPLE__
#       define PX_APPLE
#       if defined __i386__
#           define PX_X86
#       elif defined __x86_64__
#           define PX_X64
#       elif defined __ppc__
#           define PX_PPC
#       elif defined __ppc64__
#           define PX_PPC
#           define PX_PPC64
#       endif
#	else
#		error "Unknown platform"
#	endif
#elif defined PX_CW
#	if defined(__PPCGEKKO__)
#		if defined(RVL)
#			define PX_WII
#		else
#			define PX_GC
#		endif
#	else
#		error "Unknown platform"
#	endif
#endif

/**
DLL export macros
*/
#ifndef PX_C_EXPORT
	#define PX_C_EXPORT extern "C"
#endif

/**
Calling convention
*/
#ifndef PX_CALL_CONV
#	if defined PX_WINDOWS
#		define PX_CALL_CONV __cdecl
#	else
#		define PX_CALL_CONV
#	endif
#endif

/**
Pack macros

	Set packing to 8 for both 32 and 64 bit builds, because otherwise 
	the 64 bit build cannot load the assets saved in 32 bit mode.
*/
#if defined(PX_VC)
#	define PX_PUSH_PACK_DEFAULT	__pragma( pack(push, 8) )
#	define PX_POP_PACK			__pragma( pack(pop) )
#elif defined(PX_GNUC)
#	define PX_PUSH_PACK_DEFAULT	_Pragma("pack(push, 8)")
#	define PX_POP_PACK			_Pragma("pack(pop)")
#else
#	define PX_PUSH_PACK_DEFAULT
#	define PX_POP_PACK
#endif

/**
Inline macro
*/
#if defined(PX_WINDOWS) || defined(PX_X360)
#	define PX_INLINE inline
#	pragma inline_depth( 255 )
#else
#	define PX_INLINE inline
#endif

/**
Force inline macro
*/
#if defined(PX_VC)
	#define PX_FORCE_INLINE __forceinline
#elif defined(PX_LINUX) // Workaround; Fedora Core 3 do not agree with force inline and PxcPool
	#define PX_FORCE_INLINE inline
#elif defined(PX_GNUC)
	#define PX_FORCE_INLINE inline __attribute__((always_inline))
#else
	#define PX_FORCE_INLINE inline
#endif

/**
Noinline macro
*/
#if defined PX_WINDOWS
#	define PX_NOINLINE __declspec(noinline)
#elif defined(PX_GNUC)
#	define PX_NOINLINE __attribute__ ((noinline))
#else
#	define PX_NOINLINE 
#endif

/**
Alignment macros

PX_ALIGN_PREFIX and PX_ALIGN_SUFFIX can be used for type alignment instead of aligning individual variables as follows:
PX_ALIGN_PREFIX(16)
struct A {
…
} PX_ALIGN_SUFFIX(16);
This declaration style is parsed correctly by Visual Assist.

*/
#ifndef PX_ALIGN
	#if defined(PX_VC)
		#define PX_ALIGN(alignment, decl) __declspec(align(alignment)) decl
		#define PX_ALIGN_PREFIX(alignment) __declspec(align(alignment))
		#define PX_ALIGN_SUFFIX(alignment)
	#elif defined(PX_GNUC)
		#define PX_ALIGN(alignment, decl) decl __attribute__ ((aligned(alignment)))
		#define PX_ALIGN_PREFIX(alignment)
		#define PX_ALIGN_SUFFIX(alignment) __attribute__ ((aligned(alignment)))
	#elif defined(PX_CW)
		#define PX_ALIGN(alignment, decl) decl __attribute__ ((aligned(alignment)))
		#define PX_ALIGN_PREFIX(alignment)
		#define PX_ALIGN_SUFFIX(alignment) __attribute__ ((aligned(alignment)))
	#else
		#define PX_ALIGN(alignment, decl)
		#define PX_ALIGN_PREFIX(alignment)
		#define PX_ALIGN_SUFFIX(alignment)
	#endif
#endif

/**
Deprecated marco
*/
#if 0 // set to 1 to create warnings for deprecated functions
#	define PX_DEPRECATED __declspec(deprecated)
#else 
#	define PX_DEPRECATED
#endif

// VC6 no '__FUNCTION__' workaround
#if defined PX_VC6 && !defined __FUNCTION__
#	define __FUNCTION__	"Undefined"
#endif

/**
General defines
*/

// static assert
#define PX_JOIN_HELPER( X, Y ) X##Y
#define PX_JOIN( X, Y ) PX_JOIN_HELPER( X, Y )
#define PX_COMPILE_TIME_ASSERT(exp)	extern char PX_JOIN(PxCompileTimeAssert, __COUNTER__)[ (exp) ? 1 : -1 ]

// avoid unreferenced parameter warning (why not just disable it?)
// PT: or why not just omit the parameter's name from the declaration????
#define PX_FORCE_PARAMETER_REFERENCE(_P) (void)(_P);

// check that exactly one of NDEBUG and _DEBUG is defined
#if !(defined NDEBUG ^ defined _DEBUG)
	#error Exactly one of NDEBUG and _DEBUG needs to be defined by preprocessor
#endif

// make sure PX_CHECKED is defined in all _DEBUG configurations as well
#if !defined(PX_CHECKED) && _DEBUG
#define PX_CHECKED
#endif

#ifdef __CUDACC__
#define PX_CUDA static __host__ __device__
#define PX_CUDA_MEMBER __host__ __device__
#else
#define PX_CUDA
#define PX_CUDA_MEMBER
#endif


/** @} */
#endif

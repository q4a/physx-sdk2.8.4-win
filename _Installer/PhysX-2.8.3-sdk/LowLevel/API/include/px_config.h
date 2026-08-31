#ifndef PXD_CONFIG_H
#define PXD_CONFIG_H

#ifdef __cplusplus
extern "C" 
{
#endif

//System headers
#include <float.h>
#include <stddef.h>

/*! 
\file 
Types and defines.
*/

/************************************************************************/
/* Compiler detection                                                   */
/************************************************************************/
#ifdef _MSC_VER
	#define	PXD_COMPILER_VC
#elif defined(CAFE)
	#define	PXD_COMPILER_GHS
	// Note: __GNUC__ is also defined when compiling for WiiU, so defined(CAFE) must be placed here
#elif __GNUC__
	#define PXD_COMPILER_GNUC
#elif defined(__MWERKS__)
	#define	PXD_COMPILER_CW
#else
	#error "Compiler not recognized"
#endif

#if defined __CUDACC__
#define PXD_COMPILER_CUDA
#define WORK_AROUND_CUDA_2_0_COMPILE_ISSUES 0
#endif

/*!
\def PXD_COMPILER_VC 
Set whenever Microsoft VC is detected as current compiler
*/

/************************************************************************/
/* Platform detection                                                   */
/************************************************************************/
#ifdef PXD_COMPILER_VC
	#ifdef _M_IX86
		#define PXD_PLATFORM_X86
		#define PXD_PLATFORM_WINDOWS
		#define PXD_PLATFORM_32

		#ifndef PXD_DISABLE_CUDA
			#define PXD_SUPPORT_CUDA
		#endif

		#ifndef PXD_DISABLE_PPU
			#define PXD_SUPPORT_PPU  // Disables all hardware (GPU and PPU)
		#endif
		//Disable SSE for now
		//#define PXD_SUPPORT_SSE		 // SJB - test this
		//#define PXD_SUPPORT_DYNAMICLIB // Disabled because we are rolling LowLevel into PhysXCore for 2.5
	#elif defined(_WIN64)
		#define PXD_PLATFORM_X86
		#define PXD_PLATFORM_WINDOWS
		#define PXD_PLATFORM_64

		#ifndef PXD_DISABLE_CUDA
			#define PXD_SUPPORT_CUDA
		#endif

		#ifndef PXD_DISABLE_PPU
			#define PXD_SUPPORT_PPU  // Disables all hardware (GPU and PPU)
		#endif
	#elif defined(_M_ARM)
		#define PXD_PLATFORM_ARM
		#define PXD_PLATFORM_WINDOWS
		#ifndef PXD_DISABLE_CUDA
			#define PXD_DISABLE_CUDA
		#endif
		#ifndef PXD_DISABLE_FPU
			#define PXD_DISABLE_PPU
		#endif
	#elif defined(_M_PPC)
		#define PXD_PLATFORM_PPC
		#define PXD_PLATFORM_XBOX360

		#define PXD_SUPPORT_SIMD 
	#else
		#error "Unknown platform"
	#endif
#elif defined(PXD_COMPILER_GNUC)
	#ifdef __CELLOS_LV2__
		#define PXD_PLATFORM_CELL
	#elif defined (ANDROID) && defined(__arm__)
		#define PXD_PLATFORM_ARM
		#define PXD_PLATFORM_ANDROID
	#elif defined (ANDROID) && defined(i386)
		#define PXD_PLATFORM_X86
		#define PXD_PLATFORM_ANDROID
	#elif defined(LINUX)
		#define PXD_PLATFORM_X86
		#define PXD_PLATFORM_LINUX
	#elif defined (__APPLE__) && defined(__BIG_ENDIAN__)
		#define PXD_PLATFORM_PPC
		#define PXD_PLATFORM_OSX
	#elif defined (__APPLE__) && defined(__LITTLE_ENDIAN__)
		#define PXD_PLATFORM_X86
		#define PXD_PLATFORM_OSX
	#elif defined (__native_client__)
		#define PXD_PLATFORM_X86
		#define PXD_PLATFORM_NATIVE_CLIENT
	#elif defined(__flash__)
		//#define PXD_PLATFORM_X86
		//#define PXD_PLATFORM_OSX
		#define PXD_PLATFORM_FLASH
	#elif defined(__webgl__)
		#define PXD_PLATFORM_WEBGL
	#else
		#error "Unknown platform"
	#endif
#elif defined(PXD_COMPILER_CW)
	#if defined(__PPCGEKKO__)
		#if defined(RVL)
			#define PXD_PLATFORM_WII
		#else
			#define PXD_PLATFORM_GAMECUBE
		#endif
	#else
		#error "Unknown platform"
	#endif
#elif defined(PXD_COMPILER_GHS)
	#if defined(CAFE)
		#define PXD_PLATFORM_WIIU
	#endif
#else
	#error "Unknown compiler"
#endif

/*!
\def PXD_PLATFORM_X86
Set when Intel x86 (32-bit) compatible platform is detected.
*/
/*!
\def PXD_PLATFORM_WINDOWS
Set when we are compiling for a windows target(32 or 64bit). When it makes sense to include windows.h.
*/
/*!
\def PXD_PLATFORM_XBOX360
Set when we are compiling on the xbox360. (Compiling on a power PC is not the same thing) 
*/
/*!
\def PXD_SUPPORT_PPU
Set when we wish to support the PPU, this is logically distinct from the platform we run on.
*/
/*!
\def PXD_SUPPORT_SSE
Set when the platform supports SSE
*/
/*!
\def PXD_SUPPORT_SIMD
Set when the platform supports SIMD (currently only the xbox360)
*/

/*!
\def PXD_SUPPORT_DYNAMICLIB
Set when we are building as a dynamic library(and should export functions)
*/

/************************************************************************/
/* Some general defines                                                 */
/************************************************************************/
#ifndef PXD_INLINE
	#define PXD_INLINE inline
#endif

/*!
\def PXD_INLINE
Macro for general inline functions
*/

#ifndef PXD_FORCE_INLINE
	#if defined(PXD_COMPILER_CUDA)
		#define PXD_FORCE_INLINE  __inline__
	#elif defined(PXD_COMPILER_VC)
		#define PXD_FORCE_INLINE __forceinline
	#elif defined(PXD_PLATFORM_LINUX) // Workaround; Fedora Core 3 do not agree with force inline and PxcPool
		#define PXD_FORCE_INLINE inline
	#elif defined(PXD_COMPILER_GNUC)
		#define PXD_FORCE_INLINE inline __attribute__((always_inline))
	#else
		#define PXD_FORCE_INLINE inline
	#endif
#endif

// generally CUDA-callable functions have to be static, else the compiler
// will generate an instance for every object file which comes from a CUDA
// source file...
#ifdef PXD_COMPILER_CUDA
	#define CUDA_CALLABLE static __host__ __device__
#else
	#define CUDA_CALLABLE
#endif

// ... but constructors can't be static
#ifdef PXD_COMPILER_CUDA
	#define CUDA_CALLABLE_MEMBER __host__ __device__
#else
	#define CUDA_CALLABLE_MEMBER
#endif

// ... and some functions like memcpy should only be used on host side
#ifdef PXD_COMPILER_CUDA
	#define CUDA_CALLABLE_HOST static __host__
#else
	#define CUDA_CALLABLE_HOST
#endif

// Alignment used by CUDA compiler (in bytes). 
// Note: if types that use these are contained within other types used in both
// host and device code, the offset within the containing data structure should 
// be aligned on the same byte-boundary.
#ifdef PXD_COMPILER_CUDA
	#define CUDA_ALIGN_4 __align__(4)
	#define CUDA_ALIGN_8 __align__(8)
	#define CUDA_ALIGN_16 __align__(16)
#else
	#define CUDA_ALIGN_4
	#define CUDA_ALIGN_8
	#define CUDA_ALIGN_16
#endif

/*!
\def PXD_FORCE_INLINE
Macro for functions that unconditionally should be inlined, given the
compiler supports that functionality.
*/

#ifdef PXD_BUILD_LIBRARY	
	#if defined(PXD_COMPILER_VC) && defined(PXD_SUPPORT_DYNAMICLIB)
		#define PXD_EXPORT_FUNCTION __declspec(dllexport)
	#else 
		#define PXD_EXPORT_FUNCTION 
	#endif
#else
	#if defined(PXD_COMPILER_VC) && defined(PXD_SUPPORT_DYNAMICLIB)
		#define PXD_EXPORT_FUNCTION __declspec(dllimport)
	#else 
		#define PXD_EXPORT_FUNCTION 
	#endif
#endif

/*!
\def PXD_EXPORT_FUNCTION
Modifier for externally exported functions.
*/

#if defined(PXD_PLATFORM_XBOX360) || defined(PXD_PLATFORM_CELL)
	#define PXD_RESTRICT __restrict
#elif defined(PXD_COMPILER_CW)
	#if __STDC_VERSION__ >= 199901L		// Enable C99 Extentions
		#define PXD_RESTRICT restrict
	#else
		#define PXD_RESTRICT
	#endif
#else
	#define PXD_RESTRICT
#endif

/*!
\def PXD_RESTRICT
Equal to C99 __restrict keyword on compilers that supports it
*/


#if defined(PXD_COMPILER_VC) && defined(PXD_PLATFORM_X86)

#define PXD_PUSH_PACK(x)	pack(push, x)
#define PXD_POP_PACK		pack(pop)

#else

// pack(1) causes PPC code to assume arbitary alignment, which forces integer instruction to be used to load/store floats.
#define PXD_PUSH_PACK(x)
#define PXD_POP_PACK

#endif

#ifndef PXD_ALIGN
	#if defined(PXD_COMPILER_VC)
		#define PXD_ALIGN(alignment, decl) __declspec(align(alignment)) decl
	#elif defined(PXD_COMPILER_GNUC)
		#define PXD_ALIGN(alignment, decl) decl __attribute__ ((aligned(alignment)))
	#elif defined(PXD_COMPILER_CW)
		#define PXD_ALIGN(alignment, decl) decl __attribute__ ((aligned(alignment)))
	#elif defined(PXD_COMPILER_GHS)
		#define PXD_ALIGN(alignment, decl) decl __attribute__ ((aligned(alignment)))
	#else
		#define PXD_ALIGN(alignment, decl)
	#endif
#endif

/************************************************************************/
/* Numerical defines                                                    */
/************************************************************************/
#define PXD_FLOAT_MAX		FLT_MAX
#define PXD_FLOAT_MIN		FLT_MIN
#define PXD_FLOAT_EPSILON	FLT_EPSILON
#define PXD_PI				3.1415926535897932384626433832795f

// PT: this used to be replicated everywhere in the code, causing bugs to sometimes reappear (e.g. TTP 3587).
// It is better to define it in a header and use the same constant everywhere. The original value (1e-05f)
// caused troubles (e.g. TTP 1705, TTP 306).
#define PXD_PARALLEL_TOLERANCE	1e-02f

// PT: typical "invalid" value in various CD algorithms
#define	PXC_INVALID		0xffffffff

/************************************************************************/
/* Compiler workarounds                                                 */
/************************************************************************/
#ifdef PXD_COMPILER_VC
#pragma warning(disable: 4355 )	// "this" used in base member initializer list
#pragma warning(disable: 4146 ) // unary minus operator applied to unsigned type.
#endif

#include "px_types.h"

// PT: skip initial "early exit" test when convexes were previously touching. Should be safe to always enable.
#define PXD_TEST_CONVEX_CONVEX_OPTIM_01	1

// PT: very experimental convex-convex optimization. Should be tested with real customer meshes before we keep it. Hence the define.
// Cache seperating axis
#define PXD_TEST_CONVEX_CONVEX_OPTIM_02	1

// PT: batch projection
//#define PXD_TEST_CONVEX_CONVEX_OPTIM_03	1

// Use a shared midphase cache.
#define PXD_TEST_CONVEX_MESH_OPTIM_01	1


// MS: Use a single friction constraint to process both friction axes. This approach combines the impulses along
//     the friction axes before clamping. Hence, we do not face the problems of a scheme that processes the
//     friction axes separately (in which case sliding boxes can show strange behaviour, for instance).
//
//     NOTE: Applies to the software patch friction model only.
//#define PXS_USE_COMBINED_FRICTION_AXIS_CONSTRAINT

#define PXS_ENABLE_OPT_BP_VOLUME_DELETION


#if !defined __CELLOS_LV2__
	#define PXD_COMPILE_TIME_ASSERT(exp)	extern char PXD_CompileTimeAssert[ size_t((exp) ? 1 : -1) ];
#else
    // GCC4 don't like the line above
	#define _CELL_PXD_COMPILE_TIME_NAME_(x) PXD_CompileTimeAssert ## x
	#define _CELL_PXD_COMPILE_TIME_NAME(x) _CELL_PXD_COMPILE_TIME_NAME_(x)
	#define PXD_COMPILE_TIME_ASSERT(exp) extern char _CELL_PXD_COMPILE_TIME_NAME(__LINE__)[ (exp) ? 1 : -1];
#endif


#ifdef __cplusplus
}
#endif

#endif

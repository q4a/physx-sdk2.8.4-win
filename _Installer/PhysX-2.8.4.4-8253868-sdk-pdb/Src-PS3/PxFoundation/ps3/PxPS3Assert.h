// SCE CONFIDENTIAL
// Copyright (C) Sony Computer Entertainment Inc.
// All Rights Reserved.

#ifndef __PS3_ASSERT_H__
#define __PS3_ASSERT_H__


#include <assert.h>
#include <stdio.h>

#if __SPU__
#include "spu_printf.h"
#endif

namespace physx
{
	namespace pubfnd2
	{
		PX_INLINE void PxPs3Assert(const char* exp, const char* file, int line)
		{
#if __SPU__
			spu_printf("SPU: Assertion failed! exp %s \n, line %d \n, file %s \n ", exp, line, file); 
#else
			printf("PPU: Assertion failed! exp %s \n, line %d \n, file %s \n ", exp, line, file); 
#endif
			assert(false);
		}
	}
}

#ifndef NDEBUG
#if __SPU__
#define PX_ASSERT(exp) do { if (__builtin_expect(!(exp), 0)) {physx::pubfnd2::PxPs3Assert(#exp,__FILE__,__LINE__);} } while (0) 
#else
#define PX_ASSERT(exp)		(void)( (!!(exp)) || (physx::pubfnd2::PxPs3Assert(#exp, __FILE__, __LINE__), false) )
#endif 
#else
#define PX_ASSERT(cond) ((void)0)
#endif


// Usage:
// SPU_COMPILE_TIME_ASSERT(sizeof(MyStructure) <= 128);
// Gives the following error message if it fails:
//  error: size of array `spu_compile_time_assert_failed' is negative
#define SPU_COMPILE_TIME_ASSERT(cond) extern char spu_compile_time_assert_failed[cond ? 1 : -1]

// Usage:
// SPU_COMPILE_TIME_ASSERT(sizeof(MyStructure) <= 128);
// Gives the following error message if it fails:
//  error: size of array `spu_compile_time_assert_failed' is negative
// This one makes no warnings, but it cannot be placed outside a function.
#define SPU_COMPILE_TIME_ASSERT_NOWARN(cond) do { extern char spu_compile_time_assert_failed[cond ? 1 : -1]; (void) spu_compile_time_assert_failed; } while (0)

// Usage:
// SPU_NAMED_COMPILE_TIME_ASSERT(MyStructure_is_more_than_128_bytes, sizeof(MyStructure) <= 128);
// Gives the following error message if it fails:
//  error: size of array `MyStructure_is_more_than_128_bytes' is negative
#define SPU_NAMED_COMPILE_TIME_ASSERT(name, cond) extern char name[cond ? 1 : -1]

// Usage:
// SPU_NAMED_COMPILE_TIME_ASSERT(MyStructure_is_more_than_128_bytes, sizeof(MyStructure) <= 128);
// Gives the following error message if it fails:
//  error: size of array `MyStructure_is_more_than_128_bytes' is negative
// This one makes no warnings, but it cannot be placed outside a function.
#define SPU_NAMED_COMPILE_TIME_ASSERT_NOWARN(name, cond) do { extern char name[cond ? 1 : -1]; (void) name; } while (0)


#endif

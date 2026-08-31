/*
 * Copyright 1993-2008 NVIDIA Corporation.  All rights reserved.
 *
 * NOTICE TO USER:   
 *
 * This source code is subject to NVIDIA ownership rights under U.S. and 
 * international Copyright laws.  Users and possessors of this source code 
 * are hereby granted a nonexclusive, royalty-free license to use this code 
 * in individual and commercial software.
 *
 * NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE 
 * CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR 
 * IMPLIED WARRANTY OF ANY KIND.  NVIDIA DISCLAIMS ALL WARRANTIES WITH 
 * REGARD TO THIS SOURCE CODE, INCLUDING ALL IMPLIED WARRANTIES OF 
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.
 * IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL, 
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS 
 * OF USE, DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE 
 * OR OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION WITH THE USE 
 * OR PERFORMANCE OF THIS SOURCE CODE.  
 *
 * U.S. Government End Users.   This source code is a "commercial item" as 
 * that term is defined at  48 C.F.R. 2.101 (OCT 1995), consisting  of 
 * "commercial computer  software"  and "commercial computer software 
 * documentation" as such terms are  used in 48 C.F.R. 12.212 (SEPT 1995) 
 * and is provided to the U.S. Government only as a commercial end item.  
 * Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through 
 * 227.7202-4 (JUNE 1995), all U.S. Government End Users acquire the 
 * source code with only those rights set forth herein. 
 *
 * Any use of this source code in individual and commercial software must 
 * include, in the user documentation and internal comments to the code,
 * the above Disclaimer and U.S. Government End Users Notice.
 */

#if !defined(__SM_20_INTRINSICS_H__)
#define __SM_20_INTRINSICS_H__

#if defined(__cplusplus) && defined(__CUDACC__)

#if !defined(__CUDA_ARCH__) || __CUDA_ARCH__ >= 200

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

#include "device_types.h"
#include "host_defines.h"

extern "C"
{

/*DEVICE_BUILTIN*/
extern __device__ void                   __threadfence_system(void);

/*DEVICE_BUILTIN*/
extern __device__ double                __ddiv_rn(double, double);
/*DEVICE_BUILTIN*/
extern __device__ double                __ddiv_rz(double, double);
/*DEVICE_BUILTIN*/
extern __device__ double                __ddiv_ru(double, double);
/*DEVICE_BUILTIN*/
extern __device__ double                __ddiv_rd(double, double);

/*DEVICE_BUILTIN*/
extern __device__ double                __drcp_rn(double);
/*DEVICE_BUILTIN*/
extern __device__ double                __drcp_rz(double);
/*DEVICE_BUILTIN*/
extern __device__ double                __drcp_ru(double);
/*DEVICE_BUILTIN*/
extern __device__ double                __drcp_rd(double);

/*DEVICE_BUILTIN*/
extern __device__ double                __dsqrt_rn(double);
/*DEVICE_BUILTIN*/
extern __device__ double                __dsqrt_rz(double);
/*DEVICE_BUILTIN*/
extern __device__ double                __dsqrt_ru(double);
/*DEVICE_BUILTIN*/
extern __device__ double                __dsqrt_rd(double);

/*DEVICE_BUILTIN*/
extern __device__ unsigned int          __ballot(int);
/*DEVICE_BUILTIN*/
extern __device__ int                   __syncthreads_count(int);
/*DEVICE_BUILTIN*/
extern __device__ int                   __syncthreads_and(int);
/*DEVICE_BUILTIN*/
extern __device__ int                   __syncthreads_or(int);

/*DEVICE_BUILTIN*/
extern __device__ long long int         clock64(void);

/* The following intrinsics are special versions of __fmaf_r?() that always 
 * map to a single-precision FMA instruction in non-FTZ mode, regardless of
 * the compiler's -ftz flag setting. These are supported for NVIDIA internal 
 * library work only.
 */

/*DEVICE_BUILTIN*/
extern __device__ float                 __fmaf_ieee_rn(float, float, float);
/*DEVICE_BUILTIN*/
extern __device__ float                 __fmaf_ieee_rz(float, float, float);
/*DEVICE_BUILTIN*/
extern __device__ float                 __fmaf_ieee_ru(float, float, float);
/*DEVICE_BUILTIN*/
extern __device__ float                 __fmaf_ieee_rd(float, float, float);

}

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

static __inline__ __device__ unsigned int ballot(bool pred)
{
  return __ballot((int)pred);
}

static __inline__ __device__ int syncthreads_count(bool pred)
{
  return __syncthreads_count((int)pred);
}

static __inline__ __device__ bool syncthreads_and(bool pred)
{
  return (bool)__syncthreads_and((int)pred);
}

static __inline__ __device__ bool syncthreads_or(bool pred)
{
  return (bool)__syncthreads_or((int)pred);
}

#endif /* !__CUDA_ARCH__ || __CUDA_ARCH__ >= 200 */

#elif !defined(__CUDACC__)

#include "crt/func_macro.h"

#if !defined(__CUDABE__)

/*******************************************************************************
*                                                                              *
* HOST IMPLEMENTATIONS FOR FUNCTIONS                                           *
*                                                                              *
*******************************************************************************/

#include "common_types.h"

__device_func__(void __threadfence_system(void))
{
  __syncthreads();
}

__device_func__(unsigned int __ballot(int pred))
{
  return (unsigned int)1;
}

__device_func__(int __syncthreads_count(int pred))
{
  return 1;
}

__device_func__(int __syncthreads_and(int pred))
{
  return pred;
}

__device_func__(int __syncthreads_or(int pred))
{
  return pred;
}

__device_func__(long long int clock64(void))
{
  return (long long int)__cuda_clock();
}

__device_func__(double __internal_ddiv_kernel (double x, double y,
                                               enum cudaRoundMode mode))
{
  volatile union __cudart_DoubleLonglongCvt cvt;
  unsigned long long a, b, q, sign;
  int expoa, expob, normalize, i;
    
  cvt.d = x;
  a = cvt.i;
  cvt.d = y;
  b = cvt.i;

  sign =  (a ^ b) & 0x8000000000000000ULL;
    
  expoa = ((int)(a >> 52) & 0x7ff) - 1;
  expob = ((int)(b >> 52) & 0x7ff) - 1;
    
  if (((unsigned)expoa >= 0x7fe) || ((unsigned)expob >= 0x7fe)) {
    /* handle NaNs */
    if ((a << 1) > 0xffe0000000000000ULL) {
      cvt.i = a | 0x0008000000000000ULL;
      return cvt.d;
    }
    if ((b << 1) > 0xffe0000000000000ULL) {
      cvt.i = b | 0x0008000000000000ULL;
      return cvt.d;
    }
    /* 0/0 and INF/INF ==> INDEFINITE */
    if ((((a << 1) == 0x0000000000000000ULL) &&
         ((b << 1) == 0x0000000000000000ULL)) ||
        (((a << 1) == 0xffe0000000000000ULL) &&
         ((b << 1) == 0xffe0000000000000ULL))) {
      cvt.i = 0xfff8000000000000ULL;
      return cvt.d;
    }
    /* 0/y or x/INF ==> 0 */
    if (((a << 1) == 0x0000000000000000ULL) ||
        ((b << 1) == 0xffe0000000000000ULL)) {
      cvt.i = sign;
      return cvt.d;
    }
    /* INF/y or x/0 ==> INF */
    if (((b << 1) == 0x0000000000000000ULL) ||
        ((a << 1) == 0xffe0000000000000ULL)) {
      cvt.i = sign | 0x7ff0000000000000ULL;
      return cvt.d;
    }
    if (expoa < 0) {
      a = a << 12;
      while ((long long)a > 0) {
        a = a + a;
        expoa--;
      }
      a = a >> 11;
    }
    if (expob < 0) {
      b = b << 12;
      while ((long long)b > 0) {
        b = b + b;
        expob--;
      }
      b = b >> 11;
    }
  }
  a = (a & 0x000fffffffffffffULL) | 0x0010000000000000ULL;
  b = (b & 0x000fffffffffffffULL) | 0x0010000000000000ULL;
  /* 1 <= x < 2 / 1 <= y < 2 => 0.5 < q < 2.0 */
  q = 0ULL;
  a = a - b;
  normalize = (long long)a < 0;
  for (i = 0; i < (54 + normalize); i++) {
    if ((long long)a < 0) {
      q = q + q;
      a = a + a;
      a = a + b;
    } else {
      q = q + q + 1;
      a = a + a;
      a = a - b;
    }
  }
  expoa = (expoa - expob) - normalize + 1022;
  if ((unsigned)expoa < 0x7fe) {
    /* no tie case for division if not denorm, one round bit sufficient */
    if (mode == cudaRoundNearest) {
      q++;
    } else if ((mode == cudaRoundPosInf) && (!sign)) {
      if ((q & 1) || (a != (unsigned long long)-(long long)b)) q += 2;
    } else if ((mode == cudaRoundMinInf) && (sign)) {
      if ((q & 1) || (a != (unsigned long long)-(long long)b)) q += 2; 
    }
    q = (q >> 1) & 0x000fffffffffffffULL;
    q = sign | (((long long)(expoa+1) << 52) + q);
  } else if (expoa >= 0x7fe) {
    /* overflow, return infinity or largest normal*/
    if ((mode == cudaRoundNearest) ||
        ((mode == cudaRoundPosInf) && !sign) ||
        ((mode == cudaRoundMinInf) && sign)) {
      q = sign | 0x7ff0000000000000ULL;
    } else {
      q = sign | 0x7fefffffffffffffULL;
    }
  } else {
    /* denormal results can involve tie cases, generate sticky bit */
    unsigned long long sticky;
    expoa = -expoa;
    if (expoa > 63) expoa = 63;
    /* 1 <= expoa <= 63 */
    sticky = (q << (64 - expoa)) | (a!=(unsigned long long)-(long long)b);
    q = q >> expoa;
    if (mode == cudaRoundNearest) {
      if ((q & 1) && (sticky || (q & 2))) q++;
    } else if ((mode == cudaRoundPosInf) && (!sign)) {
      if ((q & 1) || (sticky)) q += 2;
    } else if ((mode == cudaRoundMinInf) && (sign)) {
      if ((q & 1) || (sticky)) q += 2;
    }
    q = q >> 1;
    q = q | sign;
  }
  cvt.i = q;
  return cvt.d;
}

__device_func__(double __ddiv_rn(double a, double b))
{
  return __internal_ddiv_kernel (a, b, cudaRoundNearest);
} 

__device_func__(double __ddiv_rz(double a, double b))
{
  return __internal_ddiv_kernel (a, b, cudaRoundZero);
} 

__device_func__(double __ddiv_ru(double a, double b))
{
  return __internal_ddiv_kernel (a, b, cudaRoundPosInf);
} 

__device_func__(double __ddiv_rd(double a, double b))
{
  return __internal_ddiv_kernel (a, b, cudaRoundMinInf);
} 

__device_func__(double __drcp_rn(double a))
{
  return __internal_ddiv_kernel (1.0, a, cudaRoundNearest);
} 

__device_func__(double __drcp_rz(double a))
{
  return __internal_ddiv_kernel (1.0, a, cudaRoundZero);
} 

__device_func__(double __drcp_ru(double a))
{
  return __internal_ddiv_kernel (1.0, a, cudaRoundPosInf);
} 

__device_func__(double __drcp_rd(double a))
{
  return __internal_ddiv_kernel (1.0, a, cudaRoundMinInf);
} 

__device_func__(double __internal_dsqrt_kernel (double a, 
                                                enum cudaRoundMode mode))
{
  volatile union __cudart_DoubleLonglongCvt cvt;
  unsigned long long ia, manthi, mantlo;
  unsigned long long t, q, r, s;
  int expoa;
  int round, sticky, odd;
  int sign;
    
  cvt.d = a;
  ia = cvt.i;
  expoa = ((int)(ia >> 52) & 0x7ff) - 1;
  sign = (int)(ia >> 63);

  /* handle special cases */
  if (((unsigned)expoa >= 0x7fe) || sign) {
    /* handle NaNs */
    if ((ia << 1) > 0xffe0000000000000ULL) {
      cvt.i |= 0x0008000000000000ULL;
      return cvt.d;
    }
    /* arguments less than -0 */
    if (ia > 0x8000000000000000ULL) {
      cvt.i = 0xfff8000000000000ULL;
      return cvt.d;
    }
    /* handle infinities */
    if ((ia << 1) == 0xffe0000000000000ULL) {
      return cvt.d;
    }
    /* handle zeros */
    if ((ia << 1) == 0x0000000000000000ULL) {
      return cvt.d;
    }        
    /* handle denormals */
    if (expoa < 0) {
      ia = ia << 12;
      while ((long long)ia > 0) {
        ia = ia + ia;
        expoa--;
      }
      ia = ia >> 11;
    }
  }
    
  /* extract mantissa */
  ia = (ia << 11) | 0x8000000000000000ULL;
  if (!(expoa & 1)) {
    /* exponent even: shift mantissa right by 1 bit */
    ia >>= 1;
  }

  manthi = ia >> 32;
  mantlo = ia & 0xffffffffULL;
    
  /* A few Newton-Raphson iterations to get initial 16 result bits */
  t = ((manthi >> 24) | 0x100) >> 1;
  t = (expoa & 1) ? (t - 10) : t;
  q = ((manthi >> 16) / t);
  t = (((q + t) >> 1) << 8) | 0xff;
  q = manthi / t;
  t = (q + t) >> 1;
  if (t > 0xffff) t = 0xffff;

  /* compute remainder and adjust first result "digit" */
  r = manthi - t * t;
  while ((long long)r < 0) {
    t--;
    r += 2 * t + 1;
  }

  /* compute second result "digit" by longhand computation */
  s = ((r << 15) + (mantlo >> 17)) / t; 
  if (s > 0xffff) s = 0xffff;

  /* compute remainder and adjust second result "digit" */
  r = (r << 32) + mantlo;
  r = r - (t << 17) * s;
  r = r - s * s;
  t = (t << 16) + s;
  while ((long long)r < 0) {
    t--;
    r += 2 * t + 1;
  }

  /* compute third result "digit" by longhand computation */
  s = (r << 15) / t;
  if (s > 0xffff) s = 0xffff;

  /* compute remainder and adjust third result "digit" */
  r = r << 32;
  r = r - (t << 17) * s;
  r = r - s * s;
  t = (t << 16) + s;
  while ((long long)r < 0) {
    t--;
    r += 2 * t + 1;
  }
    
  /* compute fourth result "digit" by longhand computation */
  s = (r << 5) / t;
  if (s > 0x3f) s = 0x3f;

  /* compute remainder and adjust fourth result "digit" */
  r = r << 12;
  r = r - ((t << 7) + s) * s;
  t = (t << 6) + s;
  while ((long long)r < 0) {
    t--;
    r += 2 * t + 1;
  }
  
  /* prepare for rounding mantissa */
  round = (int)(t & 1);
  sticky = (r != 0ULL);    
  t = t >> 1;
  odd = (int)(t & 1);

  /* round mantissa */
  if (mode == cudaRoundNearest) {
    t += round && (sticky || odd);
  } else if (mode == cudaRoundPosInf) {
    t += round || sticky;
  }

  /* construct final result */
  expoa = (expoa >> 1) + 0x1ff;
  q = (((unsigned long long int)expoa) << 52) + t;
  cvt.i = q;
  return cvt.d;
}    

__device_func__(double __dsqrt_rn(double a))
{
  return __internal_dsqrt_kernel (a, cudaRoundNearest);
} 

__device_func__(double __dsqrt_rz(double a))
{
  return __internal_dsqrt_kernel (a, cudaRoundZero);
} 

__device_func__(double __dsqrt_ru(double a))
{
  return __internal_dsqrt_kernel (a, cudaRoundPosInf);
} 

__device_func__(double __dsqrt_rd(double a))
{
  return __internal_dsqrt_kernel (a, cudaRoundMinInf);
} 

__device_func__(float __fmaf_ieee_rn(float a, float b, float c))
{
  return __fmaf_rn(a, b, c);
}

__device_func__(float __fmaf_ieee_ru(float a, float b, float c))
{
  return __fmaf_ru(a, b, c);
}

__device_func__(float __fmaf_ieee_rd(float a, float b, float c))
{
  return __fmaf_rd(a, b, c);
}

__device_func__(float __fmaf_ieee_rz(float a, float b, float c))
{
  return __fmaf_rz(a, b, c);
}

#endif /* !defined(__CUDABE__) */ 

#endif /* __cplusplus && __CUDACC__ */

#endif /* !__SM_20_INTRINSICS_H__ */

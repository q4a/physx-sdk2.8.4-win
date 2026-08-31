/*
 * Copyright 1993-2009 NVIDIA Corporation.  All rights reserved.
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

#if !defined(__DEVICE_FUNCTIONS_H__)
#define __DEVICE_FUNCTIONS_H__

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

#if defined(__cplusplus) && defined(__CUDACC__)

#include "device_types.h"
#include "host_defines.h"

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

extern "C"
{

/*DEVICE_BUILTIN*/
extern __device__ int                    __mulhi(int, int);
/*DEVICE_BUILTIN*/
extern __device__ unsigned int           __umulhi(unsigned int, unsigned int);

/*DEVICE_BUILTIN*/
extern __device__ long long int          __mul64hi(long long int, long long int);
/*DEVICE_BUILTIN*/
extern __device__ unsigned long long int __umul64hi(unsigned long long int, unsigned long long int);

/*DEVICE_BUILTIN*/
extern __device__ float                  __int_as_float(int);
/*DEVICE_BUILTIN*/
extern __device__ int                    __float_as_int(float);


/*DEVICE_BUILTIN*/
extern __device__ void                   __syncthreads(void);
/*DEVICE_BUILTIN*/
extern __device__ void                   __prof_trigger(int);
/*DEVICE_BUILTIN*/
extern __device__ void                   __threadfence(void);
/*DEVICE_BUILTIN*/
extern __device__ void                   __threadfence_block(void);
/*DEVICE_BUILTIN*/
extern __device__ void                   __trap(void);
/*DEVICE_BUILTIN*/
extern __device__ void                   __brkpt(int);

/*DEVICE_BUILTIN*/
extern __device__ float                  __saturatef(float);

/*DEVICE_BUILTIN*/
extern __device__ unsigned int           __sad(int, int, unsigned int);
/*DEVICE_BUILTIN*/
extern __device__ unsigned int           __usad(unsigned int, unsigned int, unsigned int);

/*DEVICE_BUILTIN*/
extern __device__ int                    __mul24(int, int);
/*DEVICE_BUILTIN*/
extern __device__ unsigned int           __umul24(unsigned int, unsigned int);

/*DEVICE_BUILTIN*/
extern __device__ float                  fdividef(float, float);
/*DEVICE_BUILTIN*/
extern __device__ float                  __fdividef(float, float);

/*DEVICE_BUILTIN*/
extern __device__ double                 fdivide(double, double);

/*DEVICE_BUILTIN*/
extern __device__ float                  __sinf(float) __THROW;

/*DEVICE_BUILTIN*/
extern __device__ float                  __cosf(float) __THROW;

/*DEVICE_BUILTIN*/
extern __device__ float                  __tanf(float) __THROW;

/*DEVICE_BUILTIN*/
extern __device__ void                   __sincosf(float, float*, float*) __THROW;

/*DEVICE_BUILTIN*/
extern __device__ float                  __expf(float) __THROW;

/*DEVICE_BUILTIN*/
extern __device__ float                  __exp10f(float) __THROW;

/*DEVICE_BUILTIN*/
extern __device__ float                  __log2f(float) __THROW;

/*DEVICE_BUILTIN*/
extern __device__ float                  __log10f(float) __THROW;

/*DEVICE_BUILTIN*/
extern __device__ float                  __logf(float) __THROW;

/*DEVICE_BUILTIN*/
extern __device__ float                  __powf(float, float) __THROW;

/*DEVICE_BUILTIN*/
extern __device__ int                    __float2int_rn(float);
/*DEVICE_BUILTIN*/
extern __device__ int                    __float2int_rz(float);
/*DEVICE_BUILTIN*/
extern __device__ int                    __float2int_ru(float);
/*DEVICE_BUILTIN*/
extern __device__ int                    __float2int_rd(float);

/*DEVICE_BUILTIN*/
extern __device__ unsigned int           __float2uint_rn(float);
/*DEVICE_BUILTIN*/
extern __device__ unsigned int           __float2uint_rz(float);
/*DEVICE_BUILTIN*/
extern __device__ unsigned int           __float2uint_ru(float);
/*DEVICE_BUILTIN*/
extern __device__ unsigned int           __float2uint_rd(float);

/*DEVICE_BUILTIN*/
extern __device__ float                  __int2float_rn(int);
/*DEVICE_BUILTIN*/
extern __device__ float                  __int2float_rz(int);
/*DEVICE_BUILTIN*/
extern __device__ float                  __int2float_ru(int);
/*DEVICE_BUILTIN*/
extern __device__ float                  __int2float_rd(int);

/*DEVICE_BUILTIN*/
extern __device__ float                  __uint2float_rn(unsigned int);
/*DEVICE_BUILTIN*/
extern __device__ float                  __uint2float_rz(unsigned int);
/*DEVICE_BUILTIN*/
extern __device__ float                  __uint2float_ru(unsigned int);
/*DEVICE_BUILTIN*/
extern __device__ float                  __uint2float_rd(unsigned int);

/*DEVICE_BUILTIN*/
extern __device__ long long int          __float2ll_rn(float);
/*DEVICE_BUILTIN*/
extern __device__ long long int          __float2ll_rz(float);
/*DEVICE_BUILTIN*/
extern __device__ long long int          __float2ll_ru(float);
/*DEVICE_BUILTIN*/
extern __device__ long long int          __float2ll_rd(float);

/*DEVICE_BUILTIN*/
extern __device__ unsigned long long int __float2ull_rn(float);
/*DEVICE_BUILTIN*/
extern __device__ unsigned long long int __float2ull_rz(float);
/*DEVICE_BUILTIN*/
extern __device__ unsigned long long int __float2ull_ru(float);
/*DEVICE_BUILTIN*/
extern __device__ unsigned long long int __float2ull_rd(float);

/*DEVICE_BUILTIN*/
extern __device__ float                  __ll2float_rn(long long int);
/*DEVICE_BUILTIN*/
extern __device__ float                  __ll2float_rz(long long int);
/*DEVICE_BUILTIN*/
extern __device__ float                  __ll2float_ru(long long int);
/*DEVICE_BUILTIN*/
extern __device__ float                  __ll2float_rd(long long int);

/*DEVICE_BUILTIN*/
extern __device__ float                  __ull2float_rn(unsigned long long int);
/*DEVICE_BUILTIN*/
extern __device__ float                  __ull2float_rz(unsigned long long int);
/*DEVICE_BUILTIN*/
extern __device__ float                  __ull2float_ru(unsigned long long int);
/*DEVICE_BUILTIN*/
extern __device__ float                  __ull2float_rd(unsigned long long int);

/*DEVICE_BUILTIN*/
extern __device__ unsigned short         __float2half_rn(float);
/*DEVICE_BUILTIN*/
extern __device__ float                  __half2float(unsigned short);

/*DEVICE_BUILTIN*/
extern __device__ float                  __fadd_rn(float, float);
/*DEVICE_BUILTIN*/
extern __device__ float                  __fadd_rz(float, float);
/*DEVICE_BUILTIN*/
extern __device__ float                  __fadd_ru(float, float);
/*DEVICE_BUILTIN*/
extern __device__ float                  __fadd_rd(float, float);

/*DEVICE_BUILTIN*/
extern __device__ float                  __fmul_rn(float, float);
/*DEVICE_BUILTIN*/
extern __device__ float                  __fmul_rz(float, float);
/*DEVICE_BUILTIN*/
extern __device__ float                  __fmul_ru(float, float);
/*DEVICE_BUILTIN*/
extern __device__ float                  __fmul_rd(float, float);

/*DEVICE_BUILTIN*/
extern __device__ float                  __fmaf_rn(float, float, float);
/*DEVICE_BUILTIN*/
extern __device__ float                  __fmaf_rz(float, float, float);
/*DEVICE_BUILTIN*/
extern __device__ float                  __fmaf_ru(float, float, float);
/*DEVICE_BUILTIN*/
extern __device__ float                  __fmaf_rd(float, float, float);

/*DEVICE_BUILTIN*/
extern __device__ float                  __frcp_rn(float);
/*DEVICE_BUILTIN*/
extern __device__ float                  __frcp_rz(float);
/*DEVICE_BUILTIN*/
extern __device__ float                  __frcp_ru(float);
/*DEVICE_BUILTIN*/
extern __device__ float                  __frcp_rd(float);

/*DEVICE_BUILTIN*/
extern __device__ float                  __fsqrt_rn(float);
/*DEVICE_BUILTIN*/
extern __device__ float                  __fsqrt_rz(float);
/*DEVICE_BUILTIN*/
extern __device__ float                  __fsqrt_ru(float);
/*DEVICE_BUILTIN*/
extern __device__ float                  __fsqrt_rd(float);

/*DEVICE_BUILTIN*/
extern __device__ float                  __fdiv_rn(float, float);
/*DEVICE_BUILTIN*/
extern __device__ float                  __fdiv_rz(float, float);
/*DEVICE_BUILTIN*/
extern __device__ float                  __fdiv_ru(float, float);
/*DEVICE_BUILTIN*/
extern __device__ float                  __fdiv_rd(float, float);

/*DEVICE_BUILTIN*/
extern __device__ int                    __clz(int);
/*DEVICE_BUILTIN*/
extern __device__ int                    __ffs(int);
/*DEVICE_BUILTIN*/
extern __device__ int                    __popc(unsigned int);
/*DEVICE_BUILTIN*/
extern __device__ unsigned int           __brev(unsigned int);

/*DEVICE_BUILTIN*/
extern __device__ int                    __clzll(long long int);
/*DEVICE_BUILTIN*/
extern __device__ int                    __ffsll(long long int);
/*DEVICE_BUILTIN*/
extern __device__ int                    __popcll(unsigned long long int);
/*DEVICE_BUILTIN*/
extern __device__ unsigned long long int __brevll(unsigned long long int);

#if !defined(__CUDA_ARCH__) || __CUDA_ARCH__ >= 130

/*DEVICE_BUILTIN*/
extern __device__ int                    __double2int_rz(double);

/*DEVICE_BUILTIN*/
extern __device__ unsigned int           __double2uint_rz(double);

/*DEVICE_BUILTIN*/
extern __device__ long long int          __double2ll_rz(double);

/*DEVICE_BUILTIN*/
extern __device__ unsigned long long int __double2ull_rz(double);

#endif /* !__CUDA_ARCH__ || __CUDA_ARCH__ >= 130 */

}

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

static __inline__ __device__ int mulhi(int a, int b)
{
  return __mulhi(a, b);
}

static __inline__ __device__ unsigned int mulhi(unsigned int a, unsigned int b)
{
  return __umulhi(a, b);
}

static __inline__ __device__ unsigned int mulhi(int a, unsigned int b)
{
  return __umulhi((unsigned int)a, b);
}

static __inline__ __device__ unsigned int mulhi(unsigned int a, int b)
{
  return __umulhi(a, (unsigned int)b);
}

static __inline__ __device__ long long int mul64hi(long long int a, long long int b)
{
  return __mul64hi(a, b);
}

static __inline__ __device__ unsigned long long int mul64hi(unsigned long long int a, unsigned long long int b)
{
  return __umul64hi(a, b);
}

static __inline__ __device__ unsigned long long int mul64hi(long long int a, unsigned long long int b)
{
  return __umul64hi((unsigned long long int)a, b);
}

static __inline__ __device__ unsigned long long int mul64hi(unsigned long long int a, long long int b)
{
  return __umul64hi(a, (unsigned long long int)b);
}

static __inline__ __device__ int float_as_int(float a)
{
  return __float_as_int(a);
}

static __inline__ __device__ float int_as_float(int a)
{
  return __int_as_float(a);
}

static __inline__ __device__ float saturate(float a)
{
  return __saturatef(a);
}

static __inline__ __device__ int mul24(int a, int b)
{
  return __mul24(a, b);
}

static __inline__ __device__ unsigned int umul24(unsigned int a, unsigned int b)
{
  return __umul24(a, b);
}

static __inline__ __device__ void trap(void)
{
  __trap();
}

static __inline__ __device__ void brkpt(int c)
{
  __brkpt(c);
}

static __inline__ __device__ void syncthreads(void)
{
  __syncthreads();
}

static __inline__ __device__ void prof_trigger(int e)
{
       if (e ==  0) __prof_trigger( 0);
  else if (e ==  1) __prof_trigger( 1);
  else if (e ==  2) __prof_trigger( 2);
  else if (e ==  3) __prof_trigger( 3);
  else if (e ==  4) __prof_trigger( 4);
  else if (e ==  5) __prof_trigger( 5);
  else if (e ==  6) __prof_trigger( 6);
  else if (e ==  7) __prof_trigger( 7);
  else if (e ==  8) __prof_trigger( 8);
  else if (e ==  9) __prof_trigger( 9);
  else if (e == 10) __prof_trigger(10);
  else if (e == 11) __prof_trigger(11);
  else if (e == 12) __prof_trigger(12);
  else if (e == 13) __prof_trigger(13);
  else if (e == 14) __prof_trigger(14);
  else if (e == 15) __prof_trigger(15);
}

static __inline__ __device__ void threadfence(bool global = true)
{
  global ? __threadfence() : __threadfence_block();
}

static __inline__ __device__ int float2int(float a, enum cudaRoundMode mode = cudaRoundZero)
{
  return mode == cudaRoundNearest ? __float2int_rn(a) :
         mode == cudaRoundPosInf  ? __float2int_ru(a) :
         mode == cudaRoundMinInf  ? __float2int_rd(a) :
                                    __float2int_rz(a);
}

static __inline__ __device__ unsigned int float2uint(float a, enum cudaRoundMode mode = cudaRoundZero)
{
  return mode == cudaRoundNearest ? __float2uint_rn(a) :
         mode == cudaRoundPosInf  ? __float2uint_ru(a) :
         mode == cudaRoundMinInf  ? __float2uint_rd(a) :
                                    __float2uint_rz(a);
}

static __inline__ __device__ float int2float(int a, enum cudaRoundMode mode = cudaRoundNearest)
{
  return mode == cudaRoundZero   ? __int2float_rz(a) :
         mode == cudaRoundPosInf ? __int2float_ru(a) :
         mode == cudaRoundMinInf ? __int2float_rd(a) :
                                   __int2float_rn(a);
}

static __inline__ __device__ float uint2float(unsigned int a, enum cudaRoundMode mode = cudaRoundNearest)
{
  return mode == cudaRoundZero   ? __uint2float_rz(a) :
         mode == cudaRoundPosInf ? __uint2float_ru(a) :
         mode == cudaRoundMinInf ? __uint2float_rd(a) :
                                   __uint2float_rn(a);
}

#elif !defined(__CUDACC__)

#include "crt/func_macro.h"

#include "host_defines.h"
#include "math_constants.h"

#if defined(__CUDABE__)

#if (__CUDA_ARCH__ < 200)

__device_func__(float __frcp_rn (float x))
{
  unsigned int expo;
  unsigned f, y;
  unsigned int argi;
  float t;
    
  argi = __float_as_int(x);
  expo = (argi >> 23);
  expo = expo & 0xff;
  f = expo - 1;
  if (f <= 0xFD) {
    y = (argi & 0x00ffffff) | 0x00800000;
    expo = (2 * 127) - expo - 2;
    t = 1.0f / x;
    argi = __float_as_int(t);
    argi = (argi & 0x00ffffff) | 0x00800000;
    if ((int)expo >= 0) {
      /* compute remainder1 */
      f = __umul24(y, argi);
      /* remainder1 must be negative. Fix if neccessary */
      if ((int)f > 0) { 
        t = __int_as_float(__float_as_int(t)-1);
        f -= y; 
      }
      /* compute remainder2 */
      expo = f + y;
      /* round result based on which remainder is smaller in magnitude */
      f = (unsigned)(-(int)f);
      if (expo < f) {
        t = __int_as_float(__float_as_int(t)+1);
      }
      return t;
    }
  }
  return 1.0f / x;
}
 
__device_func__(float __frcp_rz (float x))
{
  unsigned int expo;
  unsigned f, y;
  unsigned int argi;
  float t;
    
  argi = __float_as_int(x);
  expo = (argi >> 23);
  expo = expo & 0xff;
  f = expo - 1;
  if (f <= 0xFD) {
    y = (argi & 0x00ffffff) | 0x00800000;
    expo = (2 * 127) - expo - 2;
    t = 1.0f / x;
    argi = __float_as_int(t);
    argi = (argi & 0x00ffffff) | 0x00800000;
    if ((int)expo >= 0) {
      f = __umul24(y, argi);
      if ((int)f > 0) { 
        t = __int_as_float(__float_as_int(t)-1);
      }
      return t;
    }
  }
  return 1.0f / x;
}

__device_func__(float __frcp_rd (float x))
{
  unsigned int expo;
  unsigned f, y;
  unsigned int argi;
  float t;
    
  argi = __float_as_int(x);
  expo = (argi >> 23);
  expo = expo & 0xff;
  f = expo - 1;
  if (f <= 0xFD) {
    y = (argi & 0x00ffffff) | 0x00800000;
    expo = (2 * 127) - expo - 2;
    t = 1.0f / x;
    argi = __float_as_int(t);
    argi = (argi & 0x00ffffff) | 0x00800000;
    if ((int)expo >= 0) {
      f = __umul24(y, argi);
      if (((int)f > 0) && (x > 0.0f)) { 
        t = __int_as_float(__float_as_int(t)-1);
      }
      if (((int)f < 0) && (x < 0.0f)) { 
        t = __int_as_float(__float_as_int(t)+1);
      }
      return t;
    }
  }
  return 1.0f / x;
}

__device_func__(float __frcp_ru (float x))
{
  unsigned int expo;
  unsigned f, y;
  unsigned int argi;
  float t;
    
  argi = __float_as_int(x);
  expo = (argi >> 23);
  expo = expo & 0xff;
  f = expo - 1;
  if (f <= 0xFD) {
    y = (argi & 0x00ffffff) | 0x00800000;
    expo = (2 * 127) - expo - 2;
    t = 1.0f / x;
    argi = __float_as_int(t);
    argi = (argi & 0x00ffffff) | 0x00800000;
    if ((int)expo >= 0) {
      f = __umul24(y, argi);
      if (((int)f > 0) && (x < 0.0f)) { 
        t = __int_as_float(__float_as_int(t)-1);
      }
      if (((int)f < 0) && (x > 0.0f)) { 
        t = __int_as_float(__float_as_int(t)+1);
      }
      return t;
    }
  }
  return 1.0f / x;
}

__device_func__(float __fsqrt_rn (float radicand))
{
  unsigned int expo, argi;
  unsigned int s, f, x;

  argi = __float_as_int(radicand);
  expo = argi >> 23;
  expo = expo & 0xff;
  f = expo - 1;

  if ((argi <= 0x80000000) && (f <= 0xFD)) {
    x = (argi << 8) | 0x80000000;
    x = x >> (expo & 1);
    argi = (((__float_as_int(rsqrtf(__int_as_float(
              __float_as_int(radicand)|1)))&0x00ffffff)|0x00800000)<<7);
    /* second NR iteration */
    s = __umulhi(argi,argi);
    f = 0x30000000 - __umulhi(x,s);
    argi = __umulhi(f,argi);
    /* compute sqrt_rn(x) as x * 1/sqrt_rn(x) */
    argi = __umulhi(x,argi);
    argi = argi >> 3;
    x = (x << 16) - (argi * argi);
    /* round to nearest based on remainder; tie case impossible */
    f = x - (2 * argi + 1);
    if ((int)f < 0) f = (unsigned)(-(int)f);
    if ((int)x < 0) x = (unsigned)(-(int)x);
    if (f < x) argi ++;
    argi = argi + (((expo + 125) & ~0x1) << 22);
    return __int_as_float(argi);
  }
  return sqrtf(radicand);
}

__device_func__(float __fsqrt_rz (float radicand))
{
  unsigned int expo, argi;
  unsigned int s, f, x;

  argi = __float_as_int(radicand);
  expo = argi >> 23;
  expo = expo & 0xff;
  f = expo - 1;

  if ((argi <= 0x80000000) && (f <= 0xFD)) {
    x = (argi << 8) | 0x80000000;
    x = x >> (expo & 1);
    argi = (((__float_as_int(rsqrtf(__int_as_float(
              __float_as_int(radicand)|1)))&0x00ffffff)|0x00800000)<<7);
    /* NR iteration */
    s = __umulhi(argi,argi);
    f = 0x30000000 - __umulhi(x,s);
    argi = __umulhi(f,argi);
    /* compute sqrt_rz(x) as x * 1/sqrt_rz(x) */
    argi = __umulhi(x,argi);
    /* compute truncated result */
    argi = (argi + 4) >> 3;
    x = (x << 16) - (argi * argi);
    if ((int)x < 0) argi--;
    argi = argi + (((expo + 125) & ~0x1) << 22);
    return __int_as_float(argi);
  }
  return sqrtf(radicand);
}

__device_func__(float __fsqrt_ru (float radicand))
{
  unsigned int expo, argi;
  unsigned int s, f, x;

  argi = __float_as_int(radicand);
  expo = argi >> 23;
  expo = expo & 0xff;
  f = expo - 1;

  if ((argi <= 0x80000000) && (f <= 0xFD)) {
    x = (argi << 8) | 0x80000000;
    x = x >> (expo & 1);
    argi = (((__float_as_int(rsqrtf(__int_as_float(
              __float_as_int(radicand)|1)))&0x00ffffff)|0x00800000)<<7);
    /* NR iteration */
    s = __umulhi(argi,argi);
    f = 0x30000000 - __umulhi(x,s);
    argi = __umulhi(f,argi);
    /* compute sqrt_ru(x) as x * 1/sqrt_ru(x) */
    argi = __umulhi(x,argi);
    argi = (argi + 4) >> 3;
    x = (x << 16) - (argi * argi);
    if ((int)x > 0) argi++;
    argi = argi + (((expo + 125) & ~0x1) << 22);
    return __int_as_float(argi);
  }
  return sqrtf(radicand);
}

__device_func__(float __fsqrt_rd (float radicand))
{
  unsigned int expo, argi;
  unsigned int s, f, x;

  argi = __float_as_int(radicand);
  expo = argi >> 23;
  expo = expo & 0xff;
  f = expo - 1;

  if ((argi <= 0x80000000) && (f <= 0xFD)) {
    x = (argi << 8) | 0x80000000;
    x = x >> (expo & 1);
    argi = (((__float_as_int(rsqrtf(__int_as_float(
              __float_as_int(radicand)|1)))&0x00ffffff)|0x00800000)<<7);
    /* NR iteration */
    s = __umulhi(argi,argi);
    f = 0x30000000 - __umulhi(x,s);
    argi = __umulhi(f,argi);
    /* compute sqrt_rd(x) as x * 1/sqrt_rd(x) */
    argi = __umulhi(x,argi);
    /* compute truncated result */
    argi = (argi + 4) >> 3;
    x = (x << 16) - (argi * argi);
    if ((int)x < 0) argi--;
    argi = argi + (((expo + 125) & ~0x1) << 22);
    return __int_as_float(argi);
  }
  return sqrtf(radicand);
}

__device_func__(float __fdiv_rn (float dividend, float divisor))
{
  unsigned long long prod;
  unsigned r, f, x, y, expox, expoy, sign;
  unsigned expo_res;
  unsigned resi, cvtxi, cvtyi;
  float t;

  cvtxi = __float_as_int(dividend);
  cvtyi = __float_as_int(divisor);
  expox = (cvtxi >> 23) & 0xff;
  expoy = (cvtyi >> 23) & 0xff;
  sign  = ((cvtxi ^ cvtyi) & 0x80000000);

  if (((expox - 1) <= 0xFD) && ((expoy - 1) <= 0xFD)) {
    expo_res = expox - expoy + 127 - 1;
    /* extract mantissas */
    y = (cvtyi << 8) | 0x80000000;
    x = (cvtxi & 0x00ffffff) | 0x00800000;
    t =__int_as_float((cvtyi & 0x00ffffff) | 0x3f800001);
    r = ((__float_as_int(1.0f / t) & 0x00ffffff) | 0x00800000) << 7;
    /* NR iteration */  
    f = (unsigned)-(int)__umulhi (y, r << 1);
    r = __umulhi (f, r << 1);
    /* produce quotient */
    prod = ((unsigned long long)x) * (r << 1);
    /* normalize mantissa */
    if (((int)((prod >> 32) << 8)) > 0) {
      expo_res--;
      prod = prod + prod;
    }
    /* preliminary mantissa */
    r = (unsigned)(prod >> 32);
    y = y >> 8;
    /* result is a normal */
    if (expo_res <= 0xFD) {
      int rem0, rem1, inc;
      /* round mantissa to nearest even */
      prod = ((unsigned long long)y) * r;
      x = x << (23 + ((prod >> 32) >> 15));
      rem1 = x - (unsigned)(prod & 0xffffffff);
      rem0 = rem1 - y;
      inc = abs(rem0) < abs(rem1);
      /* merge sign, mantissa, exponent for final result */
      resi = sign | ((expo_res << 23) + r + inc);
      return __int_as_float(resi);
    } else if ((int)expo_res >= 254) {
      /* overflow: return infinity */
      resi = sign | 0x7f800000;
      return __int_as_float(resi);
    } else {
      /* underflow, may still round to normal */
      int rem0, rem1, inc;
      prod = ((unsigned long long)y) * r;
      x = x << (23 + ((prod >> 32) >> 15));
      rem1 = x - (unsigned)(prod & 0xffffffff);
      rem0 = rem1 - y;
      inc = abs(rem0) < abs(rem1);
      resi = ((expo_res << 23) + r + inc);
      if (resi != 0x00800000) resi = 0;
      return __int_as_float(sign | resi);
    }
  }
  if (__cuda_fabsf(divisor) > CUDART_TWO_TO_126_F) {
    divisor  *= 0.25f;
    dividend *= 0.25f;
  }
  return __fdividef (dividend, divisor);
}

__device_func__(float __fdiv_rz (float dividend, float divisor))
{
  unsigned long long prod;
  unsigned r, f, x, y, expox, expoy, sign;
  unsigned expo_res;
  unsigned resi, cvtxi, cvtyi;
  float t;

  cvtxi = __float_as_int(dividend);
  cvtyi = __float_as_int(divisor);
  expox = (cvtxi >> 23) & 0xff;
  expoy = (cvtyi >> 23) & 0xff;
  sign  = ((cvtxi ^ cvtyi) & 0x80000000);

  if (((expox - 1) <= 0xFD) && ((expoy - 1) <= 0xFD)) {
    expo_res = expox - expoy + 127 - 1;
    /* extract mantissas */
    y = (cvtyi << 8) | 0x80000000;
    x = (cvtxi & 0x00ffffff) | 0x00800000;
    t =__int_as_float((cvtyi & 0x00ffffff) | 0x3f800001);
    r = ((__float_as_int(1.0f / t) & 0x00ffffff) | 0x00800000) << 7;
    /* NR iteration */  
    f = (unsigned)-(int)__umulhi (y, r << 1);
    r = __umulhi (f, r << 1);
    /* produce quotient */
    prod = ((unsigned long long)x) * (r << 1);
    /* normalize mantissa */
    if (((int)((prod >> 32) << 8)) > 0) {
      expo_res--;
      prod = prod + prod;
    }
    /* preliminary mantissa */
    prod += 0x0000000080000000ULL;
    r = (unsigned)(prod >> 32);
    y = y >> 8;
    if (expo_res <= 0xFD) {
      /* result is a normal */
      int rem1;
      prod = ((unsigned long long)y) * r;
      x = x << (23 + ((prod >> 32) >> 15));
      rem1 = x - (unsigned)(prod & 0xffffffff);
      if (rem1 < 0) r--;
      resi = (expo_res << 23) + r;
      if (resi == 0x7f800000) resi = 0x7f7fffff;
      return __int_as_float(sign | resi);
    } else if ((int)expo_res >= 254) {
      /* overflow: return largest normal */
      resi = 0x7f7fffff;
      return __int_as_float(sign |resi);
    } else {
      /* underflow: result is smallest normal or zero */
      int rem1;
      prod = ((unsigned long long)y) * r;
      x = x << (23 + ((prod >> 32) >> 15));
      rem1 = x - (unsigned)(prod & 0xffffffff);
      if (rem1 < 0) r--;
      resi = ((expo_res << 23) + r);
      if (resi != 0x00800000) resi = 0;
      return __int_as_float(sign | resi);
    }
  }
  if (__cuda_fabsf(divisor) > CUDART_TWO_TO_126_F) {
    divisor  *= 0.25f;
    dividend *= 0.25f;
  }
  return __fdividef (dividend, divisor);
}

__device_func__(float __fdiv_ru (float dividend, float divisor))
{
  unsigned long long prod;
  unsigned r, f, x, y, expox, expoy, sign;
  unsigned expo_res;
  unsigned resi, cvtxi, cvtyi;
  float t;

  cvtxi = __float_as_int(dividend);
  cvtyi = __float_as_int(divisor);
  expox = (cvtxi >> 23) & 0xff;
  expoy = (cvtyi >> 23) & 0xff;
  sign  = ((cvtxi ^ cvtyi) & 0x80000000);

  if (((expox - 1) <= 0xFD) && ((expoy - 1) <= 0xFD)) {
    expo_res = expox - expoy + 127 - 1;
    /* extract mantissas */
    y = (cvtyi << 8) | 0x80000000;
    x = (cvtxi & 0x00ffffff) | 0x00800000;
    t =__int_as_float((cvtyi & 0x00ffffff) | 0x3f800001);
    r = ((__float_as_int(1.0f / t) & 0x00ffffff) | 0x00800000) << 7;
    /* NR iteration */  
    f = (unsigned)-(int)__umulhi (y, r << 1);
    r = __umulhi (f, r << 1);
    /* produce quotient */
    prod = ((unsigned long long)x) * (r << 1);
    /* normalize mantissa */
    if (((int)((prod >> 32) << 8)) > 0) {
      expo_res--;
      prod = prod + prod;
    }
    /* preliminary mantissa */
    prod += 0x0000000080000000ULL;
    r = (unsigned)(prod >> 32);
    y = y >> 8;
    if (expo_res <= 0xFD) {
      /* result is a normal */
      int rem1;
      prod = ((unsigned long long)y) * r;
      x = x << (23 + ((prod >> 32) >> 15));
      rem1 = x - (unsigned)(prod & 0xffffffff);
      if ((rem1 < 0) &&  (sign)) r--;
      if ((rem1 > 0) && (!sign)) r++;
      resi = (expo_res << 23) + r;
      if ((resi == 0x7f800000) && (sign)) resi = 0x7f7fffff;
      return __int_as_float(sign | resi);
    } else if ((int)expo_res >= 254) {
      /* overflow: return largest normal */
      resi = sign ? 0x7f7fffff : 0x7f800000;
      return __int_as_float(sign | resi);
    } else {
      /* underflow: result is smallest normal or zero */
      int rem1;
      prod = ((unsigned long long)y) * r;
      x = x << (23 + ((prod >> 32) >> 15));
      rem1 = x - (unsigned)(prod & 0xffffffff);
      if ((rem1 < 0) &&  (sign)) r--;
      if ((rem1 > 0) && (!sign)) r++;
      resi = ((expo_res << 23) + r);
      if (resi != 0x00800000) resi = 0;
      return __int_as_float(sign | resi);
    }
  }
  if (__cuda_fabsf(divisor) > CUDART_TWO_TO_126_F) {
    divisor  *= 0.25f;
    dividend *= 0.25f;
  }
  return __fdividef (dividend, divisor);
}

__device_func__(float __fdiv_rd (float dividend, float divisor))
{
  unsigned long long prod;
  unsigned r, f, x, y, expox, expoy, sign;
  unsigned expo_res;
  unsigned resi, cvtxi, cvtyi;
  float t;

  cvtxi = __float_as_int(dividend);
  cvtyi = __float_as_int(divisor);
  expox = (cvtxi >> 23) & 0xff;
  expoy = (cvtyi >> 23) & 0xff;
  sign  = ((cvtxi ^ cvtyi) & 0x80000000);

  if (((expox - 1) <= 0xFD) && ((expoy - 1) <= 0xFD)) {
    expo_res = expox - expoy + 127 - 1;
    /* extract mantissas */
    y = (cvtyi << 8) | 0x80000000;
    x = (cvtxi & 0x00ffffff) | 0x00800000;
    t =__int_as_float((cvtyi & 0x00ffffff) | 0x3f800001);
    r = ((__float_as_int(1.0f / t) & 0x00ffffff) | 0x00800000) << 7;
    /* NR iteration */  
    f = (unsigned)-(int)__umulhi (y, r << 1);
    r = __umulhi (f, r << 1);
    /* produce quotient */
    prod = ((unsigned long long)x) * (r << 1);
    /* normalize mantissa */
    if (((int)((prod >> 32) << 8)) > 0) {
      expo_res--;
      prod = prod + prod;
    }
    /* preliminary mantissa */
    prod += 0x0000000080000000ULL;
    r = (unsigned)(prod >> 32);
    y = y >> 8;
    if (expo_res <= 0xFD) {
      /* result is a normal */
      int rem1;
      prod = ((unsigned long long)y) * r;
      x = x << (23 + ((prod >> 32) >> 15));
      rem1 = x - (unsigned)(prod & 0xffffffff);
      if ((rem1 < 0) && (!sign)) r--;
      if ((rem1 > 0) &&  (sign)) r++;
      resi = (expo_res << 23) + r;
      if ((resi == 0x7f800000) && (!sign)) resi = 0x7f7fffff;
      return __int_as_float(sign | resi);
    } else if ((int)expo_res >= 254) {
      /* overflow: return largest normal */
      resi = sign ? 0x7f800000 : 0x7f7fffff;
      return __int_as_float(sign |resi);
    } else {
      /* underflow: result is smallest normal or zero */
      int rem1;
      prod = ((unsigned long long)y) * r;
      x = x << (23 + ((prod >> 32) >> 15));
      rem1 = x - (unsigned)(prod & 0xffffffff);
      if ((rem1 < 0) && (!sign)) r--;
      if ((rem1 > 0) &&  (sign)) r++;
      resi = ((expo_res << 23) + r);
      if (resi != 0x00800000) resi = 0;
      return __int_as_float(sign | resi);
    }
  }
  if (__cuda_fabsf(divisor) > CUDART_TWO_TO_126_F) {
    divisor  *= 0.25f;
    dividend *= 0.25f;
  }
  return __fdividef (dividend, divisor);
}

__device_func__(float __fadd_ru (float a, float b))
{
  unsigned int expo_x, expo_y;
  unsigned int xxi, yyi, temp;
    
  xxi = __float_as_int(a);
  yyi = __float_as_int(b);

  /* make bigger operand the augend */
  expo_y = yyi << 1;
  if (expo_y > (xxi << 1)) {
    expo_y = xxi;
    xxi    = yyi;
    yyi    = expo_y;
  }
    
  temp = 0xff;
  expo_x = temp & (xxi >> 23);
  expo_x = expo_x - 1;
  expo_y = temp & (yyi >> 23);
  expo_y = expo_y - 1;
    
  if ((expo_x <= 0xFD) && 
      (expo_y <= 0xFD)) {
        
    expo_y = expo_x - expo_y;
    if (expo_y > 25) {
      expo_y = 31;
    }
    temp = xxi ^ yyi;
    xxi = xxi & ~0x7f000000;
    xxi = xxi |  0x00800000;
    yyi = yyi & ~0xff000000;
    yyi = yyi |  0x00800000;
        
    if ((int)temp < 0) {
      /* signs differ, effective subtraction */
      temp = 32 - expo_y;
      temp = (expo_y) ? (yyi << temp) : 0;
      temp = (unsigned int)(-((int)temp));
      xxi = xxi - (yyi >> expo_y) - (temp ? 1 : 0);
      if (xxi & 0x00800000) {
        if (expo_x <= 0xFD) {
          xxi = (xxi + (expo_x << 23));
          xxi += (temp && !(xxi & 0x80000000));
          return __int_as_float(xxi);
        }
      } else {
        if ((temp | (xxi << 1)) == 0) {
          /* operands cancelled, resulting in a clean zero */
          xxi = 0;
          return __int_as_float(xxi);
        }
        /* normalize result */
        yyi = xxi & 0x80000000;
        do {
          xxi = (xxi << 1) | (temp >> 31);
          temp <<= 1;
          expo_x--;
        } while (!(xxi & 0x00800000));
        xxi = xxi | yyi;
      }
    } else {
      /* signs are the same, effective addition */
      temp = 32 - expo_y;
      temp = (expo_y) ? (yyi << temp) : 0;
      xxi = xxi + (yyi >> expo_y);
      if (!(xxi & 0x01000000)) {
        if (expo_x <= 0xFD) {
          xxi = xxi + (expo_x << 23);
          xxi += (temp && !(xxi & 0x80000000));
          return __int_as_float(xxi);
        }
      } else {
        /* normalize result */
        temp = (xxi << 31) | (temp >> 1);
        xxi = ((xxi & 0x80000000) | (xxi >> 1)) & ~0x40000000;
        expo_x++;
      }
    }
    if (expo_x <= 0xFD) {
      xxi += (temp && !(xxi & 0x80000000));
      xxi = xxi + (expo_x << 23);
      return __int_as_float(xxi);
    }
    if ((int)expo_x >= 254) {
      /* overflow: return infinity or largest normal */
      temp = xxi & 0x80000000;
      xxi = (temp ? 0xff7fffff : 0x7F800000);
      return __int_as_float(xxi);
    }
    /* underflow: zero or smallest normal */
    yyi = xxi &  0x80000000;
    xxi = xxi & ~0xff000000;
    expo_x = (unsigned int)(-((int)expo_x));
    xxi = (xxi >> expo_x);
    if ((expo_x > 25) || (xxi != 0x00800000)) xxi = 0;
    return __int_as_float(yyi | xxi);
  } else {
    return a + b;
  }
}

__device_func__(float __fadd_rd (float a, float b))
{
  unsigned int expo_x, expo_y;
  unsigned int xxi, yyi, temp;
    
  xxi = __float_as_int(a);
  yyi = __float_as_int(b);

  /* make bigger operand the augend */
  expo_y = yyi << 1;
  if (expo_y > (xxi << 1)) {
    expo_y = xxi;
    xxi    = yyi;
    yyi    = expo_y;
  }
    
  temp = 0xff;
  expo_x = temp & (xxi >> 23);
  expo_x = expo_x - 1;
  expo_y = temp & (yyi >> 23);
  expo_y = expo_y - 1;
    
  if ((expo_x <= 0xFD) && 
      (expo_y <= 0xFD)) {
        
    expo_y = expo_x - expo_y;
    if (expo_y > 25) {
      expo_y = 31;
    }
    temp = xxi ^ yyi;
    xxi = xxi & ~0x7f000000;
    xxi = xxi |  0x00800000;
    yyi = yyi & ~0xff000000;
    yyi = yyi |  0x00800000;
        
    if ((int)temp < 0) {
      /* signs differ, effective subtraction */
      temp = 32 - expo_y;
      temp = (expo_y) ? (yyi << temp) : 0;
      temp = (unsigned int)(-((int)temp));
      xxi = xxi - (yyi >> expo_y) - (temp ? 1 : 0);
      if (xxi & 0x00800000) {
        if (expo_x <= 0xFD) {
          xxi = xxi & ~0x00800000; /* lop off integer bit */
          xxi = (xxi + (expo_x << 23)) + 0x00800000;
          xxi += (temp && (xxi & 0x80000000));
          return __int_as_float(xxi);
        }
      } else {
        if ((temp | (xxi << 1)) == 0) {
          /* operands cancelled, resulting in a clean zero */
          xxi = 0x80000000;
          return __int_as_float(xxi);
        }
        /* normalize result */
        yyi = xxi & 0x80000000;
        do {
          xxi = (xxi << 1) | (temp >> 31);
          temp <<= 1;
          expo_x--;
        } while (!(xxi & 0x00800000));
        xxi = xxi | yyi;
      }
    } else {
      /* signs are the same, effective addition */
      temp = 32 - expo_y;
      temp = (expo_y) ? (yyi << temp) : 0;
      xxi = xxi + (yyi >> expo_y);
      if (!(xxi & 0x01000000)) {
        if (expo_x <= 0xFD) {
          expo_y = xxi & 1;
          xxi = xxi + (expo_x << 23);
          xxi += (temp && (xxi & 0x80000000));
          return __int_as_float(xxi);
        }
      } else {
        /* normalize result */
        temp = (xxi << 31) | (temp >> 1);
        xxi = ((xxi & 0x80000000) | (xxi >> 1)) & ~0x40000000;
        expo_x++;
      }
    }
    if (expo_x <= 0xFD) {
      xxi += (temp && (xxi & 0x80000000));
      xxi = xxi + (expo_x << 23);
      return __int_as_float(xxi);
    }
    if ((int)expo_x >= 254) {
      /* overflow: return infinity or largest normal */
      temp = xxi & 0x80000000;
      xxi = (temp ? 0xFF800000 : 0x7f7fffff);
      return __int_as_float(xxi);
    }
    /* underflow: zero or smallest normal */
    yyi = xxi &  0x80000000;
    xxi = xxi & ~0xff000000;
    expo_x = (unsigned int)(-((int)expo_x));
    xxi = (xxi >> expo_x);
    if ((expo_x > 25) || (xxi != 0x00800000)) xxi = 0;
    return __int_as_float(yyi | xxi);
  } else {
    a = a + b;
    xxi = xxi ^ yyi;
    if ((a == 0.0f) && ((int)xxi < 0)) a = __int_as_float(0x80000000);
    return a;
  }
}

__device_func__(float __fmul_ru (float a, float b))
{
  unsigned long long product;
  unsigned int expo_x, expo_y;
  unsigned int xxi, yyi;
    
  xxi = __float_as_int(a);
  yyi = __float_as_int(b);

  expo_y = 0xFF;
  expo_x = expo_y & (xxi >> 23);
  expo_x = expo_x - 1;
  expo_y = expo_y & (yyi >> 23);
  expo_y = expo_y - 1;
    
  if ((expo_x <= 0xFD) && 
      (expo_y <= 0xFD)) {
    expo_x = expo_x + expo_y;
    expo_y = xxi ^ yyi;
    xxi = xxi & 0x00ffffff;
    yyi = yyi << 8;
    xxi = xxi | 0x00800000;
    yyi = yyi | 0x80000000;
    /* compute product */
    product = ((unsigned long long)xxi) * yyi;
    expo_x = expo_x - 127 + 2;
    expo_y = expo_y & 0x80000000;
    xxi = (unsigned int)(product >> 32);
    yyi = (unsigned int)(product & 0xffffffff);
    /* normalize mantissa */
    if (xxi < 0x00800000) {
      xxi = (xxi << 1) | (yyi >> 31);
      yyi = (yyi << 1);
      expo_x--;
    }
    if (expo_x <= 0xFD) {
      xxi = xxi | expo_y;          /* OR in sign bit */
      xxi = xxi + (expo_x << 23);  /* add in exponent */
      /* round result */
      xxi += (yyi && !expo_y);
      return __int_as_float(xxi);
    } else if ((int)expo_x >= 254) {
      /* overflow: return infinity or largest normal */
      xxi = (expo_y ? 0xff7fffff : 0x7F800000);
      return __int_as_float(xxi);
    } else {
      /* underflow: zero, or smallest normal */
      expo_x = ((unsigned int)-((int)expo_x));
      xxi += (yyi && !expo_y);
      xxi = (xxi >> expo_x);
      if ((expo_x > 25) || (xxi != 0x00800000)) xxi = 0;
      return __int_as_float(expo_y | xxi);
    }
  } else {
    return a * b;
  }
}

__device_func__(float __fmul_rd (float a, float b))
{
  unsigned long long product;
  unsigned int expo_x, expo_y;
  unsigned int xxi, yyi;
    
  xxi = __float_as_int(a);
  yyi = __float_as_int(b);

  expo_y = 0xFF;
  expo_x = expo_y & (xxi >> 23);
  expo_x = expo_x - 1;
  expo_y = expo_y & (yyi >> 23);
  expo_y = expo_y - 1;
    
  if ((expo_x <= 0xFD) && 
      (expo_y <= 0xFD)) {
    expo_x = expo_x + expo_y;
    expo_y = xxi ^ yyi;
    xxi = xxi & 0x00ffffff;
    yyi = yyi << 8;
    xxi = xxi | 0x00800000;
    yyi = yyi | 0x80000000;
    /* compute product */
    product = ((unsigned long long)xxi) * yyi;
    expo_x = expo_x - 127 + 2;
    expo_y = expo_y & 0x80000000;
    xxi = (unsigned int)(product >> 32);
    yyi = (unsigned int)(product & 0xffffffff);
    /* normalize mantissa */
    if (xxi < 0x00800000) {
      xxi = (xxi << 1) | (yyi >> 31);
      yyi = (yyi << 1);
      expo_x--;
    }
    if (expo_x <= 0xFD) {
      xxi = xxi | expo_y;          /* OR in sign bit */
      xxi = xxi + (expo_x << 23);  /* add in exponent */
      /* round result */
      xxi += (yyi && expo_y);
      return __int_as_float(xxi);
    } else if ((int)expo_x >= 254) {
      /* overflow: return infinity or largest normal */
      xxi = expo_y | (expo_y ?0x7F800000 : 0x7f7fffff);
      return __int_as_float(xxi);
    } else {
      /* underflow: zero, or smallest normal */
      expo_x = ((unsigned int)-((int)expo_x));
      xxi += (yyi && expo_y);
      xxi = (xxi >> expo_x);
      if ((expo_x > 25) || (xxi != 0x00800000)) xxi = 0;
      return __int_as_float(expo_y | xxi);
    }
  } else {
    return a * b;
  }
}

__device_func__(float __fmaf_rn (float a, float b, float c))
{
  unsigned long long product;
  unsigned int xx, yy, zz, ww;
  unsigned int temp, s, u;
  unsigned int expo_x, expo_y, expo_z;

  xx = __float_as_int(a);
  yy = __float_as_int(b);
  zz = __float_as_int(c);

  /* Match 'denormals are zero' behavior of the GPU */
  if ((xx << 1) < 0x01000000) xx &= 0x80000000;
  if ((yy << 1) < 0x01000000) yy &= 0x80000000;
  if ((zz << 1) < 0x01000000) zz &= 0x80000000;
   
  temp = 0xff;
  expo_x = temp & (xx >> 23);
  expo_x = expo_x - 1;
  expo_y = temp & (yy >> 23);
  expo_y = expo_y - 1;
  expo_z = temp & (zz >> 23);
  expo_z = expo_z - 1;

  if (!((expo_x <= 0xFD) && 
        (expo_y <= 0xFD) &&
        (expo_z <= 0xFD))) {
    /* fmad (nan, y, z) --> nan
       fmad (x, nan, z) --> nan
       fmad (x, y, nan) --> nan 
    */
    if ((yy << 1) > 0xff000000) {
      return rsqrtf(b);
    }
    if ((zz << 1) > 0xff000000) {
      return rsqrtf(c);
    }
    if ((xx << 1) > 0xff000000) {
      return rsqrtf(a);
    }
    /* fmad (0, inf, z) --> NaN
       fmad (inf, 0, z) --> NaN
       fmad (-inf,+y,+inf) --> NaN
       fmad (+x,-inf,+inf) --> NaN
       fmad (+inf,-y,+inf) --> NaN
       fmad (-x,+inf,+inf) --> NaN
       fmad (-inf,-y,-inf) --> NaN
       fmad (-x,-inf,-inf) --> NaN
       fmad (+inf,+y,-inf) --> NaN
       fmad (+x,+inf,-inf) --> NaN
    */
    if ((((xx << 1) == 0) && ((yy << 1) == 0xff000000)) ||
        (((yy << 1) == 0) && ((xx << 1) == 0xff000000))) {
      return rsqrtf(__int_as_float(0xffc00000));
    }
    if ((zz << 1) == 0xff000000) {
      if (((yy << 1) == 0xff000000) || ((xx << 1) == 0xff000000)) {
        if ((int)(xx ^ yy ^ zz) < 0) {
          return rsqrtf(__int_as_float(0xffc00000));
        }
      }
    }
    /* fmad (inf, y, z) --> inf
       fmad (x, inf, z) --> inf
       fmad (x, y, inf) --> inf
    */
    if ((xx << 1) == 0xff000000) {
      xx = xx ^ (yy & 0x80000000);
      return __int_as_float(xx);
    }
    if ((yy << 1) == 0xff000000) {
      yy = yy ^ (xx & 0x80000000);
      return __int_as_float(yy);
    }
    if ((zz << 1) == 0xff000000) {
      return __int_as_float(zz);
    }
    /* fmad (+0, -y, -0) --> -0
       fmad (-0, +y, -0) --> -0
       fmad (+x, -0, -0) --> -0
       fmad (-x, +0, -0) --> -0
    */
    if (zz == 0x80000000) {
      if (((xx << 1) == 0) || ((yy << 1) == 0)) {
        if ((int)(xx ^ yy) < 0) {
          return __int_as_float(zz);
        }
      }
    }
    /* fmad (0, y, 0) --> +0
       fmad (x, 0, 0) --> +0
    */
    if (((zz << 1) == 0) && 
        (((xx << 1) == 0) || ((yy << 1) == 0))) {
      zz &= 0x7fffffff;
      return __int_as_float(zz);
    }
    /* fmad (0, y, z) --> z
       fmad (x, 0, z) --> z
     */
    if (((xx << 1) == 0) || ((yy << 1) == 0)) {
      return __int_as_float(zz);
    }
    /* normalize x, if denormal */
    if (expo_x == (unsigned)-1) {
      temp = xx & 0x80000000;
      xx = xx << 8;
      while (!(xx & 0x80000000)) {
        xx <<= 1;
        expo_x--;
      }
      expo_x++;
      xx = (xx >> 8) | temp;
    }
    /* normalize y, if denormal */
    if (expo_y == (unsigned)-1) {
      temp = yy & 0x80000000;
      yy = yy << 8;
      while (!(yy & 0x80000000)) {
        yy <<= 1;
        expo_y--;
      }
      expo_y++;
      yy = (yy >> 8) | temp;
    }
    /* normalize z, if denormal */
    if ((expo_z == (unsigned)-1) && ((zz << 1) != 0)) {
      temp = zz & 0x80000000;
      zz = zz << 8;
      while (!(zz & 0x80000000)) {
        zz <<= 1;
        expo_z--;
      }
      expo_z++;
      zz = (zz >> 8) | temp;
    }
  }
    
  expo_x = expo_x + expo_y;
  expo_y = xx ^ yy;
  xx = xx & 0x00ffffff;
  yy = yy << 8;
  xx = xx | 0x00800000;
  yy = yy | 0x80000000;

  product = ((unsigned long long)xx) * yy;
  xx = (unsigned)(product >> 32);
  yy = (unsigned)(product & 0xffffffff);

  expo_x = expo_x - 127 + 2;
  expo_y = expo_y & 0x80000000;
  /* normalize mantissa */
  if (xx < 0x00800000) {
    xx = (xx << 1) | (yy >> 31);
    yy = (yy << 1);
    expo_x--;
  }
  temp = 0;

  if ((zz << 1) != 0) { /* z is not zero */
    s = zz & 0x80000000;
    zz &= 0x00ffffff;
    zz |= 0x00800000;
    ww = 0;
    /* compare and swap. put augend into xx:yy */
    if ((int)expo_z > (int)expo_x) {
      temp = expo_z;
      expo_z = expo_x;
      expo_x = temp;
      temp = zz;
      zz = xx;
      xx = temp;
      temp = ww;
      ww = yy;
      yy = temp;
      temp = expo_y;
      expo_y = s;
      s = temp;
    }
    /* augend_sign = expo_y, augend_mant = xx:yy, augend_expo = expo_x */
    /* addend_sign = s, addend_mant = zz:ww, addend_expo = expo_z */
    expo_z = expo_x - expo_z;
    u = expo_y ^ s;
    if (expo_z <= 49) {
      /* denormalize addend */
      temp = 0;
      while (expo_z >= 32) {
        temp = ww | (temp != 0);
        ww = zz;
        zz = 0;
        expo_z -= 32;
      }
      if (expo_z) {
        temp = ((temp >> expo_z) | (ww << (32 - expo_z)) | 
                ((temp << (32 - expo_z)) != 0));
        ww = (ww >> expo_z) | (zz << (32 - expo_z));
        zz = (zz >> expo_z);
      }
      
    } else {
      temp = 1;
      ww = 0;
      zz = 0;
    }            
    if ((int)u < 0) {
      /* signs differ, effective subtraction */
      temp = (unsigned)(-(int)temp);
      s = (temp != 0);
      u = yy - s;
      s = u > yy;
      yy = u - ww;
      s += yy > u;
      xx = (xx - zz) - s;
      if (!(xx | yy | temp)) {
        /* complete cancelation, return 0 */
        return __int_as_float(xx);
      }
      if ((int)xx < 0) {
        /* ooops, augend had smaller mantissa. Negate mantissa and flip
           sign of result*/
        temp = ~temp;
        yy = ~yy;
        xx = ~xx;
        if (++temp == 0) {
          if (++yy == 0) {
            ++xx;
          }
        }
        expo_y ^= 0x80000000;
      }
      /* normalize mantissa, if necessary */
      while (!(xx & 0x00800000)) {
        xx = (xx << 1) | (yy >> 31);
        yy = (yy << 1);
        expo_x--;
      }
    } else {
      /* signs are the same, effective addition */
      yy = yy + ww;
      s =  yy < ww;
      xx = xx + zz + s;
      if (xx & 0x01000000) {
        temp = temp | (yy << 31);
        yy = (yy >> 1) | (xx << 31);
        xx = ((xx & 0x80000000) | (xx >> 1)) & ~0x40000000;
        expo_x++;
      }
    }
  }
  temp = yy | (temp != 0);
  if (expo_x <= 0xFD) {
    /* normal */
    xx |= expo_y; /* or in sign bit */
    s = xx & 1; /* mantissa lsb */
    xx += (temp == 0x80000000) ? s : (temp >> 31);
    xx = xx + (expo_x << 23); /* add in exponent */
    return __int_as_float(xx);
  } else if ((int)expo_x >= 126) {
    /* overflow */
    xx = expo_y | 0x7f800000;
    return __int_as_float(xx);
  }
  /* subnormal */
  expo_x = (unsigned int)(-(int)expo_x);
  /* Match 'flush to zero' response of the GPU */
  xx += (temp >= 0x80000000);
  if (xx >= 0x01000000) {
    xx = xx >> 1;
    expo_x--;
  }
  if (expo_x > 0) xx = 0;
  xx = expo_y | xx;
  return __int_as_float(xx);
}

__device_func__(float __fmaf_rz (float a, float b, float c))
{
  unsigned long long product;
  unsigned int xx, yy, zz, ww;
  unsigned int temp, s, u;
  unsigned int expo_x, expo_y, expo_z;

  xx = __float_as_int(a);
  yy = __float_as_int(b);
  zz = __float_as_int(c);

  /* Match 'denormals are zero' behavior of the GPU */
  if ((xx << 1) < 0x01000000) xx &= 0x80000000;
  if ((yy << 1) < 0x01000000) yy &= 0x80000000;
  if ((zz << 1) < 0x01000000) zz &= 0x80000000;
   
  temp = 0xff;
  expo_x = temp & (xx >> 23);
  expo_x = expo_x - 1;
  expo_y = temp & (yy >> 23);
  expo_y = expo_y - 1;
  expo_z = temp & (zz >> 23);
  expo_z = expo_z - 1;

  if (!((expo_x <= 0xFD) && 
        (expo_y <= 0xFD) &&
        (expo_z <= 0xFD))) {
    /* fmad (nan, y, z) --> nan
       fmad (x, nan, z) --> nan
       fmad (x, y, nan) --> nan 
    */
    if ((yy << 1) > 0xff000000) {
      return rsqrtf(b);
    }
    if ((zz << 1) > 0xff000000) {
      return rsqrtf(c);
    }
    if ((xx << 1) > 0xff000000) {
      return rsqrtf(a);
    }
    /* fmad (0, inf, z) --> NaN
       fmad (inf, 0, z) --> NaN
       fmad (-inf,+y,+inf) --> NaN
       fmad (+x,-inf,+inf) --> NaN
       fmad (+inf,-y,+inf) --> NaN
       fmad (-x,+inf,+inf) --> NaN
       fmad (-inf,-y,-inf) --> NaN
       fmad (-x,-inf,-inf) --> NaN
       fmad (+inf,+y,-inf) --> NaN
       fmad (+x,+inf,-inf) --> NaN
    */
    if ((((xx << 1) == 0) && ((yy << 1) == 0xff000000)) ||
        (((yy << 1) == 0) && ((xx << 1) == 0xff000000))) {
      return rsqrtf(__int_as_float(0xffc00000));
    }
    if ((zz << 1) == 0xff000000) {
      if (((yy << 1) == 0xff000000) || ((xx << 1) == 0xff000000)) {
        if ((int)(xx ^ yy ^ zz) < 0) {
          return rsqrtf(__int_as_float(0xffc00000));
        }
      }
    }
    /* fmad (inf, y, z) --> inf
       fmad (x, inf, z) --> inf
       fmad (x, y, inf) --> inf
    */
    if ((xx << 1) == 0xff000000) {
      xx = xx ^ (yy & 0x80000000);
      return __int_as_float(xx);
    }
    if ((yy << 1) == 0xff000000) {
      yy = yy ^ (xx & 0x80000000);
      return __int_as_float(yy);
    }
    if ((zz << 1) == 0xff000000) {
      return __int_as_float(zz);
    }
    /* fmad (+0, -y, -0) --> -0
       fmad (-0, +y, -0) --> -0
       fmad (+x, -0, -0) --> -0
       fmad (-x, +0, -0) --> -0
    */
    if (zz == 0x80000000) {
      if (((xx << 1) == 0) || ((yy << 1) == 0)) {
        if ((int)(xx ^ yy) < 0) {
          return __int_as_float(zz);
        }
      }
    }
    /* fmad (0, y, 0) --> +0
       fmad (x, 0, 0) --> +0
    */
    if (((zz << 1) == 0) && 
        (((xx << 1) == 0) || ((yy << 1) == 0))) {
      zz &= 0x7fffffff;
      return __int_as_float(zz);
    }
    /* fmad (0, y, z) --> z
       fmad (x, 0, z) --> z
     */
    if (((xx << 1) == 0) || ((yy << 1) == 0)) {
      return __int_as_float(zz);
    }
    /* normalize x, if denormal */
    if (expo_x == (unsigned)-1) {
      temp = xx & 0x80000000;
      xx = xx << 8;
      while (!(xx & 0x80000000)) {
        xx <<= 1;
        expo_x--;
      }
      expo_x++;
      xx = (xx >> 8) | temp;
    }
    /* normalize y, if denormal */
    if (expo_y == (unsigned)-1) {
      temp = yy & 0x80000000;
      yy = yy << 8;
      while (!(yy & 0x80000000)) {
        yy <<= 1;
        expo_y--;
      }
      expo_y++;
      yy = (yy >> 8) | temp;
    }
    /* normalize z, if denormal */
    if ((expo_z == (unsigned)-1) && ((zz << 1) != 0)) {
      temp = zz & 0x80000000;
      zz = zz << 8;
      while (!(zz & 0x80000000)) {
        zz <<= 1;
        expo_z--;
      }
      expo_z++;
      zz = (zz >> 8) | temp;
    }
  }
    
  expo_x = expo_x + expo_y;
  expo_y = xx ^ yy;
  xx = xx & 0x00ffffff;
  yy = yy << 8;
  xx = xx | 0x00800000;
  yy = yy | 0x80000000;

  product = ((unsigned long long)xx) * yy;
  xx = (unsigned)(product >> 32);
  yy = (unsigned)(product & 0xffffffff);

  expo_x = expo_x - 127 + 2;
  expo_y = expo_y & 0x80000000;
  /* normalize mantissa */
  if (xx < 0x00800000) {
    xx = (xx << 1) | (yy >> 31);
    yy = (yy << 1);
    expo_x--;
  }
  temp = 0;

  if ((zz << 1) != 0) { /* z is not zero */
    s = zz & 0x80000000;
    zz &= 0x00ffffff;
    zz |= 0x00800000;
    ww = 0;
    /* compare and swap. put augend into xx:yy */
    if ((int)expo_z > (int)expo_x) {
      temp = expo_z;
      expo_z = expo_x;
      expo_x = temp;
      temp = zz;
      zz = xx;
      xx = temp;
      temp = ww;
      ww = yy;
      yy = temp;
      temp = expo_y;
      expo_y = s;
      s = temp;
    }
    /* augend_sign = expo_y, augend_mant = xx:yy, augend_expo = expo_x */
    /* addend_sign = s, addend_mant = zz:ww, addend_expo = expo_z */
    expo_z = expo_x - expo_z;
    u = expo_y ^ s;
    if (expo_z <= 49) {
      /* denormalize addend */
      temp = 0;
      while (expo_z >= 32) {
        temp = ww | (temp != 0);
        ww = zz;
        zz = 0;
        expo_z -= 32;
      }
      if (expo_z) {
        temp = ((temp >> expo_z) | (ww << (32 - expo_z)) | 
                ((temp << (32 - expo_z)) != 0));
        ww = (ww >> expo_z) | (zz << (32 - expo_z));
        zz = (zz >> expo_z);
      }
      
    } else {
      temp = 1;
      ww = 0;
      zz = 0;
    }            
    if ((int)u < 0) {
      /* signs differ, effective subtraction */
      temp = (unsigned)(-(int)temp);
      s = (temp != 0);
      u = yy - s;
      s = u > yy;
      yy = u - ww;
      s += yy > u;
      xx = (xx - zz) - s;
      if (!(xx | yy | temp)) {
        /* complete cancelation, return 0 */
        return __int_as_float(xx);
      }
      if ((int)xx < 0) {
        /* ooops, augend had smaller mantissa. Negate mantissa and flip
           sign of result*/
        temp = ~temp;
        yy = ~yy;
        xx = ~xx;
        if (++temp == 0) {
          if (++yy == 0) {
            ++xx;
          }
        }
        expo_y ^= 0x80000000;
      }
      /* normalize mantissa, if necessary */
      while (!(xx & 0x00800000)) {
        xx = (xx << 1) | (yy >> 31);
        yy = (yy << 1);
        expo_x--;
      }
    } else {
      /* signs are the same, effective addition */
      yy = yy + ww;
      s =  yy < ww;
      xx = xx + zz + s;
      if (xx & 0x01000000) {
        temp = temp | (yy << 31);
        yy = (yy >> 1) | (xx << 31);
        xx = ((xx & 0x80000000) | (xx >> 1)) & ~0x40000000;
        expo_x++;
      }
    }
  }
  temp = yy | (temp != 0);
  if (expo_x <= 0xFD) {
    /* normal */
    xx |= expo_y; /* or in sign bit */
    xx = xx + (expo_x << 23); /* add in exponent */
    return __int_as_float(xx);
  } else if ((int)expo_x >= 126) {
    /* overflow */
    xx = expo_y | 0x7f7fffff;
    return __int_as_float(xx);
  }
  /* subnormal */
  return __int_as_float(expo_y);
}

__device_func__(float __fmaf_ru (float a, float b, float c))
{
  unsigned long long product;
  unsigned int xx, yy, zz, ww;
  unsigned int temp, s, u;
  unsigned int expo_x, expo_y, expo_z;

  xx = __float_as_int(a);
  yy = __float_as_int(b);
  zz = __float_as_int(c);

  /* Match 'denormals are zero' behavior of the GPU */
  if ((xx << 1) < 0x01000000) xx &= 0x80000000;
  if ((yy << 1) < 0x01000000) yy &= 0x80000000;
  if ((zz << 1) < 0x01000000) zz &= 0x80000000;
   
  temp = 0xff;
  expo_x = temp & (xx >> 23);
  expo_x = expo_x - 1;
  expo_y = temp & (yy >> 23);
  expo_y = expo_y - 1;
  expo_z = temp & (zz >> 23);
  expo_z = expo_z - 1;

  if (!((expo_x <= 0xFD) && 
        (expo_y <= 0xFD) &&
        (expo_z <= 0xFD))) {
    /* fmad (nan, y, z) --> nan
       fmad (x, nan, z) --> nan
       fmad (x, y, nan) --> nan 
    */
    if ((yy << 1) > 0xff000000) {
      return rsqrtf(b);
    }
    if ((zz << 1) > 0xff000000) {
      return rsqrtf(c);
    }
    if ((xx << 1) > 0xff000000) {
      return rsqrtf(a);
    }
    /* fmad (0, inf, z) --> NaN
       fmad (inf, 0, z) --> NaN
       fmad (-inf,+y,+inf) --> NaN
       fmad (+x,-inf,+inf) --> NaN
       fmad (+inf,-y,+inf) --> NaN
       fmad (-x,+inf,+inf) --> NaN
       fmad (-inf,-y,-inf) --> NaN
       fmad (-x,-inf,-inf) --> NaN
       fmad (+inf,+y,-inf) --> NaN
       fmad (+x,+inf,-inf) --> NaN
    */
    if ((((xx << 1) == 0) && ((yy << 1) == 0xff000000)) ||
        (((yy << 1) == 0) && ((xx << 1) == 0xff000000))) {
      return rsqrtf(__int_as_float(0xffc00000));
    }
    if ((zz << 1) == 0xff000000) {
      if (((yy << 1) == 0xff000000) || ((xx << 1) == 0xff000000)) {
        if ((int)(xx ^ yy ^ zz) < 0) {
          return rsqrtf(__int_as_float(0xffc00000));
        }
      }
    }
    /* fmad (inf, y, z) --> inf
       fmad (x, inf, z) --> inf
       fmad (x, y, inf) --> inf
    */
    if ((xx << 1) == 0xff000000) {
      xx = xx ^ (yy & 0x80000000);
      return __int_as_float(xx);
    }
    if ((yy << 1) == 0xff000000) {
      yy = yy ^ (xx & 0x80000000);
      return __int_as_float(yy);
    }
    if ((zz << 1) == 0xff000000) {
      return __int_as_float(zz);
    }
    /* fmad (+0, -y, -0) --> -0
       fmad (-0, +y, -0) --> -0
       fmad (+x, -0, -0) --> -0
       fmad (-x, +0, -0) --> -0
    */
    if (zz == 0x80000000) {
      if (((xx << 1) == 0) || ((yy << 1) == 0)) {
        if ((int)(xx ^ yy) < 0) {
          return __int_as_float(zz);
        }
      }
    }
    /* fmad (0, y, 0) --> +0
       fmad (x, 0, 0) --> +0
    */
    if (((zz << 1) == 0) && 
        (((xx << 1) == 0) || ((yy << 1) == 0))) {
      zz &= 0x7fffffff;
      return __int_as_float(zz);
    }
    /* fmad (0, y, z) --> z
       fmad (x, 0, z) --> z
     */
    if (((xx << 1) == 0) || ((yy << 1) == 0)) {
      return __int_as_float(zz);
    }
    /* normalize x, if denormal */
    if (expo_x == (unsigned)-1) {
      temp = xx & 0x80000000;
      xx = xx << 8;
      while (!(xx & 0x80000000)) {
        xx <<= 1;
        expo_x--;
      }
      expo_x++;
      xx = (xx >> 8) | temp;
    }
    /* normalize y, if denormal */
    if (expo_y == (unsigned)-1) {
      temp = yy & 0x80000000;
      yy = yy << 8;
      while (!(yy & 0x80000000)) {
        yy <<= 1;
        expo_y--;
      }
      expo_y++;
      yy = (yy >> 8) | temp;
    }
    /* normalize z, if denormal */
    if ((expo_z == (unsigned)-1) && ((zz << 1) != 0)) {
      temp = zz & 0x80000000;
      zz = zz << 8;
      while (!(zz & 0x80000000)) {
        zz <<= 1;
        expo_z--;
      }
      expo_z++;
      zz = (zz >> 8) | temp;
    }
  }
    
  expo_x = expo_x + expo_y;
  expo_y = xx ^ yy;
  xx = xx & 0x00ffffff;
  yy = yy << 8;
  xx = xx | 0x00800000;
  yy = yy | 0x80000000;

  product = ((unsigned long long)xx) * yy;
  xx = (unsigned)(product >> 32);
  yy = (unsigned)(product & 0xffffffff);

  expo_x = expo_x - 127 + 2;
  expo_y = expo_y & 0x80000000;
  /* normalize mantissa */
  if (xx < 0x00800000) {
    xx = (xx << 1) | (yy >> 31);
    yy = (yy << 1);
    expo_x--;
  }
  temp = 0;

  if ((zz << 1) != 0) { /* z is not zero */
    s = zz & 0x80000000;
    zz &= 0x00ffffff;
    zz |= 0x00800000;
    ww = 0;
    /* compare and swap. put augend into xx:yy */
    if ((int)expo_z > (int)expo_x) {
      temp = expo_z;
      expo_z = expo_x;
      expo_x = temp;
      temp = zz;
      zz = xx;
      xx = temp;
      temp = ww;
      ww = yy;
      yy = temp;
      temp = expo_y;
      expo_y = s;
      s = temp;
    }
    /* augend_sign = expo_y, augend_mant = xx:yy, augend_expo = expo_x */
    /* addend_sign = s, addend_mant = zz:ww, addend_expo = expo_z */
    expo_z = expo_x - expo_z;
    u = expo_y ^ s;
    if (expo_z <= 49) {
      /* denormalize addend */
      temp = 0;
      while (expo_z >= 32) {
        temp = ww | (temp != 0);
        ww = zz;
        zz = 0;
        expo_z -= 32;
      }
      if (expo_z) {
        temp = ((temp >> expo_z) | (ww << (32 - expo_z)) | 
                ((temp << (32 - expo_z)) != 0));
        ww = (ww >> expo_z) | (zz << (32 - expo_z));
        zz = (zz >> expo_z);
      }
      
    } else {
      temp = 1;
      ww = 0;
      zz = 0;
    }            
    if ((int)u < 0) {
      /* signs differ, effective subtraction */
      temp = (unsigned)(-(int)temp);
      s = (temp != 0);
      u = yy - s;
      s = u > yy;
      yy = u - ww;
      s += yy > u;
      xx = (xx - zz) - s;
      if (!(xx | yy | temp)) {
        /* complete cancelation, return 0 */
        return __int_as_float(xx);
      }
      if ((int)xx < 0) {
        /* ooops, augend had smaller mantissa. Negate mantissa and flip
           sign of result*/
        temp = ~temp;
        yy = ~yy;
        xx = ~xx;
        if (++temp == 0) {
          if (++yy == 0) {
            ++xx;
          }
        }
        expo_y ^= 0x80000000;
      }
      /* normalize mantissa, if necessary */
      while (!(xx & 0x00800000)) {
        xx = (xx << 1) | (yy >> 31);
        yy = (yy << 1);
        expo_x--;
      }
    } else {
      /* signs are the same, effective addition */
      yy = yy + ww;
      s =  yy < ww;
      xx = xx + zz + s;
      if (xx & 0x01000000) {
        temp = temp | (yy << 31);
        yy = (yy >> 1) | (xx << 31);
        xx = ((xx & 0x80000000) | (xx >> 1)) & ~0x40000000;
        expo_x++;
      }
    }
  }
  temp = yy | (temp != 0);
  if (expo_x <= 0xFD) {
    /* normal */
    xx |= expo_y; /* or in sign bit */
    xx += (temp && !expo_y); /* round result */
    xx = xx + (expo_x << 23); /* add in exponent */
    return __int_as_float(xx);
  } else if ((int)expo_x >= 126) {
    /* overflow */
    xx = expo_y | (expo_y ? 0x7f7fffff : 0x7F800000);
    return __int_as_float(xx);
  }
  /* subnormal */
  expo_x = ((unsigned int)-((int)expo_x));
  xx += (temp && !expo_y);
  xx = (xx >> expo_x);
  if ((expo_x > 25) || (xx != 0x00800000)) xx = 0;
  return __int_as_float(expo_y | xx);
}

__device_func__(float __fmaf_rd (float a, float b, float c))
{
  unsigned long long product;
  unsigned int xx, yy, zz, ww;
  unsigned int temp, s, u;
  unsigned int expo_x, expo_y, expo_z;

  xx = __float_as_int(a);
  yy = __float_as_int(b);
  zz = __float_as_int(c);

  /* Match 'denormals are zero' behavior of the GPU */
  if ((xx << 1) < 0x01000000) xx &= 0x80000000;
  if ((yy << 1) < 0x01000000) yy &= 0x80000000;
  if ((zz << 1) < 0x01000000) zz &= 0x80000000;
   
  temp = 0xff;
  expo_x = temp & (xx >> 23);
  expo_x = expo_x - 1;
  expo_y = temp & (yy >> 23);
  expo_y = expo_y - 1;
  expo_z = temp & (zz >> 23);
  expo_z = expo_z - 1;

  if (!((expo_x <= 0xFD) && 
        (expo_y <= 0xFD) &&
        (expo_z <= 0xFD))) {
    /* fmad (nan, y, z) --> nan
       fmad (x, nan, z) --> nan
       fmad (x, y, nan) --> nan 
    */
    if ((yy << 1) > 0xff000000) {
      return rsqrtf(b);
    }
    if ((zz << 1) > 0xff000000) {
      return rsqrtf(c);
    }
    if ((xx << 1) > 0xff000000) {
      return rsqrtf(a);
    }
    /* fmad (0, inf, z) --> NaN
       fmad (inf, 0, z) --> NaN
       fmad (-inf,+y,+inf) --> NaN
       fmad (+x,-inf,+inf) --> NaN
       fmad (+inf,-y,+inf) --> NaN
       fmad (-x,+inf,+inf) --> NaN
       fmad (-inf,-y,-inf) --> NaN
       fmad (-x,-inf,-inf) --> NaN
       fmad (+inf,+y,-inf) --> NaN
       fmad (+x,+inf,-inf) --> NaN
    */
    if ((((xx << 1) == 0) && ((yy << 1) == 0xff000000)) ||
        (((yy << 1) == 0) && ((xx << 1) == 0xff000000))) {
      return rsqrtf(__int_as_float(0xffc00000));
    }
    if ((zz << 1) == 0xff000000) {
      if (((yy << 1) == 0xff000000) || ((xx << 1) == 0xff000000)) {
        if ((int)(xx ^ yy ^ zz) < 0) {
          return rsqrtf(__int_as_float(0xffc00000));
        }
      }
    }
    /* fmad (inf, y, z) --> inf
       fmad (x, inf, z) --> inf
       fmad (x, y, inf) --> inf
    */
    if ((xx << 1) == 0xff000000) {
      xx = xx ^ (yy & 0x80000000);
      return __int_as_float(xx);
    }
    if ((yy << 1) == 0xff000000) {
      yy = yy ^ (xx & 0x80000000);
      return __int_as_float(yy);
    }
    if ((zz << 1) == 0xff000000) {
      return __int_as_float(zz);
    }
    /* fmad (+0, -y, -0) --> -0
       fmad (-0, +y, -0) --> -0
       fmad (+x, -0, -0) --> -0
       fmad (-x, +0, -0) --> -0
    */
    if (zz == 0x80000000) {
      if (((xx << 1) == 0) || ((yy << 1) == 0)) {
        if ((int)(xx ^ yy) < 0) {
          return __int_as_float(zz);
        }
      }
    }
    /* fmad (0, y, 0) --> +0
       fmad (x, 0, 0) --> +0
    */
    if (((zz << 1) == 0) && 
        (((xx << 1) == 0) || ((yy << 1) == 0))) {
      zz = (xx ^ yy ^ zz) & 0x80000000;
      return __int_as_float(zz);
    }
    /* fmad (0, y, z) --> z
       fmad (x, 0, z) --> z
     */
    if (((xx << 1) == 0) || ((yy << 1) == 0)) {
      return __int_as_float(zz);
    }
    /* normalize x, if denormal */
    if (expo_x == (unsigned)-1) {
      temp = xx & 0x80000000;
      xx = xx << 8;
      while (!(xx & 0x80000000)) {
        xx <<= 1;
        expo_x--;
      }
      expo_x++;
      xx = (xx >> 8) | temp;
    }
    /* normalize y, if denormal */
    if (expo_y == (unsigned)-1) {
      temp = yy & 0x80000000;
      yy = yy << 8;
      while (!(yy & 0x80000000)) {
        yy <<= 1;
        expo_y--;
      }
      expo_y++;
      yy = (yy >> 8) | temp;
    }
    /* normalize z, if denormal */
    if ((expo_z == (unsigned)-1) && ((zz << 1) != 0)) {
      temp = zz & 0x80000000;
      zz = zz << 8;
      while (!(zz & 0x80000000)) {
        zz <<= 1;
        expo_z--;
      }
      expo_z++;
      zz = (zz >> 8) | temp;
    }
  }
    
  expo_x = expo_x + expo_y;
  expo_y = xx ^ yy;
  xx = xx & 0x00ffffff;
  yy = yy << 8;
  xx = xx | 0x00800000;
  yy = yy | 0x80000000;

  product = ((unsigned long long)xx) * yy;
  xx = (unsigned)(product >> 32);
  yy = (unsigned)(product & 0xffffffff);

  expo_x = expo_x - 127 + 2;
  expo_y = expo_y & 0x80000000;
  /* normalize mantissa */
  if (xx < 0x00800000) {
    xx = (xx << 1) | (yy >> 31);
    yy = (yy << 1);
    expo_x--;
  }
  temp = 0;

  if ((zz << 1) != 0) { /* z is not zero */
    s = zz & 0x80000000;
    zz &= 0x00ffffff;
    zz |= 0x00800000;
    ww = 0;
    /* compare and swap. put augend into xx:yy */
    if ((int)expo_z > (int)expo_x) {
      temp = expo_z;
      expo_z = expo_x;
      expo_x = temp;
      temp = zz;
      zz = xx;
      xx = temp;
      temp = ww;
      ww = yy;
      yy = temp;
      temp = expo_y;
      expo_y = s;
      s = temp;
    }
    /* augend_sign = expo_y, augend_mant = xx:yy, augend_expo = expo_x */
    /* addend_sign = s, addend_mant = zz:ww, addend_expo = expo_z */
    expo_z = expo_x - expo_z;
    u = expo_y ^ s;
    if (expo_z <= 49) {
      /* denormalize addend */
      temp = 0;
      while (expo_z >= 32) {
        temp = ww | (temp != 0);
        ww = zz;
        zz = 0;
        expo_z -= 32;
      }
      if (expo_z) {
        temp = ((temp >> expo_z) | (ww << (32 - expo_z)) | 
                ((temp << (32 - expo_z)) != 0));
        ww = (ww >> expo_z) | (zz << (32 - expo_z));
        zz = (zz >> expo_z);
      }
      
    } else {
      temp = 1;
      ww = 0;
      zz = 0;
    }            
    if ((int)u < 0) {
      /* signs differ, effective subtraction */
      temp = (unsigned)(-(int)temp);
      s = (temp != 0);
      u = yy - s;
      s = u > yy;
      yy = u - ww;
      s += yy > u;
      xx = (xx - zz) - s;
      if (!(xx | yy | temp)) {
        /* complete cancelation, return -0 */
        return __int_as_float(0x80000000);
      }
      if ((int)xx < 0) {
        /* ooops, augend had smaller mantissa. Negate mantissa and flip
           sign of result*/
        temp = ~temp;
        yy = ~yy;
        xx = ~xx;
        if (++temp == 0) {
          if (++yy == 0) {
            ++xx;
          }
        }
        expo_y ^= 0x80000000;
      }
      /* normalize mantissa, if necessary */
      while (!(xx & 0x00800000)) {
        xx = (xx << 1) | (yy >> 31);
        yy = (yy << 1);
        expo_x--;
      }
    } else {
      /* signs are the same, effective addition */
      yy = yy + ww;
      s =  yy < ww;
      xx = xx + zz + s;
      if (xx & 0x01000000) {
        temp = temp | (yy << 31);
        yy = (yy >> 1) | (xx << 31);
        xx = ((xx & 0x80000000) | (xx >> 1)) & ~0x40000000;
        expo_x++;
      }
    }
  }
  temp = yy | (temp != 0);
  if (expo_x <= 0xFD) {
    /* normal */
    xx |= expo_y; /* or in sign bit */
    xx += (temp && expo_y); /* round result */
    xx = xx + (expo_x << 23); /* add in exponent */
    return __int_as_float(xx);
  } else if ((int)expo_x >= 126) {
    /* overflow */
    xx = expo_y | (expo_y ? 0x7f800000 : 0x7F7FFFFF);
    return __int_as_float(xx);
  }
  /* subnormal */
  expo_x = ((unsigned int)-((int)expo_x));
  xx += (temp && expo_y);
  xx = (xx >> expo_x);
  if ((expo_x > 25) || (xx != 0x00800000)) xx = 0;
  return __int_as_float(expo_y | xx);
}

#endif /* __CUDA_ARCH__ < 200 */ 

#else  /* defined(__CUDABE__) */

#include "common_types.h"

static __device__ const unsigned char __internal_rcpTab[128] = 
{
  0xff, 0xfd, 0xfb, 0xf9, 0xf7, 0xf5, 0xf4, 0xf2, 
  0xf0, 0xee, 0xed, 0xeb, 0xe9, 0xe8, 0xe6, 0xe4,
  0xe3, 0xe1, 0xe0, 0xde, 0xdd, 0xdb, 0xda, 0xd8, 
  0xd7, 0xd5, 0xd4, 0xd3, 0xd1, 0xd0, 0xcf, 0xcd,
  0xcc, 0xcb, 0xca, 0xc8, 0xc7, 0xc6, 0xc5, 0xc4, 
  0xc2, 0xc1, 0xc0, 0xbf, 0xbe, 0xbd, 0xbc, 0xbb,
  0xba, 0xb9, 0xb8, 0xb7, 0xb6, 0xb5, 0xb4, 0xb3, 
  0xb2, 0xb1, 0xb0, 0xaf, 0xae, 0xad, 0xac, 0xab,
  0xaa, 0xa9, 0xa8, 0xa8, 0xa7, 0xa6, 0xa5, 0xa4, 
  0xa3, 0xa3, 0xa2, 0xa1, 0xa0, 0x9f, 0x9f, 0x9e,
  0x9d, 0x9c, 0x9c, 0x9b, 0x9a, 0x99, 0x99, 0x98, 
  0x97, 0x97, 0x96, 0x95, 0x95, 0x94, 0x93, 0x93,
  0x92, 0x91, 0x91, 0x90, 0x8f, 0x8f, 0x8e, 0x8e, 
  0x8d, 0x8c, 0x8c, 0x8b, 0x8b, 0x8a, 0x89, 0x89,
  0x88, 0x88, 0x87, 0x87, 0x86, 0x85, 0x85, 0x84, 
  0x84, 0x83, 0x83, 0x82, 0x82, 0x81, 0x81, 0x80
};

static __device__ const unsigned int __internal_invSqrtCubeTab[96] =
{ 
  0xfa0bf8fe, 0xee6b28fa, 0xe5f024f7, 0xdaf268f3,
  0xd2f000f0, 0xc890c0ec, 0xc10378e9, 0xb9a758e6,
  0xb4da40e4, 0xadcea0e1, 0xa6f278de, 0xa279c0dc,
  0x9beb48d9, 0x97a5c4d7, 0x916340d4, 0x8d4fc8d2,
  0x895000d0, 0x8563b8ce, 0x818ac0cc, 0x7dc4e8ca,
  0x7a1200c8, 0x7671d8c6, 0x72e440c4, 0x6f6908c2,
  0x6db240c1, 0x6a523cbf, 0x670424bd, 0x6563c0bc,
  0x623028ba, 0x609ce8b9, 0x5d8364b7, 0x5bfd18b6,
  0x58fd40b4, 0x5783a8b3, 0x560e48b2, 0x533000b0,
  0x51c70caf, 0x506238ae, 0x4da4c0ac, 0x4c4c10ab,
  0x4af768aa, 0x49a6b8a9, 0x485a00a8, 0x471134a7,
  0x45cc58a6, 0x434e40a4, 0x4214f8a3, 0x40df88a2,
  0x3fade0a1, 0x3e8000a0, 0x3d55dc9f, 0x3c2f789e,
  0x3c2f789e, 0x3b0cc49d, 0x39edc09c, 0x38d2609b,
  0x37baa89a, 0x36a68899, 0x35960098, 0x34890497,
  0x34890497, 0x337f9896, 0x3279ac95, 0x31774094,
  0x30784893, 0x30784893, 0x2f7cc892, 0x2e84b091,
  0x2d900090, 0x2d900090, 0x2c9eac8f, 0x2bb0b88e,
  0x2bb0b88e, 0x2ac6148d, 0x29dec08c, 0x29dec08c,
  0x28fab08b, 0x2819e88a, 0x2819e88a, 0x273c5889,
  0x273c5889, 0x26620088, 0x258ad487, 0x258ad487,
  0x24b6d886, 0x24b6d886, 0x23e5fc85, 0x23184084,
  0x23184084, 0x224d9883, 0x224d9883, 0x21860882,
  0x21860882, 0x20c18081, 0x20c18081, 0x20000080
};

__device_func__(float __internal_frcp_kernel (float x,enum cudaRoundMode mode))
{
  unsigned long long prod;
  volatile union __cudart_FloatUintCvt arg;
  unsigned int expo;
  unsigned int sign;
  unsigned f, y;
    
  arg.f = x;
  sign = arg.i & 0x80000000;
  expo = (arg.i >> 23);
  expo = expo & 0xff;
  f = expo - 1;
    
  if (f <= 0xFD) {
    y = (arg.i << 8);
    y = y | 0x80000000;
    /* initial approximation */
    arg.i = __internal_rcpTab[(y >> 24) - 128];
    /* first NR iteration */
    f = arg.i * arg.i;
    f = f << 16;
    prod = ((unsigned long long)y) * f;
    arg.i = (arg.i << 24) - (unsigned)(prod >> 32);
    /* second NR iteration */  
    f = arg.i + arg.i;
    prod = ((unsigned long long)y) * f;
    f = (unsigned)(-(int)(prod >> 32));
    prod = ((unsigned long long)arg.i) * f;
    y = y >> 8;
    /* compute exponent */
    expo = (2 * 127) - expo - 2;
    arg.i = (unsigned)(prod >> 32);
    if (mode == cudaRoundNearest) {
      arg.i = arg.i >> 6;
    } else {
      arg.i = (arg.i + 32) >> 6;
    }
    if ((int)expo >= 0) {
      f = y * arg.i;
      arg.i = ((expo << 23) + arg.i) | sign;
    } else {
      /* result is a denormal */
      expo = -(int)expo;
      arg.i = arg.i >> expo;
      f = y * arg.i;
      arg.i = arg.i | sign;
    }
    if (mode == cudaRoundNearest) {
      expo = f + y;
      if ((int)f < 0) f = (unsigned)(-(int)f);
      if ((int)expo < 0) expo = (unsigned)(-(int)expo);
      if (expo < f) arg.i++;
    } else if (mode == cudaRoundZero) {
      if ((int)f > 0) arg.i = arg.i - 1;
    } else if (mode == cudaRoundPosInf) {
      if (((int)f > 0) &&  sign) arg.i = arg.i - 1;
      if (((int)f < 0) && !sign) arg.i = arg.i + 1;
    } else { /* mode == cudaRoundMinInf */
      if (((int)f > 0) && !sign) arg.i = arg.i - 1;
      if (((int)f < 0) &&  sign) arg.i = arg.i + 1;
    }
    return arg.f;
  } else {
    /* zero returns infinity. Must handle negative zero as well */
    if (!(arg.i << 1)) {
      arg.i = 0x7F800000 | arg.i;
      return arg.f;
    } 
    /* infinity returns zero of like sign */
    if ((arg.i << 1) == 0xff000000) {
      arg.i &= 0x80000000;
      return arg.f;
    }
    /* convert SNaNs to QNaNs */
    if ((arg.i << 1) > 0xff000000) {
      arg.i |= 0x00400000;
      return arg.f;
    }
    /* denormals */
    f = 0;
    arg.i <<= 8;
    do {
      f++;
      arg.i <<= 1;
    } while ((int)arg.i > 0);
    arg.i >>= 8;
    arg.i |= sign;
    arg.f = __internal_frcp_kernel (arg.f, mode);
    expo = ((arg.i << 1) >> 24);
    if ((expo + f) < 255) {
      arg.i = (arg.i + (f << 23));
      return arg.f;
    } 
    if (mode == cudaRoundNearest) {
      arg.i = (arg.i & 0x80000000) | 0x7f800000;
    } else if (mode == cudaRoundZero) {
      arg.i = (arg.i & 0x80000000) | 0x7f7fffff;
    } else if (mode == cudaRoundPosInf) {
      arg.i = (arg.i & 0x80000000) | ((sign) ? 0x7f7fffff : 0x7f800000);
    } else { /* mode == cudaRoundMinInf */
      arg.i = (arg.i & 0x80000000) | ((sign) ? 0x7f800000 : 0x7f7fffff);
    }
    return arg.f;
  }
}

__device_func__(float __internal_fsqrt_kernel (float radicand,
                                               enum cudaRoundMode mode))
{
  unsigned long long prod;
  volatile union __cudart_FloatUintCvt arg;
  unsigned int expo;
  unsigned int s, f, x;

  arg.f = radicand;
  expo = arg.i >> 23;
  expo = expo & 0xff;
  f = expo - 1;

  if ((arg.i <= 0x80000000) && (f <= 0xFD)) {
    /* normalize input argument */
    x = (arg.i << 8) | 0x80000000;
    x = x >> (expo & 1);
    /* initial approximation */
    arg.i = f = __internal_invSqrtCubeTab[((unsigned)x >> 25) - 32];
    /* first NR iteration */
    prod = ((unsigned long long)x) * f;
    arg.i = ((arg.i * 3) << 22) - (unsigned)(prod >> 32);
    /* second NR iteration */
    prod = ((unsigned long long)arg.i) * arg.i;
    s = (unsigned)(prod >> 32);
    prod = ((unsigned long long)x) * s;
    f = 0x30000000 - (unsigned)(prod >> 32);
    prod = ((unsigned long long)f) * arg.i; 
    arg.i = (unsigned)(prod >> 32);
    /* compute sqrt(x) as x * 1/sqrt(x) */
    prod = ((unsigned long long)x) * arg.i;
    arg.i = (unsigned)(prod >> 32);
    if (mode == cudaRoundNearest) {
      arg.i = arg.i >> 3;
    } else {
      arg.i = (arg.i + 4) >> 3;
    }
    x = (x << 16) - (arg.i * arg.i);
    /* round to nearest based on remainder; tie case impossible */
    if (mode == cudaRoundNearest) {
      f = x - (2 * arg.i + 1);
      if ((int)f < 0) f = (unsigned)(-(int)f);
      if ((int)x < 0) x = (unsigned)(-(int)x);
      if (f < x) arg.i ++;
    } else if ((mode == cudaRoundZero) || (mode == cudaRoundMinInf)) {
      if ((int)x < 0) arg.i--;
    } else if (mode == cudaRoundPosInf) {
      if ((int)x > 0) arg.i++;
    }
    arg.i = arg.i + (((expo + 125) & ~0x1) << 22);
    return arg.f;
  } else {
    /* if zero, or positive infinity, return argument */
    if (!(arg.i << 1) || (arg.i == 0x7F800000)) {
      return arg.f;
    } 
    /* if NaN, return argument, possibly converted to QNaN */
    if ((arg.i << 1) > 0xFF000000) {
      arg.i |= 0x00400000;
      return arg.f;
    }
    /* if negative, return NaN: INDEFINITE */
    if (arg.i & 0x80000000) {
      arg.i = 0xFFC00000;
      return arg.f;
    } 
    /* denormal, normalize it before computing square root */
    x = 0;
    arg.i <<= 8;
    do {
      x++;
      arg.i <<= 1;
    } while ((int)arg.i > 0);
    arg.i >>= 8;
    arg.i += (x & 1) << 23;
    x     += (x & 1);
    arg.f = __internal_fsqrt_kernel (arg.f, mode);
    arg.i -= ((x >> 1) << 23);
    return arg.f;
  }
}

__device_func__(float __internal_fdiv_kernel (float dividend, float divisor,
                                              enum cudaRoundMode mode))
{
  unsigned long long prod;
  unsigned r, f, x, y, expox, expoy, sign;
  volatile union __cudart_FloatUintCvt cvtx, cvty, res;

  cvtx.f = dividend;
  cvty.f = divisor;
  expox = ((cvtx.i >> 23) & 0xff) - 1;
  expoy = ((cvty.i >> 23) & 0xff) - 1;
  sign  = ((cvtx.i ^ cvty.i) & 0x80000000);

  if ((expox <= 0xFD) && (expoy <= 0xFD)) {
divide:
    expox = expox - expoy + 127 - 1;
    expoy = expox;
    /* extract mantissas */
    y = (cvty.i << 8) | 0x80000000;
    x = (cvtx.i & 0x00ffffff) | 0x00800000;
    /* initial approximation */
    r = __internal_rcpTab[(y >> 24) - 128];
    /* first NR iteration */
    f = r * r;
    prod = ((unsigned long long)y) * (f << 16);
    r = (r << 24) - (unsigned)(prod >> 32);
    /* second NR iteration */
    prod = ((unsigned long long)y) * (r << 1);
    f = (unsigned)-(int)(prod >> 32);
    prod = ((unsigned long long)f) * (r << 1);
    r = (unsigned)(prod >> 32);
    /* produce quotient */
    prod = ((unsigned long long)x) * (r << 1);
    /* normalize mantissa */
    if (((int)((prod >> 32) << 8)) > 0) {
      expox--;
      prod = prod + prod;
    }
    if (mode == cudaRoundNearest) {
      /* preliminary mantissa */
      r = (unsigned)(prod >> 32);
      y = y >> 8;
      /* result is a normal */
      if (expox <= 0xFD) {
        int rem0, rem1, inc;
        /* round mantissa to nearest even */
        prod = ((unsigned long long)y) * r;
        x = x << (23 + ((prod >> 32) >> 15));
        rem1 = x - (unsigned)(prod & 0xffffffff);
        rem0 = rem1 - y;
        inc = abs(rem0) < abs(rem1);
        /* merge sign, mantissa, exponent for final result */
        res.i = sign | ((expox << 23) + r + inc);
        return res.f;
      } else if ((int)expox >= 254) {
        /* overflow: return infinity */
        res.i = sign | 0x7f800000;
        return res.f;
      } else {
        /* underflow: result is zero, denormal, or smallest normal */
        int shift = -(int)expox;
        if (shift > 23) {
          /* result is zero or smallest denormal */
          r = (shift < 25) && ((x != y) || (r > 0x00ff0000));
          res.i = sign | r;
          return res.f;
        } 
        if (x == y) {
          /* result is denormal */
          shift = -(int)expoy;
          r = 0x00800000 >> shift;
          res.i = sign | r;
          return res.f;
        }
        {
          unsigned long long tempx;
          long long remlo, remhi;
          /* result is denormal or smallest normal */
          r = r >> shift;
          prod  = ((unsigned long long)y) * r;
          tempx = ((unsigned long long)x) << (23 - shift);
          remlo = 2 * tempx - 2 * prod - y;
          remhi = remlo + 2 * tempx;
          if (remlo < 0) remlo = -remlo;
          if (remhi < 0) remhi = -remhi;
          if (remhi < remlo) tempx = 2 * tempx;
          remlo = tempx - prod;
          remhi = remlo - y;
          if (remlo < 0) remlo = -remlo;
          if (remhi < 0) remhi = -remhi;
          if ((remhi < remlo) || ((remhi == remlo) && (r & 1))) r++;
          res.i = sign | r;
          return res.f;
        }
      }
    } else if (mode == cudaRoundZero) {
      /* preliminary mantissa */
      prod += 0x0000000080000000ULL;
      r = (unsigned)(prod >> 32);
      y = y >> 8;
      /* result is a normal */
      if (expox <= 0xFD) {
        int rem1;
        prod = ((unsigned long long)y) * r;
        x = x << (23 + ((prod >> 32) >> 15));
        rem1 = x - (unsigned)(prod & 0xffffffff);
        if (rem1 < 0) r--;
        r = (expox << 23) + r;
        if (r == 0x7f800000) r = 0x7f7fffff;
        res.i = sign | r;
        return res.f;
      } else if ((int)expox >= 254) {
        /* overflow: return largest normal */
        res.i = sign | 0x7f7fffff;
        return res.f;
      } else {
        /* underflow: result is zero, denormal, or smallest normal */
        int shift = -(int)expox;
        if ((x == y) && (shift < 31)) {
          shift = -(int)expoy;
          r = 0x00800000 >> shift;
          res.i = sign | r;
          return res.f;
        }
        if (shift > 23) {
          r = 0;
          res.i = sign | r;
          return res.f;
        } 
        {
          unsigned long long tempx;
          long long remlo, remhi;
          /* result is denormal or smallest normal */
          r = r >> shift;
          prod  = ((unsigned long long)y) * r;
          tempx = ((unsigned long long)x) << (23 - shift);
          remlo = 2 * tempx - 2 * prod - y;
          remhi = remlo + 2 * tempx;
          if (remlo < 0) remlo = -remlo;
          if (remhi < 0) remhi = -remhi;
          if (remhi < remlo) tempx = 2 * tempx;
          remlo = tempx - prod;
          if ((remlo < 0) & (r != 0)) r--;
          res.i = sign | r;
          return res.f;
        }
      }
    } else if (mode == cudaRoundPosInf) {
      /* preliminary mantissa */
      prod += 0x0000000080000000ULL;
      r = (unsigned)(prod >> 32);
      y = y >> 8;
      /* result is a normal */
      if (expox <= 0xFD) {
        int rem1;
        prod = ((unsigned long long)y) * r;
        x = x << (23 + ((prod >> 32) >> 15));
        rem1 = x - (unsigned)(prod & 0xffffffff);
        if ((rem1 < 0) &&  (sign)) r--;
        if ((rem1 > 0) && (!sign)) r++;
        r = (expox << 23) + r;
        if ((r == 0x7f800000) && (sign)) r = 0x7f7fffff;
        res.i = sign | r;
        return res.f;
      } else if ((int)expox >= 254) {
        /* overflow: return largest normal, or infinity */
        r = sign ? 0x7f7fffff : 0x7f800000;
        res.i = sign | r;
        return res.f;
      } else {
        /* underflow: result is zero, denormal, or smallest normal */
        int shift = -(int)expox;
        if ((x == y) && (shift <= 24)) {
          shift = -(int)expoy;
          r = 0x00800000 >> shift;
          if (r == 0) r = !sign; 
          res.i = sign | r;
          return res.f;
        }
        if (shift > 23) {
          r = !sign; 
          res.i = sign | r;
          return res.f;
        } 
        {
          unsigned long long tempx;
          long long remlo, remhi;
          /* result is denormal or smallest normal */
          r = r >> shift;
          prod  = ((unsigned long long)y) * r;
          tempx = ((unsigned long long)x) << (23 - shift);
          remlo = 2 * tempx - 2 * prod - y;
          remhi = remlo + 2 * tempx;
          if (remlo < 0) remlo = -remlo;
          if (remhi < 0) remhi = -remhi;
          if (remhi < remlo) tempx = 2 * tempx;
          remlo = tempx - prod;
          if ((remlo < 0) && (r != 0) &&  (sign)) r--;
          if ((remlo > 0) &&             (!sign)) r++;
          res.i = sign | r;
          return res.f;
        }
      }
    } else if (mode == cudaRoundMinInf) {
      /* preliminary mantissa */
      prod += 0x0000000080000000ULL;
      r = (unsigned)(prod >> 32);
      y = y >> 8;
      /* result is a normal */
      if (expox <= 0xFD) {
        int rem1;
        prod = ((unsigned long long)y) * r;
        x = x << (23 + ((prod >> 32) >> 15));
        rem1 = x - (unsigned)(prod & 0xffffffff);
        if ((rem1 < 0) && (!sign)) r--;
        if ((rem1 > 0) &&  (sign)) r++;
        r = (expox << 23) + r;
        if ((r == 0x7f800000) && (!sign)) r = 0x7f7fffff;
        res.i = sign | r;
        return res.f;
      } else if ((int)expox >= 254) {
        /* overflow: return largest normal, or infinity */
        r = sign ? 0x7f800000 : 0x7f7fffff;
        res.i = sign | r;
        return res.f;
      } else {
        /* underflow: result is zero, denormal, or smallest normal */
        int shift = -(int)expox;
        if ((x == y) && (shift <= 24)) {
          shift = -(int)expoy;
          r = 0x00800000 >> shift;
          if (r == 0) r = !!sign; 
          res.i = sign | r;
          return res.f;
        }
        if (shift > 23) {
          r = !!sign; 
          res.i = sign | r;
          return res.f;
        } 
        {
          unsigned long long tempx;
          long long remlo, remhi;
          /* result is denormal or smallest normal */
          r = r >> shift;
          prod  = ((unsigned long long)y) * r;
          tempx = ((unsigned long long)x) << (23 - shift);
          remlo = 2 * tempx - 2 * prod - y;
          remhi = remlo + 2 * tempx;
          if (remlo < 0) remlo = -remlo;
          if (remhi < 0) remhi = -remhi;
          if (remhi < remlo) tempx = 2 * tempx;
          remlo = tempx - prod;
          if ((remlo < 0) && (r != 0) && (!sign)) r--;
          if ((remlo > 0) &&              (sign)) r++;
          res.i = sign | r;
          return res.f;
        }
      }
    }
  }
  {
    int xzero, yzero, xinf, yinf, xnan, ynan;
    
    xnan = (cvtx.i << 1) > 0xff000000;
    ynan = (cvty.i << 1) > 0xff000000;
    /* handle NaNs. Convert SNaNs to QNaNs */
    if (xnan) {
      res.i = cvtx.i | 0x00400000;
      return res.f;
    }
    if (ynan) {
      res.i = cvty.i | 0x00400000;
      return res.f;
    }
    xzero = (cvtx.i << 1) == 0x00000000;
    yzero = (cvty.i << 1) == 0x00000000;
    xinf  = (cvtx.i << 1) == 0xff000000;
    yinf  = (cvty.i << 1) == 0xff000000;
    /* 0/0 and INF/INF are invalid operations. Return INDEFINITE */
    if ((xzero & yzero) | (xinf & yinf)) {
      res.i = 0xffc00000;
      return res.f;
    }
    /* x/INF and 0/y -> 0 */
    if (xzero | yinf) {
      res.i = sign;
      return res.f;
    }
    /* x/0 and INF/y -> INF */
    if (yzero | xinf) {
      res.i = sign | 0x7f800000;
      return res.f;
    }
    /* normalize denormals */
    if ((int)expox < 0) {
      cvtx.i = cvtx.i << 9;
      while ((int)cvtx.i >= 0) {
        expox--;
        cvtx.i = cvtx.i + cvtx.i;
      }
      cvtx.i = cvtx.i >> 8;
    }
    if ((int)expoy < 0) {
      cvty.i = cvty.i << 9;
      while ((int)cvty.i >= 0) {
        expoy--;
        cvty.i = cvty.i + cvty.i;
      }
      cvty.i = cvty.i >> 8;
    }
    goto divide;
  }
}

__device_func__(float __internal_fmul_kernel (float a, float b, 
                                              enum cudaRoundMode mode))
{
  unsigned long long product;
  volatile union __cudart_FloatUintCvt xx, yy;
  unsigned expo_x, expo_y;
    
  xx.f = a;
  yy.f = b;

  expo_y = 0xFF;
  expo_x = expo_y & (xx.i >> 23);
  expo_x = expo_x - 1;
  expo_y = expo_y & (yy.i >> 23);
  expo_y = expo_y - 1;
    
  if ((expo_x <= 0xFD) && 
      (expo_y <= 0xFD)) {
multiply:
    expo_x = expo_x + expo_y;
    expo_y = xx.i ^ yy.i;
    xx.i = xx.i & 0x00ffffff;
    yy.i = yy.i << 8;
    xx.i = xx.i | 0x00800000;
    yy.i = yy.i | 0x80000000;
    /* compute product */
    product = ((unsigned long long)xx.i) * yy.i;
    expo_x = expo_x - 127 + 2;
    expo_y = expo_y & 0x80000000;
    xx.i = (unsigned int)(product >> 32);
    yy.i = (unsigned int)(product & 0xffffffff);
    /* normalize mantissa */
    if (xx.i < 0x00800000) {
      xx.i = (xx.i << 1) | (yy.i >> 31);
      yy.i = (yy.i << 1);
      expo_x--;
    }
    if (expo_x <= 0xFD) {
      xx.i = xx.i | expo_y;          /* OR in sign bit */
      xx.i = xx.i + (expo_x << 23);  /* add in exponent */
      /* round result to nearest or even */
      if (mode == cudaRoundNearest) {
        if (yy.i < 0x80000000) return xx.f;
        xx.i += ((yy.i == 0x80000000) ? (xx.i & 1) : (yy.i >> 31));
      } else if (mode == cudaRoundZero) {          
      } else if (mode == cudaRoundPosInf) {
        xx.i += (yy.i && !expo_y);
      } else if (mode == cudaRoundMinInf) {
        xx.i += (yy.i && expo_y);
      }
      return xx.f;    
    } else if ((int)expo_x >= 254) {
      /* overflow: return infinity or largest normal */
      if (mode == cudaRoundNearest) {
         xx.i = expo_y | 0x7F800000;
      } else if (mode == cudaRoundZero) {
         xx.i = expo_y | 0x7F7FFFFF;
      } else if (mode == cudaRoundPosInf) {
         xx.i = (expo_y ? 0xff7fffff : 0x7F800000);
      } else { /* (mode == cudaRoundMinInf) */
         xx.i = (expo_y ? 0xFF800000 : 0x7f7fffff);
      }
      return xx.f;
    } else {
      /* zero, denormal, or smallest normal */
      expo_x = ((unsigned int)-((int)expo_x));
      if (mode == cudaRoundNearest) {
        if (expo_x > 25) {
          /* massive underflow: return 0 */
          xx.i = expo_y;
          return xx.f;
        } else {
          yy.i = (xx.i << (32 - expo_x)) | ((yy.i) ? 1 : 0);
          xx.i = expo_y + (xx.i >> expo_x);
          xx.i += ((yy.i == 0x80000000) ? (xx.i & 1) : (yy.i >> 31));
          return xx.f;
        }
      } else if (mode == cudaRoundZero) {
        if (expo_x > 25) expo_x = 25; 
        xx.i = expo_y + (xx.i >> expo_x);
        return xx.f;
      } else if (mode == cudaRoundPosInf) {
        if (expo_x > 25) expo_x = 25;
        yy.i = (xx.i << (32 - expo_x)) | ((yy.i) ? 1 : 0);
        xx.i = expo_y + (xx.i >> expo_x);
        xx.i += (yy.i && !expo_y);
        return xx.f;
      } else { /* (mode == cudaRoundMinInf) */
        if (expo_x > 25) expo_x = 25;
        yy.i = (xx.i << (32 - expo_x)) | ((yy.i) ? 1 : 0);
        xx.i = expo_y + (xx.i >> expo_x);
        xx.i += (yy.i && expo_y);
        return xx.f;
      } 
    }
  } else {
    product = xx.i ^ yy.i;
    product = product & 0x80000000;
    if (!(xx.i & 0x7fffffff)) {
      if (expo_y != 254) {
        xx.i = (unsigned int)product;
        return xx.f;
      }
      expo_y = yy.i << 1;
      if (expo_y == 0xFF000000) {
        xx.i = expo_y | 0x00C00000;
      } else {
        xx.i = yy.i | 0x00400000;
      }
      return xx.f;
    }
    if (!(yy.i & 0x7fffffff)) {
      if (expo_x != 254) {
        xx.i = (unsigned int)product;
        return xx.f;
      }
      expo_x = xx.i << 1;
      if (expo_x == 0xFF000000) {
        xx.i = expo_x | 0x00C00000;
      } else {
        xx.i = xx.i | 0x00400000;
      }
      return xx.f;
    }
    if ((expo_y != 254) && (expo_x != 254)) {
      expo_y++;
      expo_x++;
      if (expo_x == 0) {
        expo_y |= xx.i & 0x80000000;
        /*
         * If both operands are denormals, we only need to normalize 
         * one of them as the result will be either a denormal or zero.
         */
        xx.i = xx.i << 8;
        while (!(xx.i & 0x80000000)) {
          xx.i <<= 1;
          expo_x--;
        }
        xx.i = (xx.i >> 8) | (expo_y & 0x80000000);
        expo_y &= ~0x80000000;
        expo_y--;
        goto multiply;
      }
      if (expo_y == 0) {
        expo_x |= yy.i & 0x80000000;
        yy.i = yy.i << 8;
        while (!(yy.i & 0x80000000)) {
          yy.i <<= 1;
          expo_y--;
        }
        yy.i = (yy.i >> 8) | (expo_x & 0x80000000);
        expo_x &= ~0x80000000;
        expo_x--;
        goto multiply;
      }
    }
    expo_x = xx.i << 1;
    expo_y = yy.i << 1;
    /* if x is NaN, return x */
    if (expo_x > 0xFF000000) {
      /* cvt any SNaNs to QNaNs */
      xx.i = xx.i | 0x00400000;
      return xx.f;
    }
    /* if y is NaN, return y */
    if (expo_y > 0xFF000000) {
      /* cvt any SNaNs to QNaNs */
      xx.i = yy.i | 0x00400000;
      return xx.f;
    } 
    xx.i = (unsigned int)product | 0x7f800000;
    return xx.f;
  }
}

__device_func__(float __internal_fmaf_kernel (float a, float b, float c,
                                              enum cudaRoundMode mode))
{
  unsigned long long product;
  unsigned int xx, yy, zz, ww;
  unsigned int temp, s, u;
  unsigned int expo_x, expo_y, expo_z;
  volatile union __cudart_FloatUintCvt cvt;
  
  cvt.f = a;
  xx = cvt.i;
  cvt.f = b;
  yy = cvt.i;
  cvt.f = c;
  zz = cvt.i;

  temp = 0xff;
  expo_x = temp & (xx >> 23);
  expo_x = expo_x - 1;
  expo_y = temp & (yy >> 23);
  expo_y = expo_y - 1;
  expo_z = temp & (zz >> 23);
  expo_z = expo_z - 1;

  if (!((expo_x <= 0xFD) && 
        (expo_y <= 0xFD) &&
        (expo_z <= 0xFD))) {
    /* fmad (nan, y, z) --> nan
       fmad (x, nan, z) --> nan
       fmad (x, y, nan) --> nan 
    */
    if ((yy << 1) > 0xff000000) {
      return b + b;
    }
    if ((zz << 1) > 0xff000000) {
      return c + c;
    }
    if ((xx << 1) > 0xff000000) {
      return a + a;
    }
    /* fmad (0, inf, z) --> NaN
       fmad (inf, 0, z) --> NaN
       fmad (-inf,+y,+inf) --> NaN
       fmad (+x,-inf,+inf) --> NaN
       fmad (+inf,-y,+inf) --> NaN
       fmad (-x,+inf,+inf) --> NaN
       fmad (-inf,-y,-inf) --> NaN
       fmad (-x,-inf,-inf) --> NaN
       fmad (+inf,+y,-inf) --> NaN
       fmad (+x,+inf,-inf) --> NaN
    */
    if ((((xx << 1) == 0) && ((yy << 1) == 0xff000000)) ||
        (((yy << 1) == 0) && ((xx << 1) == 0xff000000))) {
      cvt.i = 0xffc00000;
      return cvt.f;
    }
    if ((zz << 1) == 0xff000000) {
      if (((yy << 1) == 0xff000000) || ((xx << 1) == 0xff000000)) {
        if ((int)(xx ^ yy ^ zz) < 0) {
          cvt.i = 0xffc00000;
          return cvt.f;
        }
      }
    }
    /* fmad (inf, y, z) --> inf
       fmad (x, inf, z) --> inf
       fmad (x, y, inf) --> inf
    */
    if ((xx << 1) == 0xff000000) {
      xx = xx ^ (yy & 0x80000000);
      cvt.i = xx;
      return cvt.f;
    }
    if ((yy << 1) == 0xff000000) {
      yy = yy ^ (xx & 0x80000000);
      cvt.i = yy;
      return cvt.f;
    }
    if ((zz << 1) == 0xff000000) {
      cvt.i = zz;
      return cvt.f;
    }
    /* fmad (+0, -y, -0) --> -0
       fmad (-0, +y, -0) --> -0
       fmad (+x, -0, -0) --> -0
       fmad (-x, +0, -0) --> -0
    */
    if (zz == 0x80000000) {
      if (((xx << 1) == 0) || ((yy << 1) == 0)) {
        if ((int)(xx ^ yy) < 0) {
          cvt.i = zz;
          return cvt.f;
        }
      }
    }
    /* fmad (0, y, 0) --> +0
       fmad (x, 0, 0) --> +0
    */
    if (((zz << 1) == 0) && 
        (((xx << 1) == 0) || ((yy << 1) == 0))) {
      if (mode == cudaRoundMinInf) {
        zz = 0x80000000 & (xx ^ yy ^ zz);
      } else {
        zz &= 0x7fffffff;
      }
      cvt.i = zz;
      return cvt.f;
    }
    /* fmad (0, y, z) --> z
       fmad (x, 0, z) --> z
     */
    if (((xx << 1) == 0) || ((yy << 1) == 0)) {
      cvt.i = zz;
      return cvt.f;
    }
    /* normalize x, if denormal */
    if (expo_x == (unsigned)-1) {
      temp = xx & 0x80000000;
      xx = xx << 8;
      while (!(xx & 0x80000000)) {
        xx <<= 1;
        expo_x--;
      }
      expo_x++;
      xx = (xx >> 8) | temp;
    }
    /* normalize y, if denormal */
    if (expo_y == (unsigned)-1) {
      temp = yy & 0x80000000;
      yy = yy << 8;
      while (!(yy & 0x80000000)) {
        yy <<= 1;
        expo_y--;
      }
      expo_y++;
      yy = (yy >> 8) | temp;
    }
    /* normalize z, if denormal */
    if ((expo_z == (unsigned)-1) && ((zz << 1) != 0)) {
      temp = zz & 0x80000000;
      zz = zz << 8;
      while (!(zz & 0x80000000)) {
        zz <<= 1;
        expo_z--;
      }
      expo_z++;
      zz = (zz >> 8) | temp;
    }
  }
    
  expo_x = expo_x + expo_y;
  expo_y = xx ^ yy;
  xx = xx & 0x00ffffff;
  yy = yy << 8;
  xx = xx | 0x00800000;
  yy = yy | 0x80000000;

  product = ((unsigned long long)xx) * yy;
  xx = (unsigned)(product >> 32);
  yy = (unsigned)(product & 0xffffffff);

  expo_x = expo_x - 127 + 2;
  expo_y = expo_y & 0x80000000;
  /* normalize mantissa */
  if (xx < 0x00800000) {
    xx = (xx << 1) | (yy >> 31);
    yy = (yy << 1);
    expo_x--;
  }
  temp = 0;

  if ((zz << 1) != 0) { /* z is not zero */
    s = zz & 0x80000000;
    zz &= 0x00ffffff;
    zz |= 0x00800000;
    ww = 0;
    /* compare and swap. put augend into xx:yy */
    if ((int)expo_z > (int)expo_x) {
      temp = expo_z;
      expo_z = expo_x;
      expo_x = temp;
      temp = zz;
      zz = xx;
      xx = temp;
      temp = ww;
      ww = yy;
      yy = temp;
      temp = expo_y;
      expo_y = s;
      s = temp;
    }
    /* augend_sign = expo_y, augend_mant = xx:yy, augend_expo = expo_x */
    /* addend_sign = s, addend_mant = zz:ww, addend_expo = expo_z */
    expo_z = expo_x - expo_z;
    u = expo_y ^ s;
    if (expo_z <= 49) {
      /* denormalize addend */
      temp = 0;
      while (expo_z >= 32) {
        temp = ww | (temp != 0);
        ww = zz;
        zz = 0;
        expo_z -= 32;
      }
      if (expo_z) {
        temp = ((temp >> expo_z) | (ww << (32 - expo_z)) | 
                ((temp << (32 - expo_z)) != 0));
        ww = (ww >> expo_z) | (zz << (32 - expo_z));
        zz = (zz >> expo_z);
      }
      
    } else {
      temp = 1;
      ww = 0;
      zz = 0;
    }            
    if ((int)u < 0) {
      /* signs differ, effective subtraction */
      temp = (unsigned)(-(int)temp);
      s = (temp != 0);
      u = yy - s;
      s = u > yy;
      yy = u - ww;
      s += yy > u;
      xx = (xx - zz) - s;
      if (!(xx | yy | temp)) {
        /* complete cancelation, return 0 */
        if (mode == cudaRoundMinInf) {
          xx = 0x80000000;
        }
        cvt.i = xx;
        return cvt.f;
      }
      if ((int)xx < 0) {
        /* ooops, augend had smaller mantissa. Negate mantissa and flip
           sign of result*/
        temp = ~temp;
        yy = ~yy;
        xx = ~xx;
        if (++temp == 0) {
          if (++yy == 0) {
            ++xx;
          }
        }
        expo_y ^= 0x80000000;
      }
      /* normalize mantissa, if necessary */
      while (!(xx & 0x00800000)) {
        xx = (xx << 1) | (yy >> 31);
        yy = (yy << 1);
        expo_x--;
      }
    } else {
      /* signs are the same, effective addition */
      yy = yy + ww;
      s =  yy < ww;
      xx = xx + zz + s;
      if (xx & 0x01000000) {
        temp = temp | (yy << 31);
        yy = (yy >> 1) | (xx << 31);
        xx = ((xx & 0x80000000) | (xx >> 1)) & ~0x40000000;
        expo_x++;
      }
    }
  }
  temp = yy | (temp != 0);
  if (expo_x <= 0xFD) {
    /* normal */
    xx |= expo_y; /* or in sign bit */
    if (mode == cudaRoundNearest) {
      s = xx & 1; /* mantissa lsb */
      xx += (temp == 0x80000000) ? s : (temp >> 31);
    } else if (mode == cudaRoundPosInf) {
      xx += temp && !expo_y;
    } else if (mode == cudaRoundMinInf) {
      xx += temp && expo_y;
    }
    xx = xx + (expo_x << 23); /* add in exponent */
    cvt.i = xx;
    return cvt.f;
  } else if ((int)expo_x >= 126) {
    /* overflow */
    if (mode == cudaRoundNearest) {  
      xx = expo_y | 0x7f800000;
    } else if (mode == cudaRoundZero) {
      xx = expo_y | 0x7F7FFFFF;
    } else if (mode == cudaRoundPosInf) {
      xx = expo_y ? 0xFF7FFFFF : 0x7f800000;
    } else if (mode == cudaRoundMinInf) {
      xx = expo_y ? 0xff800000 : 0x7f7fffff;
    }
    cvt.i = xx;
    return cvt.f;
  }
  /* subnormal */
  expo_x = (unsigned int)(-(int)expo_x);
  if (expo_x > 25) {
    /* massive underflow: return 0, or smallest denormal */
    xx = 0;
    if (mode == cudaRoundPosInf) {
      xx += !expo_y;
    } else if (mode == cudaRoundMinInf) {
      xx += !!expo_y;
    }
    cvt.i = expo_y | xx;
    return cvt.f;
  }
  temp = (xx << (32 - expo_x)) | ((temp) ? 1 : 0);
  xx = xx >> expo_x;
  if (mode == cudaRoundNearest) {
    xx = xx + ((temp == 0x80000000) ? (xx & 1) : (temp >> 31));
  } else if (mode == cudaRoundPosInf) {
    xx = xx + (!expo_y && temp);
  } else if (mode == cudaRoundMinInf) {
    xx = xx + (expo_y && temp);
  }
  xx = expo_y + xx; /* add in sign bit */
  cvt.i = xx;
  return cvt.f;
}

__device_func__(float __internal_fadd_kernel (float a, float b, 
                                              enum cudaRoundMode mode))
{
  volatile union __cudart_FloatUintCvt xx, yy;
  unsigned int expo_x;
  unsigned int expo_y;
  unsigned int temp;

  xx.f = a;
  yy.f = b;

  /* make bigger operand the augend */
  expo_y = yy.i << 1;
  if (expo_y > (xx.i << 1)) {
    expo_y = xx.i;
    xx.i   = yy.i;
    yy.i   = expo_y;
  }
    
  temp = 0xff;
  expo_x = temp & (xx.i >> 23);
  expo_x = expo_x - 1;
  expo_y = temp & (yy.i >> 23);
  expo_y = expo_y - 1;

  if ((expo_x <= 0xFD) && 
      (expo_y <= 0xFD)) {
add:
    expo_y = expo_x - expo_y;
    if (expo_y > 25) {
      expo_y = 31;
    }
    temp = xx.i ^ yy.i;
    xx.i = xx.i & ~0x7f000000;
    xx.i = xx.i |  0x00800000;
    yy.i = yy.i & ~0xff000000;
    yy.i = yy.i |  0x00800000;

    if ((int)temp < 0) {
      /* signs differ, effective subtraction */
      temp = 32 - expo_y;
      temp = (expo_y) ? (yy.i << temp) : 0;
      temp = (unsigned)(-((int)temp));
      xx.i = xx.i - (yy.i >> expo_y) - (temp ? 1 : 0);
      if (xx.i & 0x00800000) {
        if (expo_x <= 0xFD) {
          xx.i = xx.i + (expo_x << 23);
          if (mode == cudaRoundNearest) {
            if (temp < 0x80000000) return xx.f;
            xx.i += ((temp == 0x80000000) ? (xx.i & 1) : (temp >> 31));
          } else if (mode == cudaRoundZero) {              
          } else if (mode == cudaRoundPosInf) {
            xx.i += (temp && !(xx.i & 0x80000000));
          } else if (mode == cudaRoundMinInf) {
            xx.i += (temp && (xx.i & 0x80000000));
          }
          return xx.f;
        }
      } else {
        if ((temp | (xx.i << 1)) == 0) {
          /* operands cancelled, resulting in a clean zero */
          if (mode == cudaRoundMinInf) {
            xx.i = 0x80000000;
          } else {
            xx.i = 0;
          }
          return xx.f;
        }
        /* normalize result */
        yy.i = xx.i & 0x80000000;
        do {
          xx.i = (xx.i << 1) | (temp >> 31);
          temp <<= 1;
          expo_x--;
        } while (!(xx.i & 0x00800000));
        xx.i = xx.i | yy.i;
      }
    } else {
      /* signs are the same, effective addition */
      temp = 32 - expo_y;
      temp = (expo_y) ? (yy.i << temp) : 0;
      xx.i = xx.i + (yy.i >> expo_y);
      if (!(xx.i & 0x01000000)) {
        if (expo_x <= 0xFD) {
          xx.i = xx.i + (expo_x << 23);
          if (mode == cudaRoundNearest) {
            if (temp < 0x80000000) return xx.f;
            xx.i += ((temp == 0x80000000) ? (xx.i & 1) : (temp >> 31));
          } else if (mode == cudaRoundZero) {
          } else if (mode == cudaRoundPosInf) {
            xx.i += (temp && !(xx.i & 0x80000000));
          } else if (mode == cudaRoundMinInf) {
            xx.i += (temp && (xx.i & 0x80000000));
          }
          return xx.f;
        }
      } else {
        /* normalize result */
        temp = (xx.i << 31) | (temp >> 1);
        xx.i = ((xx.i & 0x80000000) | (xx.i >> 1)) & ~0x40000000;
        expo_x++;
      }
    }
    if (expo_x <= 0xFD) {
      xx.i = xx.i + (expo_x << 23);
      if (mode == cudaRoundNearest) {
        if (temp < 0x80000000) return xx.f;
        xx.i += ((temp == 0x80000000) ? (xx.i & 1) : (temp >> 31));
      } else if (mode == cudaRoundZero) {
      } else if (mode == cudaRoundPosInf) {
        xx.i += (temp && !(xx.i & 0x80000000));
      } else if (mode == cudaRoundMinInf) {
        xx.i += (temp && (xx.i & 0x80000000));
      }
      return xx.f;
    }
    if ((int)expo_x >= 254) {
      /* overflow: return infinity or largest normal */
      temp = xx.i & 0x80000000;
      if (mode == cudaRoundNearest) {
        xx.i = (temp) | 0x7f800000;
      } else if (mode == cudaRoundZero) {
        xx.i = (temp) | 0x7f7fffff;
      } else if (mode == cudaRoundMinInf) {
        xx.i = (temp ? 0xFF800000 : 0x7f7fffff);
      } else if (mode == cudaRoundPosInf) {
        xx.i = (temp ? 0xff7fffff : 0x7F800000);
      }
      return xx.f;
    }
    /* underflow: denormal, or smallest normal */
    expo_y = expo_x + 32;
    yy.i = xx.i &  0x80000000;
    xx.i = xx.i & ~0xff000000;
    expo_x = (unsigned)(-((int)expo_x));
    temp = xx.i << expo_y | ((temp) ? 1 : 0);
    xx.i = yy.i | (xx.i >> expo_x);
    if (mode == cudaRoundNearest) {
      xx.i += (temp == 0x80000000) ? (xx.i & 1) : (temp >> 31);
    } else if (mode == cudaRoundZero) {
    } else if (mode == cudaRoundPosInf) {
      xx.i += (temp && !yy.i);
    } else if (mode == cudaRoundMinInf) {
      xx.i += (temp && yy.i);
    }
    return xx.f;
  } else {
    /* handle special cases separately */
    if (!(yy.i << 1)) {
      if (mode == cudaRoundMinInf) {
        if (!(xx.i << 1)) {
          xx.i = xx.i | yy.i;
        }
      } else {
        if (xx.i == 0x80000000) {
          xx.i = yy.i;
        }
      }
      if ((xx.i << 1) > 0xff000000) {
        xx.i |= 0x00400000;
      }
      return xx.f;
    }
    if ((expo_y != 254) && (expo_x != 254)) {
      /* remove sign bits */
      if (expo_x == (unsigned int) -1) {
        temp = xx.i & 0x80000000;
        xx.i = xx.i << 8;
        while (!(xx.i & 0x80000000)) {
          xx.i <<= 1;
          expo_x--;
        }
        expo_x++;
        xx.i = (xx.i >> 8) | temp;
      }
      if (expo_y == (unsigned int) -1) {
        temp = yy.i & 0x80000000;
        yy.i = yy.i << 8;
        while (!(yy.i & 0x80000000)) {
          yy.i <<= 1;
          expo_y--;
        }
        expo_y++;
        yy.i = (yy.i >> 8) | temp;
      }
      goto add;
    }
    expo_x = xx.i << 1;
    expo_y = yy.i << 1;
    /* if x is NaN, return x */
    if (expo_x > 0xff000000) {
      /* cvt any SNaNs to QNaNs */
      xx.i = xx.i | 0x00400000;
      return xx.f;
    }
    /* if y is NaN, return y */
    if (expo_y > 0xff000000) {
      /* cvt any SNaNs to QNaNs */
      xx.i = yy.i | 0x00400000;
      return xx.f;
    }
    if ((expo_x == 0xff000000) && (expo_y == 0xff000000)) {
      /*
       * subtraction of infinities with the same sign, and addition of
       * infinities of unlike sign is undefined: return NaN INDEFINITE
       */
      expo_x = xx.i ^ yy.i;
      xx.i = xx.i | ((expo_x) ? 0xffc00000 : 0);
      return xx.f;
    }
    /* handle infinities */
    if (expo_y == 0xff000000) {
      xx.i = yy.i;
    }
    return xx.f;
  }
}

__device_func__(float __frcp_rn (float a))
{
  return __internal_frcp_kernel (a, cudaRoundNearest);
}

__device_func__(float __frcp_rz (float a))
{
  return __internal_frcp_kernel (a, cudaRoundZero);
}

__device_func__(float __frcp_rd (float a))
{
  return __internal_frcp_kernel (a, cudaRoundMinInf);
}

__device_func__(float __frcp_ru (float a))
{
  return __internal_frcp_kernel (a, cudaRoundPosInf);
}

__device_func__(float __fsqrt_rn (float a))
{
  return __internal_fsqrt_kernel (a, cudaRoundNearest);
}

__device_func__(float __fsqrt_rz (float a))
{
  return __internal_fsqrt_kernel (a, cudaRoundZero);
}

__device_func__(float __fsqrt_rd (float a))
{
  return __internal_fsqrt_kernel (a, cudaRoundMinInf);
}

__device_func__(float __fsqrt_ru (float a))
{
  return __internal_fsqrt_kernel (a, cudaRoundPosInf);
}

__device_func__(float __fdiv_rn (float a, float b))
{
  return __internal_fdiv_kernel (a, b, cudaRoundNearest);
}

__device_func__(float __fdiv_rz (float a, float b))
{
  return __internal_fdiv_kernel (a, b, cudaRoundZero);
}

__device_func__(float __fdiv_rd (float a, float b))
{
  return __internal_fdiv_kernel (a, b, cudaRoundMinInf);
}

__device_func__(float __fdiv_ru (float a, float b))
{
  return __internal_fdiv_kernel (a, b, cudaRoundPosInf);
}

__device_func__(float __fadd_rd (float a, float b))
{
  return __internal_fadd_kernel (a, b, cudaRoundMinInf);
}

__device_func__(float __fadd_ru (float a, float b))
{
  return __internal_fadd_kernel (a, b, cudaRoundPosInf);
}

__device_func__(float __fmul_rd (float a, float b))
{
  return __internal_fmul_kernel (a, b, cudaRoundMinInf);
}

__device_func__(float __fmul_ru (float a, float b))
{
  return __internal_fmul_kernel (a, b, cudaRoundPosInf);
}

__device_func__(float __fmaf_rn (float a, float b, float c))
{
  return __internal_fmaf_kernel (a, b, c, cudaRoundNearest);
}

__device_func__(float __fmaf_rz (float a, float b, float c))
{
  return __internal_fmaf_kernel (a, b, c, cudaRoundZero);
}

__device_func__(float __fmaf_ru (float a, float b, float c))
{
  return __internal_fmaf_kernel (a, b, c, cudaRoundPosInf);
}

__device_func__(float __fmaf_rd (float a, float b, float c))
{
  return __internal_fmaf_kernel (a, b, c, cudaRoundMinInf);
}

__device_func__(int                    __cuda___isnan(double a));
__device_func__(int                    __cuda___isnanf(float a));
__device_func__(int                    __double2int_rz(double));
__device_func__(unsigned int           __double2uint_rz(double));
__device_func__(long long int          __double2ll_rz(double));
__device_func__(unsigned long long int __double2ull_rz(double));

#define __internal_clamp(val, max, min, nan)                                         \
       if (sizeof(val) == sizeof(double) && __cuda___isnan((double)val)) return nan; \
       if (sizeof(val) == sizeof(float) && __cuda___isnanf((float)val)) return nan;  \
       if (val >= max) return max;                                                   \
       if (val <= min) return min

/*******************************************************************************
*                                                                              *
* HOST IMPLEMENTATIONS FOR FUNCTIONS WITH BUILTIN NVOPENCC OPREATIONS          *
*                                                                              *
*******************************************************************************/

__device_func__(int __mulhi(int a, int b))
{
  long long int c = (long long int)a * (long long int)b;

  return (int)(c >> 32);
}

__device_func__(unsigned int __umulhi(unsigned int a, unsigned int b))
{
  unsigned long long int c = (unsigned long long int)a * (unsigned long long int)b;

  return (unsigned int)(c >> 32);
}

__device_func__(unsigned long long int __umul64hi(unsigned long long int a, unsigned long long int b))
{
  unsigned int           a_lo = (unsigned int)a;
  unsigned long long int a_hi = a >> 32;
  unsigned int           b_lo = (unsigned int)b;
  unsigned long long int b_hi = b >> 32;
  unsigned long long int m1 = a_lo * b_hi;
  unsigned long long int m2 = a_hi * b_lo;
  unsigned int           carry;

  carry = (0ULL + __umulhi(a_lo, b_lo) + (unsigned int)m1 + (unsigned int)m2) >> 32;

  return a_hi * b_hi + (m1 >> 32) + (m2 >> 32) + carry;
}

__device_func__(long long int __mul64hi(long long int a, long long int b))
{
  long long int res;
  res = __umul64hi(a, b);
  if (a < 0LL) res = res - b;
  if (b < 0LL) res = res - a;
  return res;
}

__device_func__(float __saturatef(float a))
{
  if (__cuda___isnanf(a)) return 0.0f; /* update of PTX spec 10/15/2008 */
  return a >= 1.0f ? 1.0f : a <= 0.0f ? 0.0f : a;
}

__device_func__(unsigned int __sad(int a, int b, unsigned int c))
{
  long long int diff = (long long int)a - (long long int)b;

  return (unsigned int)(__cuda_llabs(diff) + (long long int)c);
}

__device_func__(unsigned int __usad(unsigned int a, unsigned int b, unsigned int c))
{
  long long int diff = (long long int)a - (long long int)b;

  return (unsigned int)(__cuda_llabs(diff) + (long long int)c);
}

__device_func__(int __mul24(int a, int b))
{
  a &= 0xffffff;
  a = (a & 0x800000) != 0 ? a | ~0xffffff : a;
  b &= 0xffffff;
  b = (b & 0x800000) != 0 ? b | ~0xffffff : b;

  return a * b;
}

__device_func__(unsigned int __umul24(unsigned int a, unsigned int b))
{
  a &= 0xffffff;
  b &= 0xffffff;

  return a * b;
}

__device_func__(float __int_as_float(int a))
{
  volatile union __cudart_FloatIntCvt u;

  u.i = a;
  return u.f;
}

__device_func__(int __float_as_int(float a))
{
  volatile union __cudart_FloatIntCvt u;

  u.f = a;
  return u.i;
}

__device_func__(long long int __internal_float2ll_kernel(float a, long long int max, long long int min, long long int nan, enum cudaRoundMode rndMode))
{
  unsigned long long int res, t = 0ULL;
  int shift;
  unsigned int ia;

  __internal_clamp(a, max, min, nan);
  ia = __float_as_int(a);
  shift = 189 - ((ia >> 23) & 0xff);
  res = (unsigned long long int)(((ia << 8) | 0x80000000) >> 1) << 32;
  if (shift >= 64) {
    t = res;
    res = 0;
  } else if (shift) {
    t = res << (64 - shift);
    res = res >> shift;
  }
  if (rndMode == cudaRoundNearest && (long long int)t < 0LL) {
    res += t == 0x8000000000000000ULL ? res & 1ULL : 1ULL;
  }
  else if (rndMode == cudaRoundMinInf && t != 0ULL && ia > 0x80000000) {
    res++;
  }
  else if (rndMode == cudaRoundPosInf && t != 0ULL && (int)ia > 0) {
    res++;
  }
  if ((int)ia < 0) res = (unsigned long long int)-(long long int)res;
  return (long long int)res;
}

__device_func__(int __internal_float2int(float a, enum cudaRoundMode rndMode))
{
  return (int)__internal_float2ll_kernel(a, 2147483647LL, -2147483648LL, 0LL, rndMode);
}

__device_func__(int __float2int_rz(float a))
{
  return __internal_float2int(a, cudaRoundZero);
}

__device_func__(int __float2int_ru(float a))
{
  return __internal_float2int(a, cudaRoundPosInf);
}

__device_func__(int __float2int_rd(float a))
{
  return __internal_float2int(a, cudaRoundMinInf);
}

__device_func__(int __float2int_rn(float a))
{
  return __internal_float2int(a, cudaRoundNearest);
}

__device_func__(long long int __internal_float2ll(float a, enum cudaRoundMode rndMode))
{
  return __internal_float2ll_kernel(a, 9223372036854775807LL, -9223372036854775807LL -1LL, -9223372036854775807LL -1LL, rndMode);
}

__device_func__(long long int __float2ll_rz(float a))
{
  return __internal_float2ll(a, cudaRoundZero);
}

__device_func__(long long int __float2ll_ru(float a))
{
  return __internal_float2ll(a, cudaRoundPosInf);
}

__device_func__(long long int __float2ll_rd(float a))
{
  return __internal_float2ll(a, cudaRoundMinInf);
}

__device_func__(long long int __float2ll_rn(float a))
{
  return __internal_float2ll(a, cudaRoundNearest);
}

__device_func__(unsigned long long int __internal_float2ull_kernel(float a, unsigned long long int max, unsigned long long int nan, enum cudaRoundMode rndMode))
{
  unsigned long long int res, t = 0ULL;
  int shift;
  unsigned int ia;

  __internal_clamp(a, max, 0LL, nan);
  ia = __float_as_int(a);
  shift = 190 - ((ia >> 23) & 0xff);
  res = (unsigned long long int)((ia << 8) | 0x80000000) << 32;
  if (shift >= 64) {
    t = res >> (int)(shift > 64);
    res = 0;
  } else if (shift) {
    t = res << (64 - shift);
    res = res >> shift;
  }
  if (rndMode == cudaRoundNearest && (long long int)t < 0LL) {
    res += t == 0x8000000000000000ULL ? res & 1ULL : 1ULL;
  }
  else if (rndMode == cudaRoundPosInf && t != 0ULL) {
    res++;
  }
  return res;
}

__device_func__(unsigned int __internal_float2uint(float a, enum cudaRoundMode rndMode))
{
  return (unsigned int)__internal_float2ull_kernel(a, 4294967295U, 0U, rndMode);
}

__device_func__(unsigned int __float2uint_rz(float a))
{
  return __internal_float2uint(a, cudaRoundZero);
}

__device_func__(unsigned int __float2uint_ru(float a))
{
  return __internal_float2uint(a, cudaRoundPosInf);
}

__device_func__(unsigned int __float2uint_rd(float a))
{
  return __internal_float2uint(a, cudaRoundMinInf);
}

__device_func__(unsigned int __float2uint_rn(float a))
{
  return __internal_float2uint(a, cudaRoundNearest);
}

__device_func__(unsigned long long int __internal_float2ull(float a, enum cudaRoundMode rndMode))
{
  return __internal_float2ull_kernel(a, 18446744073709551615ULL, 9223372036854775808ULL, rndMode);
}

__device_func__(unsigned long long int __float2ull_rz(float a))
{
  return __internal_float2ull(a, cudaRoundZero);
}

__device_func__(unsigned long long int __float2ull_ru(float a))
{
  return __internal_float2ull(a, cudaRoundPosInf);
}

__device_func__(unsigned long long int __float2ull_rd(float a))
{
  return __internal_float2ull(a, cudaRoundMinInf);
}

__device_func__(unsigned long long int __float2ull_rn(float a))
{
  return __internal_float2ull(a, cudaRoundNearest);
}

__device_func__(int __internal_normalize64(unsigned long long int *a))
{
  int lz = 0;

  if ((*a & 0xffffffff00000000ULL) == 0ULL) {
    *a <<= 32;
    lz += 32;
  }
  if ((*a & 0xffff000000000000ULL) == 0ULL) {
    *a <<= 16;
    lz += 16;
  }
  if ((*a & 0xff00000000000000ULL) == 0ULL) {
    *a <<= 8;
    lz += 8;
  }
  if ((*a & 0xf000000000000000ULL) == 0ULL) {
    *a <<= 4;
    lz += 4;
  }
  if ((*a & 0xC000000000000000ULL) == 0ULL) {
    *a <<= 2;
    lz += 2;
  }
  if ((*a & 0x8000000000000000ULL) == 0ULL) {
    *a <<= 1;
    lz += 1;
  }
  return lz;
}

__device_func__(int __internal_normalize(unsigned int *a))
{
  unsigned long long int t = (unsigned long long int)*a;
  int lz = __internal_normalize64(&t);
  
  *a = (unsigned int)(t >> 32);

  return lz - 32;
}

__device_func__(float __internal_int2float_kernel(int a, enum cudaRoundMode rndMode))
{
  volatile union __cudart_FloatUintCvt res;
  int shift;
  unsigned int t;
  res.i = a;
  if (a == 0) return res.f;
  if (a < 0) res.i = (unsigned int)-a;
  shift = __internal_normalize((unsigned int*)&res.i);
  t = res.i << 24;
  res.i = (res.i >> 8);
  res.i += (127 + 30 - shift) << 23;
  if (a < 0) res.i |= 0x80000000;
  if ((rndMode == cudaRoundNearest) && (t >= 0x80000000)) {
    res.i += (t == 0x80000000) ? (res.i & 1) : (t >> 31);
  }
  else if ((rndMode == cudaRoundMinInf) && t && (a < 0)) {
    res.i++;
  }
  else if ((rndMode == cudaRoundPosInf) && t && (a > 0)) {
    res.i++;
  }
  return res.f;
}

__device_func__(float __int2float_rz(int a))
{
  return __internal_int2float_kernel(a, cudaRoundZero);
}

__device_func__(float __int2float_ru(int a))
{
  return __internal_int2float_kernel(a, cudaRoundPosInf);
}

__device_func__(float __int2float_rd(int a))
{
  return __internal_int2float_kernel(a, cudaRoundMinInf);
}

__device_func__(float __int2float_rn(int a))
{
  return __internal_int2float_kernel(a, cudaRoundNearest);
}

__device_func__(float __internal_uint2float_kernel(unsigned int a, enum cudaRoundMode rndMode))
{
  volatile union __cudart_FloatUintCvt res;
  int shift;
  unsigned int t;
  res.i = a;
  if (a == 0) return res.f;
  shift = __internal_normalize((unsigned int*)&res.i);
  t = res.i << 24;
  res.i = (res.i >> 8);
  res.i += (127 + 30 - shift) << 23;
  if (rndMode == cudaRoundNearest) {
    res.i += (t == 0x80000000) ? (res.i & 1) : (t >> 31);
  }
  else if ((rndMode == cudaRoundPosInf) && t) {
    res.i++;
  }
  return res.f;
}

__device_func__(float __uint2float_rz(unsigned int a))
{
  return __internal_uint2float_kernel(a, cudaRoundZero);
}

__device_func__(float __uint2float_ru(unsigned int a))
{
  return __internal_uint2float_kernel(a, cudaRoundPosInf);
}

__device_func__(float __uint2float_rd(unsigned int a))
{
  return __internal_uint2float_kernel(a, cudaRoundMinInf);
}

__device_func__(float __uint2float_rn(unsigned int a))
{
  return __internal_uint2float_kernel(a, cudaRoundNearest);
}

__device_func__(float __internal_ull2float_kernel(unsigned long long int a, enum cudaRoundMode rndMode))
{
  unsigned long long int temp;
  unsigned int res, t;
  int shift;
  if (a == 0ULL) return 0.0f;
  temp = a;
  shift = __internal_normalize64(&temp);
  temp = (temp >> 8) | ((temp & 0xffULL) ? 1ULL : 0ULL);
  res = (unsigned int)(temp >> 32);
  t = (unsigned int)temp;
  res += (127 + 62 - shift) << 23; /* add in exponent */
  if (rndMode == cudaRoundNearest) {
    res += (t == 0x80000000) ? (res & 1) : (t >> 31);
  } else if (rndMode == cudaRoundPosInf) {
    res += (t != 0);
  }
  return __int_as_float(res);
}

__device_func__(float __internal_ll2float_kernel(long long int a, enum cudaRoundMode rndMode))
{
  unsigned long long int temp;
  volatile float res = 0.0f;

  if (a < 0LL) {
    temp = (~((unsigned long long int)a)) + 1ULL;
    if (rndMode == cudaRoundPosInf) {
      rndMode = cudaRoundMinInf;
    } else if (rndMode == cudaRoundMinInf) {
      rndMode = cudaRoundPosInf;
    }
  } else {
    temp = (unsigned long long int)a;
  }
  res = __internal_ull2float_kernel (temp, rndMode);
  if (a < 0LL) {
    res = -res;
  }
  return res;
}  
    
__device_func__(float __ll2float_rn(long long int a))
{
  return __internal_ll2float_kernel(a, cudaRoundNearest);
}      

__device_func__(float __ll2float_rz(long long int a))
{
  return __internal_ll2float_kernel(a, cudaRoundZero);
}
   
__device_func__(float __ll2float_ru(long long int a))
{
  return __internal_ll2float_kernel(a, cudaRoundPosInf);
}
  
__device_func__(float __ll2float_rd(long long int a))
{
  return __internal_ll2float_kernel(a, cudaRoundMinInf);
}   

__device_func__(float __ull2float_rn(unsigned long long int a))
{
  return __internal_ull2float_kernel(a, cudaRoundNearest);
}
 
__device_func__(float __ull2float_rz(unsigned long long int a))
{
  return __internal_ull2float_kernel(a, cudaRoundZero);
}
   
__device_func__(float __ull2float_ru(unsigned long long int a))
{
  return __internal_ull2float_kernel(a, cudaRoundPosInf);
}
  
__device_func__(float __ull2float_rd(unsigned long long int a))
{
  return __internal_ull2float_kernel(a, cudaRoundMinInf);
}   

__device_func__(unsigned short __float2half_rn(float f))
{
  unsigned int x = __float_as_int (f);
  unsigned int u = (x & 0x7fffffff), remainder, shift, lsb, lsb_s1, lsb_m1;
  unsigned int sign, exponent, mantissa;

  /* Get rid of +NaN/-NaN case first. */
  if (u > 0x7f800000) {
    return 0x7fff;
  }
  
  sign = ((x >> 16) & 0x8000);
  
  /* Get rid of +Inf/-Inf, +0/-0.  */
  if (u > 0x477fefff) {
    return sign | 0x7c00;
  }
  if (u < 0x33000001) {
    return sign | 0x0000;
  }

  exponent = ((u >> 23) & 0xff);
  mantissa = (u & 0x7fffff);

  if (exponent > 0x70) {
    shift = 13;
    exponent -= 0x70;
  } else {
    shift = 0x7e - exponent;
    exponent = 0;
    mantissa |= 0x800000;
  }
  lsb = (1 << shift);
  lsb_s1 = (lsb >> 1);
  lsb_m1 = (lsb - 1);
  
  /* Round to nearest even.  */
  remainder = (mantissa & lsb_m1);
  mantissa >>= shift;
  if (remainder > lsb_s1 || (remainder == lsb_s1 && (mantissa & 0x1))) {
    ++mantissa;
    if (!(mantissa & 0x3ff)) {
      ++exponent;
      mantissa = 0;
    }
  }  

  return sign | (exponent << 10) | mantissa;  
}

__device_func__(float __half2float(unsigned short h))
{
  unsigned int sign = ((h >> 15) & 1);
  unsigned int exponent = ((h >> 10) & 0x1f);
  unsigned int mantissa = ((h & 0x3ff) << 13);

  if (exponent == 0x1f) {  /* NaN or Inf */
    mantissa = (mantissa
                ? (sign = 0, 0x7fffff)
                : 0);
    exponent = 0xff;
  } else if (!exponent) {  /* Denorm or Zero */
    if (mantissa) {
      unsigned int msb;
      exponent = 0x71;
      do {
        msb = (mantissa & 0x400000);
        mantissa <<= 1;  /* normalize */
        --exponent;
      } while (!msb);
      mantissa &= 0x7fffff;  /* 1.mantissa is implicit */
    }
  } else {
    exponent += 0x70;
  }

  return __int_as_float ((sign << 31) | (exponent << 23) | mantissa);
}

__device_func__(float __fadd_rz(float a, float b))
{
  return __internal_fadd_kernel(a, b, cudaRoundZero);
}

__device_func__(float __fmul_rz(float a, float b))
{
  return __internal_fmul_kernel(a, b, cudaRoundZero);
}

__device_func__(float __fadd_rn(float a, float b))
{
  return __internal_fadd_kernel(a, b, cudaRoundNearest);
}

__device_func__(float __fmul_rn(float a, float b))
{
  return __internal_fmul_kernel(a, b, cudaRoundNearest);
}

__device_func__(void __brkpt(int c))
{
  /* TODO */
}

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

extern int CUDARTAPI __cudaSynchronizeThreads(void**, void*);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#if defined(__GNUC__)

__device_func__(inline __attribute__((always_inline)) void __syncthreads(void))
{
  volatile int _ = 0;
  L: if (__cudaSynchronizeThreads((void**)&&L, (void*)&_)) goto L;
}

#elif defined(_WIN32)

#define __syncthreads() \
        (void)__cudaSynchronizeThreads((void**)0, (void*)0)

#endif /* __GNUC__ */

__device_func__(void __prof_trigger(int a))
{
}

__device_func__(void __threadfence(void))
{
  __syncthreads();
}

__device_func__(void __threadfence_block(void))
{
  __syncthreads();
}


#if defined(__GNUC__)

__device_func__(void __trap(void))
{
  __builtin_trap();
}

#elif defined(_WIN32)

__device_func__(void __trap(void))
{
  __debugbreak();
}

#endif /* __GNUC__ */

#endif /* __CUDABE__ */

/*******************************************************************************
*                                                                              *
* DEVICE IMPLEMENTATIONS FOR FUNCTIONS WITH BUILTIN NVOPENCC OPERATIONS        *
*                                                                              *
*******************************************************************************/
#if !defined(__CUDABE__)
__device_func__(float __fdividef(float a, float b))
{
  volatile float aa = a;
  volatile float bb = b;
  /* match range restrictions of the device function */
  if (__cuda_fabsf(bb) > CUDART_TWO_TO_126_F) {
    if (__cuda_fabsf(aa) <= CUDART_NORM_HUGE_F) {
      return ((aa / bb) / CUDART_NORM_HUGE_F) / CUDART_NORM_HUGE_F;
    } else {
      bb = 1.0f / bb;
      bb = bb / CUDART_NORM_HUGE_F;
      return aa * bb;
    }
  } else {
    return aa / bb;
  }
}
#endif /* !defined(__CUDABE__) */

__device_func__(float __sinf(float a))
{
#if !defined(__CUDABE__)
  if ((__float_as_int(a) << 1) == 0xff000000) {
    return __fadd_rn (a, -a); /* return NaN */
  }
#endif /* !defined(__CUDABE__) */
  return sinf(a);
}

__device_func__(float __cosf(float a))
{
#if !defined(__CUDABE__)
  if ((__float_as_int(a) << 1) == 0xff000000) {
    return __fadd_rn (a, -a); /* return NaN */
  }
#endif /* !defined(__CUDABE__) */
  return cosf(a);
}

__device_func__(float __log2f(float a))
{
  return log2f(a);
}

/*******************************************************************************
*                                                                              *
* SHARED HOST AND DEVICE IMPLEMENTATIONS                                       *
*                                                                              *
*******************************************************************************/
__device_func__(float __tanf(float a))
{
  return __fdividef (__sinf(a), __cosf(a));
}

__device_func__(void __sincosf(float a, float *sptr, float *cptr))
{
  *sptr = __sinf(a);
  *cptr = __cosf(a);
}

__device_func__(float __expf(float a))
{
  return __cuda_exp2f(a * CUDART_L2E_F);
}

__device_func__(float __exp10f(float a))
{
  return __cuda_exp2f(a * CUDART_L2T_F);
}

__device_func__(float __log10f(float a))
{
  return CUDART_LG2_F * __log2f(a);
}

__device_func__(float __logf(float a))
{
  return CUDART_LN2_F * __log2f(a);
}

__device_func__(float __powf(float a, float b))
{
  return __cuda_exp2f(b * __log2f(a));
}

__device_func__(float fdividef(float a, float b))
{
#if defined(__USE_FAST_MATH__) && !defined(__CUDA_PREC_DIV)
  return __fdividef(a, b);
#else /* __USE_FAST_MATH__ && !__CUDA_PREC_DIV */
  return a / b;
#endif /* __USE_FAST_MATH__ && !__CUDA_PREC_DIV */
}

#if !defined(__CUDABE__) || (__CUDA_ARCH__ < 200)

__device_func__(int __clz(int a))
{
  return (a)?(158-(__float_as_int(__uint2float_rz((unsigned int)a))>>23)):32;
}

__device_func__(int __clzll(long long int a))
{
  int ahi = ((int)((unsigned long long)a >> 32));
  int alo = ((int)((unsigned long long)a & 0xffffffffULL));
  int res;
  if (ahi) {
    res = 0;
  } else {
    res = 32;
    ahi = alo;
  }
  res = res + __clz(ahi);
  return res;
}

__device_func__(int __popc(unsigned int a))
{
  a = a - ((a >> 1) & 0x55555555);
  a = (a & 0x33333333) + ((a >> 2) & 0x33333333);
  a = (a + (a >> 4)) & 0x0f0f0f0f;
  a = ((__umul24(a, 0x808080) << 1) + a) >> 24;
  return a;
}

__device_func__(int __popcll(unsigned long long int a))
{
  unsigned int ahi = ((unsigned int)(a >> 32));
  unsigned int alo = ((unsigned int)(a & 0xffffffffULL));
  alo = alo - ((alo >> 1) & 0x55555555);
  alo = (alo & 0x33333333) + ((alo >> 2) & 0x33333333);
  ahi = ahi - ((ahi >> 1) & 0x55555555);
  ahi = (ahi & 0x33333333) + ((ahi >> 2) & 0x33333333);
  alo = alo + ahi;
  alo = (alo & 0x0f0f0f0f) + ((alo >> 4) & 0x0f0f0f0f);
  alo = ((__umul24(alo, 0x808080) << 1) + alo) >> 24;
  return alo;
}

__device_func__(unsigned int __brev(unsigned int a))
{
  /* Use Knuth's algorithm from http://www.hackersdelight.org/revisions.pdf */
  unsigned int t;
  a = (a << 15) | (a >> 17);
  t = (a ^ (a >> 10)) & 0x003f801f; 
  a = (t + (t << 10)) ^ a;
  t = (a ^ (a >>  4)) & 0x0e038421; 
  a = (t + (t <<  4)) ^ a;
  t = (a ^ (a >>  2)) & 0x22488842; 
  a = (t + (t <<  2)) ^ a;
  return a;
}

__device_func__(unsigned long long int __brevll(unsigned long long int a))
{
  unsigned int hi = (unsigned int)(a >> 32);
  unsigned int lo = (unsigned int)(a & 0xffffffffULL);
  unsigned int t;
  t  = __brev(lo);
  lo = __brev(hi);
  return ((unsigned long long int)t << 32) + (unsigned long long int)lo;
}

#endif /* __CUDABE__ || __CUDA_ARCH__ < 200 */

__device_func__(int __ffs(int a))
{
  return 32 - __clz (a & -a);
}

__device_func__(int __ffsll(long long int a))
{
  return 64 - __clzll (a & -a);
}

#if defined(CUDA_DOUBLE_MATH_FUNCTIONS) && defined(CUDA_FLOAT_MATH_FUNCTIONS)

#error -- conflicting mode for double math routines

#endif /* CUDA_DOUBLE_MATH_FUNCTIONS && CUDA_FLOAT_MATH_FUNCTIONS */

#if defined(CUDA_FLOAT_MATH_FUNCTIONS)

__device_func__(double fdivide(double a, double b))
{
  return (double)fdividef((float)a, (float)b);
}

#if !defined(__CUDABE__)

__device_func__(int __double2int_rz(double a))
{
  return __float2int_rz((float)a);
}

__device_func__(unsigned int __double2uint_rz(double a))
{
  return __float2uint_rz((float)a);
}

__device_func__(long long int __double2ll_rz(double a))
{
  return __float2ll_rz((float)a);
}

__device_func__(unsigned long long int __double2ull_rz(double a))
{
  return __float2ull_rz((float)a);
}

#endif /* !__CUDABE__ */

#endif /* CUDA_FLOAT_MATH_FUNCTIONS */

#if defined(CUDA_DOUBLE_MATH_FUNCTIONS)

__device_func__(double fdivide(double a, double b))
{
  return a / b;
}

#if !defined(__CUDABE__)

__device_func__(int __internal_double2int(double a, enum cudaRoundMode rndMode));
__device_func__(unsigned int __internal_double2uint(double a, enum cudaRoundMode rndMode));
__device_func__(long long int __internal_double2ll(double a, enum cudaRoundMode rndMode));
__device_func__(unsigned long long int __internal_double2ull(double a, enum cudaRoundMode rndMode));

__device_func__(int __double2int_rz(double a))
{
  return __internal_double2int(a, cudaRoundZero);
}

__device_func__(unsigned int __double2uint_rz(double a))
{
  return __internal_double2uint(a, cudaRoundZero);
}

__device_func__(long long int __double2ll_rz(double a))
{
  return __internal_double2ll(a, cudaRoundZero);
}

__device_func__(unsigned long long int __double2ull_rz(double a))
{
  return __internal_double2ull(a, cudaRoundZero);
}

#endif /* !__CUDABE__ */

#endif /* CUDA_DOUBLE_MATH_FUNCTIONS */

#endif /* __cplusplus && __CUDACC__ */

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

#include "sm_11_atomic_functions.h"
#include "sm_12_atomic_functions.h"
#include "sm_13_double_functions.h"
#include "sm_20_atomic_functions.h"
#include "sm_20_intrinsics.h"
#include "surface_functions.h"
#include "texture_fetch_functions.h"

#endif /* !__DEVICE_FUNCTIONS_H__ */

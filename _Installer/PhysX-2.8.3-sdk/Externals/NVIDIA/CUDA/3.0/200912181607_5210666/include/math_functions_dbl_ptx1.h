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

#if !defined(__MATH_FUNCTIONS_DBL_PTX1_H__)
#define __MATH_FUNCTIONS_DBL_PTX1_H__

#if defined(__cplusplus) && defined(__CUDACC__)

#elif !defined(__CUDACC__)

#include "crt/func_macro.h"

__device_func__(double __cuda_fabs(double a))
{
  return (float)__cuda_fabsf((float)a);
}

__device_func__(double __cuda_fmax(double a, double b))
{
  return (float)__cuda_fmaxf((float)a, (float)b);
}

__device_func__(double __cuda_fmin(double a, double b))
{
  return (float)__cuda_fminf((float)a, (float)b);
}

__device_func__(int __cuda___finite(double a))
{
  return __cuda___finitef((float)a);
}

__device_func__(int __cuda___isinf(double a))
{
  return __cuda___isinff((float)a);
}

__device_func__(int __cuda___isnan(double a))
{
  return __cuda___isnanf((float)a);
}

__device_func__(int __cuda___signbit(double a))
{
  return __cuda___signbitf((float)a);
}

__device_func__(double __cuda_sqrt(double a))
{
  return (double)__cuda_sqrtf((float)a);
}

__device_func__(double __cuda_rsqrt(double a))
{
  return (double)__cuda_rsqrtf((float)a);
}

__device_func__(double __cuda_ceil(double a))
{
  return (double)__cuda_ceilf((float)a);
}

__device_func__(double __cuda_trunc(double a))
{
  return (double)__cuda_truncf((float)a);
}

__device_func__(double __cuda_floor(double a))
{
  return (double)__cuda_floorf((float)a);
}

__device_func__(double __cuda_copysign(double a, double b))
{
  return (double)__cuda_copysignf((float)a, (float)b);
}

__device_func__(double __cuda_sin(double a))
{
  return (double)__cuda_sinf((float)a);
}

__device_func__(double __cuda_sinpi(double a))
{
  return (double)__cuda_sinpif((float)a);
}

__device_func__(double __cuda_cos(double a))
{
  return (double)__cuda_cosf((float)a);
}

__device_func__(void __cuda_sincos(double a, double *sptr, double *cptr))
{
  float fs, fc;

  __cuda_sincosf((float)a, &fs, &fc);

  *sptr = (double)fs;
  *cptr = (double)fc;
}

__device_func__(double __cuda_tan(double a))
{
  return (double)__cuda_tanf((float)a);
}

__device_func__(double __cuda_exp(double a))
{
  return (double)__cuda_expf((float)a);
}

__device_func__(double __cuda_exp2(double a))
{
  return (double)__cuda_exp2f((float)a);
}

__device_func__(double __cuda_exp10(double a))
{
  return (double)__cuda_exp10f((float)a);
}

__device_func__(double __cuda_expm1(double a))
{
  return (double)__cuda_expm1f((float)a);
}

__device_func__(double __cuda_cosh(double a))
{
  return (double)__cuda_coshf((float)a);
}

__device_func__(double __cuda_sinh(double a))
{
  return (double)__cuda_sinhf((float)a);
}

__device_func__(double __cuda_tanh(double a))
{
  return (double)__cuda_tanhf((float)a);
}

__device_func__(double __cuda_asin(double a))
{
  return (double)__cuda_asinf((float)a);
}

__device_func__(double __cuda_acos(double a))
{
  return (double)__cuda_acosf((float)a);
}

__device_func__(double __cuda_atan(double a))
{
  return (double)__cuda_atanf((float)a);
}

__device_func__(double __cuda_atan2(double a, double b))
{
  return (double)__cuda_atan2f((float)a, (float)b);
}

__device_func__(double __cuda_log(double a))
{
  return (double)__cuda_logf((float)a);
}

__device_func__(double __cuda_log2(double a))
{
  return (double)__cuda_log2f((float)a);
}

__device_func__(double __cuda_log10(double a))
{
  return (double)__cuda_log10f((float)a);
}

__device_func__(double __cuda_log1p(double a))
{
  return (double)__cuda_log1pf((float)a);
}

__device_func__(double __cuda_acosh(double a))
{
  return (double)__cuda_acoshf((float)a);
}

__device_func__(double __cuda_asinh(double a))
{
  return (double)__cuda_asinhf((float)a);
}

__device_func__(double __cuda_atanh(double a))
{
  return (double)__cuda_atanhf((float)a);
}

__device_func__(double __cuda_hypot(double a, double b))
{
  return (double)__cuda_hypotf((float)a, (float)b);
}

__device_func__(double __cuda_cbrt(double a))
{
  return (double)__cuda_cbrtf((float)a);
}

__device_func__(double __cuda_rcbrt(double a))
{
  return (double)__cuda_rcbrtf((float)a);
}

__device_func__(double __cuda_erf(double a))
{
  return (double)__cuda_erff((float)a);
}

__device_func__(double __cuda_erfinv(double a))
{
  return (double)__cuda_erfinvf((float)a);
}

__device_func__(double __cuda_erfc(double a))
{
  return (double)__cuda_erfcf((float)a);
}

__device_func__(double __cuda_erfcinv(double a))
{
  return (double)__cuda_erfcinvf((float)a);
}

__device_func__(double __cuda_lgamma(double a))
{
  return (double)__cuda_lgammaf((float)a);
}

__device_func__(double __cuda_tgamma(double a))
{
  return (double)__cuda_tgammaf((float)a);
}

__device_func__(double __cuda_ldexp(double a, int b))
{
  return (double)__cuda_ldexpf((float)a, b);
}

__device_func__(double __cuda_scalbn(double a, int b))
{
  return (double)__cuda_scalbnf((float)a, b);
}

__device_func__(double __cuda_scalbln(double a, long b))
{
  return (double)__cuda_scalblnf((float)a, b);
}

__device_func__(double __cuda_frexp(double a, int *b))
{
  return (double)__cuda_frexpf((float)a, b);
}

__device_func__(double __cuda_modf(double a, double *b))
{
  float fb;
  float fa = __cuda_modff((float)a, &fb);

  *b = (double)fb;

  return (double)fa;  
}

__device_func__(double __cuda_fmod(double a, double b))
{
  return (double)__cuda_fmodf((float)a, (float)b);
}

__device_func__(double __cuda_remainder(double a, double b))
{
  return (double)__cuda_remainderf((float)a, (float)b);
}

__device_func__(double __cuda_remquo(double a, double b, int *c))
{
  return (double)__cuda_remquof((float)a, (float)b, c);
}

__device_func__(double __cuda_nextafter(double a, double b))
{
  return (double)__cuda_nextafterf((float)a, (float)b);
}

__device_func__(double __cuda_nan(const char *tagp))
{
  return (double)__cuda_nanf(tagp);
}

__device_func__(double __cuda_pow(double a, double b))
{
  return (double)__cuda_powf((float)a, (float)b);
}

__device_func__(double __cuda_round(double a))
{
  return (double)__cuda_roundf((float)a);
}

__device_func__(long __cuda_lround(double a))
{
  return __cuda_lroundf((float)a);
}

__device_func__(long long __cuda_llround(double a))
{
  return __cuda_llroundf((float)a);
}

__device_func__(double __cuda_rint(double a))
{
  return (double)__cuda_rintf((float)a);
}

__device_func__(long __cuda_lrint(double a))
{
  return __cuda_lrintf((float)a);
}

__device_func__(long long __cuda_llrint(double a))
{
  return __cuda_llrintf((float)a);
}

__device_func__(double __cuda_nearbyint(double a))
{
  return (double)__cuda_nearbyintf((float)a);
}

__device_func__(double __cuda_fdim(double a, double b))
{
  return (double)__cuda_fdimf((float)a, (float)b);
}

__device_func__(int __cuda_ilogb(double a))
{
  return __cuda_ilogbf((float)a);
}

__device_func__(double __cuda_logb(double a))
{
  return (double)__cuda_logbf((float)a);
}

__device_func__(double __cuda_fma(double a, double b, double c))
{
  return (double)__cuda_fmaf((float)a, (float)b, (float)c);
}

#if __APPLE__
__device_func__(int __cuda___isfinited(double a))
{
  return (double)__cuda___finitef((float)a);
}

__device_func__(int __cuda___signbitd(double a))
{
  return (double)__cuda___signbitf((float)a);
}
#endif

#endif /* __cplusplus && __CUDACC__ */

#endif /* __MATH_FUNCTIONS_DBL_PTX1_H__ */

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

#if !defined(__CUDA_INTERNAL_COMPILATION__)

#define __CUDA_INTERNAL_COMPILATION__
#define __glob_pref_var(var) \
        __global_##var
#define __global_var(var) \
        (*__glob_pref_var(var))
#define __shadow_var(c, cpp) \
        __shadow_pref_var(c, cpp)
#define __text__
#define __surf__
#define __dv(v)
#define __name__shadow_var(c, cpp) \
        __pick(#c, #cpp)
#define __name__text_var(c, cpp) \
        __pick(#c, #cpp)
#define __shadow_pref_var(c, cpp) \
        __pick(c##__cuda_shadow_variable__, cpp##__cuda_shadow_variable__)
#define __device_stub_name(c, cpp) \
        __pick(c, cpp)
#define __text_var(c, cpp) \
        __pick(c, cpp)
#define __cppref__ \
        __pick(, &)

#if defined(_WIN32) && !defined(_WIN64)

#define __pad__(f) \
        f

#else /* _WIN32 && !_WIN64 */

#define __pad__(f)

#endif /* _WIN32 && !_WIN64 */

#if defined(__APPLE__)

#define __extern_weak__ \
        __weak_import__,

#elif defined(__GNUC__)

#define __extern_weak__

#endif /* __APPLE__ */

#if defined(__cplusplus)

#define __pick(c, cpp) \
        cpp

#else /* __cplusplus */

#define __pick(c, cpp) \
        c

typedef char bool;

#endif /* __cplusplus */

#if !defined(__GNUC__) || __GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 3)

#define __specialization_static \
        static

#else /* !__GNUC__ || __GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 3) */

#define __specialization_static

#endif /* !__GNUC__ || __GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 3) */

#include "cuda_runtime_api.h"
#include "storage_class.h"

#else /* !__CUDA_INTERNAL_COMPILATION__ */

#include "host_defines.h"

#define __cudaRegisterBinary()                                                 \
        __cudaFatCubinHandle = __cudaRegisterFatBinary((void*)__cudaFatCubin); \
        atexit(__cudaUnregisterBinaryUtil)
#define __cudaRegisterVariable(var, ext, size, constant, global) \
        __cudaRegisterVar(__cudaFatCubinHandle, (char*)&__host##var, (char*)__device##var, __name##var, ext, size, constant, global)
#define __cudaRegisterGlobalTexture(tex, dim, norm, ext) \
        __cudaRegisterTexture(__cudaFatCubinHandle, (const struct textureReference*)&tex, __tex_var(tex), __name##tex, dim, norm, ext)
#define __cudaRegisterGlobalSurface(surf, dim, ext) \
        __cudaRegisterSurface(__cudaFatCubinHandle, (const struct surfaceReference*)&surf, __tex_var(surf), __name##surf, dim, ext)
#define __cudaRegisterUnsizedShared(var) \
        __cudaRegisterShared(__cudaFatCubinHandle, (void**)__device_var(var))
#define __cudaRegisterSharedVariable(var, size, align, sc) \
        __cudaRegisterSharedVar(__cudaFatCubinHandle, (void**)__device_var(var), size, align, sc)
#define __cudaRegisterEntry(funptr, fun, thread_limit) \
        __cudaRegisterFunction(__cudaFatCubinHandle, (const char*)funptr, (char*)__device_fun(fun), #fun, __cuda_tl__(thread_limit), __ids)

#define __cudaInitArgBlock(arg) \
        *(void**)(void*)&arg = (void*)0
#define __cudaSetupArg(arg, offset) \
        if (cudaSetupArgument((void*)(char*)&arg, sizeof(arg), (size_t)&offset->arg) != cudaSuccess) \
          return
#define __cudaLaunch(fun) \
        { volatile static char *__f; __f = fun; (void)cudaLaunch(fun); }

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

extern void** CUDARTAPI __cudaRegisterFatBinary(
  void *fatCubin
);

extern void CUDARTAPI __cudaUnregisterFatBinary(
  void **fatCubinHandle
);

extern void CUDARTAPI __cudaRegisterVar(
        void **fatCubinHandle,
        char  *hostVar,
        char  *deviceAddress,
  const char  *deviceName,
        int    ext,
        int    size,
        int    constant,
        int    global
);

extern void CUDARTAPI __cudaRegisterTexture(
        void                    **fatCubinHandle,
  const struct textureReference  *hostVar,
  const void                    **deviceAddress,
  const char                     *deviceName,
        int                       dim,       
        int                       norm,      
        int                       ext        
);

extern void CUDARTAPI __cudaRegisterSurface(
        void                    **fatCubinHandle,
  const struct surfaceReference  *hostVar,
  const void                    **deviceAddress,
  const char                     *deviceName,
        int                       dim,       
        int                       ext        
);

extern void CUDARTAPI __cudaRegisterShared(
  void **fatCubinHandle,
  void **devicePtr
);

extern void CUDARTAPI __cudaRegisterSharedVar(
  void   **fatCubinHandle,
  void   **devicePtr,
  size_t   size,
  size_t   alignment,
  int      storage
);

extern void CUDARTAPI __cudaRegisterFunction(
        void   **fatCubinHandle,
  const char    *hostFun,
        char    *deviceFun,
  const char    *deviceName,
        int      thread_limit,
        uint3   *tid,
        uint3   *bid,
        dim3    *bDim,
        dim3    *gDim,
        int     *wSize
);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#if defined(__GNUC__) && defined(__cplusplus)

extern int atexit(void(*)(void)) throw();

#else /* __GNUC__ && __cplusplus */

extern int __cdecl atexit(void(__cdecl *)(void));

#endif /* __GNUC__ && __cplusplus */

static void **__cudaFatCubinHandle;

static void __cdecl __cudaUnregisterBinaryUtil(void)
{
  __cudaUnregisterFatBinary(__cudaFatCubinHandle);
}

#if defined(__device_emulation)

#if defined(__cplusplus) && !defined(__multi_core__)

#define __cuda_emu__ \
        __cuda_emu::

#else /* __cplusplus */

#define __cuda_emu__ 

#endif /* __cplusplus */

#define __device_fun(fun) \
        __cuda_emu__ __device_wrapper_##fun
#define __device_var(var) \
        &__cuda_emu__ var
#define __tex_var(var) \
        &__cuda_emu__ __texture_var(var)
#define __cudaFatCubin \
        0
#define __cuda_tl__(l) \
        l

#if defined(__multi_core__)

#define __ids \
        (uint3*)0, (uint3*)0, &blockDim, &gridDim, &warpSize

#else /* __multi_core__ */

#define __ids \
        (uint3*)&__cuda_emu__ threadIdx, (uint3*)&__cuda_emu__ blockIdx, (dim3*)&__cuda_emu__ blockDim, (dim3*)&__cuda_emu__ gridDim, &__cuda_emu__ warpSize

#endif /* __multi_core__ */

#else /* __device_emulation */

#define __device_fun(fun) \
        #fun
#define __device_var(var) \
        #var
#define __tex_var(var) \
        0
#define __cudaFatCubin \
        &__fatDeviceText
#define __cuda_tl__(l) \
        -1
#define __ids \
        (uint3*)0, (uint3*)0, (dim3*)0, (dim3*)0, (int*)0

#include "common_functions.h"

#endif /* __device_emulation */

/* UTILITY MACROS */
#define __device__global_var(var) \
        __device_var(var)
#define __name__global_var(var) \
        #var
#define __host__global_var(var) \
        __glob_pref_var(var)
#define __device__shadow_var(c, cpp) \
        __device_var(c)
#define __host__shadow_var(c, cpp) \
        __shadow_pref_var(c, cpp)

#if defined(_WIN32)

#if defined(__cplusplus)

#pragma warning(disable: 4099)

#endif /* __cplusplus */

#if !defined(_WIN64)

#pragma warning(disable: 4408)

#endif /* !_WIN64 */

#endif /* _WIN32 */

#endif /* !__CUDA_INTERNAL_COMPILATION__ */

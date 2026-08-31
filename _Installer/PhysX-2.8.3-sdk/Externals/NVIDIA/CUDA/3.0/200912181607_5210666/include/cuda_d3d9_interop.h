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

#if !defined(__CUDA_D3D9_INTEROP_H__)
#define __CUDA_D3D9_INTEROP_H__

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

#if defined(_WIN32)

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

#include "host_defines.h"
#include <d3d9.h>

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

extern __host__ cudaError_t CUDARTAPI cudaGraphicsD3D9RegisterResource(struct cudaGraphicsResource **resource, IDirect3DResource9 *pD3DResource, unsigned int flags);
extern __host__ cudaError_t CUDARTAPI cudaD3D9GetDevice(int *device, const char *pszAdapterName);
extern __host__ cudaError_t CUDARTAPI cudaD3D9SetDirect3DDevice(IDirect3DDevice9 *pD3DDevice);

/**
 * CUDA D3D9 Register Flags
 */
enum cudaD3D9RegisterFlags
{
  cudaD3D9RegisterFlagsNone  = 0,  ///< Default; Resource can be accessed througa void*
  cudaD3D9RegisterFlagsArray = 1,  ///< Resource can be accessed through a CUarray*
};

/**
 * CUDA D3D9 Map Flags
 */
enum cudaD3D9MapFlags
{
  cudaD3D9MapFlagsNone         = 0,  ///< Default; Assume resource can be read/written
  cudaD3D9MapFlagsReadOnly     = 1,  ///< CUDA kernels will not write to this resource
  cudaD3D9MapFlagsWriteDiscard = 2,  ///< CUDA kernels will only write to and will not read from this resource
};

extern __host__ cudaError_t CUDARTAPI cudaD3D9GetDirect3DDevice(IDirect3DDevice9 **ppDxDevice);
extern __host__ cudaError_t CUDARTAPI cudaD3D9RegisterResource(IDirect3DResource9 *pResource, unsigned int flags);
extern __host__ cudaError_t CUDARTAPI cudaD3D9UnregisterResource(IDirect3DResource9 *pResource);
extern __host__ cudaError_t CUDARTAPI cudaD3D9MapResources(int count, IDirect3DResource9 **ppResources);
extern __host__ cudaError_t CUDARTAPI cudaD3D9UnmapResources(int count, IDirect3DResource9 **ppResources);
extern __host__ cudaError_t CUDARTAPI cudaD3D9ResourceSetMapFlags(IDirect3DResource9 *pResource, unsigned int flags); 
extern __host__ cudaError_t CUDARTAPI cudaD3D9ResourceGetSurfaceDimensions(size_t *pWidth, size_t *pHeight, size_t *pDepth, IDirect3DResource9 *pResource, unsigned int face, unsigned int level); 
extern __host__ cudaError_t CUDARTAPI cudaD3D9ResourceGetMappedArray(cudaArray **ppArray, IDirect3DResource9 *pResource, unsigned int face, unsigned int level);
extern __host__ cudaError_t CUDARTAPI cudaD3D9ResourceGetMappedPointer(void **pPointer, IDirect3DResource9 *pResource, unsigned int face, unsigned int level);
extern __host__ cudaError_t CUDARTAPI cudaD3D9ResourceGetMappedSize(size_t *pSize, IDirect3DResource9 *pResource, unsigned int face, unsigned int level);
extern __host__ cudaError_t CUDARTAPI cudaD3D9ResourceGetMappedPitch(size_t *pPitch, size_t *pSlicePitch, IDirect3DResource9 *pResource, unsigned int face, unsigned int level);

extern __host__ cudaError_t CUDARTAPI cudaD3D9Begin(IDirect3DDevice9 *pDevice);
extern __host__ cudaError_t CUDARTAPI cudaD3D9End(void);
extern __host__ cudaError_t CUDARTAPI cudaD3D9RegisterVertexBuffer(IDirect3DVertexBuffer9 *pVB);
extern __host__ cudaError_t CUDARTAPI cudaD3D9UnregisterVertexBuffer(IDirect3DVertexBuffer9 *pVB);
extern __host__ cudaError_t CUDARTAPI cudaD3D9MapVertexBuffer(void **dptr, IDirect3DVertexBuffer9 *pVB);
extern __host__ cudaError_t CUDARTAPI cudaD3D9UnmapVertexBuffer(IDirect3DVertexBuffer9 *pVB);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _WIN32 */

#endif /* __CUDA_D3D9_INTEROP_H__ */

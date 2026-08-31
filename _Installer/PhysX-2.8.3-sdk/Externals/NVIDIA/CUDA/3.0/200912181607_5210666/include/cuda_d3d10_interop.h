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

#if !defined(__CUDA_D3D10_INTEROP_H__)
#define __CUDA_D3D10_INTEROP_H__

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
#include <d3d10_1.h>

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
*                                                                              *
*                                                                              *
*                                                                              *
*******************************************************************************/

extern __host__ cudaError_t CUDARTAPI cudaGraphicsD3D10RegisterResource(struct cudaGraphicsResource **resource, ID3D10Resource *pD3DResource, unsigned int flags);
extern __host__ cudaError_t CUDARTAPI cudaD3D10SetDirect3DDevice(ID3D10Device *pD3DDevice);
extern __host__ cudaError_t CUDARTAPI cudaD3D10GetDevice(int *device, IDXGIAdapter *pAdapter);

/**
 * CUDA D3D10 Register Flags
 */
enum cudaD3D10RegisterFlags
{
  cudaD3D10RegisterFlagsNone  = 0,  ///< Default; Resource can be accessed through a void*
  cudaD3D10RegisterFlagsArray = 1,  ///< Resource can be accessed through a CUarray*
};

/**
 * CUDA D3D10 Map Flags
 */
enum cudaD3D10MapFlags
{
  cudaD3D10MapFlagsNone         = 0,  ///< Default; Assume resource can be read/written
  cudaD3D10MapFlagsReadOnly     = 1,  ///< CUDA kernels will not write to this resource
  cudaD3D10MapFlagsWriteDiscard = 2,  ///< CUDA kernels will only write to and will not read from this resource
};

extern __host__ cudaError_t CUDARTAPI cudaD3D10RegisterResource(ID3D10Resource *pResource, unsigned int flags);
extern __host__ cudaError_t CUDARTAPI cudaD3D10UnregisterResource(ID3D10Resource *pResource);
extern __host__ cudaError_t CUDARTAPI cudaD3D10MapResources(int count, ID3D10Resource **ppResources);
extern __host__ cudaError_t CUDARTAPI cudaD3D10UnmapResources(int count, ID3D10Resource **ppResources);
extern __host__ cudaError_t CUDARTAPI cudaD3D10ResourceGetMappedArray(cudaArray **ppArray, ID3D10Resource *pResource, unsigned int subResource);
extern __host__ cudaError_t CUDARTAPI cudaD3D10ResourceSetMapFlags(ID3D10Resource *pResource, unsigned int flags); 
extern __host__ cudaError_t CUDARTAPI cudaD3D10ResourceGetSurfaceDimensions(size_t *pWidth, size_t *pHeight, size_t *pDepth, ID3D10Resource *pResource, unsigned int subResource); 
extern __host__ cudaError_t CUDARTAPI cudaD3D10ResourceGetMappedPointer(void **pPointer, ID3D10Resource *pResource, unsigned int subResource);
extern __host__ cudaError_t CUDARTAPI cudaD3D10ResourceGetMappedSize(size_t *pSize, ID3D10Resource *pResource, unsigned int subResource);
extern __host__ cudaError_t CUDARTAPI cudaD3D10ResourceGetMappedPitch(size_t *pPitch, size_t *pSlicePitch, ID3D10Resource *pResource, unsigned int subResource);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _WIN32 */

#endif /* __CUDA_D3D10_INTEROP_H__ */

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

#ifndef CUDAD3D10_H
#define CUDAD3D10_H

#ifdef __cplusplus
extern "C" {
#endif

CUresult CUDAAPI cuD3D10GetDevice( CUdevice *pCudaDevice, IDXGIAdapter *pAdapter );
CUresult CUDAAPI cuD3D10CtxCreate( CUcontext *pCtx, CUdevice *pCudaDevice, unsigned int Flags, ID3D10Device *pD3DDevice );
CUresult CUDAAPI cuGraphicsD3D10RegisterResource( CUgraphicsResource *pCudaResource, ID3D10Resource *pD3DResource, unsigned int Flags );

//
// CUDA 2.x compatibility API. These functions are deprecated, please use the ones above.
//

// Flags to register a resource
typedef enum CUD3D10register_flags_enum {
    CU_D3D10_REGISTER_FLAGS_NONE  = 0x00,
    CU_D3D10_REGISTER_FLAGS_ARRAY = 0x01,
} CUD3D10register_flags;

// Flags to map or unmap a resource
typedef enum CUD3D10map_flags_enum {
    CU_D3D10_MAPRESOURCE_FLAGS_NONE         = 0x00,
    CU_D3D10_MAPRESOURCE_FLAGS_READONLY     = 0x01,
    CU_D3D10_MAPRESOURCE_FLAGS_WRITEDISCARD = 0x02,    
} CUD3D10map_flags;


CUresult CUDAAPI cuD3D10RegisterResource( ID3D10Resource *pResource, unsigned int Flags );
CUresult CUDAAPI cuD3D10UnregisterResource( ID3D10Resource *pResource );

CUresult CUDAAPI cuD3D10MapResources( unsigned int count, ID3D10Resource **ppResources );
CUresult CUDAAPI cuD3D10UnmapResources( unsigned int count, ID3D10Resource **ppResources );
CUresult CUDAAPI cuD3D10ResourceSetMapFlags( ID3D10Resource *pResource, unsigned int Flags );

CUresult CUDAAPI cuD3D10ResourceGetMappedArray( CUarray *pArray, ID3D10Resource *pResource, unsigned int SubResource );
CUresult CUDAAPI cuD3D10ResourceGetMappedPointer( CUdeviceptr *pDevPtr, ID3D10Resource *pResource, unsigned int SubResource );
CUresult CUDAAPI cuD3D10ResourceGetMappedSize( unsigned int *pSize, ID3D10Resource *pResource, unsigned int SubResource );
CUresult CUDAAPI cuD3D10ResourceGetMappedPitch( unsigned int *pPitch, unsigned int *pPitchSlice, ID3D10Resource *pResource, unsigned int SubResource );
CUresult CUDAAPI cuD3D10ResourceGetSurfaceDimensions( unsigned int *pWidth, unsigned int *pHeight, unsigned int *pDepth, ID3D10Resource *pResource, unsigned int SubResource );

#ifdef __cplusplus
};
#endif

#endif


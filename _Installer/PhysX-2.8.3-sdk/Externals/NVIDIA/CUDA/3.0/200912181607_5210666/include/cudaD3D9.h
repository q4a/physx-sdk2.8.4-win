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

#ifndef CUDAD3D9_H
#define CUDAD3D9_H

#ifdef __cplusplus
extern "C" {
#endif

CUresult CUDAAPI cuD3D9GetDevice( CUdevice *pCudaDevice, const char *pszAdapterName);
CUresult CUDAAPI cuD3D9CtxCreate( CUcontext *pCtx, CUdevice *pCudaDevice, unsigned int Flags, IDirect3DDevice9 *pD3DDevice );
CUresult CUDAAPI cuGraphicsD3D9RegisterResource( CUgraphicsResource *pCudaResource, IDirect3DResource9 *pD3DResource, unsigned int Flags );

//
// CUDA 2.x compatibility API. These functions are deprecated, please use the ones above.
//

// Flags to register a resource
typedef enum CUd3d9register_flags_enum {
    CU_D3D9_REGISTER_FLAGS_NONE  = 0x00,
    CU_D3D9_REGISTER_FLAGS_ARRAY = 0x01,
} CUd3d9register_flags;

//
// Flags to map or unmap a resource
//
typedef enum CUd3d9map_flags_enum {
    CU_D3D9_MAPRESOURCE_FLAGS_NONE         = 0x00,
    CU_D3D9_MAPRESOURCE_FLAGS_READONLY     = 0x01,
    CU_D3D9_MAPRESOURCE_FLAGS_WRITEDISCARD = 0x02,    
} CUd3d9map_flags;

CUresult CUDAAPI cuD3D9GetDirect3DDevice( IDirect3DDevice9 **ppD3DDevice );

CUresult CUDAAPI cuD3D9RegisterResource( IDirect3DResource9 *pResource, unsigned int Flags );
CUresult CUDAAPI cuD3D9UnregisterResource( IDirect3DResource9 *pResource );

CUresult CUDAAPI cuD3D9MapResources( unsigned int count, IDirect3DResource9 **ppResource );
CUresult CUDAAPI cuD3D9UnmapResources( unsigned int count, IDirect3DResource9 **ppResource );
CUresult CUDAAPI cuD3D9ResourceSetMapFlags( IDirect3DResource9 *pResource, unsigned int Flags ); 

CUresult CUDAAPI cuD3D9ResourceGetSurfaceDimensions( unsigned int *pWidth, unsigned int *pHeight, unsigned int *pDepth, IDirect3DResource9 *pResource, unsigned int Face, unsigned int Level );
CUresult CUDAAPI cuD3D9ResourceGetMappedArray( CUarray *pArray, IDirect3DResource9 *pResource, unsigned int Face, unsigned int Level );
CUresult CUDAAPI cuD3D9ResourceGetMappedPointer( CUdeviceptr *pDevPtr, IDirect3DResource9 *pResource, unsigned int Face, unsigned int Level );
CUresult CUDAAPI cuD3D9ResourceGetMappedSize( unsigned int *pSize, IDirect3DResource9 *pResource, unsigned int Face, unsigned int Level );
CUresult CUDAAPI cuD3D9ResourceGetMappedPitch( unsigned int *pPitch, unsigned int *pPitchSlice, IDirect3DResource9 *pResource, unsigned int Face, unsigned int Level );

// CUDA 1.x compatibility API. These functions are deprecated, please use the ones above.

CUresult CUDAAPI cuD3D9Begin( IDirect3DDevice9 *pDevice );
CUresult CUDAAPI cuD3D9End( void );
CUresult CUDAAPI cuD3D9RegisterVertexBuffer( IDirect3DVertexBuffer9 *pVB );
CUresult CUDAAPI cuD3D9MapVertexBuffer( CUdeviceptr *pDevPtr, unsigned int *pSize, IDirect3DVertexBuffer9 *pVB );
CUresult CUDAAPI cuD3D9UnmapVertexBuffer( IDirect3DVertexBuffer9 *pVB );
CUresult CUDAAPI cuD3D9UnregisterVertexBuffer( IDirect3DVertexBuffer9 *pVB );

#ifdef __cplusplus
};
#endif

#endif


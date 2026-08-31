// This code contains NVIDIA Confidential Information and is disclosed 
// under the Mutual Non-Disclosure Agreement.
//
// Notice
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES 
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO 
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT, 
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
// 
// Information and code furnished is believed to be accurate and reliable. 
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such 
// information or for any infringement of patents or other rights of third parties that may 
// result from its use. No license is granted by implication or otherwise under any patent 
// or patent rights of NVIDIA Corporation. Details are subject to change without notice. 
// This code supersedes and replaces all information previously supplied. 
// NVIDIA Corporation products are not authorized for use as critical 
// components in life support devices or systems without express written approval of 
// NVIDIA Corporation.
//
// Copyright 2008, NVIDIA Corporation. All rights reserved.

#ifndef __GPU_FINDER_
#define __GPU_FINDER_

#include "nvapi.h"


#define gpuUnavailable          (0)
#define gpuFoundNoActiveHeads   (1<<0)
#define gpuFoundHeadless        (1<<1)
#define gpuComputeCapable       (1<<2)


// We use cudaGetDeviceProperties() to determine the compute capabilities
// of the device.  These represent CUDA 1.1.  I figured we shouldn't be using
// GPUs which are dumber than that.
#define PHYSX_MINIMUM_MAJOR_COMPUTE_CAPABILITY  1
#define PHYSX_MINIMUM_MINOR_COMPUTE_CAPABILITY  0	        // G80+
#define PHYSX_MINIMUM_MULTIPROCESSOR_COUNT      0           // allow all CUDA capable MP counts
#define PHYSX_MINIMUM_MEM_SIZE                  (0x10000000-0x00800000)  // 248MB

class GpuFinder 
{
public:
    static int findAvailableGpu();

private:
    GpuFinder();
    static NvU32 findPhysGpuIdx(NvPhysicalGpuHandle *handleList,
                                NvPhysicalGpuHandle physHandle, 
                                NvU32 handleCount);

    static bool  gpuIsComputeCapable(int devIdx, NvPhysicalGpuHandle gpuHandle);

	static void  getGpuName(char * name,int devIdx);
};

#endif /** __GPU_FINDER_ */
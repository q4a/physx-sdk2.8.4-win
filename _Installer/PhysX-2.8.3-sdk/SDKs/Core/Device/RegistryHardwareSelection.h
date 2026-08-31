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

#ifndef __HARDWARE_SELECTION_
#define __HARDWARE_SELECTION_

#define AG_REG_BASE         "Software\\Ageia Technologies"
#define HW_SEL_SUBKEY       "HwSelection"
#define CPU_SEL_STRING      "CPU"
#define GPU_SEL_STRING      "GPU"
#define PPU_SEL_STRING      "PPU"

#define CUDA_DEV_SUBKEY     "CudaDeviceOrdinal"

class RegistryHardwareSelection 
{
public:
	enum SelectionType
	{
		HW_SEL_CPU,
		HW_SEL_GPU,
		HW_SEL_PPU,
	};

	static SelectionType		 getSelection();
    static int                   getCudaDeviceOrdinal(bool &useHardware);
private:
    static bool                  readRegKey(bool currentUser, 
                                            const char* inKey, 
                                            char* outData, 
                                            unsigned int* length);
    RegistryHardwareSelection();
    
};

#endif /** __HARDWARE_SELECTION_ */
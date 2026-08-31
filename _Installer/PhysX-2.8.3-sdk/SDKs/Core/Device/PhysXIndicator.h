 /************************ BEGIN COPYRIGHT NOTICE ***************************\
|*                                                                           *|
|* Copyright 2009 by NVIDIA Corporation.  All rights reserved.  All          *|
|* information contained herein is proprietary and confidential to NVIDIA    *|
|* Corporation.  Any use, reproduction, or disclosure without the written    *|
|* permission of NVIDIA Corporation is prohibited.                           *|
|*                                                                           *|
 \************************** END COPYRIGHT NOTICE ***************************/

#ifndef __PHYSXINDICATOR_H__
#define __PHYSXINDICATOR_H__

#if defined(WIN32)
#	include "AgMMFile.h"
#endif 

class PhysXCpuIndicator
{
public:
	PhysXCpuIndicator();
	~PhysXCpuIndicator();

#if defined(WIN32)
	AgMMFile mSharedMemConfig;
#endif
};

class PhysXGpuIndicator
{
public:
	PhysXGpuIndicator();
	~PhysXGpuIndicator();
};

#endif
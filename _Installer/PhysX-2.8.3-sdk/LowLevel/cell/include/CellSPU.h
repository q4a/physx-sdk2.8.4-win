/* NVIDIA/SCE CONFIDENTIAL
   NVIDIA PhysX(TM) SDK for PlayStation(R)3 Version PS3_PhysX_283_B2_forSDK330_hlsrc
*/
// SCE CONFIDENTIAL
// Copyright (C) Sony Computer Entertainment Inc.
// All Rights Reserved.

#ifndef _CELLSPU_H_
#define _CELLSPU_H_

#ifdef __SPU__
	#include <stdlib.h>

	#include <spu_printf.h>

	#include <cell/dma.h>
	#include <cell/atomic.h>
#else
	#include <stdio.h>
	#define spu_printf printf
	
	#include "emulation/CellDMASoftwareEmulation.h"
	#include "emulation/CellAtomicSoftwareEmulation.h"
#endif

#endif // _CELLSPU_H_



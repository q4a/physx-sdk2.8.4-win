#ifndef __NX_MODIFIER_DEFS_H__
#define __NX_MODIFIER_DEFS_H__
/*
 * Copyright 2009-2010 NVIDIA Corporation.  All rights reserved.
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
namespace physx {
namespace apex {

PX_PUSH_PACK_DEFAULT

/**
Roll type of a particle
*/
struct NxApexMeshParticleRollType
{
	enum Enum
	{
		SPHERICAL = 0, ///< roll as a sphere
		CUBIC,		   ///< as a cubical object	
		FLAT_X,		   ///< as a flat object (shortest axis is x) 	
		FLAT_Y,		   ///< as a flat object (shortest axis is y)
		FLAT_Z,		   ///< as a flat object (shortest axis is z) 	
		LONG_X,		   ///< as a long object (longest axis is x) 		
		LONG_Y,		   ///< as a long object (longest axis is y) 			
		LONG_Z,		   ///< as a long object (longest axis is z) 				
		SPRITE,		   ///< as a sprite		

		COUNT
	};
};

/**
Modifiers list
Note: These are serialized to disk, so if you reorder them or change existing modifier types, you
will need to version the stream and map the old values to the new values.
If new values are just appended, no other special care needs to be handled.
*/
enum ModifierTypeEnum
{
	ModifierType_Invalid						= 0,
	ModifierType_Rotation						= 1,
	ModifierType_SimpleScale					= 2,
	ModifierType_RandomScale					= 3,
	ModifierType_ColorVsLife					= 4,
	ModifierType_ColorVsDensity					= 5,
	ModifierType_SubtextureVsLife				= 6,
	ModifierType_OrientAlongVelocity			= 7,
	ModifierType_ScaleAlongVelocity				= 8,
	ModifierType_RandomSubtexture				= 9,
	ModifierType_RandomRotation					= 10,
	ModifierType_ScaleVsLife					= 11,
	ModifierType_ScaleVsDensity					= 12,
	ModifierType_ScaleVsCameraDistance			= 13,
	ModifierType_ViewDirectionSorting			= 14,
	ModifierType_RotationRate					= 15,
	ModifierType_RotationRateVsLife				= 16,
	ModifierType_OrientScaleAlongScreenVelocity	= 17,

	ModifierType_Count
};

/**
Stage at which the modifier is applied
*/
enum ModifierStage
{
	ModifierStage_Spawn = 0,		///< at the spawn
	ModifierStage_Continuous = 1,	///< on every frame

	ModifierStage_Count
};

enum ColorChannel
{
	ColorChannel_Red	= 0,
	ColorChannel_Green	= 1,
	ColorChannel_Blue	= 2,
	ColorChannel_Alpha	= 3
};

enum ScaleAxis
{
	ScaleAxis_X			= 0,
	ScaleAxis_Y			= 1,
	ScaleAxis_Z			= 2
};

enum ModifierUsage
{
	ModifierUsage_Spawn			= 0x01,
	ModifierUsage_Continuous	= 0x02,

	ModifierUsage_Sprite		= 0x04,
	ModifierUsage_Mesh			= 0x08,
};

PX_POP_PACK

}} // namespace physx::apex

#endif //__NX_MODIFIER_DEFS_H__

#ifndef NX_CLOTHING_MATERIAL_H
#define NX_CLOTHING_MATERIAL_H
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
#include "NxApex.h"
#include "PxFoundation.h"

namespace physx {
namespace apex {

PX_PUSH_PACK_DEFAULT

class NxClothingMaterialLibrary;

/**
\brief Stub frontend for a material inside a material library
*/
class NxClothingMaterial
{
public:
	/**
	\brief Returns the name of the material
	*/
	virtual const char* getName() const = 0;

	/**
	\brief Returns the containing library (aka parent)
	*/
	virtual NxClothingMaterialLibrary* getLibrary() = 0;

	/**
	\brief Release the material, only works if it is not referenced anymore
	*/
	virtual void release() = 0;

	/**
	\brief The stretching stiffness of a cloth/softbody

	\see NxClothingMaterial::getHardStretchLimitation
	*/
	virtual physx::PxF32 getStretchingStiffness() const = 0;

	/**
	\brief The bending stiffness of a cloth/softbody
	*/
	virtual physx::PxF32 getBendingStiffness() const = 0;

	/**
	\brief A more accurate bending force, comes with a performance penalty
	*/
	virtual bool getOrthoBending() const = 0;

	/**
	\brief The amount of damping per constraint
	*/
	virtual physx::PxF32 getDamping() const = 0;

	/**
	\brief Damping around the COM of the Cloth instead of on each constraint. Only of limited use for clothing
	*/
	virtual bool getComDamping() const = 0;

	/**
	\brief The friction between cloth/softbody and rigid bodies.
	*/
	virtual physx::PxF32 getFriction() const = 0;

	/**
	\brief Maximum number of solver iterations permitted, lower physical LoDs will only have a fraction of this value
	*/
	virtual physx::PxU32 getSolverIterations() const = 0;
	
	/**
	\brief Modifies the amount of gravity that is applied to this piece of clothing
	*/
	virtual physx::PxF32 getGravityScale() const = 0;

	/**
	\brief Project some constraints to enforce stretch limitations.
	*/
	virtual physx::PxF32 getHardStretchLimitation() const = 0;

	/**
	\brief Deformation factor of the max distance sphere

	The sphere will transform into the normal of a vertex the more this value goes towards -1
	The sphere will transform into a disc perpendicular to the normal the more this value goes towards 1
	*/
	virtual physx::PxF32 getMaxDistanceBias() const = 0;

	/**
	\brief Number of hierarchical solver iterations
	*/
	virtual physx::PxU32 getHierarchicalSolverIterations() const = 0;

	/**
	\brief Returns the number of NxClothingActors that reference this material
	*/
	virtual physx::PxU32 getReferenceCount() const = 0;


	/**
	\brief The stretching stiffness of a cloth/softbody, needs to be in range (0, 1]

	\see NxClothingMaterial::setHardStretchLimitation
	*/
	virtual void setStretchingStiffness(physx::PxF32 stretching) = 0;

	/**
	\brief The bending stiffness of a cloth/softbody, needs to be in range [0, 1]
	*/
	virtual void setBendingStiffness(physx::PxF32 bending) = 0;

	/**
	\brief A more accurate bending force, comes with a performance penalty
	*/
	virtual void setOrthoBending(bool ortho) = 0;

	/**
	\brief The amount of damping per constraint
	*/
	virtual void setDamping(physx::PxF32 damping) = 0;

	/**
	\brief Damping around the COM of the Cloth instead of on each constraint. Only of limited use for clothing
	*/
	virtual void setComDamping(bool com) = 0;

	/**
	\brief The friction between cloth/softbody and rigid bodies.
	*/
	virtual void setFriction(physx::PxF32 friction) = 0;

	/**
	\brief Maximum number of solver iterations permitted, lower physical LoDs will only have a fraction of this value
	*/
	virtual void setSolverIterations(physx::PxU32 solverIterations) = 0;
	
	/**
	\brief Modifies the amount of gravity that is applied to this piece of clothing. Must be in range [0, inf)

	Values higher than 1.0 will generate accelerations higher than gravity
	Values between 0.0 and 1.0 will generate moon gravity
	*/
	virtual void setGravityScale(physx::PxF32 gravityScale) = 0;

	/**
	\brief Project some constraints to enforce stretch limitations. Must be in [1, inf) or 0 to disable
	*/
	virtual void setHardStretchLimitation(physx::PxF32 hardStretchLimitation) = 0;

	/**
	\brief Deformation factor of the max distance sphere, must be in [-1, 1] range, 0 to turn off

	The sphere will transform into the normal of a vertex the more this value goes towards -1
	The sphere will transform into a disc perpendicular to the normal the more this value goes towards 1
	*/
	virtual void setMaxDistanceBias(physx::PxF32 maxDistanceBias) = 0;

	/**
	\brief Number of hierarchical solver iterations, 0 to turn off

	If either the hierarchical solver iterations or the hierarchical levels are set to 0, this feature is turned off
	\see NxClothingAssetAuthoring::setHierarchicalLevels()
	*/
	virtual void setHierarchicalSolverIterations(physx::PxU32 hierarchicalSolverIterations) = 0;
};


PX_POP_PACK

}} // namespace physx::apex

#endif

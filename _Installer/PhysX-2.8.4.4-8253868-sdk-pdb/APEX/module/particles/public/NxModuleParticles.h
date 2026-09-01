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
#ifndef __NX_MODULE_PARTICLES_H__
#define __NX_MODULE_PARTICLES_H__

#include "NxApex.h"
#include <limits.h>

class NxCompartment;

namespace physx {
namespace apex {

PX_PUSH_PACK_DEFAULT

class NxFluidIosAsset;
class NxFluidIosAssetAuthoring;

/**
\brief APEX Particles module descriptor. Used for initializing the module.  
*/
class NxModuleParticlesDesc : public NxApexDesc
{
public:

	/**
	\brief constructor sets to default.
	*/
	PX_INLINE NxModuleParticlesDesc() : NxApexDesc()
	{
		init();
	}

	/**
	\brief sets members to default values.
	*/
	PX_INLINE void setToDefault()
	{
		NxApexDesc::setToDefault();
		init();
	}

	/**
	\brief checks if this is a valid descriptor.
	*/
	PX_INLINE bool isValid() const
	{
		bool retVal = NxApexDesc::isValid();
		return retVal;
	}

private:

	PX_INLINE void init()
	{
	}
};

/**
\brief APEX Particles module. A particle system based upon PhysX SDK particles
*/
class NxModuleParticles : public NxModule
{
protected:
	virtual									~NxModuleParticles(){}

public:
	/// Initializer. \sa NxModuleParticlesDesc
	//virtual void							init( const NxModuleParticlesDesc & moduleParticlesDesc ) = 0;
	//virtual void init( ::NxParameterized::Interface &desc ) = 0;
	/** 
	\brief Sets the compartment that will be used for non-SPH calculations in the given ApexScene.
	
	\sa NxCompartment
	*/
	virtual void                            setCompartment( const NxApexScene&, NxCompartment& comp ) = 0;
	/** 
	\brief Gets the compartment that is used for non-SPH calculations in the given ApexScene.
	
	\sa NxCompartment
	*/
	virtual const NxCompartment *           getCompartment( const NxApexScene& ) const = 0;

	/** 
	\brief Sets the compartment that will be used for SPH calculations in the given ApexScene.
	
	If none provided, the compartment for non-SPH calculations will br used
	\sa NxCompartment
	*/
    virtual void                            setSPHCompartment( const NxApexScene&, NxCompartment& comp ) = 0;
	/** 
	\brief Gets the compartment that is used for SPH calculations in the given ApexScene.
	
	\sa NxCompartment
	*/
  	virtual const NxCompartment *           getSPHCompartment( const NxApexScene& ) const = 0;
};

#if !defined(_USRDLL)
void instantiateModuleParticles();
#endif

PX_POP_PACK

}} // namespace physx::apex

#endif // #ifndef __NX_MODULE_PARTICLES_H__

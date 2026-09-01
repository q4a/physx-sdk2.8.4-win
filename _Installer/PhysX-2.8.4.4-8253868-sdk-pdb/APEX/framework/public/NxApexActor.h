#include "NxApex.h"
#ifndef __NX_APEX_ACTOR_H__
#define __NX_APEX_ACTOR_H__
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

/*!
\file
\brief classes NxApexActor, NxApexActorSource
*/

// PhysX SDK class declarations
class NxActorDesc;
class NxActorDescBase;
class NxShapeDesc;
class NxBodyDesc;

namespace physx {
namespace apex {

PX_PUSH_PACK_DEFAULT

class NxApexAsset;

/**
\brief Base class for APEX module objects.
*/
class NxApexActor : public NxApexInterface
{
public:
	/**
	\brief Returns the owning asset
	*/
	virtual NxApexAsset * getOwner() const = 0;

	/**
	\brief Returns the range of possible values for physical Lod overwrite

	\param [out] min		The minimum lod value
	\param [out] max		The maximum lod value
	\param [out] intOnly	Only integers are allowed if this is true, gets rounded to nearest
	
	\note The max value can change with different graphical Lods
	\see NxApexActor::forcePhysicalLod()
	*/
	virtual void getPhysicalLodRange(physx::PxF32 &min, physx::PxF32 &max, bool &intOnly) = 0;

	/**
	\brief Get current physical lod.
	*/
	virtual physx::PxF32 getActivePhysicalLod() = 0;

	/**
	\brief Force an APEX Actor to use a certian physical Lod
	
	\param [in] lod	Overwrite the Lod system to use this Lod.

	\note Setting the lod value to a negative number will turn off the overwrite and proceed with regular Lod computations
	\see NxApexActor::getPhysicalLodRange()
	*/
	virtual void forcePhysicalLod(physx::PxF32 lod) = 0;

	/**
	\brief Ensure that all module-cached data is cached.
	*/
	virtual void cacheModuleData() const {}
};

/**
\brief Base class for APEX classes that spawn PhysX SDK Actors
*/
class NxApexActorSource
{
public:
	/**
	\brief Set the current actor template

	User can specify a descriptor template for actors that this object may create.  APEX may customize these suggested settings.
	Does not include shape array, as we only want base properties.  Specify base shape properties using setShapeBase().

	Already created / existing actors will not be changed if the actor template is changed!  The actor template will only be used for new
	actors created after this is called!

	members that are ignored:
	globalPose
	body
	type

	These fields should be left at their default values as set by the desc constructor.

	Because it is not possible to instance the type NxActorDescBase directly, it is simplest to pass a pointer to a NxActorDesc.

	You can pass NULL to not have a template.
	*/
	virtual void setActorTemplate(const NxActorDescBase *) = 0;

	/**
	\brief Retrieve the current actor template
	
	If the template is NULL this will return false; otherwise it will fill in dest and return true.
	*/
	virtual bool getActorTemplate(NxActorDescBase & dest) const = 0;

	/**
	\brief Sets the current shape template

	User can specify a descriptor template for shapes that this object may create.  APEX may customize these suggested settings.
	Already created / existing shapes will not be changed if the shape template is changed!  The shape template will only be used for new
	shapes created after this is called!

	members that are ignored:
	type
	localPose
	ccdSkeleton

	These fields should be left at their default values as set by the desc constructor.

	Because it is not possible to instance the type NxShapeDesc directly, it is simplest to pass a pointer to a NxSphereShapeDesc.
	*/
	virtual void setShapeTemplate(const NxShapeDesc *) = 0;

	/**
	\brief Retrieve the current shape template
	
	If the template is NULL this will return false; otherwise it will fill in dest and return true.
	*/
	virtual bool getShapeTemplate(NxShapeDesc & dest) const = 0;

	/**
	\brief Sets the current body template

	User can specify a descriptor template for bodies that this object may create.  APEX may customize these suggested settings.
	Already created / existing bodies will not be changed if the body template is changed!  The body template will only be used for
	new bodies created after this is called!

	members that are ignored:
	massLocalPose
	massSpaceInertia
	mass
	linearVelocity
	angularVelocity

	These fields should be left at their default values as set by the desc constructor.

	Because it is not possible to instance the type NxShapeDesc directly, it is simplest to pass a pointer to a NxSphereShapeDesc.
	*/	
	virtual void setBodyTemplate(const NxBodyDesc *) = 0;

	/**
	\brief Retrieve the current body template
	
	If the template is NULL this will return false; otherwise it will fill in dest and return true.
	*/
	virtual bool getBodyTemplate(NxBodyDesc & dest) const = 0;

};

PX_POP_PACK

}} // end namespace physx::apex

#endif // __NX_APEX_ACTOR_H__

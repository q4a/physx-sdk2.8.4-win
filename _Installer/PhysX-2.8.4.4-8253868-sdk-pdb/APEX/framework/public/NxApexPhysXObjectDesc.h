#include "NxApex.h"
#ifndef __NX_APEX_PHYSX_OBJECT_DESC_H__
#define __NX_APEX_PHYSX_OBJECT_DESC_H__
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
\brief class NxApexPhysXObjectDesc
*/

namespace physx {
namespace apex {

class NxApexActor;

PX_PUSH_PACK_DEFAULT

/** 
 * \brief PhysX object descriptor
 *
 * Class which describes how a PhysX object is being used by APEX.  Data access is
 * non virtual for performance reasons.
 */
class NxApexPhysXObjectDesc
{
protected:
	/**
	\brief Object interaction flags

	These flags determine how this PhysX object should interact with user callbacks.  For
	instance whether contact callbacks with the object should be ignored.
	*/
	physx::PxU32				mFlags;

	enum {
		TRANSFORM   = (1<<31),	//!< If set, ignore this object's xform in active actor callbacks
		RAYCASTS    = (1<<30),  //!< If set, ignore this object in raycasts
		CONTACTS    = (1<<29)   //!< If set, ignore this object in contact callbacks
	};


public:
	/** \brief Returns the number of NxApexActors associated with the PhysX object */
	virtual physx::PxU32				getApexActorCount() const = 0;
	/** \brief Returns the indexed NxApexActor pointer */
	virtual const NxApexActor*	getApexActor( physx::PxU32 i ) const = 0;

	/** \brief Returns whether this object's xform should be ignored */
	bool ignoreTransform() const			{ return (mFlags & (physx::PxU32)TRANSFORM) ? true : false; }
	/** \brief Returns whether this object should be ignored by raycasts */
	bool ignoreRaycasts() const				{ return (mFlags & (physx::PxU32)RAYCASTS) ? true : false; }
	/** \brief Returns whether this object should be ignored by contact report callbacks */
	bool ignoreContacts() const				{ return (mFlags & (physx::PxU32)CONTACTS) ? true : false; }
	/** \brief Returns a user defined status bit */
	bool getUserDefinedFlag(physx::PxU32 index) const { return (mFlags & (physx::PxU32)(1<<index)) ? true : false; }

	/**
	\brief User data, for use by APEX
	
	For internal use by APEX.  Please do not modify this field.  You may use the PhysX object
	userData or NxApexActor userData field.
	*/
	void *				userData;
};

// To get owning NxApexActor's authorable object type name:
//		getActor()->getOwner()->getAuthObjName();
// To get owning NxApexActor's authorable object type ID:
//		getActor()->getOwner()->getAuthObjType();

PX_POP_PACK

}} // end namespace physx::apex

#endif //__NX_APEX_PHYSX_OBJECT_DESC_H__

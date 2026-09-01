#include "NxApex.h"
#ifndef __NX_APEX_DESC_H__
#define __NX_APEX_DESC_H__
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
\brief class NxApexDesc
*/

namespace physx {
namespace apex {

PX_PUSH_PACK_DEFAULT

/**
\brief Base class for all APEX Descriptor classes

A descriptor class of type NxXDesc is typically passed to a createX() function.  Descriptors have 
several advantages over simply passing a number of explicit parameters to create():

- all parameters can have default values so the user only needs to know about the ones he needs to change
- new parameters can be added without changing the user code, along with defaults
- the user and the SDK can validate the parameter's correctness using isValid()
- if creation fails, the user can look at the code of isValid() to see what exactly is not being accepted by the SDK
- some object types can save out their state into descriptors again for serialization

Care should be taken that derived descriptor classes do not initialize their base class members multiple times,
once in the constructor, and once in setToDefault()!
*/
class NxApexDesc
{
public:
	/**
	\brief for standard init of user data member
	*/
	void * userData;

	/**
	\brief constructor sets to default.
	*/
	PX_INLINE NxApexDesc()
	{
		setToDefault();
	}
	/**
	\brief (re)sets the structure to the default.	
	*/
	PX_INLINE void setToDefault()
	{
		userData = 0;
	}
	/**
	\brief Returns true if the descriptor is valid.
	\return true if the current settings are valid.
	*/
	PX_INLINE bool isValid() const
	{
		return true;
	}
};

PX_POP_PACK

}} // end namespace physx::apex

#endif

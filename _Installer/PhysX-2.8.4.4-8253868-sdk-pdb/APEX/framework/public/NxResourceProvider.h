#include "NxApex.h"
#ifndef __NX_APEX_RESOURCE_PROVIDER_H__
#define __NX_APEX_RESOURCE_PROVIDER_H__
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
\brief class NxResourceProvider
*/

namespace physx {
namespace apex {

class NxResourceCallback;

PX_PUSH_PACK_DEFAULT

/*!
\brief A user provided class for mapping names to pointers or integers

Named resource provider - a name-to-pointer utility.  User must provide the pointed-to data.
				        - also supports name-to-integer
*/

class NxResourceProvider
{
public:
	/**
	\brief Register a callback

	Register a callback function for unresolved named resources.  This function will be called by APEX when
	an unresolved named resource is requested.  The function will be called at most once for each named
	resource.  The function must directly return the pointer to the resource or NULL.
	*/
	virtual void registerCallback( NxResourceCallback *impl ) = 0;

	/**
	\brief Provide the pointer for the specified named resource
	*/
	virtual void setResource( const char *nameSpace, const char *name, void* resource,bool incRefCount=false) = 0;


	/**
	\brief Provide the unsigned integer for the specified named resource
	*/
	virtual void   	setResourceU32(const char *nameSpace, const char *name,physx::PxU32 id, bool incRefCount=false) = 0;

	/**
	\brief Retrieve the pointer to the specified named resource.

	The game application (level loader) may use this to find assets by
	name, as a convenience.  If the named resource has not yet been
	loaded it will trigger a call to NxResourceCallback::requestResource(),
	assuming an NxResourceCallback instance was registered with the APEX SDK.
	*/
	virtual void *getResource( const char *nameSpace, const char *name ) = 0;

	/**
	\brief Releases all resources in this namespace.
	\return the total number of resources released.
	*/
	virtual physx::PxU32  	releaseAllResourcesInNamespace(const char *nameSpace) = 0;

	/**
	\brief Releases a single resource.
	\return the outstanding referernce count after the release is performed.
	*/
	virtual physx::PxU32  	releaseResource(const char *nameSpace, const char *name) = 0;

	/**
	\brief Reports if a current resource exists and, if so, the reference count.
	*/
	virtual bool    findRefCount(const char *nameSpace, const char *name, physx::PxU32 &refCount) = 0;

	/**
	\brief Locates an existing resource
	
	This function will *not* call back to the application if the resource does not exist.
	Only reports currently set resources.
	*/
	virtual void * 	findResource(const char *nameSpace, const char *name) = 0;

	/**
	\brief Locates an existing integer resource.
	*/
	virtual physx::PxU32 	findResourceU32(const char *nameSpace, const char *name) = 0;

	/**
	\brief Returns a list of all resources in a particular namespace.
	*/
	virtual void ** findAllResources(const char *nameSpace, physx::PxU32 &count) = 0;

	/**
	\brief Returns a list of the names of all resources within a particular namespace
	*/
	virtual const char ** findAllResourceNames(const char *nameSpace,physx::PxU32 &count) = 0;

	/**
	\brief Returns a list of all registered namespaces.
	*/
	virtual const char ** findNameSpaces(physx::PxU32 &count) = 0;

	/**
	\brief Write contents of resource table to error stream.
	*/
	virtual void dumpResourceTable() = 0;
};

PX_POP_PACK

}} // end namespace physx::apex

#endif

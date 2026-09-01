#include "NxApex.h"
#ifndef NX_APEX_RENDER_DATA_PROVIDER_H
#define NX_APEX_RENDER_DATA_PROVIDER_H
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
\brief class NxApexRenderDataProvider
*/

namespace physx {
namespace apex {

PX_PUSH_PACK_DEFAULT

/**
\brief An actor instance that provides renderable data
*/
class NxApexRenderDataProvider
{
public:
	/**
	\brief Lock the renderable resources of this NxApexRenderable actor

	Locks the renderable data of this NxApexRenderable actor.  If the user uses an NxApexRenderableIterator
	to retrieve the list of NxApexRenderables, then locking is handled for them automatically by APEX.  If the
	user is storing NxApexRenderable pointers and using them ad-hoc, then they must use this API to lock the
	actor while updateRenderResources() and/or dispatchRenderResources() is called.  If an iterator is not being
	used, the user is also responsible for insuring the NxApexRenderable has not been deleted by another game
	thread.
	*/
	virtual void lockRenderResources() = 0;

	/**
	\brief Unlocks the renderable data of this NxApexRenderable actor.

	See locking semantics for NxApexRenderDataProvider::lockRenderResources().
	*/
	virtual void unlockRenderResources() = 0;

	/**
	\brief Update the renderable data of this NxApexRenderable actor.

    When called, this method will use the NxUserRenderResourceManager interface to inform the user
    about its render resource needs.  It will also call the writeBuffer() methods of various graphics
    buffers.  It must be called by the user each frame before any calls to dispatchRenderResources().
    If the actor is not being rendered, this function may also be skipped.
	*/
	virtual void updateRenderResources(bool rewriteBuffers = false, void * userRenderData = 0) = 0;
};

PX_POP_PACK

}} // end namespace physx::apex

#endif

#include "NxApex.h"
#ifndef __NX_APEX_CONTEXT_H__
#define __NX_APEX_CONTEXT_H__
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
\brief class NxApexContext
*/

namespace physx {
namespace apex {

class NxApexRenderable;
class NxApexRenderableIterator;

PX_PUSH_PACK_DEFAULT

/**
\brief A container for NxApexActors
*/
class NxApexContext
{
public:
	/**
	\brief Removes all actors from the context and releases them
    */
	virtual void		              removeAllActors() = 0;

	/**
	\brief Create an iterator for all renderables in this context
    */
    virtual NxApexRenderableIterator *createRenderableIterator() = 0;

	/**
	\brief Release a renderable iterator

	Equivalent to calling the iterator's release method.
    */
    virtual void					  releaseRenderableIterator(NxApexRenderableIterator&) = 0;

protected:
	virtual ~NxApexContext() {}
};

/**
\brief Iterate over all renderable NxApexActors in an NxApexContext

An NxApexRenderableIterator is a lock-safe iterator over all renderable
NxApexActors in an NxApexContext.  Actors which are locked are skipped in the initial
pass and deferred till the end.  The returned NxApexRenderable is locked by the
iterator and remains locked until you call getNext().

The NxApexRenderableIterator is also deletion safe.  If an actor is deleted
from the NxApexContext in another thread, the iterator will skip that actor.

An NxApexRenderableIterator should not be held for longer than a single simulation
step.  It should be allocated on demand and released after use.
*/
class NxApexRenderableIterator : public NxApexInterface
{
public:
	/**
	\brief Return the first renderable in an NxApexContext
	*/
    virtual NxApexRenderable *getFirst() = 0;
	/**
	\brief Return the next unlocked renderable in an NxApexContext
	*/
    virtual NxApexRenderable *getNext() = 0;
	/**
	\brief Refresh the renderable actor list for this context

	This function is only necessary if you believe actors have been added or
	deleted since the iterator was created.
	*/
	virtual void			  reset() = 0;
};

PX_POP_PACK

}} // end namespace physx::apex

#endif // __NX_APEX_CONTEXT_H__

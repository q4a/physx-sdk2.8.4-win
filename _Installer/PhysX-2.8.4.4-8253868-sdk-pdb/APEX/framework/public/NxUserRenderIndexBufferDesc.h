#include "NxApex.h"
#ifndef NX_USER_RENDER_INDEXBUFFER_DESC_H
#define NX_USER_RENDER_INDEXBUFFER_DESC_H
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
\brief class NxUserRenderIndexBufferDesc, structs NxRenderDataFormat and NxUserRenderIndexBufferDesc
*/

namespace physx
{
namespace pxtask
{
	class CudaContextManager;
}
};

namespace physx {
namespace apex {

PX_PUSH_PACK_DEFAULT
#pragma warning(push)
#pragma warning(disable:4121)

/**
\brief describes the semantics and layout of an index buffer
*/
class NxUserRenderIndexBufferDesc
{
public:
	NxUserRenderIndexBufferDesc(void)
	{
		registerInCUDA = false;
		interopContext = 0; 
		maxIndices = 0;
		hint       = NxRenderBufferHint::STATIC;
		format     = NxRenderDataFormat::UNSPECIFIED;
		primitives = NxRenderPrimitiveType::TRIANGLES;
	}
	
	bool isValid(void) const
	{
		physx::PxU32 numFailed = 0;
		numFailed += (registerInCUDA && !interopContext) ? 1 : 0;
		return (numFailed == 0);
	}
	
public:

	/**
	\brief The maximum amount of indices this buffer will ever hold.
	*/
	physx::PxU32				maxIndices;

	/**
	\brief Hint on how often this buffer is updated
	*/
	NxRenderBufferHint::Enum	hint;

	/**
	\brief The format of this buffer (only one implied semantic)
	*/
	NxRenderDataFormat::Enum	format;

	/**
	\brief Rendering primitive type (triangle, line strip, etc)
	*/
	NxRenderPrimitiveType::Enum	primitives;

	/**
	\brief Declare if the resource must be registered in CUDA upon creation
	*/
	bool						registerInCUDA;

	/**
	\brief The CUDA context

	This context must be used to register and unregister the resource every time the
	device is lost and recreated.
	*/
	physx::pxtask::CudaContextManager	*interopContext;  
};

#pragma warning(pop)
PX_POP_PACK

}} // end namespace physx::apex

#endif

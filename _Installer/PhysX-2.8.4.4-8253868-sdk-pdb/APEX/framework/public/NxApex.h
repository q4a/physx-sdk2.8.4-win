#ifndef __NX_APEX_H__
#define __NX_APEX_H__
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

/**
\file
\brief The top level include file for all of the APEX API.

Include this whenever you want to use anything from the APEX API
in a source file.
*/

// PhysX 3.0 Foundation types and classes under physx namespace
namespace physx
{
	namespace pubfnd2{}
	namespace shdfnd2{}
	namespace apex{}
	using namespace pubfnd2;
	using namespace shdfnd2;
	using namespace apex;
};

namespace NxParameterized
{
	class Traits;
	class Interface;
	class Serializer;
};


#include "PxPreprocessor.h"
#include "PxSimpleTypes.h"
#include "PxAssert.h"
#include "PxFileBuf.h"
#include "PxBounds3.h"
#include "PxVec2.h"
#include "PxVec3.h"
#include "PxMat33Legacy.h"
#include "PxMat34Legacy.h"


// APEX public API:
// In general, APEX public headers will not be included 'alone', so they
// should not include their prerequisites.

#include "NxApexDefs.h"
#include "NxApexDesc.h"
#include "NxApexInterface.h"
#include "NxApexSDK.h"

#include "NxApexActor.h"
#include "NxApexContext.h"
#include "NxApexNameSpace.h"
#include "NxApexPhysXObjectDesc.h"
#include "NxApexRenderDataProvider.h"
#include "NxApexRenderable.h"
#include "NxApexAssetPreview.h"
#include "NxApexAsset.h"
#include "NxApexRenderContext.h"
#include "NxApexScene.h"
#include "NxApexSDKCachedData.h"
#include "NxApexUserProgress.h"
#include "NxModule.h"
#include "NxInstancedObjectSimulationAsset.h"

#include "NxApexRenderDataFormat.h"
#include "NxApexRenderBufferData.h"
#include "NxUserRenderResourceManager.h"
#include "NxUserRenderVertexBufferDesc.h"
#include "NxUserRenderInstanceBufferDesc.h"
#include "NxUserRenderSpriteBufferDesc.h"
#include "NxUserRenderIndexBufferDesc.h"
#include "NxUserRenderBoneBufferDesc.h"
#include "NxUserRenderResourceDesc.h"
#include "NxUserRenderResource.h"
#include "NxUserRenderVertexBuffer.h"
#include "NxUserRenderInstanceBuffer.h"
#include "NxUserRenderSpriteBuffer.h"
#include "NxUserRenderIndexBuffer.h"
#include "NxUserRenderBoneBuffer.h"
#include "NxUserRenderer.h"

#include "NxVertexFormat.h"
#include "NxRenderMesh.h"
#include "NxRenderMeshActorDesc.h"
#include "NxRenderMeshActor.h"
#include "NxRenderMeshAsset.h"
#include "NxResourceCallback.h"
#include "NxResourceProvider.h"

#include "NxApexStream.h"

#endif // __NX_APEX_H__

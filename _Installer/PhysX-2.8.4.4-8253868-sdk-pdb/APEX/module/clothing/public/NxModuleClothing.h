#ifndef NX_MODULE_CLOTHING_H
#define NX_MODULE_CLOTHING_H
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
#include "NxApexDesc.h"
#include "NxModule.h"

namespace physx {
namespace apex {

PX_PUSH_PACK_DEFAULT

class NxClothingAsset;
class NxClothingAssetAuthoring;
class NxClothingMaterialLibrary;
class NxClothingMaterialLibraryAuthoring;
class NxClothingPhysicalMesh;

class IProgressListener;
class NxRenderMeshAssetAuthoring;
class NxClothingIsoMesh;

/**
\mainpage APEX Clothing API Documentation

\section overview Overview

This document contains a full API documentation for all public classes.

It also contains the list of deprecated methods that will be removed in APEX 1.1
*/



/**
\brief APEX Clothing Module

Used to generate simulated clothing on (mostly humanoid) characters.
*/
class NxModuleClothing : public NxModule
{
public:
	/**
	\brief creates an empty authoring asset.

	\param [in] name Needs to be unique, there's no other clothing asset with the same name.
	*/
	virtual NxClothingAssetAuthoring* createClothingAssetAuthoring(const char* name) = 0;

	/**
	\brief Reads a clothing asset from a stream

	\param [in] stream		The input stream.
	\param [in] userData	An optional userdata pointer for the clothing asset.
	\param [in] name		A unique name for the asset.
	\param [in] progress	An optional callback for progress display.

	\return The new clothing asset or NULL
	*/
	virtual NxClothingAsset* createClothingAsset(physx::PxFileBuf& stream, void* userData, const char* name, IProgressListener* progress) = 0;

	/**
	\brief creates an empty physical mesh. A custom mesh can be assigned to it.
	*/
	virtual NxClothingPhysicalMesh* createEmptyPhysicalMesh() = 0;

	/**
	\brief creates a physical mesh based on a render mesh asset. This will be a 1 to 1 copy of the render mesh

	\param [in] asset			The render mesh that is used as source for the physical mesh
	\param [in] subdivision		Modify the physical mesh such that all edges that are longer than (bounding box diagonal / subdivision) are split up
	\param [in] mergeVertices	All vertices with the same position will be welded together.
	\param [in] closeHoles		Close any hole found in the mesh.
	\param [in] progress		An optional callback for progress display.
	*/
	virtual NxClothingPhysicalMesh* createSingleLayeredMesh(NxRenderMeshAssetAuthoring* asset, physx::PxU32 subdivision, bool mergeVertices, bool closeHoles, IProgressListener* progress) = 0;

	/**
	\brief create an iso mesh as intermediate step to generating a physical mesh

	\param [in] asset				The render mesh that is used as source for the physical mesh.
	\param [in] subdivision			The grid size (derived by bounding box diagonal / subdivision) for the iso surface.
	\param [in] keepNBiggestMeshes	When having multiple disjointed iso meshes, only keep the N largest of them. 0 means inf.
	\param [in] progress			An optional callback for progress display.
	*/
	virtual NxClothingIsoMesh* createMultiLayeredMesh(NxRenderMeshAssetAuthoring* asset, physx::PxU32 subdivision, physx::PxU32 keepNBiggestMeshes, IProgressListener* progress) = 0;

	/**
	\brief Create a clothing material library from a stream

	\param [in] stream		pointing to a stream that contains a material library
	\param [in] name		name of the material library. Pointer *must* be valid until material library is released
	\param [in] userData	optional user data
	*/
	virtual NxClothingMaterialLibrary* createMaterialLibrary(physx::PxFileBuf& stream, const char* name, void* userData) = 0;

	/**
	\brief Create a clothing material library from scratch

	\param [in] name		name of the material library. Pointer *must* be valid until material library is released
	\param [in] userData	optional user data
	*/
	virtual NxClothingMaterialLibraryAuthoring* createMaterialLibraryAuthoring(const char* name, void* userData) = 0;

	/**
	\brief The number of registered clothing materials
	*/
	virtual physx::PxU32 getNumMaterialLibraries() const = 0;

	/**
	\brief Returns a material library pointer for each index that is smaller than NxModuleClothing::getNumMaterialLibraries()
	*/
	virtual NxClothingMaterialLibrary* getMaterialLibrary(physx::PxU32 index) = 0;

	/**
	\brief The number of registered clothing material authorings
	*/
	virtual physx::PxU32 getNumMaterialLibraryAuthorings() const = 0;

	/**
	\brief Returns an authoring material library pointer for each index that is smaller than NxModuleClothing::getNumMaterialLibraries()
	*/
	virtual NxClothingMaterialLibraryAuthoring* getMaterialLibraryAuthoring(physx::PxU32 index) = 0;

protected:
	virtual ~NxModuleClothing() {}
};

#if !defined(_USRDLL)
void instantiateModuleClothing();
#endif


PX_POP_PACK

}} // namespace physx::apex

#endif

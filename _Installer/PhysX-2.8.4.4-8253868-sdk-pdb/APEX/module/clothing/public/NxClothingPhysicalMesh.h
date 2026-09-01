#ifndef NX_CLOTHING_PHYSICAL_MESH_H
#define NX_CLOTHING_PHYSICAL_MESH_H
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
#include "NxApexInterface.h"

class NxClothConstrainCoefficients;

namespace physx {
namespace apex {

PX_PUSH_PACK_DEFAULT

/**
\brief Contains the data for handing out statistics about a NxClothingPhysicalMesh
*/
struct NxClothingPhysicalMeshStats
{
	/// the number of bytes allocated for the physical mesh
	physx::PxU32	totalBytes;
	/// the number of vertices
	physx::PxU32	numVertices;
	/// the number of indices
	physx::PxU32	numIndices;
};


/**
\brief Holder for a physical mesh, this can be generated through various ways (see single- and multi-layered clothing) or hand crafted
*/
class NxClothingPhysicalMesh : public NxApexInterface
{
public:
	/**
	\brief returns the number of vertices
	*/
	virtual physx::PxU32 getNumVertices() const = 0;

	/**
	\brief returns the number of incides
	*/
	virtual physx::PxU32 getNumIndices() const = 0;

	/**
	\brief writes the indices to a destination buffer

	\param [out] indexDestination	destination buffer where to write the indices
	\param [in] byteStride			stride of the destination buffer
	\param [in] numIndices			number of indices the buffer can hold. This can be smaller than getNumIndices()
	*/
	virtual void getIndices(void* indexDestination, physx::PxU32 byteStride, physx::PxU32 numIndices) const = 0;

	/**
	\brief returns whether the mesh is built out of tetrahedra or triangles
	*/
	virtual bool isTetrahedralMesh() const = 0;

	/**
	\brief This will simplify the current mesh.

	\param [in] subdivisions	used to derive the maximal length a new edge can get.<br>
								Divide the bounding box diagonal by this value to get the maximal edge length for newly created edges<br>
								Use 0 to not restrict the maximal edge length
	\param [in] maxSteps		The maximum number of edges to be considered for simplification.<br>
								Use 0 to turn off
	\param [in] maxError		The maximal quadric error an edge can cause to be considered simplifyable.<br>
								Use any value < 0 to turn off
	\param [in] progress		Callback class that will be fired every now and then to update a progress bar in the gui.
	\return The number of edges collapsed
	*/
	virtual void simplify(physx::PxU32 subdivisions, physx::PxI32 maxSteps, physx::PxF32 maxError, IProgressListener* progress) = 0;

	/**
	\brief Create a physical mesh from scratch

	Overwrites all vertices/indices, and invalidates all misc vertex buffers. vertices must be physx::PxVec3 and indices physx::PxU32.
	*/
	virtual void setGeometry(bool tetraMesh, physx::PxU32 numVertices, physx::PxU32 vertexByteStride, const void* vertices, physx::PxU32 numIndices, physx::PxU32 indexByteStride, const void* indices) = 0;


	/**
	\brief reads flags per vertex. The only flag supported so far is NX_SOFTBODY_VERTEX_SECONDARY
	*/
	virtual void getVertexFlags(physx::PxU32* data, physx::PxU32 byteStride) = 0;

	/**
	\brief sets flags per vertex. The only flag supported so far is NX_SOFTBODY_VERTEX_SECONDARY
	*/
	virtual void setVertexFlags(const physx::PxU32* data, physx::PxU32 byteStride) = 0;


	// direct access to specific buffers
	/**
	\brief writes the indices into a user specified buffer.
	
	Returns false if the buffer doesn't exist. 
	The buffer must be bigger than sizeof(physx::PxU32) * getNumIndices()
	*/
	virtual bool getIndices(physx::PxU32* indices, physx::PxU32 byteStride) const = 0;

	/**
	\brief Writes the vertex positions into a user specified buffer.
	
	Returns false if the buffer doesn't exist. 
	The buffer must be bigger than sizeof(physx::PxVec3) * getNumVertices()
	*/
	virtual bool getVertices(physx::PxVec3* vertices, physx::PxU32 byteStride) const = 0;

	/**
	\brief Writes the normals into a user specified buffer.
	
	Returns false if the buffer doesn't exist. 
	The buffer must be bigger than sizeof(physx::PxVec3) * getNumVertices()
	*/
	virtual bool getNormals(physx::PxVec3* normals, physx::PxU32 byteStride) const = 0;

	/**
	\brief Returns the number of bones per vertex.
	*/
	virtual physx::PxU32 getNumBonesPerVertex() const = 0;

	/**
	\brief Writes the bone indices into a user specified buffer.

	Returns false if the buffer doesn't exist. 
	The buffer must be bigger than sizeof(physx::PxU16) * getNumVertices() * getNumBonesPerVertex().
	(numBonesPerVertex is the same as in the graphical mesh for LOD 0)

	The bytestride is applied only after writing numBonesPerVertex and thus must be >= sizoef(physx::PxU16) * numBonesPerVertex
	*/
	virtual bool getBoneIndices(physx::PxU16* boneIndices, physx::PxU32 byteStride) const = 0;

	/**
	\brief Writes the bone weights into a user specified buffer.

	Returns false if the buffer doesn't exist. 
	The buffer must be bigger than sizeof(physx::PxF32) * getNumVertices() * getNumBonesPerVertex().
	(numBonesPerVertex is the same as in the graphical mesh for LOD 0)
	The bytestride is applied only after writing numBonesPerVertex and thus must be >= sizoef(physx::PxF32) * numBonesPerVertex
	*/
	virtual bool getBoneWeights(physx::PxF32* boneWeights, physx::PxU32 byteStride) const = 0;

	/**
	\brief Writes the cloth constrain coefficients into a user specified buffer.
	
	Returns false if the buffer doesn't exist. The buffer must be bigger than sizeof(NxClothConstrainCoefficients) * getNumVertices().
	*/
	virtual bool getConstrainCoefficients(NxClothConstrainCoefficients* coeffs, physx::PxU32 byteStride) const = 0;

	/**
	\brief Returns stats (sizes, counts) for the asset.  See NxClothingPhysicalMeshStats.
	*/
	virtual void getStats( NxClothingPhysicalMeshStats& stats ) const = 0;

};

PX_POP_PACK

}} // namespace physx::apex

#endif // __NX_CLOTHING_PHYSICAL_MESH_H__

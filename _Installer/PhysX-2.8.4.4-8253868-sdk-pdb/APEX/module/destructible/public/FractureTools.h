#ifndef FRACTURE_TOOLS
#define FRACTURE_TOOLS
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
#include "ExplicitHierarchicalMesh.h"

PX_PUSH_PACK_DEFAULT

namespace FractureTools {

/**
	These parameters are passed into the fracturing functions to guide mesh processing.
*/
struct MeshProcessingParameters
{
	/**
		If this is true, separate mesh islands will be turned into separate chunks.
		Default = false.
	*/
	bool mIslandGeneration;

	/**
		Debug output verbosity level.  The higher the number, the more messages are output.
		Default = 0.
	*/
	int mVerbosity;

			/** Constructor sets defaults */
			MeshProcessingParameters()
			{
				setToDefault();
			}

			/** Set default values */
	void	setToDefault()
			{
				mIslandGeneration = false;
				mVerbosity = 0;
			}
};


/**
	Interace to a "cutout set," used with chippable fracturing.  A cutout set is created from a bitmap.  The
	result is turned into cutouts which are applied to the mesh.  For example, a bitmap which looks like a brick
	pattern will generate a cutout for each "brick," forming the cutout set.

	Each cutout is a 2D entity, meant to be projected onto various faces of a mesh.  They are represented
	by a set of 2D vertices, which form closed loops.  More than one loop may represent a single cutout, if
	the loops are forced to be convex.  Otherwise, a cutout is represented by a single loop.
*/
class ICutoutSet
{
public:
							/** Returns the number of cutouts in the set. */
	virtual physx::PxU32			getCutoutCount() const = 0;

							/**
								Applies to the cutout indexed by cutoutIndex:
								Returns the number of vertices in the cutout.
							*/
	virtual physx::PxU32			getCutoutVertexCount( physx::PxU32 cutoutIndex ) const = 0;

							/**
								Applies to the cutout indexed by cutoutIndex:
								Returns the number of loops in this cutout.
							*/
	virtual physx::PxU32			getCutoutLoopCount( physx::PxU32 cutoutIndex ) const = 0;

							/**
								Applies to the cutout indexed by cutoutIndex:
								Returns the vertex indexed by vertexIndex.  (Only the X and Y coordinates are used.)
							*/
	virtual const physx::PxVec3&	getCutoutVertex( physx::PxU32 cutoutIndex, physx::PxU32 vertexIndex ) const = 0;

							/**
								Applies to the cutout indexed by cutoutIndex:
								Returns the number of vertices in the loop indexed by loopIndex.
							*/
	virtual physx::PxU32			getCutoutLoopSize( physx::PxU32 coutoutIndex, physx::PxU32 loopIndex ) const = 0;

							/**
								Applies to the cutout indexed by cutoutIndex:
								Returns the vertex index of the vertex indexed by vertexNum, in the loop
								indexed by loopIndex.
							*/
	virtual physx::PxU32			getCutoutLoopVertexIndex( physx::PxU32 cutoutIndex, physx::PxU32 loopIndex, physx::PxU32 vertexNum ) const = 0;

							/**
								Applies to the cutout indexed by cutoutIndex:
								Returns the flags of the vertex indexed by vertexNum, in the loop
								indexed by loopIndex.
							*/
	virtual physx::PxU32			getCutoutLoopVertexFlags( physx::PxU32 cutoutIndex, physx::PxU32 loopIndex, physx::PxU32 vertexNum ) const = 0;

							/** Serialization */
	virtual void			serialize( physx::PxFileBuf& stream ) const = 0;
	virtual void			deserialize( physx::PxFileBuf& stream ) = 0;

							/** Releases all memory and deletes itself. */
	virtual void			release() = 0;

protected:
							/** Protected destructor.  Use the release() method. */
	virtual					~ICutoutSet() {}
};


/**
	NoiseParameters
	These parameters are used to build a splitting surface.
*/
struct NoiseParameters
{
	/**
		Size of the fluctuations, relative to mesh size
	*/
	float	amplitude;

	/**
		Noise frequencey relative to 1/(grid spacing).  On scales much smaller than this, the function is smooth.
		On scales much large, the function looks uncorrelated
	*/
	float	frequency;

	/**
		Suggested number of grid elements across the mesh.  The actual number may vary depending
		on the mesh's proportions.
	*/
	int		gridSize;

	/**
		Noise function to use.  This parameter is currently unused.
		Noise is generated by superposition of many fourier modes in random directions,
		with frequencies randomly chosen in a band around the input frequency,
	*/
	int		type;

			/** Constructor sets defaults */
			NoiseParameters()
			{
				setToDefault();
			}

			/**
				Set default values:

				amplitude = 0.0f;
				frequency = 0.25f;
				gridSize = 10;
				type = 0;
			*/
	void	setToDefault()
			{
				amplitude = 0.0f;
				frequency = 0.25f;
				gridSize = 10;
				type = 0;
			}
};


/**
	SliceParameters

	The slicing parameters for X, Y, and Z slicing of a mesh.
*/
struct SliceParameters
{
	/**
		Which axis order to slice the mesh.
		This only matters if there is randomness in the slice surface.
	*/
	enum Order
	{
		XYZ,
		YZX,
		ZXY,
		ZYX,
		YXZ,
		XZY,
		Through
	};

	/** The slicing order (see the Order enum) */
	unsigned order;

	/** How many times to slice along each axis */
	unsigned splitsPerPass[3];

	/**
		Variation in slice position along each axis.  This is a relative quantity.
		linearVariation[axis] = 0 means the slicing offsets are evenly spaced across the mesh along the axis.
		linearVariation[axis] = 1 means the slicing offsets are randomly chosen in a range of width 1/(splitsPerPass[axis]+1)
								  times the width of the mesh along the axis.
	*/
	float linearVariation[3];

	/**
		Variation in the slice surface angle along each axis.
		0 variation means the slice surfaces are axis-aligned.  Otherwise, the surface normal will be varied randomly,
		with angle to the axis somewhere within the given variation (in radians).
	*/
	float angularVariation[3];

	/** The noise for each slicing direction */
	NoiseParameters	noise[3];

			/** Constructor sets defaults */
			SliceParameters()
			{
				setToDefault();
			}

			/** Sets all NoiseParameters to their defaults:
				order = XYZ;
				splitsPerPass[0] = splitsPerPass[1] = splitsPerPass[2] = 1;
				linearVariation[0] = linearVariation[1] = linearVariation[2] = 0.1f;
				angularVariation[0] = angularVariation[1] = angularVariation[2] = 20.0f*3.1415927f/180.0f;
				noise[0].setToDefault();
				noise[1].setToDefault();
				noise[2].setToDefault();
			*/
	void	setToDefault()
			{
				order = XYZ;
				splitsPerPass[0] = splitsPerPass[1] = splitsPerPass[2] = 1;
				linearVariation[0] = linearVariation[1] = linearVariation[2] = 0.1f;
				angularVariation[0] = angularVariation[1] = angularVariation[2] = 20.0f*3.1415927f/180.0f;
				noise[0].setToDefault();
				noise[1].setToDefault();
				noise[2].setToDefault();
			}
};


/**
	FractureSliceDesc

	Descriptor for slice-mode fracturing.
*/
struct FractureSliceDesc
{
	/** How many times to recurse the slicing process */
	unsigned			maxDepth;

	/** Array of slice parameters; must be of length maxDepth */
	SliceParameters*	sliceParameters;

	/**
		If this is true, the targetProportions (see below) will be used.
	*/
	bool				useTargetProportions;

	/**
		If useTargetProportions is true, the splitsPerPass values will not necessarily be used.
		Instead, the closest values will be chosen at each recursion of slicing, in order to make
		the pieces match the target proportions as closely as possible.

		Note: the more noise there is in the slicing surfaces, the less accurate these proportions will be.
	*/
	float				targetProportions[3];

			/** Constructor sets defaults */
			FractureSliceDesc()
			{
				setToDefault();
			}

			/**
				Sets the default values:

				maxDepth = 0;
				sliceParameters = NULL;
				useTargetProportions = false;
				targetProportions[0] = targetProportions[1] = targetProportions[2] = 1.0f;
			*/
	void	setToDefault()
			{
				maxDepth = 0;
				sliceParameters = NULL;
				useTargetProportions = false;
				targetProportions[0] = targetProportions[1] = targetProportions[2] = 1.0f;
			}
};


/**
	FractureCutoutDesc

	Descriptor for cutout-mode (chippable) fracturing.
*/
struct FractureCutoutDesc
{
	/** Enum describing the directions to apply cutout fracturing. */
	enum Directions
	{
		NegativeX =	1<<0,
		PositiveX =	1<<1,
		NegativeY =	1<<2,
		PositiveY =	1<<3,
		NegativeZ =	1<<4,
		PositiveZ =	1<<5
	};

	/** The directions to apply cutout fracturing.  (See the Directions enum.) */
	unsigned directions;

	/**
		The depth to apply cutout fracturing in each direction.
		0 has a special value; it means cut all the way through
	*/
	physx::PxF32 depths[6];

	/**
		If true, non-convex cutouts will be split into convex ones.
	*/
	bool splitNonconvexRegions;

	/**
		If true, slice-mode fracturing will be applied to each cutout piece.
		The cutout function must be provided with a FractureSliceDesc as well to describe
		the slice parameters.  These parameters, however, must be interpreted from the
		point of view of the cutout direction.  That is, X and Y slice parameters will be
		used to slice along the cutout faces.  The Z slice parameters will be used to slice
		into the cutout faces.
	*/
	bool applySlicingToCutoutRegions;

	/** Scale to apply to the X coordinates of the cutout set (along the various cutout directions). */
	float cutoutWidthScale[6];

	/** Scale to apply to the Y coordinates of the cutout set (along the various cutout directions). */
	float cutoutHeightScale[6]; 

	/** Offset to apply to the X coordinates of the cutout set (along the various cutout directions). */
	float cutoutWidthOffset[6];

	/** Offset to apply to the Y coordinates of the cutout set (along the various cutout directions). */
	float cutoutHeightOffset[6];

	/** If true, the cutout map will be flipped in the X direction (along the various cutout directions). */
	bool cutoutWidthInvert[6];

	/** If true, the cutout map will be flipped in the Y direction (along the various cutout directions). */
	bool cutoutHeightInvert[6];

	/** The interpreted size of the cutout map in the X direction */
	physx::PxF32 cutoutSizeX;

	/** The interpreted size of the cutout map in the Y direction */
	physx::PxF32 cutoutSizeY;

	/**
		Threshold angle to merge (smoothe) vertex normals around cutout, in degrees.
		If the exterior angle between two facets of a cutout region no more than this, the vertex normals and tangents will be
		averaged at the facet interface.  A value of 0 effectively disables smoothing.
		Default value = 60 degrees.
	*/
	physx::PxF32 facetNormalMergeThresholdAngle;

	/**
		Describes the characteristics of the backface cutting surface.  If the noise is 0, the cutting surface
		will be a plane.  Otherwise, there will be some variation, or roughness, to the surface.
	*/
	NoiseParameters	backfaceNoise;

			/** Constructor sets defaults */
			FractureCutoutDesc()
			{
				setToDefault();
			}

			/**
				Set default values:

				directions = 0;
				depths[0..5] = 1.0f;
				cutoutWidthScale[0..5] = 1.0f;
				cutoutHeightScale[0..5] = 1.0f; 
				cutoutWidthOffset[0..5] = 0.0f;
				cutoutHeightOffset[0..5] = 0.0f;
				cutoutWidthInvert[0..5] = false;
				cutoutHeightInvert[0..5] = false;
				cutoutSizeX = 1.0f;
				cutoutSizeY = 1.0f;
				facetNormalMergeThresholdAngle = 60.0f;
				splitNonconvexRegions = false;
				applySlicingToCutoutRegions = false;
				backfaceNoise.setToDefault();
			*/
	void	setToDefault()
			{
				directions = 0;
				for( physx::PxU32 i = 0; i < 6; ++i )
				{
					depths[i] = 1.0f;
					cutoutWidthScale[i] = 1.0f;
					cutoutHeightScale[i] = 1.0f; 
					cutoutWidthOffset[i] = 0.0f;
					cutoutHeightOffset[i] = 0.0f;
					cutoutWidthInvert[i] = false;
					cutoutHeightInvert[i] = false;
				}
				cutoutSizeX = 1.0f;
				cutoutSizeY = 1.0f;
				facetNormalMergeThresholdAngle = 60.0f;
				splitNonconvexRegions = false;
				applySlicingToCutoutRegions = false;
				backfaceNoise.setToDefault();
			}
};


/**
	FractureMaterialDesc

	Descriptor for materials applied to interior faces.
*/
struct FractureMaterialDesc
{
	/** The UV scale (geometric distance/unit texture distance) for interior materials.
	Default = {1.0f,1.0f}.
	*/
	float interiorUVScale[2];

	/** Whether or not to apply the core mesh material to the surfaces of other chunks it touches.
	Default = true.
	*/
	bool applyCoreMeshMaterialToNeighborChunks;

			/** Constructor sets defaults */
			FractureMaterialDesc()
			{
				setToDefault();
			}

			/**
				Set default values:
				interiorUVScale[0] = interiorUVScale[1] = 1.0f;
				applyCoreMeshMaterialToNeighborChunks = true;
			*/
	void	setToDefault()
			{
				interiorUVScale[0] = interiorUVScale[1] = 1.0f;
				applyCoreMeshMaterialToNeighborChunks = true;
			}
};


/**
	Tools for fracturing meshes.
*/


/** Instantiates a blank ICutoutSet */
ICutoutSet* createCutoutSet();

/**
	Builds a cutout set (which must have been initially created by createCutoutSet()).
	Uses a bitmap described by pixelBuffer, bufferWidth, and bufferHeight.  Each pixel is represented
	by one byte in the buffer.

	cutoutSet: the ICutoutSet to build
	pixelBuffer: pointer to be beginning of the pixel buffer
	bufferWidth: the width of the buffer in pixels
	bufferHeight: the height of the buffer in pixels
	snapThreshold: the pixel distance at which neighboring cutout vertices and segments may be fudged into alignment.
*/
void buildCutoutSet( ICutoutSet& cutoutSet, const physx::PxU8* pixelBuffer, physx::PxU32 bufferWidth, physx::PxU32 bufferHeight, physx::PxF32 snapThreshold );

/**
	Builds a new IExplicitHierarchicalMesh from an array of triangles.

	iHMesh: the IExplicitHierarchicalMesh to build
	meshTriangles: pointer to array of NxExplicitRenderTriangles which make up the mesh
	meshTriangleCount the size of the meshTriangles array
	submeshData: pointer to array of NxExplicitSubmeshData, describing the submeshes
	submeshCount: the size of the submeshData array
	meshPartition: if not NULL, an array of size meshPartitionCount, giving the end elements of contiguous subsets of meshTriangles.
		If meshPartition is NULL, one partition is assumed.
		When there is one partition, these triangles become the level 0 part.
		When there is more than one partition, these triangles become level 1 parts, while the union of the parts will be the level 0 part.
	meshPartitionCount: if meshPartition is not NULL, this is the size of the meshPartition array.
*/
bool buildExplicitHierarchicalMesh
(
	physx::IExplicitHierarchicalMesh& iHMesh,
	const physx::NxExplicitRenderTriangle* meshTriangles,
	physx::PxU32 meshTriangleCount,
	const physx::NxExplicitSubmeshData* submeshData,
	physx::PxU32 submeshCount,
	physx::PxU32* meshPartition = NULL,
	physx::PxU32 meshPartitionCount = 0
);

/**
	Splits the mesh in part[0], forming a hierarchy of fractured meshes in parts[1...]

	hMesh: the mesh to split
	iHMeshCore: if this mesh is not empty, part 0 will be used as an indestructible "core" of the fractured
		mesh.  That is, it will be subtracted from hMesh, and placed at level 1 of the hierarchy.  The remainder
		of hMesh will be split as usual, creating parts at level 1 (and possibly deeper).
	interiorSubmeshIndex: which submesh to use for the newly created interior triangles
	meshProcessingParams: describes generic mesh processing directives
	desc: describes the slicing surfaces (see FractureSliceDesc)
	hullMethod: convex hulls will be generated for each part using the method.  See NxConvexHullMethod.
	materialDesc: material parameters, such as UV scale, core mesh material application, etc.  See FractureMaterialDesc.
	randomSeed: seed for the random number generator, to ensure reproducibility.
	progressListener: The user must instantiate an IProgressListener, so that this function may report progress of this operation
	cancel: if not NULL and *cancel is set to true, the root mesh will be restored to its original state, and the function will return at its earliest opportunity.  Meant to be set from another thread.

	returns true if successful.
*/
bool createHierarchicallySplitMesh
(
	physx::IExplicitHierarchicalMesh& hMesh,
	physx::IExplicitHierarchicalMesh& iHMeshCore,
	bool exportCoreMesh,
	physx::PxU32 interiorSubmeshIndex,
	const MeshProcessingParameters& meshProcessingParams,
	const FractureSliceDesc& desc,
	physx::NxConvexHullMethod::Enum hullMethod,
	const FractureMaterialDesc& materialDesc,
	physx::PxU32 randomSeed,
	physx::IProgressListener& progressListener,
	volatile bool* cancel = NULL
);

/**
	Chips the mesh in part[0], forming a hierarchy of fractured meshes in parts[1...]

	hMesh: the mesh to split
	interiorSubmeshIndex: which submesh to use for the newly created interior triangles
	meshProcessingParams: describes generic mesh processing directives
	desc: describes the slicing surfaces (see FractureCutoutDesc)
	iCutoutSet: the cutout set to use for fracturing (see ICutoutSet)
	sliceDesc: used if desc.applySlicingToCutoutRegions = true
	hullMethod: convex hulls will be generated for each part using the method.  See NxConvexHullMethod.
	materialDesc: material parameters, such as UV scale, core mesh material application, etc.  See FractureMaterialDesc.
	randomSeed: seed for the random number generator, to ensure reproducibility.
	progressListener: The user must instantiate an IProgressListener, so that this function may report progress of this operation
	cancel: if not NULL and *cancel is set to true, the root mesh will be restored to its original state, and the function will return at its earliest opportunity.  Meant to be set from another thread.

	returns true if successful.
*/
bool createChippedMesh
(
	physx::IExplicitHierarchicalMesh& hMesh,
	physx::PxU32 interiorSubmeshIndex,
	const MeshProcessingParameters& meshProcessingParams,
	const FractureCutoutDesc& desc,
	const ICutoutSet& iCutoutSet,
	const FractureSliceDesc& sliceDesc,
	physx::NxConvexHullMethod::Enum hullMethod,
	const FractureMaterialDesc& materialDesc,
	physx::PxU32 randomSeed,
	physx::IProgressListener& progressListener,
	volatile bool* cancel = NULL
);

PX_POP_PACK

} // namespace FractureTools

#endif // #ifndef FRACTURE_TOOLS

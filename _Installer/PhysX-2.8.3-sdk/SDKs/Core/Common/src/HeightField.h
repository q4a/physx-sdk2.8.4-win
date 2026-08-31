#ifndef NX_COLLISION_HEIGHTFIELD
#define NX_COLLISION_HEIGHTFIELD
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Allocateable.h"
#include "NvHeightField.h"
#include "NxBounds3.h"
#include "NxSphere.h"
#include "NxTriangle.h"
#include "NxHeightFieldSample.h"

#define HAS_HEIGHTFIELD_UPDATEBLOCK 1

class NpHeightField;
class NxHeightFieldDesc;

class HeightField: public NvHeightField, public NxFoundation::NxAllocateable
	{
	public:
											HeightField();
											~HeightField();

		// NvHeightField
	virtual		void						release();
	virtual		bool						saveToDesc(NxHeightFieldDesc&)	const;
	virtual		bool						loadFromDesc(const NxHeightFieldDesc&);
	virtual		NxU32						saveCells(void * destBuffer, NxU32 destBufferSize) const;
	virtual		NpHeightField*				getNpHeightField()						{ return mNpHeightField;	}
	virtual		void						setNpHeightField(NpHeightField* hf)		{ mNpHeightField = hf;		}

	virtual		NxU32						getNbRowsV()					const	{ return getNbRows(); }
	virtual		NxU32						getNbColumnsV()					const	{ return getNbColumns(); }
	virtual		NxHeightFieldFormat			getFormatV()					const	{ return getFormat(); }
	virtual		NxU32						getSampleStrideV()				const	{ return getSampleStride(); }
	virtual		NxReal						getVerticalExtentV()			const	{ return getVerticalExtent(); }
	virtual		NxReal						getThicknessV()					const	{ return getThickness(); }
	virtual		NxReal						getConvexEdgeThresholdV()		const	{ return getConvexEdgeThreshold(); }
	virtual		NxU32						getFlagsV()						const	{ return getFlags(); }
	virtual		NxReal						getHeightV(NxReal x, NxReal z)	const	{ return (x < 0 || z < 0 || x > nbRows-1 || z > nbColumns-1) ? 0 : getHeight(x, z); }
	virtual		const void*					getCellsV()						const	{ return samples; }
	virtual		void						updateBlock(int xBase, int yBase, int width, int height, int rowStride, void* data);
	// ~NvHeightField

	NX_INLINE	NxU32						getNbRows()						const	{ return nbRows; }
	NX_INLINE	NxU32						getNbColumns()					const	{ return nbColumns; }
	NX_INLINE	NxHeightFieldFormat			getFormat()						const	{ return format; }
	NX_INLINE	NxU32						getSampleStride()				const	{ return sampleStride; }
	NX_INLINE	NxReal						getThickness()					const	{ return thickness; }
	NX_INLINE	NxReal						getVerticalExtent()				const	{ return verticalExtent; }
	NX_INLINE	NxReal						getConvexEdgeThreshold()		const	{ return convexEdgeThreshold; }
	NX_INLINE	NxU32						getFlags()						const	{ return flags; }

	NX_INLINE	bool						isDeltaHeightInsideExtent(NxReal dy, NxReal eps = 0) const;
	NX_INLINE	bool						isDeltaHeightOppositeExtent(NxReal dy) const;

	NX_INLINE	bool						isZerothVertexShared(NxU32 vertexIndex) const;
	NX_INLINE	NxMaterialIndex				getMaterialIndex0(NxU32 vertexIndex) const;
	NX_INLINE	NxMaterialIndex				getMaterialIndex1(NxU32 vertexIndex) const;
	NX_INLINE	NxReal						getHeight(NxU32 vertexIndex) const;

	NX_INLINE	bool						isValidVertex(NxU32 vertexIndex) const;
	NX_INLINE	NxVec3						getVertex(NxU32 vertexIndex) const;
	NX_INLINE	void						getVertexEdgeIndices(NxU32 vertexIndex, NxU32 edgeIndices[8], NxU32& count) const;
	NX_INLINE	bool						isConvexVertex(NxU32 vertexIndex) const;

	NX_INLINE	bool						isValidEdge(NxU32 edgeIndex) const;
	NX_INLINE	void						getEdgeTriangleIndices(NxU32 edgeIndex, NxU32 triangleIndices[2], NxU32& count) const;
	NX_INLINE	void						getEdgeVertexIndices(NxU32 edgeIndex, NxU32& vertexIndex0, NxU32& vertexIndex1) const;
	NX_INLINE	bool						isConvexEdge(NxU32 edgeIndex) const;

	NX_INLINE	void						computeCellCoordinates(NxReal x, NxReal z, NxU32& row, NxU32& column, NxReal& fracX, NxReal& fracZ) const;
	NX_INLINE	NxU32						getMinRow(NxReal x) const;
	NX_INLINE	NxU32						getMaxRow(NxReal x) const;
	NX_INLINE	NxU32						getMinColumn(NxReal z) const;
	NX_INLINE	NxU32						getMaxColumn(NxReal z) const;

	NX_INLINE	bool						isValidTriangle(NxU32 triangleIndex) const;
	NX_INLINE	bool						isFirstTriangle(NxU32 triangleIndex) const;
	NX_INLINE	NxMaterialIndex				getTriangleMaterial(NxU32 triangleIndex) const;
	NX_INLINE	void						getTriangleVertexIndices(NxU32 triangleIndex, NxU32& vertexIndex0, NxU32& vertexIndex1, NxU32& vertexIndex2) const;
	NX_INLINE	void						getTriangleEdgeIndices(NxU32 triangleIndex, NxU32& edgeIndex0, NxU32& edgeIndex1, NxU32& edgeIndex2) const;
	NX_INLINE	NxVec3						getTriangleNormal(NxU32 triangleIndex) const;

	NX_INLINE	NxReal						getHeight(NxReal x, NxReal z) const;
	NX_INLINE	NxVec3						getNormal(NxReal x, NxReal z) const;

	NX_INLINE	NxU32						getTriangleIndex(NxReal x, NxReal z) const;
	NX_INLINE	NxMaterialIndex				getMaterial(NxReal x, NxReal z) const;

	NX_INLINE	NxReal						getMinHeight()					const	{ return minHeight; }
	NX_INLINE	NxReal						getMaxHeight()					const	{ return maxHeight; }

	NX_INLINE	NxMaterialIndex				getCommonMaterialIndex0()		const	{ return commonMaterialIndex0; }
	NX_INLINE	NxMaterialIndex				getCommonMaterialIndex1()		const	{ return commonMaterialIndex1; }

	static		const NxU32					MATERIAL_MASK = 0x7f; // seven bit materials
	
	protected:

				//proxy
				NpHeightField*				mNpHeightField;
				
				//properties
				NxU32						nbRows;
				NxU32						nbColumns;
				NxHeightFieldFormat			format;
				NxU32						sampleStride;
				void *						samples;						
				NxReal						verticalExtent;
				NxReal						thickness;
				NxReal						convexEdgeThreshold;
				NxU32						flags;

				NxReal						minHeight;
				NxReal						maxHeight;

				NxMaterialIndex				commonMaterialIndex0;
				NxMaterialIndex				commonMaterialIndex1;

				// methods
				void						releaseMemory();

	NX_INLINE	void *						byteOffset(NxU32 offset) { return (void*)(((NxU8*)samples) + offset); }
	NX_INLINE	const void*					byteOffset(NxU32 offset) const { return (const void*)(((NxU8*)samples) + offset); }

	NX_INLINE	const NxHeightFieldSample&	getSample(NxU32 vertexIndex) const;
};

NX_INLINE const NxHeightFieldSample& HeightField::getSample(NxU32 vertexIndex) const
	{
	NX_ASSERT(isValidVertex(vertexIndex));
	return *(const NxHeightFieldSample*)byteOffset(vertexIndex*sampleStride);
	}

NX_INLINE bool HeightField::isValidVertex(NxU32 vertexIndex) const
	{
	return vertexIndex < nbRows*nbColumns;
	}

NX_INLINE bool HeightField::isDeltaHeightInsideExtent(NxReal dy, NxReal eps) const	
	{ 
	return (thickness!=0)
		? (thickness <= 0 && dy <= eps && dy >= thickness) || (thickness > 0 && dy > -eps && dy < thickness) 
		: (verticalExtent <= 0 && dy <= eps) || (verticalExtent > 0 && dy > -eps);
	}

NX_INLINE bool HeightField::isDeltaHeightOppositeExtent(NxReal dy) const	
	{ 
	return (thickness != 0)
		? (thickness <= 0 && dy > 0) || (thickness > 0 && dy < 0) 
		: (verticalExtent <= 0 && dy > 0) || (verticalExtent > 0 && dy < 0);
	}

NX_INLINE bool HeightField::isZerothVertexShared(NxU32 vertexIndex) const
	{
	return (getSample(vertexIndex).tessFlag & NX_HF_0TH_VERTEX_SHARED);
	}

NX_INLINE NxReal HeightField::getHeight(NxU32 vertexIndex) const
	{
	return NxReal(getSample(vertexIndex).height);
	}

NX_INLINE NxMaterialIndex HeightField::getMaterialIndex0(NxU32 vertexIndex) const
	{
	return getSample(vertexIndex).materialIndex0;
	}

NX_INLINE NxMaterialIndex HeightField::getMaterialIndex1(NxU32 vertexIndex) const
	{
	return getSample(vertexIndex).materialIndex1;
	}

NX_INLINE NxVec3 HeightField::getVertex(NxU32 vertexIndex) const
	{
	NxU32 row    = vertexIndex / nbColumns;
	NxU32 column = vertexIndex % nbColumns;
	return NxVec3(NxReal(row), getHeight(row * nbColumns + column), NxReal(column));
	}

NX_INLINE void HeightField::getVertexEdgeIndices(NxU32 vertexIndex, NxU32 edgeIndices[8], NxU32& count) const
	{
	NxU32 row    = vertexIndex / nbColumns;
	NxU32 column = vertexIndex % nbColumns;
	count = 0;
	
	if (row > 0) 
		{
		/*
		//   COL -->
		//       
		// R     |
		// O     |
		// W     +  
		// |      
		// V  
		*/
		edgeIndices[count++] = 3 * (vertexIndex - nbColumns) + 2;
		}
	
	if (column < nbColumns-1)
		{
		/*
		//   COL -->
		//       
		// R       /
		// O      / 
		// W     +--
		// |      \
		// V       \ 
		*/
		if (row > 0)
			{
			/*
			//   COL -->
			//       
			// R       /
			// O      / 
			// W     +
			// |       
			// V
			*/
			if (!isZerothVertexShared(vertexIndex - nbColumns))
				{
				edgeIndices[count++] = 3 * (vertexIndex - nbColumns) + 1;
				}
			}
		/*
		//   COL -->
		//       
		// R     
		// O     
		// W     +--
		// |       
		// V         
		*/
		edgeIndices[count++] = 3 * vertexIndex;
		if (row < nbRows - 1)
			{
			/*
			//   COL -->
			//       
			// R     
			// O     
			// W     +  
			// |      \
			// V       \ 
			*/
			if (isZerothVertexShared(vertexIndex))
				{
				edgeIndices[count++] = 3 * vertexIndex + 1;
				}
			}
		}
		/*
		//   COL -->
		//       
		// R     
		// O     
		// W     +  
		// |     |
		// V     | 
		*/
		if (row < nbRows - 1)
			{
			edgeIndices[count++] = 3 * vertexIndex + 2;
			}
	if (column > 0)
		{
		/*
		//   COL -->
		//        
		// R   \  
		// O    \ 
		// W   --+
		// |    / 
		// V   /   
		*/
		if (row < nbRows - 1)
			{
			if (!isZerothVertexShared(vertexIndex - 1))
				{
				edgeIndices[count++] = 3 * (vertexIndex - 1) + 1;
				}
			}
		edgeIndices[count++] = 3 * (vertexIndex - 1);
		if (row > 0)
			{
			if (isZerothVertexShared(vertexIndex - nbColumns - 1))
				{
				edgeIndices[count++] = 3 * (vertexIndex - nbColumns - 1) + 1;
				}
			}
		}
	}

NX_INLINE bool HeightField::isConvexVertex(NxU32 vertexIndex) const
	{
#ifdef NX_HEIGHTFIELD_DEBUG
	NX_ASSERT(heightField->isValidVertex(vertexIndex));
#endif
	NxU32 row    = vertexIndex / nbColumns;
	NxU32 column = vertexIndex % nbColumns;
		
	NxReal definedInX = false;
	NxReal definedInZ = false;

	NxReal convexityX = 0;
	NxReal convexityZ = 0;

	NxReal h0 = NxReal(2) * getHeight(vertexIndex);

	if ((row > 0) &&  (row < nbRows - 1))
		{
		convexityX = h0 - getHeight(vertexIndex + nbColumns) - getHeight(vertexIndex - nbColumns);
		definedInX = true;
		}
	if ((column > 0) &&  (column < nbColumns - 1))
		{
		convexityZ = h0 - getHeight(vertexIndex + 1) - getHeight(vertexIndex - 1);
		definedInZ = true;
		}

	if (definedInX || definedInZ)
		{
		// saddle points
		if ((convexityX > 0) && (convexityZ < 0)) 
			return false;		
		if ((convexityX < 0) && (convexityZ > 0)) 
			return false;
		// inequality depends on vertical extent and offset by threshold.
		if (getVerticalExtent() <= 0 && getThickness() <= 0) 
			return (convexityX + convexityZ) > convexEdgeThreshold;
		else 
			return (convexityX + convexityZ) < -convexEdgeThreshold;
		}
	
	// this has to be one of the two corner vertices
	return true;
	}

NX_INLINE bool HeightField::isValidEdge(NxU32 edgeIndex) const
	{
	NxU32 cell   = (edgeIndex / 3);
	NxU32 row    = cell / nbColumns;
	NxU32 column = cell % nbColumns;
	switch (edgeIndex % 3)
		{
		case 0:
			if (row > nbRows - 1) return false;
			if (column >= nbColumns - 1) return false;
			break;
		case 1:
			if (row >= nbRows - 1) return false;
			if (column >= nbColumns - 1) return false;
			break;
		case 2:
			if (row >= nbRows - 1) return false;
			if (column > nbColumns - 1) return false;
			break;
		}
	return true;
	}

NX_INLINE void HeightField::getEdgeTriangleIndices(NxU32 edgeIndex, NxU32 triangleIndices[2], NxU32& count) const
	{
	NxU32 cell   = edgeIndex / 3;
	NxU32 row    = cell / nbColumns;
	NxU32 column = cell % nbColumns;
	count = 0;
	switch (edgeIndex % 3)
		{
		case 0:
			if (column < nbColumns - 1)
				{
				if (row > 0)
					{
					if (isZerothVertexShared(cell - nbColumns))
						triangleIndices[count++] = ((cell - nbColumns) << 1);
					else 
						triangleIndices[count++] = ((cell - nbColumns) << 1) + 1;
					}
				if (row < nbRows - 1)
					{
					if (isZerothVertexShared(cell))
						triangleIndices[count++] = (cell << 1) + 1;
					else 
						triangleIndices[count++] = cell << 1;
					}
				}
			break;
		case 1:
			if ((row < nbRows - 1) && (column < nbColumns - 1))
				{
				triangleIndices[count++] = cell << 1;
				triangleIndices[count++] = (cell << 1) + 1;
				}
			break;
		case 2:
			if (row < nbRows - 1)
				{
				if (column > 0)
					triangleIndices[count++] = ((cell - 1) << 1) + 1;
				if (column < nbColumns - 1)
					triangleIndices[count++] = cell << 1;
				}
			break;
		}
	}

NX_INLINE void HeightField::getEdgeVertexIndices(NxU32 edgeIndex, NxU32& vertexIndex0, NxU32& vertexIndex1) const
	{
	NxU32 cell = edgeIndex / 3;
	switch (edgeIndex % 3)
		{
		case 0:
			vertexIndex0 = cell;
			vertexIndex1 = cell + 1;
			break;
		case 1:
			if (isZerothVertexShared(cell))
				{
				vertexIndex0 = cell;
				vertexIndex1 = cell + nbColumns + 1;
				}
			else
				{
				vertexIndex0 = cell + 1;
				vertexIndex1 = cell + nbColumns;
				}
			break;
		case 2:
			vertexIndex0 = cell;
			vertexIndex1 = cell + nbColumns;
			break;
		}
	}

NX_INLINE bool HeightField::isConvexEdge(NxU32 edgeIndex) const
	{
	NxU32 cell = edgeIndex / 3;

	NxU32 row = cell / nbColumns;
	if (row > nbRows-2) return false;

	NxU32 column = cell % nbColumns;
	if (column > nbColumns-2) return false;

	NxReal h0 = 0, h1 = 0, h2 = 0, h3 = 0;
	NxReal convexity = 0;

	switch (edgeIndex % 3)
		{
		case 0:
			if (row < 1) return false;
			if(isZerothVertexShared(cell - nbColumns)) 
				{
				//      <------ COL  
				//       +----+  0  R
				//       |   /  /#  O
				//       |  /  / #  W
				//       | /  /  #  |
				//       |/  /   #  |
				//       +  +====1  |
				//                  |
				//                  |
				//                  |
				//                  |
				//                  |
				//                  |
				//                  V
				//      
				h0 = getHeight(cell - nbColumns);
				h1 = getHeight(cell);
				}
			else
				{
				//      <------ COL  
				//       0  +----+  R
				//       #\  \   |  O
				//       # \  \  |  W
				//       #  \  \ |  |
				//       #   \  \|  |
				//       1====+  +  |
				//                  |
				//                  |
				//                  |
				//                  |
				//                  |
				//                  |
				//                  V
				//      
				h0 = getHeight(cell - nbColumns + 1);
				h1 = getHeight(cell + 1);
				}
			if(isZerothVertexShared(cell)) 
				{
				//      <------ COL  
				//                  R
				//                  O
				//                  W
				//                  |
				//                  |
				//                  |
				//       2====+  0  |
				//       #   /  /|  |
				//       #  /  / |  |
				//       # /  /  |  |
				//       #/  /   |  |
				//       3  +----+  |
				//                  V
				//      
				h2 = getHeight(cell + 1);
				h3 = getHeight(cell + nbColumns + 1);
				}
			else
				{
				//      <------ COL  
				//                  R
				//                  O
				//                  W
				//                  |
				//                  |
				//                  |
				//       +  +====2  |
				//       |\  \   #  |
				//       | \  \  #  |
				//       |  \  \ #  |
				//       |   \  \#  |
				//       +----+  3  |
				//                  V
				//      
				h2 = getHeight(cell);
				h3 = getHeight(cell + nbColumns);
				}
			//convex = (h3-h2) < (h1-h0);
			convexity = (h1-h0) - (h3-h2);
			break;
		case 1:
			h0 = getHeight(cell);
			h1 = getHeight(cell + 1);
			h2 = getHeight(cell + nbColumns);
			h3 = getHeight(cell + nbColumns + 1);
			if (isZerothVertexShared(cell))
				//convex = (h0 + h3) > (h1 + h2);
				convexity = (h0 + h3) - (h1 + h2);
			else 
				//convex = (h2 + h1) > (h0 + h3);
				convexity = (h2 + h1) - (h0 + h3);
			break;
		case 2:
			if (column < 1) return false;
			if(isZerothVertexShared(cell-1)) 
				{
				//      <-------------- COL  
				//                1====0  + R
				//                +   /  /| O
				//                +  /  / | W
				//                + /  /  | |
				//                +/  /   | |
				//                +  +----+ V
				//      
				h0 = getHeight(cell - 1);
				h1 = getHeight(cell);
				}
			else
				{
				//      <-------------- COL  
				//                +  +----+ R
				//                +\  \   | O
				//                + \  \  | W
				//                +  \  \ | |
				//                +   \  \| |
				//                1====0  + V
				//      
				h0 = getHeight(cell - 1 + nbColumns);
				h1 = getHeight(cell + nbColumns);
				}
			if(isZerothVertexShared(cell)) 
				{
				//      <-------------- COL  
				//       +----+  +          R
				//       |   /  /+          O
				//       |  /  / +          W
				//       | /  /  +          |
				//       |/  /   +          |
				//       +  3====2          V
				//      
				h2 = getHeight(cell + nbColumns);
				h3 = getHeight(cell + nbColumns + 1);
				}
			else
				{
				//      <-------------- COL  
				//       +  3====2          R
				//       |\  \   +          O
				//       | \  \  +          W
				//       |  \  \ +          |
				//       |   \  \+          |
				//       +----+  +          V
				//      
				h2 = getHeight(cell);
				h3 = getHeight(cell + 1);
				}
			//convex = (h3-h2) < (h1-h0);
			convexity = (h1-h0) - (h3-h2);
			break;
		}

		if (verticalExtent <= 0 && thickness <= 0)
			{
			return convexity > convexEdgeThreshold;
			}
		else
			{
			return convexity < -convexEdgeThreshold;
			}
	}

NX_INLINE void HeightField::computeCellCoordinates(NxReal x, NxReal z, NxU32& row, NxU32& column, NxReal& fracX, NxReal& fracZ) const
	{
	// ptchernev: 
	// Had to remove this assert since it may happen sometimes due to float precision (or lack thereof).
	// When HeightFieldShape::raycast() clips the ray the clipped points may end up ever so slightly on
	// the wrong side of the height field bounds. I could have fixed the problem there but I could only 
	// guarantee that the points where inside the shape space bounds. Something may still have gone wrong 
	// when transforming to height field grid space.
	// NX_ASSERT(!((x < 0) || (z < 0) || (x > nbRows - 1) || (z > nbColumns - 1)));
	
	// ptchernev:
	// Apparently there is no standard for float to int conversion of negative values (cause that would make sense).
	if (x < 0) x = 0;
	if (z < 0) z = 0;
	
	row = (NxU32)x;
	fracX = x - NxReal(row);
	if (row > nbRows - 2) 
		{
		row = nbRows - 2;
		fracX = NxReal(1);
		}

	column = (NxU32)z;
	fracZ = z - NxReal(column);
	if (column > nbColumns - 2) 
		{
		column = nbColumns - 2;
		fracZ = NxReal(1);
		}
	}

NX_INLINE NxU32 HeightField::getMinRow(NxReal x) const
	{
	return NxMath::clamp(NxI32(NxMath::floor(x)), NxI32(nbRows-2), NxI32(0));
	}

NX_INLINE NxU32 HeightField::getMaxRow(NxReal x) const
	{
	return NxMath::clamp(NxI32(NxMath::ceil(x)), NxI32(nbRows-1), NxI32(0));
	}

NX_INLINE NxU32 HeightField::getMinColumn(NxReal z) const
	{
	return NxMath::clamp(NxI32(NxMath::floor(z)), NxI32(nbColumns-2), NxI32(0));
	}

NX_INLINE NxU32 HeightField::getMaxColumn(NxReal z) const
	{
	return NxMath::clamp(NxI32(NxMath::ceil(z)), NxI32(nbColumns-1), NxI32(0));
	}

NX_INLINE bool HeightField::isValidTriangle(NxU32 triangleIndex) const
	{
	NxU32 cell = triangleIndex >> 1;
	NxU32 row  = cell / nbColumns;
	if (row >= (nbRows - 1)) return false;
	NxU32 column = cell % nbColumns;
	if (column >= (nbColumns - 1)) return false;
	return true;
	}

NX_INLINE bool HeightField::isFirstTriangle(NxU32 triangleIndex) const
	{
	return ((triangleIndex & 0x1) == 0);
	}

NX_INLINE NxMaterialIndex HeightField::getTriangleMaterial(NxU32 triangleIndex) const
	{
	return isFirstTriangle(triangleIndex) ? getMaterialIndex0(triangleIndex >> 1) : getMaterialIndex1(triangleIndex >> 1);
	}

NX_INLINE void HeightField::getTriangleVertexIndices(NxU32 triangleIndex, NxU32& vertexIndex0, NxU32& vertexIndex1, NxU32& vertexIndex2) const
	{
	NxU32 cell = triangleIndex >> 1;
	if (isZerothVertexShared(cell))
		{
		//      <---- COL  
		//      0----2  1 R
		//      | 1 /  /| O
		//      |  /  / | W
		//      | /  /  | |
		//      |/  / 0 | |
		//      1  2----0 V
		//      
		if (isFirstTriangle(triangleIndex))
			{
			vertexIndex0 = cell + nbColumns;
			vertexIndex1 = cell;
			vertexIndex2 = cell + nbColumns + 1;
			}
		else
			{
			vertexIndex0 = cell + 1;
			vertexIndex1 = cell + nbColumns + 1;
			vertexIndex2 = cell;
			}
		}
	else
		{
		//      <---- COL  
		//      2  1----0 R
		//      |\  \ 0 | O
		//      | \  \  | W
		//      |  \  \ | |
		//      | 1 \  \| |
		//      0----1  2 V
		//                   
		if (isFirstTriangle(triangleIndex))
			{
			vertexIndex0 = cell;
			vertexIndex1 = cell + 1;
			vertexIndex2 = cell + nbColumns;
			}
		else
			{
			vertexIndex0 = cell + nbColumns + 1;
			vertexIndex1 = cell + nbColumns;
			vertexIndex2 = cell + 1;
			}
		}
	}

NX_INLINE void HeightField::getTriangleEdgeIndices(NxU32 triangleIndex, NxU32& edgeIndex0, NxU32& edgeIndex1, NxU32& edgeIndex2) const
	{
	NxU32 cell = triangleIndex >> 1;
	NxU32 edge = 3 * cell;
	//      <-- COL  
	//      +--0--+ R
	//      |:   :| O
	//      | : : | W
	//      |  1  2 |
	//      | : : | |
	//      |:   :| |
	//      +-----+ V
	//      
	if (isZerothVertexShared(cell))
		{
		//      <---- COL  
		//      +-2--+  + R
		//      |   /  /| O
		//      0  1  / | W
		//      | /  1  0 |
		//      |/  /   | |
		//      +  +--2-+ V
		//      
		if (isFirstTriangle(triangleIndex))
			{
			edgeIndex0 = edge + 2;
			edgeIndex1 = edge + 1;
			edgeIndex2 = edge + 3*nbColumns;
			}
		else
			{
			edgeIndex0 = edge + 3 + 2;
			edgeIndex1 = edge + 1;
			edgeIndex2 = edge;
			}
		}
	else
		{
		//      <---- COL  
		//      +  +--0-+ R
		//      |\  \   | O
		//      | \  1  2 W
		//      2  1  \ | |
		//      |   \  \| |
		//      +-0--+  + V
		//      
		if (isFirstTriangle(triangleIndex))
			{
			edgeIndex0 = edge;
			edgeIndex1 = edge + 1;
			edgeIndex2 = edge + 2;
			}
		else
			{
			edgeIndex0 = edge + 3*nbColumns;
			edgeIndex1 = edge + 1;
			edgeIndex2 = edge + 3 + 2;
			}
		}
	}

NX_INLINE NxVec3 HeightField::getTriangleNormal(NxU32 triangleIndex) const
	{
	NxU32 v0;
	NxU32 v1;
	NxU32 v2;
	getTriangleVertexIndices(triangleIndex, v0, v1, v2); 
	NxReal h0 = getHeight(v0);
	NxReal h1 = getHeight(v1);
	NxReal h2 = getHeight(v2);
	NxVec3 n(0,1,0);
	NxU32 cell = triangleIndex >> 1;
	if (isZerothVertexShared(cell))
		{
		//      <---- COL  
		//      0----2  1 R
		//      | 1 /  /| O
		//      |  /  / | W
		//      | /  /  | |
		//      |/  / 0 | |
		//      1  2----0 V
		//      
		if (isFirstTriangle(triangleIndex))
			{
			n.x = -(h0-h1);
			n.z = -(h2-h0);
			}
		else
			{
			n.x = -(h1-h0);
			n.z = -(h0-h2);
			}
		}
	else
		{
		//      <---- COL  
		//      2  1----0 R
		//      |\  \ 0 | O
		//      | \  \  | W
		//      |  \  \ | |
		//      | 1 \  \| |
		//      0----1  2 V
		//                   
		if (isFirstTriangle(triangleIndex))
			{
			n.x = -(h2-h0);
			n.z = -(h1-h0);
			}
		else
			{
			n.x = -(h0-h2);
			n.z = -(h0-h1);
			}
		}
	return n;
	}

NX_INLINE NxReal HeightField::getHeight(NxReal x, NxReal z) const
	{
	NxU32 row, column;
	NxReal fracX, fracZ;
	computeCellCoordinates(x, z, row, column, fracX, fracZ);
	NxU32 vertexIndex = row * nbColumns + column;
	if (isZerothVertexShared(vertexIndex))
		{
		//    <----Z---+
		//      +----+ | 
		//      |   /| |
		//      |  / | X
		//      | /  | |
		//      |/   | |
		//      +----+ |
		//             V
		if (fracZ > fracX)
			{
			//    <----Z---+
			//      1----0 | 
			//      |   /  |
			//      |  /   X
			//      | /    |
			//      |/     |
			//      2      |
			//             V
			NxReal h0 = getHeight(vertexIndex);
			NxReal h1 = getHeight(vertexIndex + 1);
			NxReal h2 = getHeight(vertexIndex + nbColumns + 1);
			return h0 + fracZ*(h1-h0) + fracX*(h2-h1);
			}
		else
			{
			//    <----Z---+
			//           0 | 
			//          /| |
			//         / | X
			//        /  | |
			//       /   | |
			//      2----1 |
			//             V
			NxReal h0 = getHeight(vertexIndex);
			NxReal h1 = getHeight(vertexIndex + nbColumns);
			NxReal h2 = getHeight(vertexIndex + nbColumns + 1);
			return h0 + fracX*(h1-h0) + fracZ*(h2-h1);
			}
		}
	else
		{
		//    <----Z---+
		//      +----+ | 
		//      |\   | |
		//      | \  | X
		//      |  \ | |
		//      |   \| |
		//      +----+ |
		//             V
		if (fracX + fracZ < 1)
			{
			//    <----Z---+
			//      1----0 | 
			//       \   | |
			//        \  | X
			//         \ | |
			//          \| |
			//           2 |
			//             V
			NxReal h0 = getHeight(vertexIndex);
			NxReal h1 = getHeight(vertexIndex + 1);
			NxReal h2 = getHeight(vertexIndex + nbColumns);
			return h0 + fracZ*(h1-h0) + fracX*(h2-h0);
			}
		else
			{
			//    <----Z---+
			//      2      | 
			//      |\     |
			//      | \    X
			//      |  \   |
			//      |   \  |
			//      0----1 |
			//             V
			//
			// Note that we need to flip fracX and fracZ since we are moving the origin
			NxReal h2 = getHeight(vertexIndex + 1);
			NxReal h1 = getHeight(vertexIndex + nbColumns);
			NxReal h0 = getHeight(vertexIndex + nbColumns + 1);
			return h0 + (1-fracZ)*(h1-h0) + (1-fracX)*(h2-h0);
			}
		}
	}
		
NX_INLINE NxVec3 HeightField::getNormal(NxReal x, NxReal z) const
	{
	NxU32 row, column;
	NxReal fracX, fracZ;
	computeCellCoordinates(x, z, row, column, fracX, fracZ);

	NxVec3 normal;
	NxU32 vertexIndex = row * nbColumns + column;
	if (isZerothVertexShared(vertexIndex))
		{
		//    <----Z---+
		//      +----+ | 
		//      |   /| |
		//      |  / | X
		//      | /  | |
		//      |/   | |
		//      +----+ |
		//             V
		if (fracZ >= fracX)
			{
			//    <----Z---+
			//      1----0 | 
			//      |   /  |
			//      |  /   X
			//      | /    |
			//      |/     |
			//      2      |
			//             V
			NxReal h0 = getHeight(vertexIndex);
			NxReal h1 = getHeight(vertexIndex + 1);
			NxReal h2 = getHeight(vertexIndex + nbColumns + 1);
			normal.set(-(h2-h1),1,-(h1-h0));
			}
		else
			{
			//    <----Z---+
			//           0 | 
			//          /| |
			//         / | X
			//        /  | |
			//       /   | |
			//      2----1 |
			//             V
			NxReal h0 = getHeight(vertexIndex);
			NxReal h1 = getHeight(vertexIndex + nbColumns);
			NxReal h2 = getHeight(vertexIndex + nbColumns + 1);
			normal.set(-(h1-h0),1,-(h2-h1));
			}
		}
	else
		{
		//    <----Z---+
		//      +----+ | 
		//      |\   | |
		//      | \  | X
		//      |  \ | |
		//      |   \| |
		//      +----+ |
		//             V
		if (fracX + fracZ <= NxReal(1))
			{
			//    <----Z---+
			//      1----0 | 
			//       \   | |
			//        \  | X
			//         \ | |
			//          \| |
			//           2 |
			//             V
			NxReal h0 = getHeight(vertexIndex);
			NxReal h1 = getHeight(vertexIndex + 1);
			NxReal h2 = getHeight(vertexIndex + nbColumns);
			normal.set(-(h2-h0),1,-(h1-h0));
			}
		else
			{
			//    <----Z---+
			//      2      | 
			//      |\     |
			//      | \    X
			//      |  \   |
			//      |   \  |
			//      0----1 |
			//             V
			//
			// Note that we need to flip fracX and fracZ since we are moving the origin
			NxReal h2 = getHeight(vertexIndex + 1);
			NxReal h1 = getHeight(vertexIndex + nbColumns);
			NxReal h0 = getHeight(vertexIndex + nbColumns + 1);
			normal.set(-(h0-h2),1,-(h0-h1));
			}
		}

	return (verticalExtent <= 0 && thickness <= 0) ? normal : -normal;
	}

NX_INLINE NxU32 HeightField::getTriangleIndex(NxReal x, NxReal z) const
	{
	NxU32 row, column;
	NxReal fracX, fracZ;
	computeCellCoordinates(x, z, row, column, fracX, fracZ);
	NxU32 cell = row * nbColumns + column;
	if (isZerothVertexShared(cell))
		return (fracZ > fracX) ? (cell << 1) + 1 : (cell << 1);
	else
		return (fracX + fracZ > 1) ? (cell << 1) + 1 : (cell << 1);
	}

NX_INLINE NxMaterialIndex HeightField::getMaterial(NxReal x, NxReal z) const
	{
	return getTriangleMaterial(getTriangleIndex(x, z));
	}

#endif

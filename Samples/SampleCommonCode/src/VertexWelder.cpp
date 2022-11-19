#include "VertexWelder.h"

#include <vector>
#include <algorithm>

#if defined WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX		//suppress windows' global min,max macros.
#include <windows.h>	//needed for gl.h
#include <gl/gl.h>
#endif


VertexWelder::VertexWelder(NxReal epsilon)
  : mEpsilon(epsilon), mMapping(NULL), mMappingSize(0), mMappingDomain(0), mNewFaces32(NULL), mNewFaces16(NULL), mUsed(false),
    mWriteVerticesPtr(NULL), mWriteVerticesStride(0), mWriteNormalsPtr(NULL), mWriteNormalsStride(0),
    mWriteUserDataPtr(NULL), mWriteUserDataStride(0), mWriteUserDataSize(0), mWriteIndicesPtr(NULL), mWriteIndicesStride(0)
{
}

// -----------------------------------------------------------------------
VertexWelder::~VertexWelder()
{
	free(mMapping);
	free(mNewFaces16);
	free(mNewFaces32);
}

// -----------------------------------------------------------------------
void VertexWelder::initialize(const NxClothMeshDesc& unweldedMesh)
{
	NxArray<NxU32> mapping;
	NxReal squaredEpsilon = mEpsilon * mEpsilon;
	for (NxU32 i = 0; i < unweldedMesh.numVertices; i++)
	{
		const NxVec3& curVec = *(const NxVec3*)(((const char*)unweldedMesh.points) + (i * unweldedMesh.pointStrideBytes));

		// Find Vertex in newVertices
		NxU32 newIndex = 0;
		for (newIndex = 0; newIndex < mNewVertices.size(); newIndex++)
		{
			NxVec3& newVec = mNewVertices[newIndex];
			if ((mEpsilon == 0 && newVec == curVec) || (newVec.distanceSquared(curVec) < squaredEpsilon))
			//if (newVec == curVec)
			{
				break;
			}
		}

		if (newIndex == mNewVertices.size())
		{
			// Not found in previous list
			mNewVertices.push_back(curVec);
		}

		mapping.push_back(newIndex);
	}

	// Store mapping
	mMappingSize = mapping.size();
	mMappingSpace = unweldedMesh.numTriangles * 3;
	mMappingDomain = mNewVertices.size();
	mMapping = (NxU32*)malloc(sizeof(NxU32) * mMappingSpace);
	memcpy(mMapping, &mapping[0], sizeof(NxU32) * mMappingSize);
	memset(((NxU32*)mMapping) + mMappingSize, 0, sizeof(NxU32) * (mMappingSpace - mMappingSize));
	mapping.clear();

	if (mNewVertices.size() < unweldedMesh.numVertices)
	{
		mUsed = true;
	}
	else
	{
		return;
	}

	if (unweldedMesh.flags & NX_MF_16_BIT_INDICES)
	{
		mNewFaces16 = (NxU16*)malloc(sizeof(NxU16) * unweldedMesh.numTriangles * 3);
	}
	else
	{
		mNewFaces32 = (NxU32*)malloc(sizeof(NxU32) * unweldedMesh.numTriangles * 3);
	}

	for (NxU32 i = 0; i < unweldedMesh.numTriangles; i++)
	{
		NxU32 triangles[3];
		const char* triangleChar = ((const char*)unweldedMesh.triangles) + (unweldedMesh.triangleStrideBytes * i);
		if (mNewFaces16)
		{
			const NxU16* tris = (const NxU16*)triangleChar;
			triangles[0] = tris[0];
			triangles[1] = tris[1];
			triangles[2] = tris[2];
		}
		else
		{
			assert(mNewFaces32 != NULL);
			const NxU32* tris = (const NxU32*)triangleChar;
			triangles[0] = tris[0];
			triangles[1] = tris[1];
			triangles[2] = tris[2];
		}

		for (NxU32 j = 0; j < 3; j++)
		{
			triangles[j] = getMapping(triangles[j]);
		}

		if (mNewFaces16)
		{
			for (NxU32 j = 0; j < 3; j++)
			{
				mNewFaces16[3*i+j] = (NxU16)(triangles[j] & 0xffff);
			}
		}
		else
		{
			for (NxU32 j = 0; j < 3; j++)
			{
				mNewFaces32[3*i+j] = triangles[j];
			}
		}
	}
}
// -----------------------------------------------------------------------
void VertexWelder::swapInBuffers(NxClothMeshDesc &meshDesc)
{
	mSwappedNumVertices    = meshDesc.numVertices;
	mSwappedPoints         = meshDesc.points;
	mSwappedPointStride    = meshDesc.pointStrideBytes;

	//mSwappedNumTriangles   = meshDesc.numTriangles;
	mSwappedTriangles      = meshDesc.triangles;
	mSwappedTriangleStride = meshDesc.triangleStrideBytes;

	meshDesc.numVertices         = mNewVertices.size();
	meshDesc.points              = &mNewVertices[0];
	meshDesc.pointStrideBytes    = sizeof(NxVec3);

	//meshDesc.numTriangles        = 
	meshDesc.triangles           = mNewFaces16 != NULL ? (void*)mNewFaces16 : (void*)mNewFaces32;
	meshDesc.triangleStrideBytes = mNewFaces16 != NULL ? sizeof(NxU16)*3 : sizeof(NxU32)*3;
	
}
// -----------------------------------------------------------------------
void VertexWelder::swapOutBuffers(NxClothMeshDesc &meshDesc)
{
	meshDesc.numVertices         = mSwappedNumVertices;
	meshDesc.points              = mSwappedPoints;
	meshDesc.pointStrideBytes    = mSwappedPointStride;
	
	//meshDesc.numTriangles        = mSwappedNumTriangles;
	meshDesc.triangles           = mSwappedTriangles;
	meshDesc.triangleStrideBytes = mSwappedTriangleStride;

	mSwappedNumVertices    = 0;
	mSwappedPoints         = NULL;
	mSwappedPointStride    = 0;

	//mSwappedNumTriangles   
	mSwappedTriangles      = NULL;
	mSwappedTriangleStride = 0;

}
// -----------------------------------------------------------------------


struct NewVertex
{
	NxI32 index;
	NxI32 unMapIndex;
	NxI32 parent;
	NxI32 unMapParent;
	NxI32 mappedVertices;
	NewVertex() : index(-1), unMapIndex(-1), parent(-1), unMapParent(-1), mappedVertices(0) {}
};
bool sortParent(const NewVertex& v1, const NewVertex& v2)
{
	return v1.parent < v2.parent;
}
bool sortIndex(const NewVertex& v1, const NewVertex& v2)
{
	return v1.index < v2.index;
}

struct DifficultVertex
{
	NxI32 mappedIndex;
	NxI32 unMappedIndex;
	//NxI32 newMappedIndex;
	NxI32 newUnMappedIndex;
	DifficultVertex() : mappedIndex(-1), unMappedIndex(-1), newUnMappedIndex(-1) {}
};

bool sortDifficultExt(const DifficultVertex& v1, const DifficultVertex& v2)
{
	if (v1.unMappedIndex < v2.unMappedIndex)
		return true;
	if (v1.unMappedIndex > v2.unMappedIndex)
		return false;
	return v1.mappedIndex < v2.mappedIndex;
}

bool sortDifficult(const DifficultVertex& v1, const DifficultVertex& v2)
{
	return v1.unMappedIndex < v2.unMappedIndex;
}

struct SourceDestPair
{
	union
	{
		struct {
			NxU32 source;
			NxU32 destination;
		};
		NxU64 _sorting;
	};
};

bool operator< (const SourceDestPair& p1, const SourceDestPair& p2)
{
	return p1._sorting <  p2._sorting;
	/*
	if (p1.source < p2.source)
		return true;
	if (p1.source > p2.source)
		return false;
	return p1.destination < p2.destination;
	*/
}

void VertexWelder::update(NxMeshData meshData)
{
	assert(mWriteVerticesPtr != NULL);

	bool updateVertices = (*(meshData.dirtyBufferFlagsPtr) & (NX_MDF_VERTICES_POS_DIRTY | NX_MDF_VERTICES_NORMAL_DIRTY)) > 0;
	bool updateIndices = (*(meshData.dirtyBufferFlagsPtr) & (NX_MDF_INDICES_DIRTY | NX_MDF_PARENT_INDICES_DIRTY)) > 0;

	NxU32 numNewVertices = *meshData.numVerticesPtr;
	NxU32 numTriangles = *meshData.numIndicesPtr / 3;
	NxU32 oldMappingDomain = mMappingDomain;
	
	NxArray<NewVertex> newVertices;
	NxArray<DifficultVertex> difficultVertices;

	mMappingDomainAddition = 0;

	if (updateVertices)
	{
		
		if (mMappingDomain < numNewVertices)
		{
#ifdef DEBUG_WELDER
			printf("------------------------------------\n");
#endif
			for (NxU32 i = mMappingDomain; i < numNewVertices; i++)
			{
				NewVertex v;
				v.index = i;
				v.parent = *(NxU32*)(((char*)meshData.parentIndicesBegin) + meshData.parentIndicesByteStride * i);
				while (v.parent >= (NxI32)mMappingDomain) {
					v.parent = *(NxU32*)(((char*)meshData.parentIndicesBegin) + meshData.parentIndicesByteStride * v.parent);
				}
#ifdef DEBUG_WELDER
				printf("New Vertex: %d %d\n", v.index, v.parent);
#endif
				newVertices.push_back(v);
			}
			std::sort(newVertices.begin(), newVertices.end(), sortParent);
		}
		
		for (NxU32 i = 0; i < mMappingSize; i++)
		{
			NxU32 mappedIndex = getMapping(i);

			NewVertex newV;
			newV.parent = mappedIndex;
			// Find all vertices that are a parent for a newly created vertex
			NxArray<NewVertex>::iterator found = std::lower_bound(newVertices.begin(), newVertices.end(), newV, sortParent);
			while (found != NULL && found->parent == mappedIndex)
			{
				found->mappedVertices ++;
				if (found->mappedVertices == 1)
				{
					found->unMapParent = i;
#ifdef DEBUG_WELDER
					printf("New Vertex Update, %d %d %d\n", found->index, found->parent, found->unMapParent);
#endif
				}
				else
				{
					// several unmapped parents
					DifficultVertex v;
					v.mappedIndex = found->index;
					v.unMappedIndex = i;
					difficultVertices.push_back(v);
#ifdef DEBUG_WELDER
					printf("Difficult Vertex %d %d\n", v.unMappedIndex, v.mappedIndex);
#endif

					if (found->mappedVertices == 2)
					{
						v.unMappedIndex = found->unMapParent;
						difficultVertices.push_back(v);
#ifdef DEBUG_WELDER
						printf("Difficult Vertex %d %d\n", v.unMappedIndex, v.mappedIndex);
#endif
					}

					found->unMapParent = -2;
				}
				found++;
			}
			

			NxVec3& vertex = *(NxVec3*)(((char*)mWriteVerticesPtr) + mWriteVerticesStride * i);
			NxVec3& normal = *(NxVec3*)(((char*)mWriteNormalsPtr) + mWriteNormalsStride* i);
			//float* texCoord = (float*)(((char*)texCoords) + texStride * i);

			const NxVec3& oldVertex = *(NxVec3*)(((char*)meshData.verticesPosBegin) + meshData.verticesPosByteStride * mappedIndex);
			const NxVec3& oldNormal = *(NxVec3*)(((char*)meshData.verticesNormalBegin) + meshData.verticesNormalByteStride * mappedIndex);

			vertex = oldVertex;
			normal = oldNormal;
		}
		// Adapt the mapping table
		std::sort(newVertices.begin(), newVertices.end(), sortIndex);
		std::sort(difficultVertices.begin(), difficultVertices.end(), sortDifficultExt);	
	}

	if (updateIndices)
	{
		std::vector<bool> bitVector(mMappingSpace, false);
#ifdef DEBUG_WELDER
		printf("updateIndices: Vertices: %d, Indices %d, gfx Vertices: %d\n", *meshData.numVerticesPtr, *meshData.numIndicesPtr, mMappingSize);
#endif
		if (difficultVertices.size() > 0)
		{
#ifdef DEBUG_WELDER
			printf("    Difficult Vertices:\n");
#endif
			for (NxU32 i = 0; i < difficultVertices.size(); i++)
			{
				DifficultVertex& v = difficultVertices[i];
#ifdef DEBUG_WELDER
				printf("      V %d %d\n", v.unMappedIndex, v.mappedIndex);
#endif
			}
		}
		assert((meshData.flags & NX_MF_16_BIT_INDICES) == 0);
		assert(meshData.indicesByteStride == 4);
		for (NxU32 i = 0; i < numTriangles; i++)
		{
			const NxU32* simTriangle = (NxU32*)(((char*)meshData.indicesBegin) + meshData.indicesByteStride * (i*3));
			NxU32* gfxTriangle = (NxU32*)(((char*)mWriteIndicesPtr) + mWriteIndicesStride* i);
			
			if (simTriangle[0] == simTriangle[1] && simTriangle[1] == simTriangle[2])
			{
				// Face was deleted (outside valid bounds probably)
				gfxTriangle[0] = gfxTriangle[1] = gfxTriangle[2] = 0;
				continue;
			}
			for (NxU32 j = 0; j < 3; j++)
			{

				DifficultVertex v;
				v.mappedIndex = simTriangle[j];
				v.unMappedIndex = gfxTriangle[j];
				if (std::binary_search(difficultVertices.begin(), difficultVertices.end(), v, sortDifficult))
				{
					NxArray<DifficultVertex>::iterator it = std::lower_bound(difficultVertices.begin(), difficultVertices.end(), v, sortDifficultExt);
#ifdef DEBUG_WELDER
					printf("-- Triangle %d (%d) (%d %d %d) (%d %d %d)", i, j, simTriangle[0], simTriangle[1], simTriangle[2], gfxTriangle[0], gfxTriangle[1], gfxTriangle[2]);
					printf(" %d %d\n", v.unMappedIndex, v.mappedIndex);
#endif

					if (it == NULL || it->mappedIndex != simTriangle[j])
					{
						// element hasn't been found

						//insert element
#ifdef DEBUG_WELDER
						printf("Adding Diff %d %d\n", v.unMappedIndex, v.mappedIndex);
#endif
						difficultVertices.push_back(v);

						// sort now, don't know whether this could be done less often, so far the list is extremely small
						std::sort(difficultVertices.begin(), difficultVertices.end(), sortDifficultExt);

						// get the freshly created item
						it = std::lower_bound(difficultVertices.begin(), difficultVertices.end(), v, sortDifficultExt);

						// element has to exist
						assert(it != NULL);
					}

					if (it->newUnMappedIndex >= 0)
					{
						gfxTriangle[j] = it->newUnMappedIndex;
					}
					else if (bitVector[it->unMappedIndex])
					{
#ifdef DEBUG_WELDER
						printf("Bit %d is true\n", it->unMappedIndex);
#endif
						// create a new gfx vertex
						it->newUnMappedIndex = mMappingSize;
						addNewVertex(gfxTriangle[j]);
						setMapping(it->newUnMappedIndex, simTriangle[j]);
						gfxTriangle[j] = it->newUnMappedIndex;
						bitVector[it->newUnMappedIndex] = true;
					}
					else
					{
#ifdef DEBUG_WELDER
						printf("Set Bit %d to true\n", it->unMappedIndex);
#endif
						bitVector[it->unMappedIndex] = true;
						it->newUnMappedIndex = it->unMappedIndex;
						setMapping(it->newUnMappedIndex, simTriangle[j]);
					}
				}
				else if (simTriangle[j] >= oldMappingDomain) // only used when not a difficult vertex
				{
					// unamp index and update
					for (NxU32 k = 0; k < newVertices.size(); k++)
					{
						NewVertex& v = newVertices[k];
						if (v.index == simTriangle[j] && v.mappedVertices == 1)
						{
#ifdef DEBUG_WELDER
							printf("- Triangle %d (%d %d %d) (%d %d %d)", i, simTriangle[0], simTriangle[1], simTriangle[2], gfxTriangle[0], gfxTriangle[1], gfxTriangle[2]);
							printf(" %d %d\n", v.unMapIndex, v.index);
#endif
							if (v.unMapIndex == -1)
							{
#ifdef DEBUG_WELDER
								printf("Add Simple\n");
#endif
								v.unMapIndex = mMappingSize;
								//addNewVertex(vertices, vertexStride, normals, normalStride, texCoords, texStride, v.unMapParent);
								addNewVertex(gfxTriangle[j]);
								gfxTriangle[j] = v.unMapIndex;
								setMapping(v.unMapIndex, v.index);
							}
							else
							{
#ifdef DEBUG_WELDER
								printf("Use Simple\n");
#endif
								gfxTriangle[j] = v.unMapIndex;
							}
							break; // for (k)
						}
					}
				}
			}
		}
	}

	if (updateVertices)
	{
		mMappingDomain = *meshData.numVerticesPtr;

#ifdef DEBUG_WELDER
		static bool sanityCheck = true;
		if (sanityCheck)
		{
			// sanity check
			NxU32 temp = 0;
			for (NxU32 i = 0; i < mMappingSize; i++)
			{
				temp = NxMath::max(getMapping(i), temp);
			}
			if (temp != mMappingDomain - 1)
			{
				printf("Mapping Domain not right, is %d, should be %d\n", temp, mMappingDomain-1);
				assert(0);
			}
			for (NxU32 i = 0; i < numTriangles; i++)
			{
				const NxU32* simTriangle = (NxU32*)(((char*)meshData.indicesBegin) + meshData.indicesByteStride * (i*3));
				NxU32* gfxTriangle = (NxU32*)(((char*)mWriteIndicesPtr) + mWriteIndicesStride * i);
				for (NxU32 j = 0; j < 3; j++)
				{
					if (simTriangle[j] != getMapping(gfxTriangle[j]))
					{
						printf("Triangle %d (%d) not correct (%d %d %d) -> (%d %d %d) != (%d %d %d)\n",
							i, 3*i+j,
							gfxTriangle[0], gfxTriangle[1], gfxTriangle[2],
							getMapping(gfxTriangle[0]), getMapping(gfxTriangle[1]), getMapping(gfxTriangle[2]),
							simTriangle[0], simTriangle[1], simTriangle[2]);
						assert(0);
					}
				}
			}
		}
#endif
	}

	return;
}

void VertexWelder::addNewVertex(NxU32 oldIndex)
{
	assert(mMappingSize + 1 < mMappingSpace);

#ifdef DEBUG_WELDER
	printf("addNewVertex %d (%d)-> %d\n", oldIndex, getMapping(oldIndex), mMappingSize);
#endif
	NxVec3& vertex = *(NxVec3*)(((char*)mWriteVerticesPtr) + mWriteVerticesStride * mMappingSize);
	NxVec3& normal = *(NxVec3*)(((char*)mWriteNormalsPtr)  + mWriteNormalsStride  * mMappingSize);
	void* userData =  (float*) (((char*)mWriteUserDataPtr) + mWriteUserDataStride * mMappingSize);

	NxVec3& parentVertex = *(NxVec3*)(((char*)mWriteVerticesPtr) + mWriteVerticesStride * oldIndex);
	NxVec3& parentNormal = *(NxVec3*)(((char*)mWriteNormalsPtr)  + mWriteNormalsStride  * oldIndex);
	void* parentUserData =  (float*) (((char*)mWriteUserDataPtr) + mWriteUserDataStride * oldIndex);

	vertex = parentVertex;
	normal = parentNormal;
	memcpy(userData, parentUserData, mWriteUserDataSize);

	mMappingSize++;
}

inline NxU32 VertexWelder::getMapping(NxU32 index)
{
	assert(index < mMappingSize);
	return mMapping[index];
}

inline void VertexWelder::setMapping(NxU32 index, NxU32 value)
{
	assert(index < mMappingSize);
#ifdef DEBUG_WELDER
	printf("Mapping for %d from %d to %d\n", index, mMapping[index], value);
#endif
	mMapping[index] = value;
}

void VertexWelder::setWriteData(void *vertices, NxU32 vertexStride, void *normals, NxU32 normalStride, void *userData, NxU32 userDataStride, NxU32 userDataSize, void *indices, NxU32 indexStride)
{
	mWriteVerticesPtr     = vertices;
	mWriteVerticesStride  = vertexStride;

	mWriteNormalsPtr      = normals;
	mWriteNormalsStride   = normalStride;

	mWriteUserDataPtr     = userData;
	mWriteUserDataStride  = userDataStride;
	mWriteUserDataSize    = userDataSize;

	mWriteIndicesPtr      = indices;
	mWriteIndicesStride   = indexStride;
}

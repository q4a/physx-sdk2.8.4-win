// ====================================================================================
//
//                                 NVIDIA PHYSX
//
// Content: Tool to read mesh data and cooking flags from cooked mesh file
//
// Comment: Works for convex and non-convex mesh files
//
// Author:  Michael Sauter
//
// ====================================================================================

#include <stdio.h>

#include "NxPhysics.h"

#include "Stream.h"
#include "Serialize.h"

#include "CookedMeshReader.h"


#define SUPPORT_CONVEX_PARTS


// Character buffer size for mesh type
const int mesh_type_size = 4;


enum MeshSerialFlag
{
	MSF_MATERIALS		=	(1<<0),
	MSF_FACE_REMAP		=	(1<<1),
	MSF_HARDWARE_MESH	=	(1<<2),
	MSF_8BIT_INDICES	=	(1<<3),
	MSF_16BIT_INDICES	=	(1<<4),
};

enum ModelFlag
{
	OPC_QUANTIZED	= (1<<0),	// Compressed/uncompressed tree
	OPC_NO_LEAF		= (1<<1),	// Leaf/NoLeaf tree
	OPC_SINGLE_NODE	= (1<<2),	// Special case for 1-node models
};


CmMeshData::CmMeshData()	// Constructor sets to default
{
	numVertices					= 0;
	numTriangles				= 0;
	pointStrideBytes			= 0;
	triangleStrideBytes			= 0;
	points						= NULL;
	triangles					= NULL;
	flags						= 0;

	materialIndexStride			= 0;
	materialIndices				= NULL;
	heightFieldVerticalAxis		= NX_NOT_HEIGHTFIELD;
	heightFieldVerticalExtent	= 0;
	convexEdgeThreshold			= 0.001f;
}


CmMeshData::~CmMeshData()
{
	if (points)
		delete points;

	if (triangles)
		delete triangles;

	if (materialIndices)
		delete materialIndices;
}


void skipIndices(UserStream* stream, NxU32 maxIdx, NxU32 nbIndices, bool max16Bit)
{
	assert(stream);

	if(maxIdx <= 0xff)
	{
		stream->advanceStream(nbIndices);
	}
	else if ((maxIdx <= 0xffff) || (max16Bit))
	{
		stream->advanceStream(nbIndices * sizeof(NxU16));
	}
	else
	{
		stream->advanceStream(nbIndices * sizeof(NxU32));
	}
}


bool hasQuantizedBox(const NxStream& stream, bool mismatch, bool& hintCollisionSpeed)
{
	hintCollisionSpeed = false;
	NxU32 modelSize = readDword(mismatch, stream);

	// Import header
	NxU8 hdr[4];
	stream.readBuffer(hdr, sizeof(NxU8)*4);
	if(hdr[0]!='O' || hdr[1]!='P' || hdr[2]!='C')
		return false;

	bool mod_mismatch = (hdr[3] != littleEndian());

	NxU32 mod_version = readDword(mod_mismatch, stream);
	NxU32 modelCode   = readDword(mod_mismatch, stream);
	
	hintCollisionSpeed = (modelCode & OPC_QUANTIZED) != OPC_QUANTIZED;

	return true;
}


bool getTriMeshData(const NxStream& stream, NxU32 version, const bool mismatch, bool& hintCollisionSpeed, CmMeshData& meshDesc)
{
	UserStream* userStream = (UserStream*)&stream;

	// Import serialization flags
	NxU32 serialFlags = readDword(mismatch, stream);

	if (serialFlags & MSF_HARDWARE_MESH)
		meshDesc.flags |= NX_MF_HARDWARE_MESH;

	// Import misc values
	meshDesc.convexEdgeThreshold = readFloat(mismatch, stream);
	meshDesc.heightFieldVerticalAxis = (NxHeightFieldAxis)readDword(mismatch, stream);
	meshDesc.heightFieldVerticalExtent = readFloat(mismatch, stream);

	// Import mesh
	meshDesc.pointStrideBytes = sizeof(NxPoint);
    meshDesc.numVertices = readDword(mismatch, stream);
	assert(meshDesc.numVertices);
    NxPoint* points = new NxPoint[meshDesc.numVertices];

	meshDesc.triangleStrideBytes = sizeof(NxTriangle32);
	meshDesc.numTriangles = readDword(mismatch, stream);
	assert(meshDesc.numTriangles);
	NxTriangle32* tris = new NxTriangle32[meshDesc.numTriangles];

	// Get vertices
	stream.readBuffer(points, sizeof(NxPoint)*meshDesc.numVertices);
    if(mismatch)
	{
		for(NxU32 i=0; i < meshDesc.numVertices; i++)
		{
			points[i].x = flip(&points[i].x);
			points[i].y = flip(&points[i].y);
			points[i].z = flip(&points[i].z);
		}
	}
	meshDesc.points = points;

	// Get triangles
	if(serialFlags & MSF_8BIT_INDICES)
	{
		for(NxU32 i=0; i < meshDesc.numTriangles; i++)
		{
			tris[i].v[0] = stream.readByte();
			tris[i].v[1] = stream.readByte();
			tris[i].v[2] = stream.readByte();
		}
	}
	else if(serialFlags & MSF_16BIT_INDICES)
	{
		for(NxU32 i=0; i < meshDesc.numTriangles; i++)
		{
			NxU16 i0 = stream.readWord();
			NxU16 i1 = stream.readWord();
			NxU16 i2 = stream.readWord();
			if(mismatch)
			{
				i0 = flip(&i0);
				i1 = flip(&i1);
				i2 = flip(&i2);
			}
			tris[i].v[0] = i0;
			tris[i].v[1] = i1;
			tris[i].v[2] = i2;
		}
	}
	else
	{
		stream.readBuffer(tris, sizeof(NxTriangle32)*meshDesc.numTriangles);
		if(mismatch)
		{
			for(NxU32 i=0; i < meshDesc.numTriangles; i++)
			{
				tris[i].v[0] = flip(&tris[i].v[0]);
				tris[i].v[1] = flip(&tris[i].v[1]);
				tris[i].v[2] = flip(&tris[i].v[2]);
			}
		}
	}
	meshDesc.triangles = tris;


	// Get materials
	if(serialFlags & MSF_MATERIALS)
	{
		NxMaterialIndex* materials = new NxMaterialIndex[meshDesc.numTriangles];
		stream.readBuffer(materials, sizeof(NxMaterialIndex)*meshDesc.numTriangles);

		if(mismatch)
		{
			for(NxU32 i=0; i < meshDesc.numTriangles; i++)
				materials[i] = flip(&materials[i]);
		}

		meshDesc.materialIndices = materials;
		meshDesc.materialIndexStride = sizeof(NxMaterialIndex);
	}


	bool convOK = false;
	long offset = ftell(userStream->fp);
	for(int j=0; j <= 1; j++)
	{
		if (j > 0)
		{
			// Reset file pointer and try to load the data as a 2.3.x SDK mesh
			int ret_val = fseek(userStream->fp, offset, SEEK_SET);
			assert(ret_val == 0);
		}

		if(serialFlags & MSF_FACE_REMAP)
		{
			NxU32* remap = new NxU32[meshDesc.numTriangles];
	//		stream.readBuffer(remap, sizeof(NxU32)*mesh.getNumTriangles());
	//		if(mismatch)
	//			{
	//			for(NxU32 i=0;i<mesh.getNumTriangles();i++)
	//				remap[i] = flip(&remap[i]);
	//			}
			if (j == 0)
				readIndices(readDword(mismatch, stream), meshDesc.numTriangles, remap, stream, mismatch);
			else
			{
				// 2.3.x SDK
				readIntBuffer(remap, meshDesc.numTriangles, mismatch, stream);
			}
			delete [] remap;
		}

	#ifdef SUPPORT_CONVEX_PARTS
		NxU32 numConvexParts = readDword(mismatch, stream);
		NxU32 numFlatParts = readDword(mismatch, stream);

		if(numConvexParts)
		{
			NxU16* convexParts = new NxU16[meshDesc.numTriangles];
			stream.readBuffer(convexParts, sizeof(NxU16)*meshDesc.numTriangles);
			if(mismatch)
			{
				for(NxU32 i=0; i < meshDesc.numTriangles; i++)
					convexParts[i] = flip(&convexParts[i]);
			}

			delete [] convexParts;
		}
		if(numFlatParts)
		{
			if(numFlatParts < 256)
			{
				NxU8* flatParts8 = new NxU8[meshDesc.numTriangles];
				stream.readBuffer(flatParts8, sizeof(NxU8)*meshDesc.numTriangles);

				delete [] flatParts8;
			}
			else
			{
				NxU16* flatParts16 = new NxU16[meshDesc.numTriangles];
				stream.readBuffer(flatParts16, sizeof(NxU16)*meshDesc.numTriangles);

				delete [] flatParts16;
			}
	//		if(mismatch)
	//			{
	//			for(NxU32 i=0;i<mesh.getNumTriangles();i++)
	//				mesh._flatParts[i] = flip(&mesh._flatParts[i]);
	//			}
		}
	#endif

		// Import tree model flag
		convOK = hasQuantizedBox(stream, mismatch, hintCollisionSpeed);

		if (convOK)
			break;
	}

	return convOK;
}


bool getConvMeshData(const NxStream& stream, NxU32 version, const bool mismatch, NxU16& useUnquantizedNormals, bool& hintCollisionSpeed, CmMeshData& meshDesc)
{
	UserStream* userStream = (UserStream*)&stream;

	// Import serialization flags
	NxU32 serialFlags = readDword(mismatch, stream);

	useUnquantizedNormals = 0;


	if (version > 0)
	{
		// Import collision hull header
		NxI8	collHullType[mesh_type_size];
		NxU32	collHullVersion;
		bool	collHullMismatch;


		bool validFile = readHeader('I', 'C', 'E', collHullType[0], collHullType[1], collHullType[2], collHullType[3],
						 collHullVersion, collHullMismatch, stream);
		if (!validFile || !((collHullType[0] == 'C') && (collHullType[1] == 'L') && (collHullType[2] == 'H') && (collHullType[3] == 'L')))
		{
			printf("Error: Invalid collision hull header.\n");
			return false;
		}

		// Import collision hull header
		NxI8	convHullType[mesh_type_size];
		NxU32	convHullVersion;
		bool	convHullMismatch;

		validFile = readHeader('I', 'C', 'E', convHullType[0], convHullType[1], convHullType[2], convHullType[3],
					convHullVersion, convHullMismatch, stream);
		if (!validFile || !((convHullType[0] == 'C') && (convHullType[1] == 'V') && (convHullType[2] == 'H') && (convHullType[3] == 'L')))
		{
			printf("Error: Invalid convex hull header.\n");
			return false;
		}

		// Import figures
		meshDesc.pointStrideBytes = sizeof(NxPoint);
		meshDesc.numVertices = readDword(convHullMismatch, stream);
		assert(meshDesc.numVertices);
		NxPoint* points  = new NxPoint[meshDesc.numVertices];

		meshDesc.triangleStrideBytes = sizeof(NxTriangle32);
		meshDesc.numTriangles = readDword(convHullMismatch, stream);
		assert(meshDesc.numTriangles);
		NxTriangle32* tris = new NxTriangle32[meshDesc.numTriangles];

		NxU32 nbEdges			= readDword(convHullMismatch, stream);

		NxU32 nbPolygons		= readDword(convHullMismatch, stream);
		const NxU32 polyDataSize = 4 + 24 + 2*sizeof(size_t);  // sizeof(PxHullPolygonData)
								// WARNING: 64bit portabilitty issue. There are two pointers in this structure
								// the size of which are assumed to match the size on the platform they were saved on,
								// i.e., sizeof(size_t) might have to be changed if this code is compiled on a
								// 64Bit system

		NxU32 nb				= readDword(convHullMismatch, stream);
		NxU32 lastOffset		= readDword(convHullMismatch, stream);

		// Import vertices
		readFloatBuffer((NxF32*) points, 3*meshDesc.numVertices, convHullMismatch, stream);
		meshDesc.points = points;

		// Import triangles
		NxU32 maxIdx = readDword(convHullMismatch, stream);
		readIndices(maxIdx, 3*meshDesc.numTriangles, (NxU32*) tris, stream, convHullMismatch);
		meshDesc.triangles = tris;

		// Import hull normals
		NxPoint* normals = new NxPoint[meshDesc.numVertices];
		if(convHullVersion >= 5)
		{
			useUnquantizedNormals = readWord(convHullMismatch, stream);
		}

		// Import hull normals
		if((convHullVersion >= 4) && !useUnquantizedNormals)
		{
			for(NxU32 i=0; i < meshDesc.numVertices; i++)
				readWord(convHullMismatch, stream);
		}
		else
		{
			readFloatBuffer((NxF32*) normals, 3*meshDesc.numVertices, convHullMismatch, stream);
			if(convHullVersion < 2)
			{
				for(NxU32 i=0; i < meshDesc.numVertices; i++)	
				{
					normals[i].x = -normals[i].x;
					normals[i].y = -normals[i].y;
					normals[i].z = -normals[i].z;
				}
			}
		}
		delete [] normals;

		// Import hull center
		readFloat(convHullMismatch, stream);
		readFloat(convHullMismatch, stream);
		readFloat(convHullMismatch, stream);

		// Import polygons
		userStream->advanceStream(nbPolygons * polyDataSize);
		/*stream.ReadBuffer(mData.mPolygons, mData.mNbPolygons*sizeof(PxHullPolygonData));
		if(Mismatch)
		{
			for(udword i=0;i<mData.mNbPolygons;i++)
			{
				Flip(mData.mPolygons[i].mNbVerts);
				Flip(mData.mPolygons[i].mFlags);
				Flip((void *&)mData.mPolygons[i].mERef16);
  				Flip((void *&)mData.mPolygons[i].mVRef8);
				Flip(mData.mPolygons[i].mPlane[0]);
				Flip(mData.mPolygons[i].mPlane[1]);
				Flip(mData.mPolygons[i].mPlane[2]);
				Flip(mData.mPolygons[i].mPlane[3]);
				Flip(mData.mPolygons[i].mMin);
				Flip(mData.mPolygons[i].mMax);
			}
		}*/

		if(convHullVersion >= 3)
		{
			userStream->advanceStream(nb);
			maxIdx = readDword(convHullMismatch, stream);
			
			skipIndices(userStream, maxIdx, nb, true);
			//for(udword i=0;i<Nb;i++)	mData.mVertexData8[i] = stream.ReadByte();
			//ReadIndices(ReadDword(Mismatch, stream), Nb, mData.mEdgeData16, stream, Mismatch);
		}

		readDword(convHullMismatch, stream);
		readDword(convHullMismatch, stream);

		// Import edges
		if(convHullVersion >= 3)
		{
			userStream->advanceStream(2* nbEdges);
			/*for(udword i=0;i<mData.mNbEdges;i++)
			{
				mData.mEdges8[i].Ref0 = stream.ReadByte();
				mData.mEdges8[i].Ref1 = stream.ReadByte();
			}*/
		}

		if((convHullVersion >= 4) && !useUnquantizedNormals)
		{
			userStream->advanceStream(nbEdges * sizeof(NxU16));
			/*for(udword i=0;i<mData.mNbEdges;i++)
				DecodeNormal(mData.mEdgeNormals[i], ReadWord(Mismatch, stream));*/
		}
		else
		{
			userStream->advanceStream(nbEdges * sizeof(NxPoint));
			//ReadFloatBuffer(&mData.mEdgeNormals->x, 3*mData.mNbEdges, Mismatch, stream);
		}

		{
			maxIdx = readDword(convHullMismatch, stream);
			skipIndices(userStream, maxIdx, nbEdges, true);

			maxIdx = readDword(convHullMismatch, stream);
			skipIndices(userStream, maxIdx, nbEdges, true);
		}
		{
			maxIdx = readDword(convHullMismatch, stream);
			skipIndices(userStream, maxIdx, nbEdges, false);
		}

		userStream->advanceStream(lastOffset);


		// Import valency header
		NxI8	valencyType[mesh_type_size];
		NxU32	valencyVersion;
		bool	valencyMismatch;

		validFile = readHeader('I', 'C', 'E', valencyType[0], valencyType[1], valencyType[2], valencyType[3],
					valencyVersion, valencyMismatch, stream);
		if (!validFile || !((valencyType[0] == 'V') && (valencyType[1] == 'A') && (valencyType[2] == 'L') && (valencyType[3] == 'E')))
		{
			printf("Error: Invalid valency header.\n");
			return false;
		}

		NxU32 valNbVerts = readDword(valencyMismatch, stream);
		NxU32 nbAdjVerts = readDword(valencyMismatch, stream);

		assert(valencyVersion == 2);

		{
			maxIdx = readDword(valencyMismatch, stream);
			skipIndices(userStream, maxIdx, valNbVerts, true);
		}
		userStream->advanceStream(nbAdjVerts);
	}
	else
	{
		// 2.3.x SDK

		// Import figures
		meshDesc.pointStrideBytes = sizeof(NxPoint);
		meshDesc.numVertices = readDword(mismatch, stream);
		assert(meshDesc.numVertices);
		NxPoint* points  = new NxPoint[meshDesc.numVertices];

		meshDesc.triangleStrideBytes = sizeof(NxTriangle32);
		meshDesc.numTriangles = readDword(mismatch, stream);
		assert(meshDesc.numTriangles);
		NxTriangle32* tris = new NxTriangle32[meshDesc.numTriangles];

		// Import vertices
		readFloatBuffer((NxF32*) points, 3*meshDesc.numVertices, mismatch, stream);
		meshDesc.points = points;

		// Import triangles
		readIntBuffer((NxU32*) tris, 3*meshDesc.numTriangles, mismatch, stream);
		meshDesc.triangles = tris;

		if(serialFlags & MSF_MATERIALS)
		{
			userStream->advanceStream(sizeof(NxMaterialIndex) * meshDesc.numTriangles);
		}

		if(serialFlags & MSF_FACE_REMAP)
		{
			userStream->advanceStream(sizeof(NxU32) * meshDesc.numTriangles);
		}
	}


	// Import tree model flag
	if (!hasQuantizedBox(stream, mismatch, hintCollisionSpeed))
		return false;

	return true;
}


bool GetCookedData(const char* file, CmMeshType& mesh_type, CmMeshData& meshDesc, bool& hintCollisionSpeed)
{
	assert(file != NULL);

	mesh_type = MT_INVALID_MESH;
	hintCollisionSpeed = false;

	NxI8	meshType[mesh_type_size];		// Mesh type ID
	NxU32	version;
	bool	mismatch;

    // check file header
    UserStream in_stream(file, true);
    if (!in_stream.isOpen())
    {
        //printf("Error: Could not open file \"%s\" for reading. Make sure the file exists.\n", file);
        return false;
    }

	// Import header
	bool validFile = readHeader('N', 'X', 'S', meshType[0], meshType[1], meshType[2], meshType[3], version, mismatch, in_stream);

    if (!validFile)
    {
        //printf("Error: Invalid header in file \"%s\".\n", file);
        return false;
    }

    if ((meshType[0] == 'M') && (meshType[1] == 'E') && (meshType[2] == 'S') && (meshType[3] == 'H'))
    {
        // general mesh
		mesh_type = MT_TRIANGLE_MESH;

		// Get old mesh
		if (getTriMeshData(in_stream, version, mismatch, hintCollisionSpeed, meshDesc))
		{
			return true;
		}
		else
		{
			//printf("Error: Loading cooked mesh file \"%s\" failed.\n", file);
			return false;
		}
    }
    else if ((meshType[0] == 'C') && (meshType[1] == 'V') && (meshType[2] == 'X') && (meshType[3] == 'M'))
    {
        // convex mesh
		mesh_type = MT_CONVEX_MESH;

		// Get old mesh
		NxU16 useUnquantizedNormals;
        if (getConvMeshData(in_stream, version, mismatch, useUnquantizedNormals, hintCollisionSpeed, meshDesc))
		{
			if (useUnquantizedNormals)
				meshDesc.flags |= NX_CF_USE_UNCOMPRESSED_NORMALS;

			return true;
		}
		else
		{
			//printf("Error: Loading cooked mesh file \"%s\" failed.\n", file);
			return false;
		}
    }
    else
    {
        //printf("Error: Unsupported mesh type in file \"%s\".\nOnly meshes and convex meshes are supported.\n", file);
        return false;
    }
}

#include "SampleRaycastCar.h"

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define TERRAIN_SIZE		33
#define TERRAIN_NB_VERTS	TERRAIN_SIZE*TERRAIN_SIZE
#define TERRAIN_NB_FACES	(TERRAIN_SIZE-1)*(TERRAIN_SIZE-1)*2
#define TERRAIN_OFFSET		0.0f
#define TERRAIN_WIDTH		20.0f
#define TERRAIN_CHAOS		70.0f //150.0f
static NxVec3* gTerrainVerts = NULL;
static NxVec3* gTerrainNormals = NULL;
static NxU32* gTerrainFaces = NULL;
static NxMaterialIndex * gTerrainMaterials = NULL;
NxMaterialIndex materialIce, materialRock, materialMud, materialGrass, materialDefault = 0;
static NxActor * gTerrain = NULL;

#include "Stream.h"
#include "cooking.h"
#include "CustomRandom.h"

float trand()
{
	static Random random(3);
	return ( (float)(random.rand() ) / ( (float)Random::rand_max+1) ) * (2.0f) -1.0f;
}


void smoothTriangle(NxU32 a, NxU32 b, NxU32 c)
	{
	NxVec3 & v0 = gTerrainVerts[a];
	NxVec3 & v1 = gTerrainVerts[b];
	NxVec3 & v2 = gTerrainVerts[c];

	NxReal avg = (v0.y + v1.y + v2.y) * 0.333f;
	avg *= 0.5f;
	v0.y = v0.y * 0.5f + avg;
	v1.y = v1.y * 0.5f + avg;
	v2.y = v2.y * 0.5f + avg;
	}

NxReal chooseTrigMaterial(NxU32 faceIndex, NxReal lastSteepness)
	{
	NxVec3 & v0 = gTerrainVerts[gTerrainFaces[faceIndex * 3]];
	NxVec3 & v1 = gTerrainVerts[gTerrainFaces[faceIndex * 3 + 1]];
	NxVec3 & v2 = gTerrainVerts[gTerrainFaces[faceIndex * 3 + 2]];

	NxVec3 edge0 = v1 - v0;
	NxVec3 edge1 = v2 - v0;

	NxVec3 normal = edge0.cross(edge1);
	normal.normalize();
	NxReal steepness = 1.0f - normal.y;
	steepness += lastSteepness;
	steepness /= 2.0f;

	if	(steepness > 0.25f)
		{
		gTerrainMaterials[faceIndex] = materialIce;
		}
	else if (steepness > 0.2f)
		{
		gTerrainMaterials[faceIndex] = materialRock;
		
		}
	else if (steepness > 0.1f)
		{
		gTerrainMaterials[faceIndex] = materialMud;
		}
	else
		gTerrainMaterials[faceIndex] = materialGrass;
	return steepness;
	}

void InitTerrain()
{

	NxMaterialDesc	m;

	NxMaterial * defaultMaterial = gScene->getMaterialFromIndex(0); 
	defaultMaterial->setRestitution(0.0f);
	defaultMaterial->setStaticFriction(0.5f);
	defaultMaterial->setDynamicFriction(0.5f);
		

	//terrain materials:

	// Ice
	m.restitution		= 1.0f;
	m.staticFriction	= 0.05f;
	m.dynamicFriction	= 0.05f;
	materialIce = gScene->createMaterial(m)->getMaterialIndex();
	// Rock
	m.restitution		= 0.3f;
	m.staticFriction	= 1.2f;
	m.dynamicFriction	= 0.9f;
	materialRock = gScene->createMaterial(m)->getMaterialIndex();
	// Mud
	m.restitution		= 0.0f;
	m.staticFriction	= 0.8f;
	m.dynamicFriction	= 0.2f;
	materialMud = gScene->createMaterial(m)->getMaterialIndex();
	// Grass
	m.restitution		= 0.1f;
	m.staticFriction	= 0.4f;
	m.dynamicFriction	= 0.4f;
	materialGrass = gScene->createMaterial(m)->getMaterialIndex();

	// Initialize terrain vertices
	gTerrainVerts = new NxVec3[TERRAIN_NB_VERTS];
	for(NxU32 y=0;y<TERRAIN_SIZE;y++)
	{
		for(NxU32 x=0;x<TERRAIN_SIZE;x++)
		{
			NxVec3 v; 
			v.set(NxF32(x)-(NxF32(TERRAIN_SIZE-1)*0.5f), 0.0f, NxF32(y)-(NxF32(TERRAIN_SIZE-1)*0.5f));
			v *= TERRAIN_WIDTH;
			gTerrainVerts[x+y*TERRAIN_SIZE] = v;
		}
	}

	struct Local
	{
		static void _Compute(bool* done, NxVec3* field, NxU32 x0, NxU32 y0, NxU32 size, NxF32 value)
		{
			// Compute new size
			size>>=1;
			if(!size) return;

			// Compute new heights
			NxF32 v0 = value*trand();
			NxF32 v1 = value*trand();
			NxF32 v2 = value*trand();
			NxF32 v3 = value*trand();
			NxF32 v4 = value*trand();

			NxU32 x1 = (x0+size)		% TERRAIN_SIZE;
			NxU32 x2 = (x0+size+size)	% TERRAIN_SIZE;
			NxU32 y1 = (y0+size)		% TERRAIN_SIZE;
			NxU32 y2 = (y0+size+size)	% TERRAIN_SIZE;

			if(!done[x1 + y0*TERRAIN_SIZE])	field[x1 + y0*TERRAIN_SIZE].y = v0 + 0.5f * (field[x0 + y0*TERRAIN_SIZE].y + field[x2 + y0*TERRAIN_SIZE].y);
			if(!done[x0 + y1*TERRAIN_SIZE])	field[x0 + y1*TERRAIN_SIZE].y = v1 + 0.5f * (field[x0 + y0*TERRAIN_SIZE].y + field[x0 + y2*TERRAIN_SIZE].y);
			if(!done[x2 + y1*TERRAIN_SIZE])	field[x2 + y1*TERRAIN_SIZE].y = v2 + 0.5f * (field[x2 + y0*TERRAIN_SIZE].y + field[x2 + y2*TERRAIN_SIZE].y);
			if(!done[x1 + y2*TERRAIN_SIZE])	field[x1 + y2*TERRAIN_SIZE].y = v3 + 0.5f * (field[x0 + y2*TERRAIN_SIZE].y + field[x2 + y2*TERRAIN_SIZE].y);
			if(!done[x1 + y1*TERRAIN_SIZE])	field[x1 + y1*TERRAIN_SIZE].y = v4 + 0.5f * (field[x0 + y1*TERRAIN_SIZE].y + field[x2 + y1*TERRAIN_SIZE].y);

			done[x1 + y0*TERRAIN_SIZE] = true;
			done[x0 + y1*TERRAIN_SIZE] = true;
			done[x2 + y1*TERRAIN_SIZE] = true;
			done[x1 + y2*TERRAIN_SIZE] = true;
			done[x1 + y1*TERRAIN_SIZE] = true;

			// Recurse through 4 corners
			value *= 0.5f;
			_Compute(done, field, x0,	y0,	size, value);
			_Compute(done, field, x0,	y1,	size, value);
			_Compute(done, field, x1,	y0,	size, value);
			_Compute(done, field, x1,	y1,	size, value);
		}
	};

	// Fractalize
	bool* done = new bool[TERRAIN_NB_VERTS];

	memset(done,0,TERRAIN_NB_VERTS);
	gTerrainVerts[0].y = 10.0f;
	gTerrainVerts[TERRAIN_SIZE-1].y = 10.0f;
	gTerrainVerts[TERRAIN_SIZE*(TERRAIN_SIZE-1)].y = 5.0f;
	gTerrainVerts[TERRAIN_NB_VERTS-1].y = 10.0f;
	Local::_Compute(done, gTerrainVerts, 0, 0, TERRAIN_SIZE, TERRAIN_CHAOS);
	for(NxU32 i=0;i<TERRAIN_NB_VERTS;i++)
		{
		gTerrainVerts[i].y += TERRAIN_OFFSET;

		//clamp terrain to positives to create a flat area for the vehicles to spawn on
		if (i % TERRAIN_SIZE > TERRAIN_SIZE*0.5f && i < TERRAIN_NB_VERTS*0.8f && i > TERRAIN_NB_VERTS*0.3f)
			gTerrainVerts[i].y = 0.0f;
		}

	delete[] done;
	

	// Initialize terrain faces
	gTerrainFaces = new NxU32[TERRAIN_NB_FACES*3];

	NxU32 k = 0;
	for(NxU32 j=0;j<TERRAIN_SIZE-1;j++)
	{
		for(NxU32 i=0;i<TERRAIN_SIZE-1;i++)
		{
			// Create first triangle
			gTerrainFaces[k] = i   + j*TERRAIN_SIZE;
			gTerrainFaces[k+1] = i   + (j+1)*TERRAIN_SIZE;
			gTerrainFaces[k+2] = i+1 + (j+1)*TERRAIN_SIZE;

			//while we're at it do some smoothing of the random terrain because its too rough to do a good demo of this effect.
			//smoothTriangle(gTerrainFaces[k],gTerrainFaces[k+1],gTerrainFaces[k+2]);
			k+=3;
			// Create second triangle
			gTerrainFaces[k] = i   + j*TERRAIN_SIZE;
			gTerrainFaces[k+1] = i+1 + (j+1)*TERRAIN_SIZE;
			gTerrainFaces[k+2] = i+1 + j*TERRAIN_SIZE;

			//smoothTriangle(gTerrainFaces[k],gTerrainFaces[k+1],gTerrainFaces[k+2]);
			k+=3;
		}
	}

	//allocate terrain materials -- one for each face.
	gTerrainMaterials = new NxMaterialIndex[TERRAIN_NB_FACES];

	NxReal lastSteepness = 0.0f;
	for(NxU32 f=0;f<TERRAIN_NB_FACES;f++)
		{
		//new: generate material indices for all the faces
		lastSteepness = chooseTrigMaterial(f, lastSteepness);
		}
	// Build vertex normals
	gTerrainNormals = new NxVec3[TERRAIN_NB_VERTS];
	NxBuildSmoothNormals(TERRAIN_NB_FACES, TERRAIN_NB_VERTS, gTerrainVerts, gTerrainFaces, NULL, gTerrainNormals, true);

	// Build physical model
	NxTriangleMeshDesc terrainDesc;
	terrainDesc.numVertices					= TERRAIN_NB_VERTS;
	terrainDesc.numTriangles				= TERRAIN_NB_FACES;
	terrainDesc.pointStrideBytes			= sizeof(NxVec3);
	terrainDesc.triangleStrideBytes			= 3*sizeof(NxU32);
	terrainDesc.points						= gTerrainVerts;
	terrainDesc.triangles					= gTerrainFaces;							
	terrainDesc.flags						= 0;
	//add the mesh material data:
	terrainDesc.materialIndexStride			= sizeof(NxMaterialIndex);
	terrainDesc.materialIndices				= gTerrainMaterials;

	terrainDesc.heightFieldVerticalAxis		= NX_Y;
	terrainDesc.heightFieldVerticalExtent	= -1000.0f;

	NxTriangleMeshShapeDesc terrainShapeDesc;
	InitCooking();
 	MemoryWriteBuffer buf;
  	bool status = CookTriangleMesh(terrainDesc, buf);
  	MemoryReadBuffer readBuffer(buf.data);
  	terrainShapeDesc.meshData = gPhysicsSDK->createTriangleMesh(readBuffer);
	//
	// Please note about the created Triangle Mesh, user needs to release it when no one uses it to save memory. It can be detected
	// by API "meshData->getReferenceCount() == 0". And, the release API is "gPhysicsSDK->releaseTriangleMesh(*meshData);"
	//
	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&terrainShapeDesc);
	gTerrain = gScene->createActor(actorDesc);
	gTerrain->userData = (void*)0;
	CloseCooking();
}

void RenderTerrain()
{
	static float* pVertList = NULL;
	static float* pNormList = NULL;
	static float* pColorList = NULL;

	if(pVertList == NULL && pNormList == NULL && pColorList == NULL)
	{
		pVertList = new float[TERRAIN_NB_FACES*3*3];
		pNormList = new float[TERRAIN_NB_FACES*3*3];
		pColorList = new float[TERRAIN_NB_FACES*3*4];

		int vertIndex = 0;
		int normIndex = 0;
		int colorIndex = 0;
		for(int i=0;i<TERRAIN_NB_FACES;i++)
		{
			NxMaterialIndex mat = gTerrainMaterials[i];
			float r,g,b;

			if(mat == materialIce)
			{
				r=1; g=1; b=1;
			} else {
				if(mat == materialRock)
				{
					r=0.3f; g=0.3f; b=0.3f;
				} else {
					if (mat == materialMud)
					{
						r=0.6f; g=0.3f; b=0.2f;
					} else {
						r=0.2f; g=0.8f; b=0.2f;
					}
				}
			}

			for(int j=0;j<3;j++)
			{
				pVertList[vertIndex++] = gTerrainVerts[gTerrainFaces[i*3+j]].x;
				pVertList[vertIndex++] = gTerrainVerts[gTerrainFaces[i*3+j]].y;
				pVertList[vertIndex++] = gTerrainVerts[gTerrainFaces[i*3+j]].z;

				pNormList[normIndex++] = gTerrainNormals[gTerrainFaces[i*3+j]].x;
				pNormList[normIndex++] = gTerrainNormals[gTerrainFaces[i*3+j]].y;
				pNormList[normIndex++] = gTerrainNormals[gTerrainFaces[i*3+j]].z;

				pColorList[colorIndex++] = r;
				pColorList[colorIndex++] = g;
				pColorList[colorIndex++] = b;
				pColorList[colorIndex++] = 1.0f;

			}
		}
	}

	if(pVertList != NULL && pNormList != NULL && pColorList != NULL)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
				
#ifdef __PPCGEKKO__
		glVertexPointer(3,GL_FLOAT, 0, TERRAIN_NB_FACES*3, pVertList);
		glNormalPointer(GL_FLOAT, 0, TERRAIN_NB_FACES*3, pNormList);
		glColorPointer(4, GL_FLOAT, 0, TERRAIN_NB_FACES*3, pColorList);
#else
		glVertexPointer(3,GL_FLOAT, 0, pVertList);
		glNormalPointer(GL_FLOAT, 0, pNormList);
		glColorPointer(4, GL_FLOAT, 0, pColorList);
#endif		
		glDrawArrays(GL_TRIANGLES, 0, TERRAIN_NB_FACES*3);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
}

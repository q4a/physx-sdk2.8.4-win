// ===============================================================================
//
//						   NVIDIA PhysX SDK Sample Program
//
// Title: Raycast Sample
// Description: This sample program shows the raycast functionality of the NVIDIA
//              PhysX SDK. There are two types of raycasts performed, the vertical
//              raycasts stop at the first hit and the horizontal raycast reports
//              all shapes it hits. The dynamic shapes found by the horizontal
//              raycast are colored in magenta during the frames in which there is
//              contact with the raycast.
//              Scene queries can be performed either in synchronous or asynchronous
//              mode. This sample demonstrates both uses, you need to change a define
//              in order to select which version you want to use, see below.
//
// ===============================================================================

//Specify whether or not to use asynchronous raycast queries (other alternative
//is to use synchronous queries)
#define SAMPLE_USE_ASYNCHRONOUS

#define NOMINMAX
#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <stdio.h>

// Physics code
#undef random
#include "NxPhysics.h"
#include "ErrorStream.h"
#include "cooking.h"
#include "Timing.h"
#include "Stream.h"
#include "Utilities.h"
#include "SamplesVRDSettings.h"
#include "UserAllocator.h"
#include "common.h"

#ifdef __PPCGEKKO__
#include "GLFontRenderer.h"
#endif

static bool				gPause = false;
static bool				gUpdateColors = true;
static NxPhysicsSDK*	gPhysicsSDK = NULL;
static NxScene*			gScene = NULL;
static NxSceneQuery*    gSceneQueryObject = NULL;
static NxRaycastHit     gHits[100];
static NxVec3			gDefaultGravity(0.0f, -9.81f, 0.0f);
static ErrorStream		gErrorStream;
static NxBounds3		gSceneBounds;
static NxF32			gElapsedTime = 0.0f;
static UserAllocator*	gAllocator = NULL;

// Render code
static NxVec3 Eye(130.0f, 100.0f, 130.0f);
static NxVec3 Dir(-0.65,-0.4,-0.6);
static NxVec3 N;
static int mx = 0;
static int my = 0;

#define TERRAIN_SIZE		65
#define TERRAIN_NB_VERTS	TERRAIN_SIZE*TERRAIN_SIZE
#define TERRAIN_NB_FACES	(TERRAIN_SIZE-1)*(TERRAIN_SIZE-1)*2
#define TERRAIN_OFFSET		-20.0f
#define TERRAIN_WIDTH		3.0f
#define TERRAIN_CHAOS		30.0f
#define ONE_OVER_RAND_MAX	(1.0f / float(RAND_MAX))
#define HALF_RAND_MAX       (RAND_MAX >> 1)
static NxVec3* gTerrainVerts = NULL;
static NxVec3* gTerrainNormals = NULL;
static NxU32* gTerrainFaces = NULL;
static NxVec3* gTerrainColors = NULL;

static void InitTerrain()
{
	// Initialize terrain vertices
	gTerrainVerts = new NxVec3[TERRAIN_NB_VERTS];
	NxU32 y;
	for(y=0;y<TERRAIN_SIZE;y++)
	{
		for(NxU32 x=0;x<TERRAIN_SIZE;x++)
		{
			gTerrainVerts[x+y*TERRAIN_SIZE] = NxVec3(NxF32(x)-(NxF32(TERRAIN_SIZE-1)*0.5f), 0.0f, NxF32(y)-(NxF32(TERRAIN_SIZE-1)*0.5f)) * TERRAIN_WIDTH;
		}
	}

	// Initialize terrain faces
	gTerrainFaces = new NxU32[TERRAIN_NB_FACES*3];
	NxU32 k = 0;
	for(NxU32 j=0;j<TERRAIN_SIZE-1;j++)
	{
		for(NxU32 i=0;i<TERRAIN_SIZE-1;i++)
		{
			// Create first triangle
			gTerrainFaces[k++] = i   + j*TERRAIN_SIZE;
			gTerrainFaces[k++] = i   + (j+1)*TERRAIN_SIZE;
			gTerrainFaces[k++] = i+1 + (j+1)*TERRAIN_SIZE;

			// Create second triangle
			gTerrainFaces[k++] = i   + j*TERRAIN_SIZE;
			gTerrainFaces[k++] = i+1 + (j+1)*TERRAIN_SIZE;
			gTerrainFaces[k++] = i+1 + j*TERRAIN_SIZE;
		}
	}

	// Initialize terrain colors
	gTerrainColors = new NxVec3[TERRAIN_NB_VERTS];
	for(y=0;y<TERRAIN_SIZE;y++)
	{
		for(NxU32 x=0;x<TERRAIN_SIZE;x++)
		{
			gTerrainColors[x+y*TERRAIN_SIZE].zero();
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
			NxF32 v0 = (value * NxF32(rand()-HALF_RAND_MAX) * ONE_OVER_RAND_MAX);
			NxF32 v1 = (value * NxF32(rand()-HALF_RAND_MAX) * ONE_OVER_RAND_MAX);
			NxF32 v2 = (value * NxF32(rand()-HALF_RAND_MAX) * ONE_OVER_RAND_MAX);
			NxF32 v3 = (value * NxF32(rand()-HALF_RAND_MAX) * ONE_OVER_RAND_MAX);
			NxF32 v4 = (value * NxF32(rand()-HALF_RAND_MAX) * ONE_OVER_RAND_MAX);

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

	memset(done, 0, TERRAIN_NB_VERTS);
	gTerrainVerts[0].y = 10.0f;
	gTerrainVerts[TERRAIN_SIZE-1].y = 10.0f;
	gTerrainVerts[TERRAIN_SIZE*(TERRAIN_SIZE-1)].y = 10.0f;
	gTerrainVerts[TERRAIN_NB_VERTS-1].y = 10.0f;
	Local::_Compute(done, gTerrainVerts, 0, 0, TERRAIN_SIZE, TERRAIN_CHAOS);
	for(NxU32 i=0;i<TERRAIN_NB_VERTS;i++)	gTerrainVerts[i].y += TERRAIN_OFFSET;

	delete[] done;

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

	terrainDesc.heightFieldVerticalAxis		= NX_Y;
	terrainDesc.heightFieldVerticalExtent	= -1000.0f;

	NxTriangleMeshShapeDesc terrainShapeDesc;

	bool status = InitCooking(gAllocator);
	if (!status) {
		printf("Unable to initialize the cooking library. Please make sure that you have correctly installed the latest version of the NVIDIA PhysX SDK.");
		exit(1);
	}

	MemoryWriteBuffer buf;
	status = CookTriangleMesh(terrainDesc, buf);
	if (!status) {
		printf("Unable to cook a triangle mesh.");
		exit(1);
	}

	MemoryReadBuffer readBuffer(buf.data);
	terrainShapeDesc.meshData = gPhysicsSDK->createTriangleMesh(readBuffer);
	//
	// Please note about the created Triangle Mesh, user needs to release it when no one uses it to save memory. It can be detected
	// by API "meshData->getReferenceCount() == 0". And, the release API is "gPhysicsSDK->releaseTriangleMesh(*meshData);"
	//

	NxActorDesc ActorDesc;
	ActorDesc.shapes.pushBack(&terrainShapeDesc);
	gScene->createActor(ActorDesc)->userData = (void*)0;

	CloseCooking();
}

static void UpdateColors(bool doShadows)
{
	if(!gUpdateColors)	return;

	// Not really a recommended way to do shadows!
	NxRay ray;
	ray.dir = NxVec3(0.0f, 1.0f, -1.0f);
	ray.dir.normalize();

	// Cast ray for each terrain vertex
	for(int y=0;y<TERRAIN_SIZE;y++)
	{
		for(int x=0;x<TERRAIN_SIZE;x++)
		{
			NxVec3 color;
			ray.orig = gTerrainVerts[x+y*TERRAIN_SIZE];

			if(doShadows && gScene->raycastAnyShape(ray, NX_DYNAMIC_SHAPES))	color.zero();
			else												color = NxVec3(0.5f, 0.4f, 0.2f);

			gTerrainColors[x+y*TERRAIN_SIZE] = color;
		}
	}

	// Blur
	for(int i=0;i<4;i++)
	{
		NxVec3* smoothed = (NxVec3*)malloc(sizeof(NxVec3)*TERRAIN_NB_VERTS);
		memset(smoothed, 0, sizeof(NxVec3)*TERRAIN_NB_VERTS);

		const NxF32 coeff = 1.0f / 5.0f;
		NxU32 offset = TERRAIN_SIZE+1;
		for(NxU32 y=1;y<TERRAIN_SIZE-1;y++)
		{
			for(NxU32 x=1;x<TERRAIN_SIZE-1;x++)
			{
				smoothed[offset]=
				gTerrainColors[offset]*coeff +
				gTerrainColors[offset-TERRAIN_SIZE]*coeff +
				gTerrainColors[offset-1]*coeff +
				gTerrainColors[offset+1]*coeff +
				gTerrainColors[offset+TERRAIN_SIZE]*coeff;

				offset++;
			}
			offset+=2;
		}
		memcpy(gTerrainColors, smoothed, sizeof(NxVec3)*TERRAIN_NB_VERTS);
		free(smoothed);
	}
}

static void RenderTerrain()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	float* pVertList = new float[TERRAIN_NB_FACES*3*3];
	float* pNormList = new float[TERRAIN_NB_FACES*3*3];
	float* pColorList = new float[TERRAIN_NB_FACES*4*3];

	int vertIndex = 0;
	int normIndex = 0;
	int colorIndex = 0;
	for(int i=0;i<TERRAIN_NB_FACES;i++)
	{
		for(int j=0;j<3;j++)
		{
		pVertList[vertIndex++] = gTerrainVerts[gTerrainFaces[i*3+j]].x;
        pVertList[vertIndex++] = gTerrainVerts[gTerrainFaces[i*3+j]].y;
        pVertList[vertIndex++] = gTerrainVerts[gTerrainFaces[i*3+j]].z;

        pNormList[normIndex++] = gTerrainNormals[gTerrainFaces[i*3+j]].x;
        pNormList[normIndex++] = gTerrainNormals[gTerrainFaces[i*3+j]].y;
        pNormList[normIndex++] = gTerrainNormals[gTerrainFaces[i*3+j]].z;

		pColorList[colorIndex++] = gTerrainColors[gTerrainFaces[i*3+j]].x;
		pColorList[colorIndex++] = gTerrainColors[gTerrainFaces[i*3+j]].y;
		pColorList[colorIndex++] = gTerrainColors[gTerrainFaces[i*3+j]].z;
		pColorList[colorIndex++] = 1.0f;
		}
	}

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(3,GL_FLOAT, 0, TERRAIN_NB_FACES*3, pVertList);    
	glNormalPointer(GL_FLOAT, 0, TERRAIN_NB_FACES*3, pNormList);
    glColorPointer(4,GL_FLOAT, 0, TERRAIN_NB_FACES*3, pColorList);	
    
    glDrawArrays(GL_TRIANGLES, 0, TERRAIN_NB_FACES*3);
	glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

	delete[] pVertList;
	delete[] pNormList;
	delete[] pColorList;
}

class myRaycastQueryReport : public NxSceneQueryReport
{
	virtual	NxQueryReportResult	onBooleanQuery(void* userData, bool result){ return NX_SQR_CONTINUE; };
	virtual	NxQueryReportResult	onShapeQuery(void* userData, NxU32 nbHits, NxShape** hits){ return NX_SQR_CONTINUE; };
	virtual	NxQueryReportResult	onSweepQuery(void* userData, NxU32 nbHits, NxSweepQueryHit* hits){ return NX_SQR_CONTINUE; };

	virtual	NxQueryReportResult	onRaycastQuery(void* userData, NxU32 nbHits, const NxRaycastHit* hits)
	{
		size_t i = (size_t)userData;
		if (nbHits > 0) {
			gHits[i] = hits[0];
		} else {
			gHits[i].shape = NULL;
		}
		return NX_SQR_CONTINUE;
	}
}gQueryReport;

//Performs the "vertical" raycasts. The raycasts are asked to only return the closest hit.
static void RaycastClosestShapes()
{
	static NxF32 t = 0.0f;
	if(!gPause)	t += gElapsedTime;
	NxF32 offset = cosf(t) * 100.0f;

	glDisable(GL_LIGHTING);

	NxRay worldRay;
	worldRay.dir = NxVec3(0.0f, -1.0f, 0.0f);

	for(NxU32 i=0;i<100;i++)
	{
		worldRay.orig = NxVec3((NxF32(i)-49.5f)*2.0f, 100.0f, 0.0f);
		worldRay.orig.z += offset;
		gHits[i].shape = NULL;
		NxRaycastHit hit;
		gSceneQueryObject->raycastClosestShape(worldRay, NX_ALL_SHAPES, hit, 0xffffffff, NX_MAX_F32, 0xffffffff, NULL, NULL, (void*)(i));
	}

	gSceneQueryObject->execute();
	gSceneQueryObject->finish(true);

	worldRay.dir = NxVec3(0.0f, -1.0f, 0.0f);

	for(NxU32 i=0;i<100;i++)
	{
		worldRay.orig = NxVec3((NxF32(i)-49.5f)*2.0f, 100.0f, 0.0f);
		worldRay.orig.z += offset;
		NxF32 dist;

		NxRaycastHit hit = gHits[i];

		if(hit.shape)
		{
			const NxVec3& worldImpact = hit.worldImpact;
			dist = hit.distance;

			if(hit.faceID != 0)
			{
				NxU32 vref0 = gTerrainFaces[hit.faceID*3+0];
				NxU32 vref1 = gTerrainFaces[hit.faceID*3+1];
				NxU32 vref2 = gTerrainFaces[hit.faceID*3+2];
				const NxVec3& v0 = gTerrainVerts[vref0];
				const NxVec3& v1 = gTerrainVerts[vref1];
				const NxVec3& v2 = gTerrainVerts[vref2];

#ifdef __PPCGEKKO__
				glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
				glBegin(GL_TRIANGLES, 3);
				glVertex3d( v0.x, v0.y, v0.z );
				glVertex3d( v1.x, v1.y, v1.z );
				glVertex3d( v2.x, v2.y, v2.z );
				glEnd();
#else				
				float tris[3][3] = { {v0.x, v0.y, v0.z}, {v1.x, v1.y, v1.z}, {v2.x, v2.y, v2.z}};
				glEnableClientState(GL_VERTEX_ARRAY);				
				glVertexPointer(3, GL_FLOAT, 0, tris);
				glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
				glDrawArrays(GL_TRIANGLES, 0, 3);
				glDisableClientState(GL_VERTEX_ARRAY);
#endif				
			}

#ifdef __PPCGEKKO__
			glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
			glBegin(GL_LINES, 2);
			glVertex3d( worldImpact.x, worldImpact.y, worldImpact.z );
			glVertex3d( worldImpact.x+1.0f, worldImpact.y, worldImpact.z );
			glEnd();
			
			glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
			glBegin(GL_LINES, 2);
			glVertex3d( worldImpact.x, worldImpact.y, worldImpact.z );
			glVertex3d( worldImpact.x, worldImpact.y+1.0f, worldImpact.z );
			glEnd();

			glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
			glBegin(GL_LINES, 2);
			glVertex3d( worldImpact.x, worldImpact.y, worldImpact.z );
			glVertex3d( worldImpact.x, worldImpact.y, worldImpact.z+1.0f );
			glEnd();
#else		
			glEnableClientState(GL_VERTEX_ARRAY);
			float lineX[2][3] = { {worldImpact.x, worldImpact.y, worldImpact.z}, {worldImpact.x+1.0f, worldImpact.y, worldImpact.z}};
			glVertexPointer(3, GL_FLOAT, 0, lineX);
			glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
			glDrawArrays(GL_LINES, 0, 2);
			float lineY[2][3] = { {worldImpact.x, worldImpact.y, worldImpact.z}, {worldImpact.x, worldImpact.y+1.0f, worldImpact.z}};
			glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
			glVertexPointer(3, GL_FLOAT, 0, lineY);
			glDrawArrays(GL_LINES, 0, 2);
			float lineZ[2][3] = { {worldImpact.x, worldImpact.y, worldImpact.z}, {worldImpact.x, worldImpact.y, worldImpact.z+1.0f}};
			glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
			glVertexPointer(3, GL_FLOAT, 0, lineZ);
			glDrawArrays(GL_LINES, 0, 2);
			glDisableClientState(GL_VERTEX_ARRAY);
#endif			
		}
		else dist = 1000.0f;

		glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
#ifdef __PPCGEKKO__
		glBegin(GL_LINES, 2);
			glVertex3d( worldRay.orig.x, worldRay.orig.y, worldRay.orig.z );
			glVertex3d( worldRay.orig.x+worldRay.dir.x*dist, worldRay.orig.y+worldRay.dir.y*dist, worldRay.orig.z+worldRay.dir.z*dist );
		glEnd();
#else		
		glEnableClientState(GL_VERTEX_ARRAY);
		float line[2][3] = {	{worldRay.orig.x, worldRay.orig.y, worldRay.orig.z}, 
								{worldRay.orig.x+worldRay.dir.x*dist, worldRay.orig.y+worldRay.dir.y*dist, worldRay.orig.z+worldRay.dir.z*dist}};
		glVertexPointer(3, GL_FLOAT, 0, line);
		glDrawArrays(GL_LINES, 0, 2);
		glDisableClientState(GL_VERTEX_ARRAY);	
#endif		
	}
	glEnable(GL_LIGHTING);
}

class myRaycastReport : public NxUserRaycastReport
{
	virtual bool onHit(const NxRaycastHit& hit)
	{
		size_t userData = (size_t)hit.shape->getActor().userData;
		userData |= 1;	// Mark as hit
		hit.shape->getActor().userData = (void*)userData;

		return true;
	}
}gMyReport;

//Performs the "horizontal" raycast. The raycast is asked to return all shapes it hits.
static void RaycastAllShapes()
{
	static NxF32 t = 0.0f;
	if(!gPause)	t += gElapsedTime;
	NxF32 offset = cosf(t) * 20.0f;

	NxRay worldRay;
	worldRay.dir = NxVec3(-1.0f, 0.0f, 0.0f);
	worldRay.orig = NxVec3(100, 10.0f+offset, 0.0f);

	gScene->raycastAllShapes(worldRay, gMyReport, NX_ALL_SHAPES);

	glDisable(GL_LIGHTING);

	NxF32 dist = 5000.0f;	// Actually infinite

#ifdef __PPCGEKKO__
		glBegin(GL_LINES, 2);
			glVertex3d( worldRay.orig.x, worldRay.orig.y, worldRay.orig.z );
			glVertex3d( worldRay.orig.x+worldRay.dir.x*dist, worldRay.orig.y+worldRay.dir.y*dist, worldRay.orig.z+worldRay.dir.z*dist );
		glEnd();
#else		
	glEnableClientState(GL_VERTEX_ARRAY);
	float line[2][3] = {	{worldRay.orig.x, worldRay.orig.y, worldRay.orig.z}, 
							{worldRay.orig.x+worldRay.dir.x*dist, worldRay.orig.y+worldRay.dir.y*dist, worldRay.orig.z+worldRay.dir.z*dist}};
	glVertexPointer(3, GL_FLOAT, 0, line);
	glColor4f(1.0f, 0.0f, 1.0f, 1.0f);
	glDrawArrays(GL_LINES, 0, 2);
	glDisableClientState(GL_VERTEX_ARRAY);	
#endif		
	glEnable(GL_LIGHTING);
}

static void CreateCube(const NxVec3& pos, int size=2, const NxVec3* initial_velocity=NULL)
{
	if(!gSceneBounds.contain(pos))
	{
		printf("Trying to create an actor outside of the scene bounds - creation aborted!\n");
		return;
	}

	// Create body
	NxBodyDesc BodyDesc;
	BodyDesc.angularDamping	= 0.5f;
	if(initial_velocity) BodyDesc.linearVelocity = *initial_velocity;

	NxBoxShapeDesc BoxDesc;
	BoxDesc.dimensions = NxVec3(float(size), float(size), float(size));

	NxActorDesc ActorDesc;
	ActorDesc.shapes.pushBack(&BoxDesc);
	ActorDesc.body			= &BodyDesc;
	ActorDesc.density		= 10.0f;
	ActorDesc.globalPose.t  = pos;
	NxActor* a = gScene->createActor(ActorDesc);
	if(a)
	{
		a->userData = (void*)(size<<1);
	}
}

static void CreateStack(int size)
{
	float CubeSize = 2.0f;
	float Spacing = 0.0001f;
	NxVec3 Pos(0.0f, CubeSize, 0.0f);
	float Offset = -size * (CubeSize * 2.0f + Spacing) * 0.5f;
	while(size)
	{
		for(int i=0;i<size;i++)
		{
			Pos.x = Offset + float(i) * (CubeSize * 2.0f + Spacing);
			CreateCube(Pos, (int)CubeSize);
		}
		Offset += CubeSize;
		Pos.y += (CubeSize * 2.0f + Spacing);
		size--;
	}
}

static void CreateTower(int size)
{
	float CubeSize = 2.0f;
	float Spacing = 0.01f;
	NxVec3 Pos(0.0f, CubeSize, 0.0f);
	while(size)
	{
		CreateCube(Pos, (int)CubeSize);
		Pos.y += (CubeSize * 2.0f + Spacing);
		size--;
	}
}

static bool InitNx()
{
	if (!gAllocator)
		gAllocator = new UserAllocator;

	// Initialize PhysicsSDK
	NxPhysicsSDKDesc desc;
	NxSDKCreateError errorCode = NXCE_NO_ERROR;
	gPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, gAllocator, &gErrorStream, desc, &errorCode);
	if(gPhysicsSDK == NULL) 
	{
		printf("\nSDK create error (%d - %s).\nUnable to initialize the PhysX SDK, exiting the sample.\n\n", errorCode, getNxSDKCreateError(errorCode));
		return false;
	}
#if SAMPLES_USE_VRD
	// The settings for the VRD host and port are found in SampleCommonCode/SamplesVRDSettings.h
	if (gPhysicsSDK->getFoundationSDK().getRemoteDebugger() && !gPhysicsSDK->getFoundationSDK().getRemoteDebugger()->isConnected())
		gPhysicsSDK->getFoundationSDK().getRemoteDebugger()->connect(SAMPLES_VRD_HOST, SAMPLES_VRD_PORT, SAMPLES_VRD_EVENTMASK);
#endif

	gPhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.025f);

	// Create a scene
	NxSceneDesc sceneDesc;
	sceneDesc.gravity				= gDefaultGravity;
#ifdef SAMPLE_USE_ASYNCHRONOUS
	//When using asynchronous queries, there must be a background thread to do
	//the raycasting work. The default is 0 background threads.
	sceneDesc.backgroundThreadCount	= 1;
#endif

	// Setting proper scene bounds speeds up raycasts against dynamic actors
	gSceneBounds.min = NxVec3(-150.0f, -50.0f, -150.0f);
	gSceneBounds.max = NxVec3(150.0f, 150.0f, 150.0f);
	sceneDesc.maxBounds		= &gSceneBounds;
	sceneDesc.boundsPlanes	= true;

	gScene = gPhysicsSDK->createScene(sceneDesc);
	if(gScene == NULL) 
	{
		printf("\nError: Unable to create a PhysX scene, exiting the sample.\n\n");
		return false;
	}

	NxMaterial * defaultMaterial = gScene->getMaterialFromIndex(0); 
	defaultMaterial->setRestitution(0.0f);
	defaultMaterial->setStaticFriction(0.5f);
	defaultMaterial->setDynamicFriction(0.5f);

	// Create scene query object
	NxSceneQueryDesc sceneQueryDesc;
	sceneQueryDesc.executeMode = NX_SQE_ASYNCHRONOUS;
	sceneQueryDesc.report = &gQueryReport;
	gSceneQueryObject = gScene->createSceneQuery(sceneQueryDesc);

	InitTerrain();

	return true;
}

static void KeyboardCallback(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:  exit(0); break;
	case ' ': CreateCube(NxVec3(0.0f, 30.0f, 0.0f), 1+(rand()&3)); break;
	case 's': CreateStack(10); break;
	case 'b': CreateStack(30); break;
	case 't': CreateTower(30); break;
	case 'p': gPause = !gPause; break;
	case 'c': if (gUpdateColors)
				  UpdateColors(false); //Redo colors without shadows
		gUpdateColors = !gUpdateColors; 
		break;
	case 101: case '8':	Eye += Dir * 2.0f; break;
	case 103: case '2':	Eye -= Dir * 2.0f; break;
	case 100: case '4':	Eye -= N * 2.0f; break;
	case 102: case '6':	Eye += N * 2.0f; break;
	case 'w':
		{
		NxVec3 t = Eye;
		NxVec3 Vel = Dir;
		Vel.normalize();
		Vel*=200.0f;
		CreateCube(t, 8, &Vel);
		}
		break;
	}
}

static void ArrowKeyCallback(int key, int x, int y)
	{
	KeyboardCallback(key,x,y);
	}
	
static void MouseCallback(int button, int state, int x, int y)
{
	mx = x;
	my = y;
}

static void MotionCallback(int x, int y)
{
	int dx = mx - x;
	int dy = my - y;
	
	Dir.normalize();
	N.cross(Dir,NxVec3(0,1,0));

	NxQuat qx(NxPiF32 * dx * 20/ 180.0f, NxVec3(0,1,0));
	qx.rotate(Dir);
	NxQuat qy(NxPiF32 * dy * 20/ 180.0f, N);
	qy.rotate(Dir);

	mx = x;
	my = y;
}

static void RenderCallback()
{
	if(gScene == NULL) return;
#ifdef __PPCGEKKO__
	gElapsedTime = 1.0/60.0;
#else		
	gElapsedTime = getElapsedTime();
#endif

	// Physics code
	if(!gPause)	
	{
		UpdateColors(true);
		gScene->simulate(1.0f/60.0f);	//Note: a real application would compute and pass the elapsed time here.
		gScene->flushStream();
		gScene->fetchResults(NX_RIGID_BODY_FINISHED, true);
	}
	// ~Physics code

	// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
	// Setup camera
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, (float)glutGet(GLUT_WINDOW_WIDTH)/(float)glutGet(GLUT_WINDOW_HEIGHT), 1.0f, 10000.0f);
	// PT: keep the look-at code in the projection matrix to have a sane lighting setup.
	// *DO* *NOT* *MOVE* *THIS*
	gluLookAt(Eye.x, Eye.y, Eye.z, Eye.x + Dir.x, Eye.y + Dir.y, Eye.z + Dir.z, 0.0f, 1.0f, 0.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	RaycastClosestShapes();
	RaycastAllShapes();

	// Keep physics & graphics in sync
	int nbActors = gScene->getNbActors();
	NxActor** actors = gScene->getActors();
	while(nbActors--)
	{
		NxActor* actor = *actors++;

		size_t userData = reinterpret_cast<size_t>(actor->userData);
		int touched = userData & 1;
		userData &= ~1;
		actor->userData = reinterpret_cast<void *>(userData);

		float size = float(userData>>1);
		if(touched)	glColor4f(1.0f, 0.0f, 1.0f, 1.0f);
		else		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		glPushMatrix();
		float glmat[16];
		actor->getGlobalPose().getColumnMajor44(glmat);
		glMultMatrixf(glmat);
		glutSolidCube(size*2.0f);
		glPopMatrix();
	}

	RenderTerrain();

#ifdef __PPCGEKKO__	
	char buf[256];
    sprintf(buf,
    "Press the keys +, -, 1, 2, and b to create various things.\n"
	"Use the arrow keys to move the camera.\n"
	"Note: this demo is slow because it computes soft shadows \nfor the boxes using\nraycasting!\n"
	"Press a to enable/disable soft shadows\n");
  
    GLFontRenderer::setScreenResolution(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	GLFontRenderer::setColor(0.9f, 1.0f, 0.0f, 1.0f);
	GLFontRenderer::print(0.01, 0.9, 0.036, buf, false, 11, true);   
#endif	

	glutSwapBuffers();	
}

static void ReshapeCallback(int width, int height)
{
	glViewport(0, 0, width, height);
}

static void IdleCallback()
{
	glutPostRedisplay();
}

static void ExitCallback()
{
	if (gPhysicsSDK)
	{
		if (gScene) gPhysicsSDK->releaseScene(*gScene);
		gPhysicsSDK->release();
	}

	if (gAllocator)
	{
		delete gAllocator;
		gAllocator = NULL;
	}
}

int main(int argc, char** argv)
{
#ifdef __PPCGEKKO__
	printf("Press the keys +, -, 1, 2, and b to create various things.\n");
	printf("Use the arrow keys to move the camera.\n");
	printf("Note: this demo is slow because it computes soft shadows for the boxes using\nraycasting!\n");
	printf("Press a to enable/disable soft shadows\n");
#else
	printf("Press the keys w, space, s, b, and t to create various things.\n");
	printf("Use the arrow keys or 2, 4, 6 and 8 to move the camera, mouse to look around.\n");
	printf("Note: this demo is slow because it computes soft shadows for the boxes using\nraycasting!\n");
	printf("Press c to enable/disable soft shadows\n");
	printf("Press p to pause\n");
#endif

	// Initialize Glut
	glutInit(&argc, argv);
	glutInitWindowSize(512, 512);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	int mainHandle = glutCreateWindow("SampleRaycast");
	glutSetWindow(mainHandle);
	glutDisplayFunc(RenderCallback);
	glutReshapeFunc(ReshapeCallback);
	glutIdleFunc(IdleCallback);
	glutKeyboardFunc(KeyboardCallback);
	glutSpecialFunc(ArrowKeyCallback);
	glutMouseFunc(MouseCallback);
	glutMotionFunc(MotionCallback);
	MotionCallback(0,0);
#ifdef __PPCGEKKO__	
	glutRemapButtonExt(GLUT_KEY_UP, '8');
	glutRemapButtonExt(GLUT_KEY_DOWN, '2');
	glutRemapButtonExt(GLUT_KEY_LEFT, '4');
	glutRemapButtonExt(GLUT_KEY_RIGHT, '6');
	glutRemapButtonExt('b', 'w');
	glutRemapButtonExt('+', ' ');
	glutRemapButtonExt('-', 's');
	glutRemapButtonExt('1', 'b');
	glutRemapButtonExt('2', 't');
	glutRemapButtonExt('a', 'c');
#endif

	// Setup default render states
	glClearColor(0.3f, 0.4f, 0.5f, 1.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
#ifndef __PPCGEKKO__	
	glEnable(GL_CULL_FACE);
#endif
	// Setup lighting
	glEnable(GL_LIGHTING);
	float AmbientColor[] = { 0.0f, 0.1f, 0.2f, 0.0f };		glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientColor);
	float DiffuseColor[] = { 1.0f, 1.0f, 1.0f, 0.0f };		glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseColor);
	float SpecularColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };		glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularColor);
	float Position[] = { -10.0f, 100.0f, -4.0f, 1.0f };		glLightfv(GL_LIGHT0, GL_POSITION, Position);
	glEnable(GL_LIGHT0);

	// Initialize physics scene and start the application main loop if scene was created
	if (InitNx())
	{
		atexit(ExitCallback);
		glutMainLoop();
	}

	return 0;
}

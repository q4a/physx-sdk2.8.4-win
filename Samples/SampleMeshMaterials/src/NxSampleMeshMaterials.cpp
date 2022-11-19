// ===============================================================================
//
//						   NVIDIA PhysX SDK Sample Program
//
// Title: Mesh Materials Sample
// Description: This sample builds on the SampleTerrain program and demonstrates how 
//              the user can assign a different material to various triangles in a 
//              triangle mesh (this is the same technique as for heightfields and 
//              other meshes)
//              
//              We make a terrain, and then based on the slope, of each triangle, 
//              we assign one of four different materials: ice, rock, mud, and grass.
//              These materials are rendered with appropriate colors to better 
//              visualize the effect.
//              
//              Boxes thrown onto the terrain by pressing 'w' will react accordingly. 
//              This means that the different materials result in different sliding 
//              and bouncing behavior.
//              
//              Additionally, you can query the material type at a certain point 
//              and use this information as a special effect.  This is demonstrated 
//              by creating small boxes that represent ice particles when a box 
//              collides with an ice surface. 
//
// ===============================================================================

#ifdef WIN32
#define NOMINMAX
#include <windows.h>
#endif

#include <stdio.h>
#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
// Physics code
#undef random
#include "NxPhysics.h"
#include "cooking.h"
#include "Stream.h"
#include "Utilities.h"
#include "SamplesVRDSettings.h"
#include "CustomRandom.h"
#include "UserAllocator.h"
#include "common.h"

#ifdef __PPCGEKKO__
#include "GLFontRenderer.h"
#endif

static bool				gPause = false;
static NxPhysicsSDK*	gPhysicsSDK = NULL;
static NxScene*			gScene = NULL;
static NxVec3			gDefaultGravity(0.0f, -98.1f, 0.0f);
static UserAllocator*	gAllocator = NULL;

#define TERRAIN_SIZE		33
#define TERRAIN_NB_VERTS	TERRAIN_SIZE*TERRAIN_SIZE
#define TERRAIN_NB_FACES	(TERRAIN_SIZE-1)*(TERRAIN_SIZE-1)*2
#define TERRAIN_OFFSET		-20.0f
#define TERRAIN_WIDTH		20.0f
#define TERRAIN_CHAOS		150.0f
static NxVec3* gTerrainVerts = NULL;
static NxVec3* gTerrainNormals = NULL;
static NxU32* gTerrainFaces = NULL;
static NxMaterialIndex * gTerrainMaterials = NULL;
static NxMaterialIndex materialIce, materialRock, materialMud, materialGrass, materialDefault = 0;
static NxActor * gTerrain = NULL;
static NxArray<NxVec3> contactBuffer;
static NxTriangleMeshShape* gMeshShape = NULL;

float trand()
	{
	static Random random(4095);
	return ((float)random.rand()/((float)random.rand_max+1))*(2.0f) -1.0f;
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

void chooseTrigMaterial(NxU32 faceIndex)
	{
	NxVec3 & v0 = gTerrainVerts[gTerrainFaces[faceIndex * 3]];
	NxVec3 & v1 = gTerrainVerts[gTerrainFaces[faceIndex * 3 + 1]];
	NxVec3 & v2 = gTerrainVerts[gTerrainFaces[faceIndex * 3 + 2]];

	NxVec3 edge0 = v1 - v0;
	NxVec3 edge1 = v2 - v0;

	NxVec3 normal = edge0.cross(edge1);
	normal.normalize();
	NxReal steepness = 1.0f - normal.y;

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
	}

static void InitTerrain()
{
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
			NxF32 v0 = value * trand();
			NxF32 v1 = value * trand();
			NxF32 v2 = value * trand();
			NxF32 v3 = value * trand();
			NxF32 v4 = value * trand();

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
	gTerrainVerts[TERRAIN_SIZE*(TERRAIN_SIZE-1)].y = 10.0f;
	gTerrainVerts[TERRAIN_NB_VERTS-1].y = 10.0f;
	Local::_Compute(done, gTerrainVerts, 0, 0, TERRAIN_SIZE, TERRAIN_CHAOS);
	for(NxU32 i=0;i<TERRAIN_NB_VERTS;i++)	
		gTerrainVerts[i].y += TERRAIN_OFFSET;

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
			smoothTriangle(gTerrainFaces[k],gTerrainFaces[k+1],gTerrainFaces[k+2]);
			k+=3;
			// Create second triangle
			gTerrainFaces[k] = i   + j*TERRAIN_SIZE;
			gTerrainFaces[k+1] = i+1 + (j+1)*TERRAIN_SIZE;
			gTerrainFaces[k+2] = i+1 + j*TERRAIN_SIZE;

			smoothTriangle(gTerrainFaces[k],gTerrainFaces[k+1],gTerrainFaces[k+2]);
			k+=3;
		}
	}

	//allocate terrain materials -- one for each face.
	gTerrainMaterials = new NxMaterialIndex[TERRAIN_NB_FACES];

	for(NxU32 f=0;f<TERRAIN_NB_FACES;f++)
		{
		//new: generate material indices for all the faces
		chooseTrigMaterial(f);
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
	terrainShapeDesc.shapeFlags |= NX_SF_FEATURE_INDICES;

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
	gTerrain = gScene->createActor(ActorDesc);
	gMeshShape = (NxTriangleMeshShape*)gTerrain->getShapes()[0];
	gTerrain->userData = (void*)0;

	CloseCooking();
}

static void RenderTerrain()
{
	static float* pVertList = NULL;
	static float* pNormList = NULL;
	static float* pColorList = NULL;

	if(pVertList == NULL && pNormList == NULL)
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

	if(pVertList != NULL && pNormList != NULL)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		
		glVertexPointer(3,GL_FLOAT, 0, TERRAIN_NB_FACES*3, pVertList);		
		glNormalPointer(GL_FLOAT, 0, TERRAIN_NB_FACES*3, pNormList);		
		glColorPointer(4, GL_FLOAT, 0, TERRAIN_NB_FACES*3, pColorList);
	
		glDrawArrays(GL_TRIANGLES, 0, TERRAIN_NB_FACES*3);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
	}
}

static void CreateCube(const NxVec3& pos, int size=2, const NxVec3* initial_velocity=NULL)
{
	// Create body
	NxBodyDesc BodyDesc;
	BodyDesc.angularDamping	= 0.5f;
//	BodyDesc.maxAngularVelocity	= 10.0f;
	if(initial_velocity)	BodyDesc.linearVelocity = *initial_velocity;

	NxBoxShapeDesc BoxDesc;
	BoxDesc.dimensions		= NxVec3(float(size), float(size), float(size));

	NxActorDesc ActorDesc;
	ActorDesc.shapes.pushBack(&BoxDesc);
	ActorDesc.body			= &BodyDesc;
	ActorDesc.density		= 10.0f;
	ActorDesc.globalPose.t  = pos;
	NxActor * cube = gScene->createActor(ActorDesc);
	cube->userData = (void*)size;
	gScene->setActorPairFlags(*cube, *gTerrain, NX_NOTIFY_ON_START_TOUCH|NX_NOTIFY_ON_TOUCH|NX_NOTIFY_ON_END_TOUCH);
}

static void CreateStack(int size)
{
	float CubeSize = 2.0f;
//	float Spacing = 0.01f;
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


class MyContactReport : public NxUserContactReport
{
	public:

	virtual NxU32 onPairCreated(NxActor& s1, NxActor& s2)
		{
		return NX_NOTIFY_ON_START_TOUCH;
		}

	virtual void  onContactNotify(NxContactPair& pair, NxU32 events)
		{
		// Iterate through contact points
		NxContactStreamIterator i(pair.stream);
		//user can call getNumPairs() here
		while(i.goNextPair())
			{
			//user can also call getShape() and getNumPatches() here
			NxU32 meshId = 2;
			if (i.getShape(0) == (NxShape*)gMeshShape)
				meshId = 0;
			else if (i.getShape(1) == (NxShape*)gMeshShape)
				meshId = 1;
			while(i.goNextPatch())
				{
				//user can also call getPatchNormal() and getNumPoints() here
				const NxVec3& contactNormal = i.getPatchNormal();
				while(i.goNextPoint())
					{
					//user can also call getPoint() and getSeparation() here
					const NxVec3& contactPoint = i.getPoint();
					if (meshId != 2)					
						{
						NxU32 featureIndex = (meshId == 0) ? i.getFeatureIndex0() : i.getFeatureIndex1();
						NxMaterialIndex meshMaterial = materialDefault;
						if (featureIndex != 0xffffffff)
							meshMaterial = gTerrainMaterials[featureIndex];
						if (meshMaterial == materialIce)
							contactBuffer.pushBack(contactPoint);
						}
					}
				}
			}
	}

}gMyContactReport;


class MyErrorStream : public NxUserOutputStream
	{
	public:
	void reportError(NxErrorCode e, const char * message, const char *file, int line)
		{
		printf("%s (%d) :", file, line);
		switch (e)
			{
			case NXE_INVALID_PARAMETER:
				printf( "invalid parameter");
				break;
			case NXE_INVALID_OPERATION:
				printf( "invalid operation");
				break;
			case NXE_OUT_OF_MEMORY:
				printf( "out of memory");
				break;
			case NXE_DB_INFO:
				printf( "info");
				break;
			case NXE_DB_WARNING:
				printf( "warning");
				break;
			default:
				printf("unknown error");
			}

		printf(" : %s\n", message);
		}

	NxAssertResponse reportAssertViolation(const char * message, const char *file, int line)
		{
		printf("access violation : %s (%s line %d)\n", message, file, line);
#ifdef WIN32
		switch (MessageBox(0, message, "AssertViolation, see console for details.", MB_ABORTRETRYIGNORE))
			{
			case IDRETRY:
				return NX_AR_CONTINUE;
			case IDIGNORE:
				return NX_AR_IGNORE;
			case IDABORT:
			default:
				return NX_AR_BREAKPOINT;
			}
#else
	return NX_AR_BREAKPOINT;
#endif
		}

	void print(const char * message)
		{
		printf(message);
		}

	} 
myErrorStream;

static bool InitNx()
{
	if (!gAllocator)
		gAllocator = new UserAllocator;

	// Initialize PhysicsSDK
	NxPhysicsSDKDesc desc;
	NxSDKCreateError errorCode = NXCE_NO_ERROR;
	gPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, gAllocator, &myErrorStream, desc, &errorCode);
	if(gPhysicsSDK == NULL) 
	{
		printf("\nSDK create error (%d - %s).\nUnable to initialize the PhysX SDK, exiting the sample.\n\n", errorCode, getNxSDKCreateError(errorCode));
		return false;
	}
#if SAMPLES_USE_VRD
	// The settings for the VRD host and port are found in SampleCommonCode/SamplesVRDSettings.h
	if (gPhysicsSDK->getFoundationSDK().getRemoteDebugger())
		gPhysicsSDK->getFoundationSDK().getRemoteDebugger()->connect(SAMPLES_VRD_HOST, SAMPLES_VRD_PORT, SAMPLES_VRD_EVENTMASK);
#endif

	gPhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.025f);

	// Create a scene
	NxSceneDesc sceneDesc;
	sceneDesc.gravity				= gDefaultGravity;
	sceneDesc.userContactReport		= &gMyContactReport;

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
		
	NxMaterialDesc	m;

	//terrain materials:

	m.restitution		= 1.0f;
	m.staticFriction	= 0.0f;
	m.dynamicFriction	= 0.0f;
	materialIce = gScene->createMaterial(m)->getMaterialIndex();
	m.restitution		= 0.3f;
	m.staticFriction	= 1.2f;
	m.dynamicFriction	= 1.0f;
	materialRock = gScene->createMaterial(m)->getMaterialIndex();
	m.restitution		= 0.0f;
	m.staticFriction	= 3.0f;
	m.dynamicFriction	= 1.0f;
	materialMud = gScene->createMaterial(m)->getMaterialIndex();
	m.restitution		= 0.0f;
	m.staticFriction	= 0.0f;
	m.dynamicFriction	= 0.0f;
	materialGrass = gScene->createMaterial(m)->getMaterialIndex();


	InitTerrain();

	return true;
}

static void createParticles()
	{

	for(NxArray<NxVec3>::Iterator i = contactBuffer.begin(); i != contactBuffer.end(); i++)
		{
		NxBodyDesc BodyDesc;
		BodyDesc.linearVelocity.set(0,10.0f, 0);

		NxBoxShapeDesc BoxDesc;
		BoxDesc.dimensions		= NxVec3(1,1,1);
		BoxDesc.materialIndex	= materialMud;

		NxActorDesc ActorDesc;
		ActorDesc.shapes.pushBack(&BoxDesc);
		ActorDesc.body			= &BodyDesc;
		ActorDesc.density		= 1.0f;
		ActorDesc.globalPose.t  = *i;
		gScene->createActor(ActorDesc)->userData = (void*)1;
		}
	contactBuffer.clear();
	}

// Render code

static int gMainHandle;
static NxVec3 Eye(0.0f, 50.0f, -300.0f);
static NxVec3 Dir(0,0,1);
static NxVec3 N;
static int mx = 0;
static int my = 0;

static void KeyboardCallback(unsigned char key, int x, int y)
{

	switch (key)
	{
	case 27:	exit(0); break;
	case ' ':			CreateCube(NxVec3(0.0f, 20.0f, 0.0f), 1+(rand()&3)); break;
	case 's':			CreateStack(10); break;
	case 'b':			CreateStack(30); break;
	case 't':			CreateTower(30); break;
	case 'p':			gPause = !gPause; break;
	case 101: case '8':	Eye += Dir * 2.0f; break;
	case 103: case '2':	Eye -= Dir * 2.0f; break;
	case 100: case '4':	Eye -= N * 2.0f; break;
	case 102: case '6':	Eye += N * 2.0f; break;
	case 'w':
		{
		NxVec3 t = Eye;
		NxVec3 Vel = Dir;
		Vel.normalize();
		Vel*=180.0f;
		CreateCube(t, 3, &Vel);
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

static void setupGLMatrix(const NxMat34& pose)
	{
	float glmat[16];
	pose.M.getColumnMajorStride4(&(glmat[0]));
	pose.t.get(&(glmat[12]));
	glmat[3] = glmat[7] = glmat[11] = 0.0f;
	glmat[15] = 1.0f;
	glMultMatrixf(&(glmat[0]));
	}


static void RenderCallback()
{
	float glmat[16];
		
	// Physics code
	if(gScene && !gPause)	
		{
		gScene->simulate(1.0f/60.0f);	//Note: a real application would compute and pass the elapsed time here.
		gScene->flushStream();
		gScene->fetchResults(NX_RIGID_BODY_FINISHED, true);
		createParticles();
		}
	// ~Physics code

	// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Setup camera
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, (float)glutGet(GLUT_WINDOW_WIDTH)/(float)glutGet(GLUT_WINDOW_HEIGHT), 1.0f, 10000.0f);
	gluLookAt(Eye.x, Eye.y, Eye.z, Eye.x + Dir.x, Eye.y + Dir.y, Eye.z + Dir.z, 0.0f, 1.0f, 0.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Keep physics & graphics in sync
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	int nbActors = gScene->getNbActors();
	NxActor** actors = gScene->getActors();
	while(nbActors--)
	{
		NxActor* actor = *actors++;

		if(!actor->userData)	continue;

		glPushMatrix();
		actor->getGlobalPose().getColumnMajor44(glmat);
		glMultMatrixf(glmat);
		glutSolidCube(float(size_t(actor->userData))*2.0f);
		glPopMatrix();
	}

	RenderTerrain();
	
#ifdef __PPCGEKKO__	
	char buf[256];
    sprintf(buf,
    "Press the keys 1,2,+,-, and b to create various things.\n"
	"Use the arrow keys to move the camera.\n"
	"Press a to pause the simulation.\n");

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
	printf("Press the keys 1,2,+,-, and b to create various things.\n");
	printf("Use the arrow keys to move the camera.\n");
	printf("Press a to pause the simulation.\n");
#else
	printf("Press the keys w,space,s,b, and t to create various things.\n");
	printf("Use the arrow keys or 2, 4, 6 and 8 to move the camera, mouse to look around.\n");
	printf("Press p to pause the simulation.\n");
#endif	
	// Initialize Glut
	glutInit(&argc, argv);
	glutInitWindowSize(512, 512);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	gMainHandle = glutCreateWindow("SampleMeshMaterials");
	glutSetWindow(gMainHandle);
	glutDisplayFunc(RenderCallback);
	glutReshapeFunc(ReshapeCallback);
	glutIdleFunc(IdleCallback);
	glutKeyboardFunc(KeyboardCallback);
	glutSpecialFunc(ArrowKeyCallback);
	glutMouseFunc(MouseCallback);
	glutMotionFunc(MotionCallback);
	MotionCallback(0,0);

#ifdef __PPCGEKKO__	
	glutRemapButtonExt('a','p');
	glutRemapButtonExt('b','w');
	glutRemapButtonExt('1','s');
	glutRemapButtonExt('2','b');
	glutRemapButtonExt('+','t');
	glutRemapButtonExt('-',' ');
	glutRemapButtonExt(GLUT_KEY_UP,'8');
	glutRemapButtonExt(GLUT_KEY_DOWN,'2');
	glutRemapButtonExt(GLUT_KEY_LEFT,'4');
	glutRemapButtonExt(GLUT_KEY_RIGHT,'6');
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
	float AmbientColor[]	= { 0.0f, 0.1f, 0.2f, 0.0f };		glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientColor);
	float DiffuseColor[]	= { 1.0f, 1.0f, 1.0f, 0.0f };		glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseColor);
	float SpecularColor[]	= { 0.0f, 0.0f, 0.0f, 0.0f };		glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularColor);
	float Position[]		= { -10.0f, 100.0f, 4.0f, 1.0f };		glLightfv(GL_LIGHT0, GL_POSITION, Position);
	glEnable(GL_LIGHT0);

	// Initialize physics scene and start the application main loop if scene was created
	if (InitNx())
	{
		atexit(ExitCallback);
		glutMainLoop();
	}

	return 0;
}

// ===============================================================================
//
//						   NVIDIA PhysX SDK Sample Program
//
// Title: Sweep Tests Sample
// Description: This sample program shows the sweep tests functionality of the NVIDIA
//              PhysX SDK.
//
// ===============================================================================

#include <stdio.h>
#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

//#define DO_PROFILING	//Enable profiling printouts

// Physics code
#undef random
#include "NxPhysics.h"
#include "ErrorStream.h"
#include "DebugRenderer.h"
#include "cooking.h"
#include "Timing.h"
#include "Stream.h"
#include "DrawObjects.h"
#include "Terrain.h"
#include "TerrainRender.h"
#include "SamplesVRDSettings.h"
#include "Utilities.h"
#include "UserAllocator.h"
#include "common.h"

#ifdef __PPCGEKKO__
#include "GLFontRenderer.h"
#endif

static bool				gPause = false;
static bool				gUseCache = false;
static NxPhysicsSDK*	gPhysicsSDK = NULL;
static NxScene*			gScene = NULL;
static NxVec3			gDefaultGravity(0.0f, -9.81f, 0.0f);
static ErrorStream		gErrorStream;
static NxBounds3		gSceneBounds;
static NxF32			gElapsedTime = 0.0f;
static NxU32			gVolume = 0;
static DebugRenderer	gDebugRenderer;
static NxActor*			gLastActor = NULL;
static UserAllocator*	gAllocator = NULL;

// Render code
static NxVec3 Eye(50.0f, 50.0f, 50.0f);
static NxVec3 Dir(-0.6,-0.2,-0.7);
static NxVec3 N;
static int mx = 0;
static int my = 0;

#define	CAM_SPEED		20.0f

#define NB_TERRAIN_X	10
#define NB_TERRAIN_Y	10
#define TOTAL_NB_TERRAINS	NB_TERRAIN_X*NB_TERRAIN_Y
#define TERRAIN_SIZE	17

#define TERRAIN_CHAOS	100.0f
#define TERRAIN_OFFSET	-20.0f
#define TERRAIN_WIDTH	20.0f

static TerrainData* gTerrainData[TOTAL_NB_TERRAINS];

static void CreatePhysicsTerrain(NxU32 index, const NxVec3& pos)
{
	NX_DELETE_SINGLE(gTerrainData[index]);

	gTerrainData[index] = new TerrainData;
	gTerrainData[index]->init(TERRAIN_SIZE, TERRAIN_OFFSET, TERRAIN_WIDTH, TERRAIN_CHAOS, false, &pos);

	// Build physical model
	NxTriangleMeshDesc terrainDesc;
	terrainDesc.numVertices					= gTerrainData[index]->nbVerts;
	terrainDesc.numTriangles				= gTerrainData[index]->nbFaces;
	terrainDesc.pointStrideBytes			= sizeof(NxVec3);
	terrainDesc.triangleStrideBytes			= 3*sizeof(NxU32);
	terrainDesc.points						= gTerrainData[index]->verts;
	terrainDesc.triangles					= gTerrainData[index]->faces;
	terrainDesc.flags						= 0;

	if(0)	// Convex-vs-heightfield currently crashes
	{
		terrainDesc.heightFieldVerticalAxis		= NX_Y;
		terrainDesc.heightFieldVerticalExtent	= -1000.0f;
	}

	InitCooking(gAllocator);
	MemoryWriteBuffer buf;
	bool status = CookTriangleMesh(terrainDesc, buf);
	MemoryReadBuffer readBuffer(buf.data);
	NxTriangleMesh* terrainMesh = gPhysicsSDK->createTriangleMesh(readBuffer);

	//
	// Please note about the created Triangle Mesh, user needs to release it when no one uses it to save memory. It can be detected
	// by API "meshData->getReferenceCount() == 0". And, the release API is "gPhysicsSDK->releaseTriangleMesh(*meshData);"
	//
	NxTriangleMeshShapeDesc terrainShapeDesc;
	terrainShapeDesc.meshData				= terrainMesh;
	terrainShapeDesc.shapeFlags				= NX_SF_FEATURE_INDICES;

	NxActorDesc ActorDesc;
	ActorDesc.shapes.pushBack(&terrainShapeDesc);
	NxActor* gTerrain = gScene->createActor(ActorDesc);
	gTerrain->userData = NULL;

	CloseCooking();
}

static void RenderTerrain()
{
	for(NxU32 i=0;i<TOTAL_NB_TERRAINS;i++)
	{
		if(gTerrainData[i])
		{
			renderTerrain(*gTerrainData[i]);
		}
	}
}

static void DrawContactPoint(const NxVec3& p, const NxVec3& n)
{
	const NxF32 coeff=10.0f;
	DrawLine(p, p+n*coeff, NxVec3(1.0f, 1.0f, 1.0f), 2.0f);

	DrawLine(p, p+NxVec3(1.0f, 0.0f, 0.0f), NxVec3(1.0f, 0.0f, 0.0f), 2.0f);
	DrawLine(p, p+NxVec3(0.0f, 1.0f, 0.0f), NxVec3(0.0f, 1.0f, 0.0f), 2.0f);
	DrawLine(p, p+NxVec3(0.0f, 0.0f, 1.0f), NxVec3(0.0f, 0.0f, 1.0f), 2.0f);
}

static void DrawContactTriangle(NxShape* shape, NxU32 triangleIndex)
{
	if(triangleIndex==0xffffffff)
		return;

	NxTriangleMeshShape* mesh = shape->isTriangleMesh();
	if(mesh)
	{
		NxTriangle worldTriangle;
		mesh->getTriangle(worldTriangle, NULL, NULL, triangleIndex, true);
		const NxVec3& p0 = worldTriangle.verts[0];
		const NxVec3& p1 = worldTriangle.verts[1];
		const NxVec3& p2 = worldTriangle.verts[2];
		DrawTriangle(p0, p1, p2, NxVec3(0.25f, 0.5f, 1.0f));
	}

	NxConvexShape* convex = shape->isConvexMesh();
	if(convex)
	{
		const NxConvexMesh& cm = convex->getConvexMesh();
		NxU32 nbHullVerts = cm.getCount(0, NX_ARRAY_VERTICES);
		NxU32 nbHullTris = cm.getCount(0, NX_ARRAY_TRIANGLES);
		const NxVec3* verts = (const NxVec3*)cm.getBase(0, NX_ARRAY_VERTICES);
		const NxU32* faces = (const NxU32*)cm.getBase(0, NX_ARRAY_TRIANGLES);

		NxU32 vref0 = faces[triangleIndex*3+0];
		NxU32 vref1 = faces[triangleIndex*3+1];
		NxU32 vref2 = faces[triangleIndex*3+2];
		NxVec3 p0 = verts[vref0];
		NxVec3 p1 = verts[vref1];
		NxVec3 p2 = verts[vref2];
		NxMat34 pose = convex->getGlobalPose();
		pose.multiply(p0, p0);
		pose.multiply(p1, p1);
		pose.multiply(p2, p2);
		DrawTriangle(p0, p1, p2, NxVec3(0.25f, 0.5f, 1.0f));
	}
}


static void PerformBoxSweep()
{
	if(!gScene)	return;

	static NxF32 t = 0.0f;
#ifdef __PPCGEKKO__
	NxF32 elapsedTime = 1.0f/60.0f;
#else	
	NxF32 elapsedTime = getElapsedTime();
#endif	
	if(!gPause)
	{
		t += elapsedTime;
	}

	NxBox testBox;
	testBox.center	= NxVec3(0.0f, 100.0f, 0.0f);
	testBox.extents	= NxVec3(10.0f, 10.0f, 10.0f);
	testBox.rot.rotX(t);

	float dist = 100.0f;
	NxVec3 dir(0.0f, -1.0f, 0.0f);
	dir.normalize();

	NxU32 flags = NX_SF_STATICS|NX_SF_DYNAMICS;	// NX_SF_ALL_HITS

	NxSweepQueryHit results[100];

#ifdef DO_PROFILING
	NxU32 time;
	StartProfile(time);
#endif
	NxU32 nb = gScene->linearOBBSweep(testBox, dir*dist, flags, NULL, 100, results, NULL);
#ifdef DO_PROFILING
	EndProfile(time);
	printf("%d - %d         \r", time, time/1024);
#endif

	NxVec3 color;
	if(nb)	color = NxVec3(1.0f, 0.0f, 0.0f);
	else	color = NxVec3(0.0f, 1.0f, 0.0f);

	DrawWireBox(testBox, color, 1.0f);
	if(nb)
	{
		testBox.center += results[0].t * dist * dir;
		DrawWireBox(testBox, color, 1.0f);

		DrawContactPoint(results[0].point, results[0].normal);
		DrawContactTriangle(results[0].hitShape, results[0].internalFaceID);
	} else {
		testBox.center += dist * dir;
		DrawWireBox(testBox, color, 1.0f);
	}
}

static void createCube(const NxVec3& pos, int size);
static void PerformActorSweep()
{
	if(!gScene)	return;

	static NxF32 t = 0.0f;
#ifdef __PPCGEKKO__
	NxF32 elapsedTime = 1.0f/60.0f;
#else	
	NxF32 elapsedTime = getElapsedTime();
#endif	
	if(!gPause)
	{
		t += elapsedTime;
	}

	static NxActor* BoxActor = NULL;
	if(!BoxActor)
	{
		createCube(NxVec3(50.0f, 50.0f, 0.0f), 4);
		BoxActor = gLastActor;
	}


	float dist = 100.0f;
	NxVec3 dir(0.5f, -1.0f, 0.0f);
	dir.normalize();

	NxU32 flags = NX_SF_STATICS|NX_SF_DYNAMICS;	// NX_SF_ALL_HITS

	static NxSweepCache* cache = NULL;
	if(gUseCache)
	{
		if(!cache)	cache = gScene->createSweepCache();
	}

	NxSweepQueryHit results[100];

#ifdef DO_PROFILING
	NxU32 time;
	StartProfile(time);
#endif
	NxU32 nb = BoxActor->linearSweep(dir*dist, flags, NULL, 100, results, NULL, cache);
#ifdef DO_PROFILING
	EndProfile(time);
	printf("%d - %d (%s)         \r", time, time/1024, gUseCache ? "Cache" : "No cache");
#endif

	NxVec3 color;
	if(nb)	color = NxVec3(1.0f, 0.0f, 0.0f);
	else	color = NxVec3(0.0f, 1.0f, 0.0f);

	NxBox testBox;
	BoxActor->getShapes()[0]->isBox()->getWorldOBB(testBox);

	DrawWireBox(testBox, color, 1.0f);
	if(nb)
	{
		testBox.center += results[0].t * dist * dir;
		DrawWireBox(testBox, color, 1.0f);

		DrawContactPoint(results[0].point, results[0].normal);
		DrawContactTriangle(results[0].hitShape, results[0].internalFaceID);
	} else {
		testBox.center += dist * dir;
		DrawWireBox(testBox, color, 1.0f);
	}
}

static void PerformCapsuleSweep()
{
	if(!gScene)	return;

	static NxF32 t = 0.0f;
#ifdef __PPCGEKKO__
	NxF32 elapsedTime = 1.0f/60.0f;
#else	
	NxF32 elapsedTime = getElapsedTime();
#endif	
	if(!gPause)
	{
		t += elapsedTime;
	}

	NxCapsule testCapsule;
	testCapsule.radius	= 5.0f;
	testCapsule.p0		= NxVec3(0.0f, 100.0f, 0.0f);
	testCapsule.p1		= NxVec3(0.0f, 100.0f, 0.0f);

	NxMat33 rot;
	rot.rotX(t);
	rot.multiply(NxVec3(0.0f, 0.0f, 0.0f), testCapsule.p0);
	rot.multiply(NxVec3(0.0f, 0.0f, 0.0f), testCapsule.p1);
	testCapsule.p0.y += 100.0f;
	testCapsule.p1.y += 100.0f;

	float dist = 100.0f;
	NxVec3 dir(0.0f, -1.0f, 0.0f);
	dir.normalize();

	NxU32 flags = NX_SF_STATICS|NX_SF_DYNAMICS;	// NX_SF_ALL_HITS

	NxSweepQueryHit results[100];

#ifdef DO_PROFILING
	NxU32 time;
	StartProfile(time);
#endif
	NxU32 nb = gScene->linearCapsuleSweep(testCapsule, dir*dist, flags, NULL, 100, results, NULL);
#ifdef DO_PROFILING
	EndProfile(time);
	printf("%d - %d         \r", time, time/1024);
#endif

	NxVec3 color;
	if(nb)	color = NxVec3(1.0f, 0.0f, 0.0f);
	else	color = NxVec3(0.0f, 1.0f, 0.0f);

	DrawWireCapsule(testCapsule, color);
	if(nb)
	{
		testCapsule.p0 += results[0].t * dist * dir;
		testCapsule.p1 += results[0].t * dist * dir;
		DrawWireCapsule(testCapsule, color);

		DrawContactPoint(results[0].point, results[0].normal);
		DrawContactTriangle(results[0].hitShape, results[0].internalFaceID);
	} else {
		testCapsule.p0 += dist * dir;
		testCapsule.p1 += dist * dir;
		DrawWireCapsule(testCapsule, color);
	}
}

static void PerformSphereSweep()
{
	if(!gScene)	return;

	static NxF32 t = 0.0f;
#ifdef __PPCGEKKO__
	NxF32 elapsedTime = 1.0f/60.0f;
#else	
	NxF32 elapsedTime = getElapsedTime();
#endif	
	if(!gPause)
	{
		t += elapsedTime;
	}

	NxCapsule testCapsule;
	testCapsule.radius	= 5.0f;
	testCapsule.p0		= testCapsule.p1 = NxVec3(sinf(t*10.0f)*4.0f, 70.0f, cosf(t*10.0f)*4.0f);

	float dist = 100.0f;
	NxVec3 dir(0.0f, -1.0f, 0.0f);
	dir.normalize();

	NxU32 flags = NX_SF_STATICS|NX_SF_DYNAMICS;	// NX_SF_ALL_HITS

	NxSweepQueryHit results[100];
	NxU32 nb = gScene->linearCapsuleSweep(testCapsule, dir*dist, flags, NULL, 100, results, NULL);

	NxVec3 color;
	if(nb)	color = NxVec3(1.0f, 0.0f, 0.0f);
	else	color = NxVec3(0.0f, 1.0f, 0.0f);

	DrawWireCapsule(testCapsule, color);
	if(nb)
	{
		testCapsule.p0 += results[0].t * dist * dir;
		testCapsule.p1 += results[0].t * dist * dir;
		DrawWireCapsule(testCapsule, color);

		DrawContactPoint(results[0].point, results[0].normal);
		DrawContactTriangle(results[0].hitShape, results[0].internalFaceID);
	} else {
		testCapsule.p0 += dist * dir;
		testCapsule.p1 += dist * dir;
		DrawWireCapsule(testCapsule, color);
	}
}

static void createCube(const NxVec3& pos, int size=2)
{
	if(!gSceneBounds.contain(pos))
	{
		printf("Objects is out of world - creation aborted!\n");
		return;
	}

	// Create body
	NxBodyDesc bodyDesc;
	bodyDesc.angularDamping	= 0.5f;
//	bodyDesc.angularVelocity.x = 50.0f;
	bodyDesc.flags |= NX_BF_DISABLE_GRAVITY;

	NxBoxShapeDesc boxDesc;
	boxDesc.dimensions = NxVec3(NxF32(size), NxF32(size), NxF32(size));

	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&boxDesc);
	actorDesc.body			= &bodyDesc;
	actorDesc.density		= 10.0f;
	actorDesc.globalPose.t  = pos;
	gLastActor				= gScene->createActor(actorDesc);
}

static void createSphere(const NxVec3& pos, NxF32 radius)
{
	if(!gSceneBounds.contain(pos))
	{
		printf("Objects is out of world - creation aborted!\n");
		return;
	}

	// Create body
	NxBodyDesc bodyDesc;
	bodyDesc.angularDamping	= 0.5f;
//	bodyDesc.angularVelocity.x = 50.0f;
	bodyDesc.flags |= NX_BF_DISABLE_GRAVITY;

	NxSphereShapeDesc sphereDesc;
	sphereDesc.radius		= radius;

	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&sphereDesc);
	actorDesc.body			= &bodyDesc;
	actorDesc.density		= 10.0f;
	actorDesc.globalPose.t  = pos;
	gLastActor				= gScene->createActor(actorDesc);
}

static void createCapsule(const NxVec3& pos, NxF32 radius, NxF32 height)
{
	if(!gSceneBounds.contain(pos))
	{
		printf("Objects is out of world - creation aborted!\n");
		return;
	}
	// Create body
	NxBodyDesc bodyDesc;
	bodyDesc.angularDamping	= 0.5f;
//	bodyDesc.linearVelocity.z = -5.0f;
//	bodyDesc.angularVelocity.x = 50.0f;
	bodyDesc.flags |= NX_BF_DISABLE_GRAVITY;

	NxCapsuleShapeDesc capsuleDesc;
	capsuleDesc.radius		= radius;
	capsuleDesc.height		= 0.0f; //height;

	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&capsuleDesc);
	actorDesc.body			= &bodyDesc;
	actorDesc.density		= 10.0f;
	actorDesc.globalPose.t  = pos;
	gLastActor				= gScene->createActor(actorDesc);
}

static void GenerateCirclePts(unsigned int nb, NxVec3* pts, float scale, float z)
{
	for(unsigned int i=0;i<nb;i++)
	{
		NxF32 angle = 6.28f*NxF32(i)/NxF32(nb);
		pts[i].x = cosf(angle)*scale;
		pts[i].y = z;
		pts[i].z = sinf(angle)*scale;
	}
}

static void createConvex(const NxVec3& pos, float scale)
{
	if(!gSceneBounds.contain(pos))
	{
		printf("Objects is out of world - creation aborted!\n");
		return;
	}
	// Create body
	NxBodyDesc bodyDesc;
	bodyDesc.angularDamping	= 0.5f;
//	bodyDesc.angularVelocity.x = 50.0f;
	bodyDesc.flags |= NX_BF_DISABLE_GRAVITY;

	int nbInsideCirclePts = NxMath::rand(3, 8);
	int nbOutsideCirclePts = NxMath::rand(3, 8);
	int nbVerts = nbInsideCirclePts + nbOutsideCirclePts;
	
	// Generate random vertices
	NxVec3* verts = new NxVec3[nbVerts];
	
	// Two methods
	if(NxMath::rand(0, 100) > 50)
	{
		for(int i=0;i<nbVerts;i++)
		{
			verts[i].x = scale * NxMath::rand(-2.5f, 2.5f);
			verts[i].y = scale * NxMath::rand(-2.5f, 2.5f);
			verts[i].z = scale * NxMath::rand(-2.5f, 2.5f);
		}
	}
	else
	{
		GenerateCirclePts(nbInsideCirclePts, verts, scale, 0.0f);
		GenerateCirclePts(nbOutsideCirclePts, verts+nbInsideCirclePts, scale*3.0f, 10.0f*scale);
	}

	// Create descriptor for convex mesh
	NxConvexMeshDesc convexDesc;
	convexDesc.numVertices			= nbVerts;
	convexDesc.pointStrideBytes		= sizeof(NxVec3);
	convexDesc.points				= verts;
	convexDesc.flags				= NX_CF_COMPUTE_CONVEX;
	
	InitCooking(gAllocator);

	// Cooking from memory
	MemoryWriteBuffer buf;
	if(CookConvexMesh(convexDesc, buf))
	{
		NxConvexShapeDesc convexShapeDesc;
		convexShapeDesc.meshData = gPhysicsSDK->createConvexMesh(MemoryReadBuffer(buf.data));

		if(convexShapeDesc.meshData)
		{
			NxActorDesc actorDesc;
			actorDesc.shapes.pushBack(&convexShapeDesc);
			actorDesc.body			= &bodyDesc;
			actorDesc.density		= 10.0f;
			actorDesc.globalPose.t  = pos;
			gLastActor				 = gScene->createActor(actorDesc);
		}
	}
	delete[] verts;

	CloseCooking();
}

static void ReleaseNx()
{
	if(gPhysicsSDK != NULL)
	{
		if(gScene != NULL) gPhysicsSDK->releaseScene(*gScene);
		gScene = NULL;
		NxReleasePhysicsSDK(gPhysicsSDK);
		gPhysicsSDK = NULL;
	}

	if (gAllocator)
	{
		delete gAllocator;
		gAllocator = NULL;
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

	gPhysicsSDK->setParameter(NX_VISUALIZATION_SCALE, 1.0f);
	//Enable the following lines to visualize the collision shapes and body axes
	//	gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1);
	//	gPhysicsSDK->setParameter(NX_VISUALIZE_BODY_MASS_AXES, 1);

	// Create a scene
	NxSceneDesc sceneDesc;
	sceneDesc.gravity		= gDefaultGravity;

	// Setting proper scene bounds speeds up sweep tests against dynamic actors
//	NxF32 coeff = 8.0f;
	NxF32 coeff = 1.0f;
	gSceneBounds.min = NxVec3(-200.0f, -200.0f, -200.0f) * coeff;
	gSceneBounds.max = NxVec3(200.0f, 200.0f, 200.0f) * coeff;
	sceneDesc.maxBounds		= &gSceneBounds;
	sceneDesc.boundsPlanes	= true;

	gScene = gPhysicsSDK->createScene(sceneDesc);
	if(gScene == NULL) 
	{
		printf("\nError: Unable to create a PhysX scene, exiting the sample.\n\n");
		return false;
	}

	NxMaterial* defaultMaterial = gScene->getMaterialFromIndex(0); 
	defaultMaterial->setRestitution(0.0f);
	defaultMaterial->setStaticFriction(0.5f);
	defaultMaterial->setDynamicFriction(0.5f);

	NxF32 Coeff = NxF32(TERRAIN_SIZE-1)*TERRAIN_WIDTH;
	NxU32 index=0;
	for(NxU32 j=0;j<NB_TERRAIN_Y;j++)
	{
		for(NxU32 i=0;i<NB_TERRAIN_X;i++)
		{
			NxVec3 pos((NxF32(i)-NxF32(NB_TERRAIN_X)*0.5f)*Coeff, 0.0f, (NxF32(j)-NxF32(NB_TERRAIN_Y)*0.5f)*Coeff);
			gTerrainData[index] = NULL;
			CreatePhysicsTerrain(index++, pos);
		}
	}
	return true;
}
#ifdef __PPCGEKKO__
int gIndex = 0;
static void ProcessIndex()
{
	switch (gIndex)
	{
		case 0:	createCube(NxVec3(0.0f, 30.0f, 0.0f), 15+(rand()&3)); break;
		case 1:	createCube(NxVec3(0.0f, 30.0f, 0.0f), 1+(rand()&3)); break;
		case 2:	createSphere(NxVec3(0.0f, 30.0f, 0.0f), 15+(rand()&3)); break;
		case 3:	createSphere(NxVec3(0.0f, 30.0f, 0.0f), 1+(rand()&3)); break;
		case 4:	createCapsule(NxVec3(0.0f, 30.0f, 0.0f), 15+(rand()&3), 30+(rand()&3)); break;
		case 5:	createCapsule(NxVec3(0.0f, 30.0f, 0.0f), 1+(rand()&3), 3+(rand()&3)); break;
		case 6:	createConvex(NxVec3(0.0f, 30.0f, 0.0f), 10+(rand()&3)); break;
		case 7:	createConvex(NxVec3(0.0f, 30.0f, 0.0f), 1+(rand()&3)); break;
	}
}
#endif
static void KeyboardCallback(unsigned char key, int x, int y)
{
	switch (key)
	{
	case '1':	gVolume = 0;	break;
	case '2':	gVolume = 1;	break;
	case '3':	gVolume = 2;	break;
	case '4':	gVolume = 3;	break;
#ifdef __PPCGEKKO__
	case '+':
		gIndex = (gIndex + 1)%8;
		ProcessIndex();
		break;
	case '-':
		gIndex = (gIndex + 7)%8;
		ProcessIndex();
		break;
#else	
	case 27:	exit(0); break;
	case 'B':	createCube(NxVec3(0.0f, 30.0f, 0.0f), 15+(rand()&3)); break;
	case 'b':	createCube(NxVec3(0.0f, 30.0f, 0.0f), 1+(rand()&3)); break;
	case 'S':	createSphere(NxVec3(0.0f, 30.0f, 0.0f), 15+(rand()&3)); break;
	case 's':	createSphere(NxVec3(0.0f, 30.0f, 0.0f), 1+(rand()&3)); break;
	case 'C':	createCapsule(NxVec3(0.0f, 30.0f, 0.0f), 15+(rand()&3), 30+(rand()&3)); break;
	case 'c':	createCapsule(NxVec3(0.0f, 30.0f, 0.0f), 1+(rand()&3), 3+(rand()&3)); break;
	case 'O':	createConvex(NxVec3(0.0f, 30.0f, 0.0f), 10+(rand()&3)); break;
	case 'o':	createConvex(NxVec3(0.0f, 30.0f, 0.0f), 1+(rand()&3)); break;
#endif	
	case 'p':	gPause = !gPause; break;
	case 'x':	gUseCache = !gUseCache;	break;
	case 'y':	if(gLastActor)	gLastActor->setAngularVelocity(NxVec3(5.0f, 0.0f, 0.0f));	break;
	case 'u':	if(gLastActor)	gLastActor->setAngularVelocity(NxVec3(0.0f, 5.0f, 0.0f));	break;
	case 'i':	if(gLastActor)	gLastActor->setAngularVelocity(NxVec3(0.0f, 0.0f, 5.0f));	break;

	case 101:	Eye += Dir * CAM_SPEED; break;
	case 103:	Eye -= Dir * CAM_SPEED; break;
	case 100:	Eye -= N * CAM_SPEED; break;
	case 102:	Eye += N * CAM_SPEED; break;
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
	gElapsedTime = 1.0f/60.0f;
#else
	gElapsedTime = getElapsedTime();
#endif

	// Physics code
	if(!gPause)	
	{
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
	gluPerspective(60.0f, ((float)glutGet(GLUT_WINDOW_WIDTH))/((float)glutGet(GLUT_WINDOW_HEIGHT)), 1.0f, 10000.0f);
	gluLookAt(Eye.x, Eye.y, Eye.z, Eye.x + Dir.x, Eye.y + Dir.y, Eye.z + Dir.z, 0.0f, 1.0f, 0.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_LIGHTING);

	NxU32 n = gVolume%4;
	if(n==0)
		PerformBoxSweep();
	else if(n==1)
		PerformCapsuleSweep();
	else if(n==2)
		PerformSphereSweep();
	// else nothing
	else
		PerformActorSweep();

	// Keep physics & graphics in sync
	int nbActors = gScene->getNbActors();
	NxActor** actors = gScene->getActors();
	while(nbActors--)
	{
		NxActor* actor = *actors++;

		if (actor->getNbShapes() > 0)
		{
			NxShape*const* shapes  = actor->getShapes();
			if (!((*shapes)->isHeightField()) && !((*shapes)->isTriangleMesh()) && !((*shapes)->isPlane()))
			{
				DrawActor(actor);
			}
		}
	
	}

	RenderTerrain();
	gDebugRenderer.renderData(*gScene->getDebugRenderable());

#ifdef __PPCGEKKO__	
	char buf[4096];
    sprintf(buf,
    "Arrow keys to move the camera\n"
	"a - test swept box\n"
	"b - test swept capsule\n"
	"1 - test swept sphere\n"
	"2 - test swept actor\n"
	"+,- create various actor");
  
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

int main(int argc, char** argv)
{
	printf("Keys:\n");
	printf("Arrow keys to move the camera\n");
#ifdef __PPCGEKKO__
	printf("a - test swept box\n");
	printf("b - test swept capsule\n");
	printf("1 - test swept sphere\n");
	printf("2 - test swept actor\n");
	printf("+,- create various actor");
#else	
	printf("1 - test swept box\n");
	printf("2 - test swept capsule\n");
	printf("3 - test swept sphere\n");
	printf("4 - test swept actor\n");
	printf("b - create a small box\n");
	printf("B - create a big box\n");
	printf("s - create a small sphere\n");
	printf("S - create a big sphere\n");
	printf("c - create a small capsule\n");
	printf("C - create a big capsule\n");
	printf("o - create a small convex\n");
	printf("O - create a big convex\n");
	printf("p - pause on/off\n");
	printf("x - use cache on/off\n");
	printf("y - spin last actor around X\n");
	printf("u - spin last actor around Y\n");
	printf("i - spin last actor around Z\n");
#endif	
	printf("\n");
#if defined(_XBOX) || defined(__CELLOS_LV2__)
	glutRemapButtonExt(0, 'b', false); // A/X to create big box
	glutRemapButtonExt(1, 's', false); // B/O to create big sphere
	glutRemapButtonExt(2, 'c', false); // X/S to create big capsule
	glutRemapButtonExt(3, 'o', false); // Y/T to create big convex
	glutRemapButtonExt(0, 'B', true);// Shift+A/X to create small box
	glutRemapButtonExt(1, 'S', true);// Shift+B/O to create small sphere
	glutRemapButtonExt(2, 'C', true);// Shift+X/S to create small capsule
	glutRemapButtonExt(3, 'O', true);// Shift+Y/T to create small convex
	glutRemapButtonExt(4, '1', true);// Digital control to switch sweep type
	glutRemapButtonExt(5, '2', true);
	glutRemapButtonExt(6, '3', true);
	glutRemapButtonExt(7, '4', true);
	glutRemapButtonExt(8, 'y', false);//Left shoulder to spin actor around X axis
	glutRemapButtonExt(9, 'u', false);//Right shoulder to spin actor around Y axis
	glutRemapButtonExt(8, 'i', true);//Shift+Left shoulder to spin actor around Z axis
#endif
#ifdef __PPCGEKKO__
	glutRemapButtonExt('a','1');
	glutRemapButtonExt('b','2');
	glutRemapButtonExt('1','3');
	glutRemapButtonExt('2','4');
	glutRemapButtonExt(GLUT_KEY_UP,101);
	glutRemapButtonExt(GLUT_KEY_DOWN,103);
	glutRemapButtonExt(GLUT_KEY_LEFT,100);
	glutRemapButtonExt(GLUT_KEY_RIGHT,102);
#endif
	// Initialize Glut
	glutInit(&argc, argv);
	glutInitWindowSize(512, 512);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	int mainHandle = glutCreateWindow("SampleSweepTests");
	glutSetWindow(mainHandle);
	glutDisplayFunc(RenderCallback);
	glutReshapeFunc(ReshapeCallback);
	glutIdleFunc(IdleCallback);
	glutKeyboardFunc(KeyboardCallback);
	glutSpecialFunc(ArrowKeyCallback);
	glutMouseFunc(MouseCallback);
	glutMotionFunc(MotionCallback);
	MotionCallback(0,0);

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
	float Position[] = { -10.0f, 1000.0f, -4.0f, 1.0f };		glLightfv(GL_LIGHT0, GL_POSITION, Position);
	glEnable(GL_LIGHT0);

	// Initialize physics scene and start the application main loop if scene was created
	if (InitNx())
	{
		atexit(ReleaseNx);
		glutMainLoop();
	}

	return 0;
}

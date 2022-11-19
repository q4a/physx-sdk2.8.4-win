// ===============================================================================
//
//						   NVIDIA PhysX SDK Sample Program
//
// Title: HeightField Sample
// Description: This sample program shows how to use the special HeightField 
//              geometry in a software scene.
//
// Originally written by: Peter Tchernev (2005.10.01)
// Additions by: 
//
// ===============================================================================

#include <stdio.h>
#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
// Physics code
#undef random
#include "NxPhysics.h"
#include "UserAllocator.h"
#include "ErrorStream.h"
#include "NxHeightField.h"
#include "NxHeightFieldDesc.h"
#include "NxHeightFieldShapeDesc.h"
#include "NxHeightFieldSample.h"
#include "NxCooking.h"
#include "Stream.h"
#include "NxTriangle.h"
#include "DrawObjects.h"
#include "DebugRenderer.h"
#include "Utilities.h"
#include "SamplesVRDSettings.h"
#include "cooking.h"
#include "common.h"

#ifdef __PPCGEKKO__
#include "GLFontRenderer.h"
#endif

static bool				gProfile = true;
static bool				gPause = false;
static ErrorStream		gErrorStream;
static DebugRenderer	gDebugRenderer;
static UserAllocator*	gAllocator = NULL;
static NxPhysicsSDK*	gPhysicsSDK = NULL;
static NxScene*			gScene = NULL;
static NxVec3			gGravity = NxVec3(0.0f, -9.81f, 0.0f);
static NxActor*			gHeightField = NULL;
static NxArray<NxU32>	gTouchedTris;
static NxVec3			gSpawnPos = NxVec3(10, 3, 10);
#define ROT_SCALE	0.1f
#define ZOOM_SCALE	0.01f
#define PAN_SCALE	0.01f

// Render code
static int gMouseX = 0;
static int gMouseY = 0;
static int gMouseButtons = 0;
static int gMouseState = 0;
static float gRotX = -50;
static float gRotY = 35;
static NxVec3 gCameraLookAt = NxVec3(15, 0, 15);
static NxVec3 gCameraOrigin = NxVec3(30, 15, 30);
static NxVec3 gCameraX = NxVec3(1, 0, 0);
static NxVec3 gCameraY = NxVec3(0, 1, 0);
static NxVec3 gCameraZ = NxVec3(0, 0, 1);
static float gCameraDistance = 30;

class MyTriggerReport : public NxUserTriggerReport
{
public:
	virtual void onTrigger(NxShape& triggerShape, NxShape& otherShape, NxTriggerFlag status)
	{
		if (otherShape.isHeightField()) return; //this won't happen, unless the heightfield is dynamic or kinematic
		otherShape.setGroup(1);
	}

} gMyTriggerReport;

class MyContactReport : public NxUserContactReport
	{
	public:
		virtual void onContactNotify(NxContactPair& pair, NxU32 events)
			{
			// Iterate through contact points
			NxContactStreamIterator i(pair.stream);
			//user can call getNumPairs() here
			while(i.goNextPair())
				{
				//user can also call getShape() and getNumPatches() here
				while(i.goNextPatch())
					{
					//user can also call getPatchNormal() and getNumPoints() here
					const NxVec3& contactNormal = i.getPatchNormal();
					while(i.goNextPoint())
						{
						//user can also call getPoint() and getSeparation() here
						const NxVec3& contactPoint = i.getPoint();

						NxU32 faceIndex = i.getFeatureIndex0();
						if(faceIndex==0xffffffff)	
							faceIndex = i.getFeatureIndex1();
						if(faceIndex!=0xffffffff)
							{
							gTouchedTris.pushBack(faceIndex);
							}
						}
					}
				}
			}

	} gMyContactReport;

static void CreateHeightField(const NxVec3& pos, NxVec3 size = NxVec3(10,1,10), NxU32 nbColumns=10, NxU32 nbRows=10)
	{
	static NxReal sixtyFourKb = 65536.0f;
	static NxReal thirtyTwoKb = 32767.5f;

	NxHeightFieldDesc heightFieldDesc;
	heightFieldDesc.nbColumns		= nbColumns;
	heightFieldDesc.nbRows			= nbRows;
	heightFieldDesc.convexEdgeThreshold = 0;

	// allocate storage for samples
	heightFieldDesc.samples			= new NxU32[nbColumns*nbRows];
	heightFieldDesc.sampleStride	= sizeof(NxU32);

	char* currentByte = (char*)heightFieldDesc.samples;
	for (NxU32 row = 0; row < nbRows; row++)
		{
		for (NxU32 column = 0; column < nbColumns; column++)
			{
			NxReal s = NxReal(row) / NxReal(nbRows);
			NxReal t = NxReal(column) / NxReal(nbColumns);
			NxI16 height = (NxI32)(0.5 * thirtyTwoKb * (NxMath::sin(5.0f*NxPiF32*s) + NxMath::cos(5.0f*NxPiF32*t)));
			//NxI16 height = - (nbRows / 2 - row) * (nbRows / 2 - row) - (nbColumns / 2 - column) * (nbColumns / 2 - column);
			
			NxHeightFieldSample* currentSample = (NxHeightFieldSample*)currentByte;
			currentSample->height = height;
			int xDiff = column-nbColumns*0.75;
			int yDiff = row-nbRows*0.75;

			currentSample->tessFlag = 0;

			int dist = abs(xDiff) + abs(yDiff);
			if (dist>8)
			{
				currentSample->materialIndex0 = 1;
				currentSample->materialIndex1 = 1;
			} else if (dist==8)
			{
				if (xDiff != 0 && yDiff != 0)
				{
					currentSample->materialIndex0 = xDiff<0 ? 1 : 2;
					currentSample->materialIndex1 = xDiff<0 ? 2 : 1;
					currentSample->tessFlag = (xDiff<0 != yDiff<0) ? 1 : 0;
				} else {
					currentSample->materialIndex0 = 1;
					currentSample->materialIndex1 = 1;
				}
			} else {
				currentSample->materialIndex0 = 2;
				currentSample->materialIndex1 = 2;
			}


			currentByte += heightFieldDesc.sampleStride;
			}
		}

	NxHeightField* heightField = gScene->getPhysicsSDK().createHeightField(heightFieldDesc);

	// data has been copied, we can free our buffer
	delete [] heightFieldDesc.samples;

	NxHeightFieldShapeDesc heightFieldShapeDesc;
	heightFieldShapeDesc.heightField	= heightField;
	heightFieldShapeDesc.shapeFlags		= NX_SF_FEATURE_INDICES | NX_SF_VISUALIZATION;
	heightFieldShapeDesc.heightScale	= size.y / sixtyFourKb;
	heightFieldShapeDesc.rowScale		= size.x / NxReal(nbRows-1);
	heightFieldShapeDesc.columnScale	= size.z / NxReal(nbColumns-1);
	//heightFieldShapeDesc.meshFlags	= NX_MESH_SMOOTH_SPHERE_COLLISIONS;
	heightFieldShapeDesc.materialIndexHighBits = 0;
	heightFieldShapeDesc.holeMaterial = 2;

	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&heightFieldShapeDesc);
	actorDesc.body		   = NULL;
	actorDesc.globalPose.t = pos;
	NxActor* newActor = gScene->createActor(actorDesc);

	gHeightField = newActor;
	}

static NxActor* CreateActorFromDesc(const NxShapeDesc& shapeDesc, const NxMat34& globalPose, bool body = true)
	{
	if (!gScene) return NULL;
	NxBodyDesc bodyDesc;
	NxActorDesc actorDesc;
	actorDesc.globalPose.t = globalPose.t;
	actorDesc.globalPose.M = globalPose.M;

	NxSphereShapeDesc sphereDesc;
	sphereDesc.localPose.t.x = 1;
	sphereDesc.radius = 1;
	sphereDesc.shapeFlags = NX_SF_VISUALIZATION | NX_TRIGGER_ENABLE;

	actorDesc.shapes.pushBack(&(NxShapeDesc&)shapeDesc);
	//actorDesc.shapes.pushBack(&(NxShapeDesc&)sphereDesc);
	if (body)
		{
		bodyDesc.angularDamping	= 0.5f;
		actorDesc.body			= &bodyDesc;
		actorDesc.density		= 10.0f;
		}
	return gScene->createActor(actorDesc);
	}

static NxActor* CreateCube(const NxVec3& pos, NxReal size = 2.0f, bool trigger = false)
	{
	NxBoxShapeDesc shapeDesc;
	shapeDesc.dimensions = NxVec3(float(size), float(size), float(size));
	shapeDesc.shapeFlags |= NX_SF_VISUALIZATION;
	if (trigger) shapeDesc.shapeFlags |= NX_TRIGGER_ENABLE;
	return CreateActorFromDesc(shapeDesc, NxMat34(NxMat33(NX_IDENTITY_MATRIX), pos), !trigger);
	}

static NxActor* CreateBall(const NxVec3& pos, NxReal size = 2.0f, bool trigger = false)
	{
	NxSphereShapeDesc shapeDesc;
	shapeDesc.radius = float(size);
	shapeDesc.shapeFlags |= NX_SF_VISUALIZATION;
	if (trigger) shapeDesc.shapeFlags |= NX_TRIGGER_ENABLE;
	return CreateActorFromDesc(shapeDesc, NxMat34(NxMat33(NX_IDENTITY_MATRIX), pos), !trigger);
	}

static NxActor* CreateCapsule(const NxVec3& pos, NxReal size = 2.0f, bool trigger = false)
	{
	NxCapsuleShapeDesc shapeDesc;
	shapeDesc.radius = size;
	shapeDesc.height = 2*size;
	shapeDesc.shapeFlags |= NX_SF_VISUALIZATION;
	if (trigger) shapeDesc.shapeFlags |= NX_TRIGGER_ENABLE;
	return CreateActorFromDesc(shapeDesc, NxMat34(NxMat33(NX_IDENTITY_MATRIX), pos), !trigger);
	}

static void GenerateCirclePts(NxU32 nb, NxVec3* pts, NxF32 scale, NxF32 z)
	{
	for(NxU32 i=0;i<nb;i++)
		{
		NxF32 angle = 2*NxPiF32*NxF32(i)/NxF32(nb);
		pts[i].x = NxMath::cos(angle)*scale;
		pts[i].y = z;
		pts[i].z = NxMath::sin(angle)*scale;
		}
	}

static NxActor* CreateConvex(const NxVec3& pos, int size=2, const NxVec3* initial_velocity=NULL, bool trigger = false)
	{

	NxU32 nbInsideCirclePts = (NxU32)NxMath::rand(3, 8);
	NxU32 nbOutsideCirclePts = (NxU32)NxMath::rand(3, 8);
	NxU32 nbVerts = nbInsideCirclePts + nbOutsideCirclePts;
	// Generate random vertices
	NxVec3* verts = (NxVec3*)NxAlloca(sizeof(NxVec3)*nbVerts);
	//			if (NxMath::rand(0, 100) > 50)	//two methods
	if (0)	//two methods
		{
		for(NxU32 i=0;i<nbVerts;i++)
			{
			verts[i].x = size * (NxReal)NxMath::rand(-1, 1);
			verts[i].y = size * (NxReal)NxMath::rand(-1, 1);
			verts[i].z = size * (NxReal)NxMath::rand(-1, 1);
			}
		}
	else
		{
		GenerateCirclePts(nbInsideCirclePts, verts, 0.5f*size, 0.0f);
		GenerateCirclePts(nbOutsideCirclePts, verts+nbInsideCirclePts, 1.0f*size, 2.0f*size);
		}

	// Create descriptor for convex mesh
	NxConvexMeshDesc convexDesc;
	convexDesc.numVertices			= nbVerts;
	convexDesc.pointStrideBytes		= sizeof(NxVec3);
	convexDesc.points				= verts;
	convexDesc.flags				= NX_CF_COMPUTE_CONVEX;

	NxConvexShapeDesc convexShapeDesc;

	// Cooking from memory
	MemoryWriteBuffer buf;
	bool status = CookConvexMesh(convexDesc, buf);
	//convexShapeDesc.scale = 10;
	convexShapeDesc.meshData = gPhysicsSDK->createConvexMesh(MemoryReadBuffer(buf.data));

	convexShapeDesc.shapeFlags |= NX_SF_VISUALIZATION;
	if (trigger) convexShapeDesc.shapeFlags |= NX_TRIGGER_ENABLE;

	if(convexShapeDesc.meshData)
		return CreateActorFromDesc(convexShapeDesc, NxMat34(NxMat33(NX_IDENTITY_MATRIX), pos), !trigger);

	return NULL;
	}

static void CreateGrid(NxVec3 pos, int size=1, int shapeType=0, int shapeSize=1)
	{
	float spacing = 1.1f * shapeSize;
	for (int z = 0; z<size; z++) 
		{
		for (int x = 0; x<size; x++) 
			{
			NxReal delta = +(shapeSize + spacing);
			NxActor* newActor = NULL;
			switch(shapeType)
				{
				case 0:
					newActor = CreateBall(pos + NxVec3((float)x*delta, 0, (float)z*delta), (NxReal)shapeSize);
					break;
				case 1:
					newActor = CreateCapsule(pos + NxVec3((float)x*delta, 0, (float)z*delta), (NxReal)shapeSize);
					break;
				case 2:
					newActor = CreateCube(pos + NxVec3((float)x*delta, 0, (float)z*delta), (NxReal)shapeSize);
					break;
				case 3:
					newActor = CreateConvex(pos + NxVec3((float)x*delta, 0, (float)z*delta), shapeSize);
					break;
				default:
					; //noop
					break;
				}

			if (gHeightField && newActor) 
				gScene->setActorPairFlags(*gHeightField, *newActor, NX_NOTIFY_ON_TOUCH);
			}
		}
	}

static bool CreateScene(NxVec3 gravity = NxVec3(0, -9.81, 0), NxReal staticFriction = 0.8f, NxReal dynamicFriction = 0.6f, NxReal restitution = 0.1f)
	{
	NxSceneDesc sceneDesc;
	sceneDesc.gravity = gravity;
	sceneDesc.userContactReport = &gMyContactReport;
	sceneDesc.userTriggerReport = &gMyTriggerReport;
	gScene = gPhysicsSDK->createScene(sceneDesc);
	if(gScene == NULL) 
	{
		printf("\nError: Unable to create a PhysX scene, exiting the sample.\n\n");
		return false;
	}
	NxMaterial * defaultMaterial = gScene->getMaterialFromIndex(0); 
	defaultMaterial->setRestitution(restitution);
	defaultMaterial->setStaticFriction(staticFriction);
	defaultMaterial->setDynamicFriction(dynamicFriction);
	return true;
	}

static bool CreatePhysicsSDK() 
	{
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
		gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_AXES, 1.0f);
		gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1.0f);
		//More visualization that might be interesting
		//gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_AABBS, 1);
		//gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_SPHERES, 1);
		//gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_EDGES, 1.0f);
		//gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_FNORMALS, 1.0f);
		//gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_VNORMALS, 1.0f);
		//gPhysicsSDK->setParameter(NX_VISUALIZE_CONTACT_NORMAL, 1.0f);
		//gPhysicsSDK->setParameter(NX_VISUALIZE_CONTACT_POINT, 1.0f);
		//gPhysicsSDK->setParameter(NX_VISUALIZE_BODY_MASS_AXES, 1.0f);
		return true;
	}

static bool ResetScene()
	{
	if(gPhysicsSDK != NULL)
		{
		if(gScene != NULL) gPhysicsSDK->releaseScene(*gScene);
		gScene = NULL;
#ifdef RESET_SDK
		NxReleasePhysicsSDK(gPhysicsSDK);
		gPhysicsSDK = NULL;
#endif
		}
	if (!gPhysicsSDK) 
		if (!CreatePhysicsSDK()) return false;

	if (!CreateScene(gGravity)) return false;
	CreateHeightField(NxVec3(0,-1,0), NxVec3(30,1,30), 40, 40);

	// We are adding a protective layer of triggers around the hole in the 
	// height field to ensure that collision is masked out between the 
	// height field and object falling through the hole.

	NxBodyDesc bodyDesc;
	bodyDesc.flags |= NX_BF_KINEMATIC;
	bodyDesc.mass  = 100.0f;
	bodyDesc.massSpaceInertia.set(1.0f, 1.0f, 1.0f);
	NxActorDesc actorDesc;
	actorDesc.globalPose.t.set(23.5f, -5.8f, 23.5f);
	actorDesc.globalPose.M.rotY(0.25f*NxPi);
	
	NxBoxShapeDesc boxShapeDesc[4];

	boxShapeDesc[0].localPose.t.set(-4.0f, 0, 0);
	boxShapeDesc[0].dimensions.set(0.2f, 4.0f, 4.0f);
	boxShapeDesc[0].shapeFlags = NX_SF_VISUALIZATION | NX_TRIGGER_ENABLE;

	boxShapeDesc[1].localPose.t.set( 4.0f, 0, 0);
	boxShapeDesc[1].dimensions.set(0.2f, 4.0f, 4.0f);
	boxShapeDesc[1].shapeFlags = NX_SF_VISUALIZATION | NX_TRIGGER_ENABLE;

	boxShapeDesc[2].localPose.t.set(0, 0, -4.0f);
	boxShapeDesc[2].dimensions.set(4.0, 4.0f, 0.2f);
	boxShapeDesc[2].shapeFlags = NX_SF_VISUALIZATION | NX_TRIGGER_ENABLE;

	boxShapeDesc[3].localPose.t.set(0, 0, 4.0f);
	boxShapeDesc[3].dimensions.set(4.0, 4.0f, 0.2f);
	boxShapeDesc[3].shapeFlags = NX_SF_VISUALIZATION | NX_TRIGGER_ENABLE;

	actorDesc.shapes.pushBack(&(NxShapeDesc&)boxShapeDesc[0]);
	actorDesc.shapes.pushBack(&(NxShapeDesc&)boxShapeDesc[1]);
	actorDesc.shapes.pushBack(&(NxShapeDesc&)boxShapeDesc[2]);
	actorDesc.shapes.pushBack(&(NxShapeDesc&)boxShapeDesc[3]);

	actorDesc.body    = &bodyDesc;
	NxActor* gTrigger = gScene->createActor(actorDesc);

	gScene->setGroupCollisionFlag(0,1,false);

	return true;
	}

static bool InitNx()
	{
	gAllocator = new UserAllocator;
	if (!InitCooking(gAllocator, &gErrorStream))
		{
		printf("\nError: Unable to initialize the cooking library, exiting the sample.\n\n");
		return false;
		}
	if (!ResetScene()) return false;

#if defined(_XBOX) || defined(__CELLOS_LV2__)
	glutRemapButtonExt(0, '1', false); // A/X to spawn spheres
	glutRemapButtonExt(1, '2', false);	// B/O to spawn capsules.
	glutRemapButtonExt(2, '3', false);	// X/S to spawn boxes.
	glutRemapButtonExt(3, '4', false);	// Y/T to spawn convexes.
	glutRemapButtonExt(9, 'R', false); // Right shoulder to reset scene
#endif
#ifdef __PPCGEKKO__
	glutRemapButtonExt('+','3');
	glutRemapButtonExt('-','4');
	glutRemapButtonExt('a','r');
	glutRemapButtonExt(GLUT_KEY_HOME,'p');
#endif
	return true;
	}

#ifdef __PPCGEKKO__
// ------------------------------------------------------------------------------------
// Because no mouse support for wii, we write different control behavior for wii. 
//Camera globals
NxReal				gCameraFov = 40.0f;
NxVec3				gCameraPos(13.5,8,63);
NxVec3				gCameraForward(0,0,-1);
NxVec3				gCameraRight(1,0,0);
NxReal				gCameraSpeed = 0.2;

static int index = 0;

void ProcessKeys(int key, int x, int y)
{
	// Process keys
	switch (key)
	{
		// Camera controls
		case GLUT_KEY_UP:	{ gCameraPos += gCameraForward*gCameraSpeed; break; }
		case GLUT_KEY_DOWN:	{ gCameraPos -= gCameraForward*gCameraSpeed; break; }
		case GLUT_KEY_LEFT:	{ gCameraPos -= gCameraRight*gCameraSpeed; break; }
		case GLUT_KEY_RIGHT:{ gCameraPos += gCameraRight*gCameraSpeed; break; }
	}
}

// ------------------------------------------------------------------------------------

void SetupCamera()
{
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	gluPerspective(gCameraFov, ((float)glutGet(GLUT_WINDOW_WIDTH)/(float)glutGet(GLUT_WINDOW_HEIGHT)), 1.0f, 10000.0f);
	gluLookAt(gCameraPos.x,gCameraPos.y,gCameraPos.z,gCameraPos.x + gCameraForward.x, gCameraPos.y + gCameraForward.y, gCameraPos.z + gCameraForward.z, 0.0f, 1.0f, 0.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
#endif

static void ExitCallback()
	{
	CloseCooking();

	if(gPhysicsSDK != NULL)
		{
		if(gScene != NULL) gPhysicsSDK->releaseScene(*gScene);
		gScene = NULL;
		NxReleasePhysicsSDK(gPhysicsSDK);
		gPhysicsSDK = NULL;
		}

	if(gAllocator) delete gAllocator;
	}

static void KeyboardCallback(unsigned char key, int x, int y)
	{
	switch (key)
		{
		case 27: exit(0); break;
		case '1': CreateGrid(gSpawnPos, 5, 0, 1); break;
		case '2': CreateGrid(gSpawnPos, 5, 1, 1); break;
		case '3': CreateGrid(gSpawnPos, 5, 2, 1); break;
		case '4': CreateGrid(gSpawnPos, 5, 3, 1); break;
		case 'R':
		case 'r': if (!ResetScene()) exit(0); break;
		case 'P':
		case 'p': gPause = !gPause; break;
		}
	}

static void ArrowKeyCallback(int key, int x, int y)
	{
#ifdef __PPCGEKKO__
	ProcessKeys(key,x,y);
#else	
	KeyboardCallback(key,x,y);
#endif	
	}

static void MouseCallback(int button, int state, int x, int y)
	{
	gMouseX = x;
	gMouseY = y;
	gMouseButtons = button;
	gMouseState = state;
	}

static void MotionCallback(int x, int y)
	{
	int dx = gMouseX - x;
	int dy = gMouseY - y;

	gMouseX = x;
	gMouseY = y;

	if (gMouseState == GLUT_DOWN)
		{
		switch(gMouseButtons)
			{
			case GLUT_LEFT_BUTTON:
				{
				gRotX += (ROT_SCALE) * (float) dx;
				gRotY += (ROT_SCALE) * (float) dy;
				}
			break;
			case GLUT_RIGHT_BUTTON:
				{
				gCameraDistance += (1 + NxMath::abs(gCameraDistance)) * (ZOOM_SCALE) * (float)(dx + dy);
				if (gCameraDistance < 1) gCameraDistance = 1;
				}
			break;
			case GLUT_MIDDLE_BUTTON:
				{
				NxReal scale = ZOOM_SCALE * NxMath::abs(gCameraDistance);
				NxVec3 pan = gCameraX*scale*(NxReal)dx - gCameraY*scale*(NxReal)dy;
				gCameraLookAt += pan;
				}
			break;
			}
		}	
	}


static void RenderCallback()
	{
	if(gScene == NULL) return;
	
	// Physics code
	if(!gPause)	
		{
		gTouchedTris.clear();
		gScene->simulate(1.0f/60.0f);	//Note: a real application would compute and pass the elapsed time here.
		gScene->flushStream();
		gScene->fetchResults(NX_RIGID_BODY_FINISHED, true);
		}
	// ~Physics code
	
	// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Setup camera
#ifdef __PPCGEKKO__   //glutLookAt in wii do things more than set Matrix so..
	SetupCamera();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_LIGHTING);
#else
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(80.0f, (float)glutGet(GLUT_WINDOW_WIDTH)/(float)glutGet(GLUT_WINDOW_HEIGHT), 1.0f, 10000.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0, 0, -gCameraDistance);
	glRotatef(gRotY, 1, 0, 0);
	glRotatef(gRotX, 0, 1, 0);
	glTranslatef(-gCameraLookAt.x, -gCameraLookAt.y, -gCameraLookAt.z);

	glPushMatrix();
	const NxDebugRenderable *dbgRenderable=gScene->getDebugRenderable();
	gDebugRenderer.renderData(*dbgRenderable);
	glEnable(GL_LIGHTING);
	glPopMatrix();
#endif


	GLfloat matrix[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
	gCameraX = NxVec3(matrix[0],matrix[4],matrix[8]);
	gCameraY = NxVec3(matrix[1],matrix[5],matrix[9]);
	gCameraZ = NxVec3(matrix[2],matrix[6],matrix[10]);

	NxVec3 U(matrix[0],matrix[1],matrix[2]);
	NxVec3 V(matrix[4],matrix[5],matrix[6]);
	NxVec3 N(matrix[8],matrix[9],matrix[10]);
	NxVec3 O(matrix[12],matrix[13],matrix[14]);
	gCameraOrigin.x = -O.dot(U);
	gCameraOrigin.y = -O.dot(V);
	gCameraOrigin.z = -O.dot(N);

	if (gScene)
		{
		for(unsigned int i=0;i<gScene->getNbActors();i++)
			{
			glColor4f(0.7f, 0.7f, 0.7f, 1.0f);
			DrawActor(gScene->getActors()[i]);
			}

		if (gHeightField)
			{

			const NxHeightFieldShape * hfs = (const NxHeightFieldShape*)gHeightField->getShapes()[0];

			int numVerticesTouched = gTouchedTris.size()*3;
			float* pVertListTouched = new float[numVerticesTouched*3];
			float* pNormListTouched = new float[numVerticesTouched*3];

			int vertIndexTouched = 0;
			int normIndexTouched = 0;

			glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
			for(NxU32 i = 0; i < gTouchedTris.size(); i++)
				{
				NxU32 triangleIndex = gTouchedTris[i];
				NxU32 flags;
				NxTriangle tri;
				NxTriangle edgeTri;
				hfs->getTriangle(tri, &edgeTri, &flags, triangleIndex, true);
				NxVec3 n = (tri.verts[1]-tri.verts[0]).cross(tri.verts[2]-tri.verts[0]);
				n.normalize();
				for(int i=0;i<3;i++)
					{
					pVertListTouched[vertIndexTouched++] = tri.verts[i].x;
					pVertListTouched[vertIndexTouched++] = tri.verts[i].y;
					pVertListTouched[vertIndexTouched++] = tri.verts[i].z;
					pNormListTouched[normIndexTouched++] = n.x;
					pNormListTouched[normIndexTouched++] = n.y;
					pNormListTouched[normIndexTouched++] = n.z;
					}
				}
			
			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_NORMAL_ARRAY);

			glVertexPointer(3,GL_FLOAT, 0, numVerticesTouched, pVertListTouched);
			glNormalPointer(GL_FLOAT, 0, numVerticesTouched, pNormListTouched);
			
			glDrawArrays(GL_TRIANGLES, 0, numVerticesTouched);

			delete[] pVertListTouched;
			delete[] pNormListTouched;

			int numVertices = (hfs->getHeightField().getNbRows()-1)*(hfs->getHeightField().getNbColumns()-1)*3*2;
			float* pVertList = new float[numVertices*3];
			float* pNormList = new float[numVertices*3];

			int vertIndex = 0;
			int normIndex = 0;

			glColor4f(0.0f, 0.7f, 0.0f, 1.0f);
			for(NxU32 row = 0; row < hfs->getHeightField().getNbRows() - 1; row++)
				{
				for(NxU32 column = 0; column < hfs->getHeightField().getNbColumns() - 1; column++)
					{
					NxTriangle tri;
					NxVec3 n;
					NxU32 triangleIndex;

					triangleIndex = 2 * (row * hfs->getHeightField().getNbColumns() + column);

					if (hfs->getTriangle(tri, NULL, NULL, triangleIndex, true))
						{
						n = (tri.verts[1]-tri.verts[0]).cross(tri.verts[2]-tri.verts[0]);
						n.normalize();
						for(int i=0;i<3;i++)
							{
							pVertList[vertIndex++] = tri.verts[i].x;
							pVertList[vertIndex++] = tri.verts[i].y;
							pVertList[vertIndex++] = tri.verts[i].z;
							pNormList[normIndex++] = n.x;
							pNormList[normIndex++] = n.y;
							pNormList[normIndex++] = n.z;
							}
						}

					triangleIndex++;
					
					if (hfs->getTriangle(tri, NULL, NULL, triangleIndex, true))
						{
						n = (tri.verts[1]-tri.verts[0]).cross(tri.verts[2]-tri.verts[0]);
						n.normalize();
						for(int i=0;i<3;i++)
							{
							pVertList[vertIndex++] = tri.verts[i].x;
							pVertList[vertIndex++] = tri.verts[i].y;
							pVertList[vertIndex++] = tri.verts[i].z;
							pNormList[normIndex++] = n.x;
							pNormList[normIndex++] = n.y;
							pNormList[normIndex++] = n.z;
							}

						}
					}
				}

			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_NORMAL_ARRAY);

			glVertexPointer(3,GL_FLOAT, 0, numVertices, pVertList);
			glNormalPointer(GL_FLOAT, 0, numVertices, pNormList);
			
			unsigned int newNumTriangles = vertIndex / 3;
			glDrawArrays(GL_TRIANGLES, 0, newNumTriangles);

			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_NORMAL_ARRAY);

			delete[] pVertList;
			delete[] pNormList;

			}
		}
#ifdef __PPCGEKKO__	
	char buf[256];
    sprintf(buf,
	"Press a to reset the scene.\n"
	"Press 1 to spawn spheres.\n"
	"Press 2 to spawn capsules.\n"
	"Press + to spawn boxes.\n"
	"Press - to spawn convexes.\n");

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
#ifdef __PPCGEKKO__	
	printf("Press HOME to pause the simulation.\n");
	printf("Press a to reset the scene.\n");
	printf("Press 1 to spawn spheres.\n");
	printf("Press 2 to spawn capsules.\n");
	printf("Press + to spawn boxes.\n");
	printf("Press - to spawn convexes.\n");
#else
	printf("Use left mouse button to orbit.\n");
	printf("Use right mouse button to zoom.\n");
	printf("Use middle mouse button to pan.\n");
	printf("Press p to pause the simulation.\n");
	printf("Press r to reset the scene.\n");
	printf("Press 1 to spawn spheres.\n");
	printf("Press 2 to spawn capsules.\n");
	printf("Press 3 to spawn boxes.\n");
	printf("Press 4 to spawn convexes.\n");
#endif	
	// Initialize Glut
	glutInit(&argc, argv);
	glutInitWindowSize(512, 512);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	int mainHandle = glutCreateWindow("SampleHeightField");
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
	float AmbientColor[] = { 0.0f, 0.1f, 0.2f, 0.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientColor);
	float DiffuseColor[] = { 1.0f, 1.0f, 1.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseColor);
	float SpecularColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularColor);
	float Position[] = { 100.0f, 100.0f, 400.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, Position);
	glEnable(GL_LIGHT0);

	// Initialize physics scene and start the application main loop if scene was created
	if (InitNx())
	{
		atexit(ExitCallback);
		glutMainLoop();
	}

	return 0;
	}

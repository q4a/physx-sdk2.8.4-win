// ===============================================================================
//
//						   NVIDIA PhysX SDK Sample Program
//
// Title: Character Controller
// Description: This sample program shows how to use the CharacterController in
//              a small test scene.
//
// ===============================================================================

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <stdio.h>

#include "NxPhysics.h"
#include "NxController.h"
#include "cooking.h"
#include "UserAllocator.h"
#include "ErrorStream.h"
#include "DebugRenderer.h"
#include "Timing.h"
#include "MouseFilter.h"
#include "Terrain.h"
#include "TerrainRender.h"
#include "CharacterControl.h"
#include "Stream.h"
#include "DrawObjects.h"
#include "Utilities.h"
#include "SamplesVRDSettings.h"


#ifdef __PPCGEKKO__
#include "GLFontRenderer.h"
#endif

#define NB_CHARACTERS	4
static NxVec3			gStartPos[NB_CHARACTERS];
static bool				gIncrease[NB_CHARACTERS];

// Physics code
static UserAllocator*	gMyAllocator = NULL;
static ErrorStream		gErrorStream;
static DebugRenderer	gDebugRenderer;

static bool				gPause = false;
static NxPhysicsSDK*	gPhysicsSDK = NULL;
NxScene*				gScene = NULL;
static NxF32			G = -98.1f;//-50.0f;
//static NxVec3			gDefaultGravity(0.0f, -98.1f, 0.0f);
static NxVec3			gDefaultGravity(0.0f, -50.1f, 0.0f);
static NxF32			gTimestepMultiplier	= 1.0f;
static NxU32			gControlledCharacterIndex = 0;
static bool				gApplyGravity = true;
bool gDrawSkinWidth	= false;
bool gFreeMove		= false;

#ifdef __PPCGEKKO__
static NxVec3	gMoveFoward(0, 0, 0);
#endif

// Render code
static NxVec3	gEye(50.0f, 50.0f, 50.0f);
static NxVec3	gDir(0.0f, 0.0f, 0.0f);
static int		gMouseX = 0;
static int		gMouseY = 0;
static bool		gHasMouseFocus = true;
static NxF32	gTargetYaw = 0.0f;
static NxF32	gTargetPitch = 0.0f;
static NxF32	gSensibility = 0.005f;
static NxF32	gPitchMin = -NxHalfPiF32*0.95f;
static NxF32	gPitchMax = NxHalfPiF32*0.95f;
static NxF32	gDesiredDistance = 10.0f;

static bool		gMotion			= false;
static bool		gUseMouseFilter	= true;
static bool		gRigidLink		= false;
static bool		gFixedStep		= false;
static NxF32	gCharacterSpeed	= 10.0f;
static NxF32	gSpeedMultiplier= 1.0f;

static NxVec3 gColorStatic(1.0f, 0.0f, 0.0f);
static NxVec3 gColorNonCollidable(1.0f, 0.0f, 0.0f);
static NxVec3 gColorCollidableNonPushable(0.5f, 0.5f, 0.5f);
static NxVec3 gCColorCollidablePushable(1.0f, 1.0f, 1.0f);

/*
  a(t) = cte = g
  v(t) = g*t + v0
  y(t) = g*t^2 + v0*t + y0
*/
class Jump
{
	public:
			Jump();

	NxF32	mV0;
	NxF32	mJumpTime;
	bool	mJump;

	void	startJump(NxF32 v0);
	void	stopJump();
	NxF32	getHeight(NxF32 elapsedTime);
};

Jump::Jump() :
	mJump		(false),
	mV0			(0.0f),
	mJumpTime	(0.0f)
{
}

void Jump::startJump(NxF32 v0)
{
	if(mJump)	return;
	mJumpTime = 0.0f;
	mV0	= v0;
	mJump = true;
}

void Jump::stopJump()
{
	if(!mJump)	return;
	mJump = false;
}

NxF32 Jump::getHeight(NxF32 elapsedTime)
{
	if(!mJump)	return 0.0f;
	mJumpTime += elapsedTime;
	NxF32 h = G*mJumpTime*mJumpTime + mV0*mJumpTime;
	return (h - gDefaultGravity.y)*elapsedTime;
}

static Jump gJump[NB_CHARACTERS];

#define TERRAIN_SIZE		33
#define TERRAIN_NB_VERTS	TERRAIN_SIZE*TERRAIN_SIZE
#define TERRAIN_NB_FACES	(TERRAIN_SIZE-1)*(TERRAIN_SIZE-1)*2
#define TERRAIN_OFFSET		-20.0f
#define TERRAIN_WIDTH		10.0f
#define TERRAIN_CHAOS		150.0f

static MouseFilter gFilter;
static TerrainData* gTerrainData = NULL;
static NxActor* gTerrain = NULL;

static void createPhysicsTerrain()
{
	gTerrainData = new TerrainData;
	gTerrainData->init(TERRAIN_SIZE, TERRAIN_OFFSET, TERRAIN_WIDTH, TERRAIN_CHAOS, true);

	// Build physical model
	NxTriangleMeshDesc terrainDesc;
	terrainDesc.numVertices					= TERRAIN_NB_VERTS;
	terrainDesc.numTriangles				= TERRAIN_NB_FACES;
	terrainDesc.pointStrideBytes			= sizeof(NxVec3);
	terrainDesc.triangleStrideBytes			= 3*sizeof(NxU32);
	terrainDesc.points						= gTerrainData->verts;
	terrainDesc.triangles					= gTerrainData->faces;
	terrainDesc.flags						= 0;

	terrainDesc.heightFieldVerticalAxis		= NX_Y;
	terrainDesc.heightFieldVerticalExtent	= -1000.0f;

	NxTriangleMeshShapeDesc terrainShapeDesc;

	MemoryWriteBuffer writeBuffer;
	bool status = CookTriangleMesh(terrainDesc, writeBuffer);
	if (!status) {
		printf("Unable to cook a triangle mesh.");
		exit(1);
	}

	terrainShapeDesc.meshData = gPhysicsSDK->createTriangleMesh(MemoryReadBuffer(writeBuffer.data));
	//
	// Please note about the created Triangle Mesh, user needs to release it when no one uses it to save memory. It can be detected
	// by API "meshData->getReferenceCount() == 0". And, the release API is "gPhysicsSDK->releaseTriangleMesh(*meshData);"
	//

	terrainShapeDesc.group					= GROUP_COLLIDABLE_NON_PUSHABLE;

	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&terrainShapeDesc);
	gTerrain = gScene->createActor(actorDesc);
	gTerrain->userData = NULL;
}

static void createStaticCube(const NxVec3& pos, int size=2, bool rotated=false)
{
	NxBoxShapeDesc boxDesc;
	boxDesc.dimensions		= NxVec3(NxF32(size), NxF32(size), NxF32(size));
	boxDesc.group			= GROUP_COLLIDABLE_PUSHABLE;

	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&boxDesc);
	actorDesc.globalPose.t  = pos;
	if(rotated)
		actorDesc.globalPose.M.rotX(0.5f);
	actorDesc.userData = &gColorStatic;
	NxActor* newActor = gScene->createActor(actorDesc);

	// When creating/changing static actors *after* creating the controller, 
	// we would need to update the cache. However, this function is called *before*.
	// ReportSceneChanged(gControlledCharacterIndex);
}

static void createCube(const NxVec3& pos, int size=2, const NxVec3* initial_velocity=NULL)
{
	static NxU32 count=0;
	NxU32 group = count % 3;
	count++;

	// Create body
	NxBodyDesc bodyDesc;
	bodyDesc.angularDamping	= 0.5f;
//	bodyDesc.maxAngularVelocity	= 10.0f;
	if(initial_velocity)	bodyDesc.linearVelocity = *initial_velocity;

	NxBoxShapeDesc boxDesc;
	boxDesc.dimensions		= NxVec3(NxF32(size), NxF32(size), NxF32(size));
	boxDesc.group			= group;

	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&boxDesc);
//	if(group==GROUP_NON_COLLIDABLE)	actorDesc.flags	|= NX_AF_DISABLE_COLLISION;
	actorDesc.body			= &bodyDesc;
	actorDesc.density		= 10.0f;
	actorDesc.globalPose.t  = pos;
	NxVec3 color(0,0,0);
	switch(group)
	{
		case GROUP_NON_COLLIDABLE:				actorDesc.userData = &gColorNonCollidable;	break;
		case GROUP_COLLIDABLE_NON_PUSHABLE:		actorDesc.userData = &gColorCollidableNonPushable;	break;
		case GROUP_COLLIDABLE_PUSHABLE:			actorDesc.userData = &gCColorCollidablePushable;	break;
	}
	NxActor* newActor		= gScene->createActor(actorDesc);
}

static void createSphere(const NxVec3& pos, NxF32 radius, const NxVec3* initial_velocity=NULL, bool staticShape=false)
{
	// Create body
	NxBodyDesc bodyDesc;
	bodyDesc.angularDamping	= 0.5f;
//	bodyDesc.maxAngularVelocity	= 10.0f;
	if(initial_velocity)	bodyDesc.linearVelocity = *initial_velocity;

	NxSphereShapeDesc sphereDesc;
	sphereDesc.radius		= radius;
//	sphereDesc.group		= GROUP_NON_COLLIDABLE;
	sphereDesc.group		= GROUP_COLLIDABLE_PUSHABLE;

	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&sphereDesc);
	if(!staticShape)
		actorDesc.body		= &bodyDesc;
	actorDesc.density		= 10.0f;
	actorDesc.globalPose.t  = pos;
	actorDesc.userData		= &gCColorCollidablePushable;
	NxActor* newActor		= gScene->createActor(actorDesc);
}

static void createCapsule(const NxVec3& pos, NxF32 radius, NxF32 height, const NxVec3* initial_velocity=NULL, bool staticShape=false)
{
	// Create body
	NxBodyDesc bodyDesc;
	bodyDesc.angularDamping	= 0.5f;
//	bodyDesc.maxAngularVelocity	= 10.0f;
	if(initial_velocity)	bodyDesc.linearVelocity = *initial_velocity;

	NxCapsuleShapeDesc capsuleDesc;
	capsuleDesc.radius		= radius;
	capsuleDesc.height		= height;
	capsuleDesc.group		= GROUP_COLLIDABLE_PUSHABLE;
	//capsuleDesc.group		= GROUP_NON_COLLIDABLE;

	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&capsuleDesc);
	if(!staticShape)
		actorDesc.body		= &bodyDesc;
	actorDesc.density		= 10.0f;
	actorDesc.globalPose.t  = pos;
	actorDesc.userData		= &gCColorCollidablePushable;
	NxActor* newActor		= gScene->createActor(actorDesc);
}

static void createTower(int size)
{
	NxF32 CubeSize = 1.0f;
	NxF32 Spacing = 0.01f;
	NxVec3 Pos(0.0f, CubeSize, 0.0f);
	while(size)
	{
		createCube(Pos, (int)CubeSize);
		Pos.y += (CubeSize * 2.0f + Spacing);
		size--;
	}
}

static void initCCT()
{
	for(int i=0;i<NB_CHARACTERS;i++)	gIncrease[i] = true;
	gStartPos[0] = NxVec3(0.0f, 3.0f, 0.0f);
	gStartPos[1] = NxVec3(10.0f, 3.0f, 0.0f);
	gStartPos[2] = NxVec3(-10.0f, 3.0f, 0.0f);
	gStartPos[3] = NxVec3(0.0f, 3.0f, 10.0f);

	// Create stairs
	for(NxU32 i=0;i<20;i++)
	{
		createStaticCube(NxVec3(-10.0f - NxF32(i), -1.6f + NxF32(i)*0.5f, 0.0f));
	}

	// Create cube too high for autostep
	createStaticCube(NxVec3(-10.0f, -1.0f, -10.0f));

	// Create rotated cube
	createStaticCube(NxVec3(10.0f, 4.0f, -10.0f), 2, true);

	// Create high cube to debug jump
	createStaticCube(NxVec3(10.0f, 10.0f, 10.0f));

	// Create high cube to debug placement API
	createStaticCube(NxVec3(-10.0f, 5.5f, 10.0f));

	createSphere(NxVec3(0.0f, 0.0f, 10.0f), 1.0f, NULL, true);
	createSphere(NxVec3(0.0f, 1.0f, 5.0f), 1.0f, NULL, true);

	createCapsule(NxVec3(0.0f, 0.0f, -10.0f), 2.0f, 3.0f, NULL, true);

	// Create walls
	const int NB=20;
	const int SIZE=2;
	const float MAXSIZE = NB*float(SIZE)*2.0f;

	for(NxU32 i=0;i<NB;i++)
	{
		createStaticCube(NxVec3((NxF32(i)/NxF32(NB-1))*MAXSIZE - MAXSIZE*0.5f, NxF32(SIZE), MAXSIZE*0.5f), SIZE);
	}

	for(NxU32 i=0;i<NB;i++)
	{
		createStaticCube(NxVec3((NxF32(i)/NxF32(NB-1))*MAXSIZE - MAXSIZE*0.5f, NxF32(SIZE), -MAXSIZE*0.5f), SIZE);
	}

	for(NxU32 i=0;i<NB;i++)
	{
		createStaticCube(NxVec3(-MAXSIZE*0.5f, NxF32(SIZE), (NxF32(i)/NxF32(NB-1))*MAXSIZE - MAXSIZE*0.5f), SIZE);
	}

	for(NxU32 i=0;i<NB;i++)
	{
		createStaticCube(NxVec3(MAXSIZE*0.5f, NxF32(SIZE), (NxF32(i)/NxF32(NB-1))*MAXSIZE - MAXSIZE*0.5f), SIZE);
	}

	CreateControllerManager(gMyAllocator);
	InitCharacterControllers(NB_CHARACTERS, gStartPos, 1, *gScene);
}

static bool createScene()
{
	if(gPhysicsSDK)
	{
		// Create a scene
		NxSceneDesc sceneDesc;
		sceneDesc.gravity				= gDefaultGravity;
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

		createPhysicsTerrain();
		initCCT();
	}
	return true;
}

static bool InitNx()
{
	gMyAllocator = new UserAllocator;

	bool status = InitCooking(gMyAllocator);
	if (!status) {
		printf("Unable to initialize the cooking library. Please make sure that you have correctly installed the latest version of the NVIDIA PhysX SDK.");
		exit(1);
	}
	
	gFilter.SetHistoryBufferLength(20);
	gFilter.SetWeightModifier(0.9f);

	// Initialize PhysicsSDK
	NxPhysicsSDKDesc desc;
	NxSDKCreateError errorCode = NXCE_NO_ERROR;
	gPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, gMyAllocator, &gErrorStream, desc, &errorCode);
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

	gPhysicsSDK->setParameter(NX_VISUALIZATION_SCALE, 1.0f);
	gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1);

#if defined(_XBOX) || defined(__CELLOS_LV2__)
	glutRemapButtonExt(8, 'R', false); //Left shoulder to reset scene
	glutRemapButtonExt(9, 'I', false); //Right shoulder to reset character
	glutRemapButtonExt(9, 'I', true); //"Shift + Right shoulder" to rigid link
	glutRemapButtonExt(11, 'P', false); //Start to pause
	glutRemapButtonExt(11, 'H', true); //"Shift + Start" to free move
	glutRemapButtonExt(0, ' ', false); //A/X to Jump
	glutRemapButtonExt(1, 'B', false); //B/O to create cube
	glutRemapButtonExt(2, 'C', false); //X/S to create capsule
	glutRemapButtonExt(3, 'S', false); //Y/T to create sphere
	glutRemapButtonExt(1, 'T', true); //"Shift+B" to create tower
	glutRemapButtonExt(0, 'G', true); //"Shift+A" to apply gravity
	glutRemapButtonExt(2, '+', true); //"Shift+X" to switch controlled character

#endif
#ifdef __PPCGEKKO__
	glutRemapButtonExt('a','r');
	glutRemapButtonExt('b','w');
	glutRemapButtonExt('+','b');
	glutRemapButtonExt('-','c');
	glutRemapButtonExt('1','s');
	glutRemapButtonExt('2','t');
	glutRemapButtonExt(GLUT_KEY_HOME,'+');
#endif
	return createScene();
}

static void releaseScene()
{
	if(gPhysicsSDK)
	{
		if(gScene)
		{
			NxScene* scene = gScene;
			gScene = NULL;
			//releaseCharacterControllers(*scene);
			ReleaseCharacterControllers(*scene);
			ReleaseControllerManager();
			gPhysicsSDK->releaseScene(*scene);
		}
	}
}

static void resetScene()
{
	releaseScene();
	if (!createScene()) exit(0);
}

static void ExitNx()
{
	if(gTerrainData!=NULL)
	{
			delete gTerrainData;
			gTerrainData=NULL;
	}

	releaseScene();
	if (gPhysicsSDK) NxReleasePhysicsSDK(gPhysicsSDK);
	gPhysicsSDK = NULL;

	CloseCooking();

	if(gMyAllocator!=NULL)
	{
			delete gMyAllocator;
			gMyAllocator=NULL;
	}
}

static void KeyboardCallback(unsigned char key, int x, int y)
{
//Add these lines because can't use mouse to move character on wii
#ifdef __PPCGEKKO__	
	static NxVec3 sFront(0.0, 0.0, -1.0);
	static NxVec3 sBack(0.0, 0.0, 1.0);
	static NxVec3 sLeft(-1.0, 0.0, 0.0);
	static NxVec3 sRight(1.0, 0.0, 0.0);
	switch (key)
	{
		case GLUT_KEY_UP:
			gMoveFoward += sFront;
			gCharacterSpeed = 10.0f;
			gMotion = true;
			break;
		case GLUT_KEY_DOWN:
			gMoveFoward += sBack;
			gCharacterSpeed = 10.0f;
			gMotion = true;
			break;
		case GLUT_KEY_LEFT:
			gMoveFoward += sLeft;
			gCharacterSpeed = 10.0f;
			gMotion = true;
			break;
		case GLUT_KEY_RIGHT:
			gMoveFoward += sRight;
			gCharacterSpeed = 10.0f;
			gMotion = true;
			break;
#else
	switch (key)
	{
		case 27:
			exit(0);
			break;
#endif			
		case ' ':
			// ### should pass a max height instead
//			startJump(14.0f);
//			startJump(2.0f);
//			startJump(70.0f);
//			startJump(2.5f);
//			startJump(10.0f);
			gJump[gControlledCharacterIndex].startJump(70.0f);
			break;
		case 'r':
		case 'R':
			resetScene(); 
			break;
		case 'i':
		case 'I':
			ResetCharacterPos(gControlledCharacterIndex, gStartPos[gControlledCharacterIndex]);
			break;
		case 'u':
		case 'U':
			UpdateCharacterExtents(gControlledCharacterIndex, gIncrease[gControlledCharacterIndex]);
			break;
		case 'b':
		case 'B':
			createCube(NxVec3(0.0f, 20.0f, 0.0f), 1);
			break;
		case 'c':
		case 'C':
			createCapsule(NxVec3(0.0f, 20.0f, 0.0f), 1.0f, 4.0f);
			break;
		case 's':
		case 'S':
			createSphere(NxVec3(0.0f, 20.0f, 0.0f), 1.0f);
			break;
		case 't':
		case 'T':
			createTower(10);
			break;
		case 'G':
		case 'g':
			gApplyGravity = !gApplyGravity;
			printf("Apply gravity: %d\n", gApplyGravity);
			break;
		case 'H':
		case 'h':
			gFreeMove = !gFreeMove;
			printf("Free move: %d\n", gFreeMove);
			break;
		case 'p':
		case 'P':
			gPause = !gPause;
			break;
		case 'E':
		case 'e':
			gDrawSkinWidth = !gDrawSkinWidth;
			printf("Draw skin width: %d\n", gDrawSkinWidth);
			break;
		case 'f':
		case 'F':
			gFixedStep = !gFixedStep; 
			printf("Fixed timestep: %d\n", gFixedStep);
			break;
		case 'l':
		case 'L':
			gRigidLink = !gRigidLink;
			printf("Rigid camera link: %d\n", gRigidLink);
			break;
		case 'm':
		case 'M':
			gUseMouseFilter = !gUseMouseFilter;
			printf("Mouse filter: %d\n", gUseMouseFilter);
			break;
		case '1':
			gTimestepMultiplier = 1.0f;
			printf("Artificial timestep multiplier: %f\n", gTimestepMultiplier);
			break;
		case '2':
			gTimestepMultiplier = 0.1f;
			printf("Artificial timestep multiplier: %f\n", gTimestepMultiplier);
			break;
		case '3':
			gTimestepMultiplier = 10.0f;
			printf("Artificial timestep multiplier: %f\n", gTimestepMultiplier);
			break;
		case '4':
			gDesiredDistance += 2.0f;
			printf("Dist to camera: %f\n", gDesiredDistance);
			break;
		case '5':
			gDesiredDistance -= 2.0f;
			printf("Dist to camera: %f\n", gDesiredDistance);
			break;
		case '7':
			gSpeedMultiplier += 0.1f;
			printf("Speed multiplier: %f\n", gSpeedMultiplier);
			break;
		case '8':
			gSpeedMultiplier -= 0.1f;
			printf("Speed multiplier: %f\n", gSpeedMultiplier);
			break;
		case '9':
			gSpeedMultiplier = 1.0f;
			printf("Speed multiplier: %f\n", gSpeedMultiplier);
			break;
		case '0':
			{
				NxExtendedVec3 tmp = GetCharacterPos(gControlledCharacterIndex);
				printf("Character's Position: %.4f, %.4f, %.4f\n", tmp.x, tmp.y, tmp.z);
			}
			break;
		case 'w':
		case 'W':
			{
			NxVec3 t = gEye;
			NxVec3 Vel = gDir;
			Vel.normalize();
			Vel*=200.0f;
			createCube(t, 1, &Vel);
			}
			break;
		case '+':
			gControlledCharacterIndex++;
			if(gControlledCharacterIndex == NB_CHARACTERS)	gControlledCharacterIndex = 0;
			break;
		case '-':
			if(gControlledCharacterIndex == 0)	
				gControlledCharacterIndex = NB_CHARACTERS - 1;
			else
				gControlledCharacterIndex--;
			break;
	}
}

static void ArrowKeyCallback(int key, int x, int y)
{
	if (key == 101)
	{
		gCharacterSpeed = 10.0f;
		gMotion = true;
	}
	else
		KeyboardCallback(key,x,y);
}

static void ArrowKeyUpCallback(int key, int x, int y)
{
	if (key == 101)
	{
		gMotion = false;
	}
}
void repositionMouse(NxF32& deltaMouseX, NxF32& deltaMouseY)
{
	int x = glutGet(GLUT_WINDOW_X);
	int y = glutGet(GLUT_WINDOW_Y);
	int width = glutGet(GLUT_WINDOW_WIDTH);
	int height = glutGet(GLUT_WINDOW_HEIGHT);

	int cx = (width>>1);
	int cy = (height>>1);

	glutWarpPointer(cx, cy);

	deltaMouseX = NxF32(cx - gMouseX);
	deltaMouseY = NxF32(cy - gMouseY);
}

static void RenderCallback();

static void MouseCallback(int button, int state, int x, int y)
{
	gMouseX = x;
	gMouseY = y;

	if(button==GLUT_LEFT_BUTTON)
	{
		gCharacterSpeed = 10.0f;
		gMotion = state==GLUT_DOWN;
	}
	else if(button==GLUT_RIGHT_BUTTON)
		{
		gCharacterSpeed = 50.0f;
		gMotion = state==GLUT_DOWN;
		}
}

static NxRay gWorldRay;

static NxF32 cameraHit()
{
	NxShape* characterShape = GetCharacterActor(gControlledCharacterIndex)->getShapes()[0];

	NxExtendedVec3 cpos = GetCharacterPos(gControlledCharacterIndex);

	NxRay worldRay;
	worldRay.orig.x	= cpos.x;
	worldRay.orig.y	= cpos.y;
	worldRay.orig.z	= cpos.z;
	worldRay.dir	= -gDir;
	NxRaycastHit hit;
	characterShape->setFlag(NX_SF_DISABLE_RAYCASTING, true);
	NxShape* shape = gScene->raycastClosestShape(worldRay, NX_ALL_SHAPES, hit);
	characterShape->setFlag(NX_SF_DISABLE_RAYCASTING, false);

	if(!gPause)	gWorldRay = worldRay;

	return shape ? hit.distance : NX_MAX_F32;
}

static void UpdateCameraVariables(int x, int y)
{
	gMouseX = x;
	gMouseY = y;

	NxF32 dmx, dmy;
	repositionMouse(dmx, dmy);

	// Apply mouse filter
	if(gUseMouseFilter)	gFilter.Apply(dmx, dmy);

	gTargetYaw		+= dmx * gSensibility;
	gTargetPitch	+= dmy * gSensibility;

	// Clamp pitch
	if(gTargetPitch<gPitchMin)	gTargetPitch = gPitchMin;
	if(gTargetPitch>gPitchMax)	gTargetPitch = gPitchMax;
}

static void MotionCallback(int x, int y)
{
	if (gHasMouseFocus) UpdateCameraVariables(x, y);
#if !defined(_XBOX) & !defined(__CELLOS_LV2__)
	RenderCallback();	// Glut insanity
#endif
}

static void PassiveMotionCallback(int x, int y)
{
	if (gHasMouseFocus) UpdateCameraVariables(x, y);
#if !defined(_XBOX) & !defined(__CELLOS_LV2__)
	RenderCallback();	// Glut insanity
#endif
}

static void EntryFunc(int state)
{
	gHasMouseFocus = state!=0;
}




static void RenderCallback()
{
	if(!gScene)	return;
#ifdef __PPCGEKKO__
	NxF32 elapsedTime = 1.0/60.0;
#else
	NxF32 elapsedTime = getElapsedTime();
#endif	
	elapsedTime *= gTimestepMultiplier;
	if(elapsedTime <= 0)
		elapsedTime = 0;
	
	// Physics code
	if(!gPause)	
	{
		for(NxU32 c=0;c<NB_CHARACTERS;c++)
		{
			NxVec3 disp = gDefaultGravity;
			if(!gApplyGravity)	disp.zero();

	//		disp.y += getHeight(0.01f/*elapsedTime*/);
	//		disp.y += getHeight(elapsedTime);

			if(gMotion && c==gControlledCharacterIndex)
			{
#ifdef __PPCGEKKO__
				NxVec3 horizontalDisp = gMoveFoward;
#else
				NxVec3 horizontalDisp = gDir;
#endif				
				horizontalDisp.y = 0.0f;
				horizontalDisp.normalize();

				disp += horizontalDisp * gCharacterSpeed * gSpeedMultiplier;
			}

	// Here:
	// - kinematic & shape in sync
	// - but debug visualization screwed up
			NxU32 collisionFlags = MoveCharacter(c, *gScene, disp, elapsedTime, COLLIDABLE_MASK, gJump[c].getHeight(elapsedTime));
			if(collisionFlags & NXCC_COLLISION_DOWN) gJump[c].stopJump();
		}

		float TimeStep = 1.0f / 60.0f;
		if(gFixedStep) gScene->setTiming(TimeStep, 1, NX_TIMESTEP_FIXED);
		else gScene->setTiming(TimeStep, 1, NX_TIMESTEP_VARIABLE);

		//NxU32 nb = gScene->cullShapes(0, NULL, NX_ALL_SHAPES, 0, NULL, NULL);
		
		gScene->simulate(elapsedTime);
		gScene->flushStream();
		gScene->fetchResults(NX_RIGID_BODY_FINISHED, true);

		NxReal maxTimestep;
		NxTimeStepMethod method;
		NxU32 maxIter;
		NxU32 numSubSteps;
		gScene->getTiming(maxTimestep, maxIter, method, &numSubSteps);
		if(numSubSteps)	UpdateControllers();

// Here:
// - debug visualization ok
// - but kinematic & shape not in sync
//NxU32 collisionFlags = moveCharacter(0, *gScene, disp, elapsedTime, COLLIDABLE_MASK, getHeight(elapsedTime));
//if(collisionFlags & NXCC_COLLISION_DOWN)
//	stopJump();

	}
	// ~Physics code

	// Setup the view matrix
	NxMat33 RotX;	RotX.rotX(gTargetPitch);
	NxMat33 RotY;	RotY.rotY(gTargetYaw+NxPiF32);
	NxMat33 ViewMat = RotY*RotX;
	gDir = ViewMat.getColumn(2);
	//printf("Dir %f %f %f - Pitch %f - Yaw %f\n", Dir.x, Dir.y, Dir.z, gTargetPitch, gTargetYaw);

	NxF32 dist = cameraHit();
	//NxF32 gMinDistance = 20.0f;
	//if(dist>gMinDistance) dist -= gMinDistance;
	if(dist>gDesiredDistance) dist = gDesiredDistance;

	NxExtendedVec3 cpos = GetCharacterPos(gControlledCharacterIndex);
	NxVec3 cameraGoal(cpos.x, cpos.y, cpos.z);
	cameraGoal -= gDir*dist;
	//NxVec3 cameraGoal = - gDir*dist;
	if(gRigidLink)
	{
		gEye = cameraGoal;
	} else {
		NxVec3 delta = cameraGoal - gEye;
		NxF32 coeff = elapsedTime*20.0f;
		if(coeff>1.0f)	coeff=1.0f;
		gEye += delta*coeff;
	}
	

	// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Setup camera
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, (float)glutGet(GLUT_WINDOW_WIDTH)/(float)glutGet(GLUT_WINDOW_HEIGHT), 1.0f, 10000.0f);
	gluLookAt(gEye.x, gEye.y, gEye.z, gEye.x + gDir.x, gEye.y + gDir.y, gEye.z + gDir.z, 0.0f, 1.0f, 0.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

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

				if (actor->userData) {
					NxVec3* color = (NxVec3*)actor->userData;
					glColor4f(color->x, color->y, color->z, 1.0f);
				}

				DrawActor(actor);
			}
		}
	}

	if(gTerrainData) renderTerrain(*gTerrainData, false);
	RenderCharacters();

	if(gScene) gDebugRenderer.renderData(*gScene->getDebugRenderable());

	glDisable(GL_LIGHTING);
	glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
#ifdef __PPCGEKKO__
	glBegin(GL_LINES, 2);
	glVertex3d(	gWorldRay.orig.x, gWorldRay.orig.y, gWorldRay.orig.z );
	glVertex3d(	gWorldRay.orig.x + gWorldRay.dir.x * 100.0f,
				gWorldRay.orig.y + gWorldRay.dir.y * 100.0f, 
				gWorldRay.orig.z + gWorldRay.dir.z * 100.0f);
	glEnd();
	
	gMoveFoward.x = 0.0;  gMoveFoward.y = 0.0; gMoveFoward.z = 0.0;
	gMotion = false;
#else	
	static GLfloat vertices[2*3];
	vertices[0] = gWorldRay.orig.x;
	vertices[1] = gWorldRay.orig.y;
	vertices[2] = gWorldRay.orig.z;
	vertices[3] = gWorldRay.orig.x + gWorldRay.dir.x * 100.0f;
	vertices[4] = gWorldRay.orig.y + gWorldRay.dir.y * 100.0f;
	vertices[5] = gWorldRay.orig.z + gWorldRay.dir.z * 100.0f;

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices);
	glDrawArrays(GL_LINES, 0, 2);
	glDisableClientState(GL_VERTEX_ARRAY);
#endif
	
	glEnable(GL_LIGHTING);

#ifdef __PPCGEKKO__	
	char buf[256];
    sprintf(buf,
    "Keys:\n"
	"  A   - reset scene\n"
	"  HOME   - control next character\n"
	"  +   - create box\n"
	"  1   - create sphere\n"
	"  -   - create capsule\n"
	"  2   - create tower\n"
	"  B   - throw box\n"
	"  Arrow key   - move actor\n" );
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
	// Initialize Glut
	glutInit(&argc, argv);
	glutInitWindowSize(512, 512);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	unsigned int gMainHandle = glutCreateWindow("SampleCharacterController");
	glutSetWindow(gMainHandle);
	glutDisplayFunc(RenderCallback);
	glutReshapeFunc(ReshapeCallback);
	glutIdleFunc(IdleCallback);
	glutKeyboardFunc(KeyboardCallback);
	glutSpecialFunc(ArrowKeyCallback);
	glutSpecialUpFunc(ArrowKeyUpCallback);
#ifndef _XBOX
	glutMouseFunc(MouseCallback);
#endif
	glutMotionFunc(MotionCallback);
	glutPassiveMotionFunc(PassiveMotionCallback);
	glutEntryFunc(EntryFunc);

	// Setup default render states
	glClearColor(0.3f, 0.4f, 0.5f, 1.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
#ifndef __PPCGEKKO__	
	glEnable(GL_CULL_FACE);
#endif

	// Setup lighting
	glEnable(GL_LIGHTING);
	NxF32 AmbientColor[] = { 0.0f, 0.1f, 0.2f, 0.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientColor);
	NxF32 DiffuseColor[] = { 1.0f, 1.0f, 1.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseColor);
	NxF32 SpecularColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularColor);
	NxF32 Position[] = { -10.0f, 100.0f, -4.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, Position);
	glEnable(GL_LIGHT0);

	printf("Keys:\n\n");
#ifdef __PPCGEKKO__
	printf("  A   - reset scene\n");
	printf("  HOME   - control next character\n");
	printf("  +   - create box\n");
	printf("  1   - create sphere\n");
	printf("  -   - create capsule\n");
	printf("  2   - create tower\n");
	printf("  B   - throw box\n");
	printf("  Arrow key   - move actor\n");
#else	
	printf("  R   - reset scene\n");
	printf("  I   - reset character position\n");
	printf("  U   - update chatacter extents (placement API)\n");
	printf("  B   - create box\n");
	printf("  S   - create sphere\n");
	printf("  C   - create capsule\n");
	printf("  T   - create tower\n");
	printf("  W   - throw box\n");
	printf("  P   - pause\n");
	printf("  F   - fixed/variable timestep\n");
	printf("  L   - rigid/smooth camera link\n");
	printf("  M   - mouse filter on/off\n");
	printf("  E   - draw skin width on/off\n");
	printf("  G   - apply gravity on/off\n");
	printf("  H   - free move on/off\n");
	printf("  1   - artificial timestep multiplier (1)\n");
	printf("  2   - artificial timestep multiplier (0.1)\n");
	printf("  3   - artificial timestep multiplier (10)\n");
	printf("  4   - increase distance to camera\n");
	printf("  5   - decrease distance to camera\n");
	printf("  7   - increase speed\n");
	printf("  8   - decrease speed\n");
	printf("  9   - reset default speed\n");
	printf("  +   - control next character\n");
	printf("  -   - control previous character\n");
	printf("Space - jump\n");
#endif

	// Initialize physics scene and start the application main loop if scene was created
	if (InitNx())
	{
		atexit(ExitNx);
		glutMainLoop();
	}

	return 0;
}

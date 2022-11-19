// ===============================================================================
//
//						   NVIDIA PhysX SDK Sample Program
//
// Title: Terrain Sample
// Description: This sample program builds a terrain and shows some of the debug
//              information that you can receive from the SDK.
//
// ===============================================================================

#include <stdio.h>
#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

//Physics code
#undef random
#include "NxPhysics.h"
#include "UserAllocator.h"
#include "Terrain.h"
#include "TerrainRender.h"
#include "Stream.h"
#include "ErrorStream.h"
#include "cooking.h"
#include "Utilities.h"
#include "SamplesVRDSettings.h"

#ifdef __PPCGEKKO__
#include "GLFontRenderer.h"
#endif

static ErrorStream		gMyErrorStream;
static UserAllocator*	gMyAllocator = NULL;
static bool				gPause = false;
static NxPhysicsSDK*	gPhysicsSDK = NULL;
static NxScene*			gScene = NULL;
static NxVec3			gDefaultGravity(0.0f, -9.81f, 0.0f);

#define TERRAIN_SIZE	33
#define TERRAIN_CHAOS	300.0f

#define TERRAIN_OFFSET	-20.0f
#define TERRAIN_WIDTH	20.0f
static TerrainData* gTerrainData = NULL;

// Render code
static NxVec3 Eye(50.0f, 50.0f, 50.0f);
static NxVec3 Dir(-0.6,-0.2,-0.7);
static NxVec3 N;
static int mx = 0;
static int my = 0;

static NxArray<NxU32> gTouchedTris;
static NxActor* gTerrain = NULL;
static NxTriangleMesh* terrainMesh = NULL;

class MyContactReport : public NxUserContactReport
{
public:
	virtual void  onContactNotify(NxContactPair& pair, NxU32 events)
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
					if(i.getSeparation()<0.0f)
					{
						const NxVec3& contactPoint = i.getPoint();

						NxU32 faceIndex = i.getFeatureIndex0();
						if(faceIndex==0xffffffff)	faceIndex = i.getFeatureIndex1();
						if(faceIndex!=0xffffffff)
						{
							gTouchedTris.pushBack(faceIndex);
						}
					}
				}
			}
		}
	}

}gMyContactReport;

static void CreatePhysicsTerrain()
{
	if(gTerrainData!=NULL)
	{
		delete gTerrainData;
		gTerrainData=NULL;		
	}

	gTerrainData = new TerrainData;
	gTerrainData->init(TERRAIN_SIZE, TERRAIN_OFFSET, TERRAIN_WIDTH, TERRAIN_CHAOS);

	// Build physical model
	NxTriangleMeshDesc terrainDesc;
	terrainDesc.numVertices					= gTerrainData->nbVerts;
	terrainDesc.numTriangles				= gTerrainData->nbFaces;
	terrainDesc.pointStrideBytes			= sizeof(NxVec3);
	terrainDesc.triangleStrideBytes			= 3*sizeof(NxU32);
	terrainDesc.points						= gTerrainData->verts;
	terrainDesc.triangles					= gTerrainData->faces;
	terrainDesc.flags						= 0;

	terrainDesc.heightFieldVerticalAxis		= NX_Y;
	terrainDesc.heightFieldVerticalExtent	= -1000.0f;

	bool status = InitCooking(gMyAllocator, &gMyErrorStream);
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
	terrainMesh = gPhysicsSDK->createTriangleMesh(readBuffer);

	//
	// Please note about the created Triangle Mesh, user needs to release it when no one uses it to save memory. It can be detected
	// by API "meshData->getReferenceCount() == 0". And, the release API is "gPhysicsSDK->releaseTriangleMesh(*meshData);"
	//

	NxTriangleMeshShapeDesc terrainShapeDesc;
	terrainShapeDesc.meshData				= terrainMesh;
	terrainShapeDesc.shapeFlags				= NX_SF_FEATURE_INDICES;

	NxActorDesc ActorDesc;
	ActorDesc.shapes.pushBack(&terrainShapeDesc);
	gTerrain = gScene->createActor(ActorDesc);
	gTerrain->userData = (void*)0;

	CloseCooking();
}

static void RenderTerrain()
{
	if(gTerrainData)
	{		
#if _XBOX
		renderTerrain(*gTerrainData);
		renderTerrainTriangles(*gTerrainData, gTouchedTris.size(), &gTouchedTris[0]);	
#else
		renderTerrainTriangles(*gTerrainData, gTouchedTris.size(), &gTouchedTris[0]);	
		renderTerrain(*gTerrainData);
#endif
	}
}

static void CreateCube(const NxVec3& pos, int size=2, const NxVec3* initial_velocity=NULL)
{
	// Create body
	NxBodyDesc BodyDesc;
	BodyDesc.angularDamping	= 0.5f;
	if(initial_velocity) BodyDesc.linearVelocity = *initial_velocity;

	NxBoxShapeDesc BoxDesc;
	BoxDesc.dimensions		= NxVec3(float(size), float(size), float(size));

	NxActorDesc ActorDesc;
	ActorDesc.shapes.pushBack(&BoxDesc);
	ActorDesc.body			= &BodyDesc;
	ActorDesc.density		= 10.0f;
	ActorDesc.globalPose.t  = pos;
	NxActor* newActor = gScene->createActor(ActorDesc);
	newActor->userData = (void*)size;

	gScene->setActorPairFlags(*gTerrain, *newActor, NX_NOTIFY_ON_TOUCH);
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
	if (!gMyAllocator)
		gMyAllocator = new UserAllocator();

	// Initialize PhysicsSDK
	NxPhysicsSDKDesc desc;
	NxSDKCreateError errorCode = NXCE_NO_ERROR;
	gPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, gMyAllocator, &gMyErrorStream, desc, &errorCode);
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
	gPhysicsSDK->setParameter(NX_DEFAULT_SLEEP_ENERGY, 0.01f);

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
	CreatePhysicsTerrain();

	CreateTower(30);

	return true;
}

static void ExitCallback()
{
#ifdef __PPCGEKKO__
	gTouchedTris.clear();
	gTouchedTris.~NxArray<NxU32>();
#endif
	
	if(gTerrainData!=NULL)
	{
		delete gTerrainData;
		gTerrainData=NULL;
	}

	if(gPhysicsSDK != NULL)
	{
		if(gScene != NULL) gPhysicsSDK->releaseScene(*gScene);
		gScene = NULL;
		NxReleasePhysicsSDK(gPhysicsSDK);
		gPhysicsSDK = NULL;
	}

	if(gMyAllocator!=NULL)
	{
		delete gMyAllocator;
		gMyAllocator=NULL;
	}
}

static void ResetScene()
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
	ExitCallback();
	if (!InitNx()) exit(0);
}


static void KeyboardCallback(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		exit(0);
		break;
	case 'r':
	case 'R':
		ResetScene();
		break;
	case ' ':
		CreateCube(NxVec3(0.0f, 20.0f, 0.0f), 1+(rand()&3));
		break;
	case 's':
	case 'S':
		CreateStack(10);
		break;
	case 'b':
	case 'B':
		CreateStack(30);
	
		break;
	case 't':
	case 'T':
		CreateTower(30);
		break;
	case 'p':
	case 'P':
		gPause = !gPause;
		break;
	case 101: case '8':	Eye += Dir * 2.0f; break;
	case 103: case '2':	Eye -= Dir * 2.0f; break;
	case 100: case '4':	Eye -= N * 2.0f; break;
	case 102: case '6':	Eye += N * 2.0f; break;
	case 'w':
	case 'W':
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
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, (float)glutGet(GLUT_WINDOW_WIDTH)/(float)glutGet(GLUT_WINDOW_HEIGHT), 1.0f, 10000.0f);
	gluLookAt(Eye.x, Eye.y, Eye.z, Eye.x + Dir.x, Eye.y + Dir.y, Eye.z + Dir.z, 0.0f, 1.0f, 0.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Keep physics & graphics in sync
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	{
		int nbActors = gScene->getNbActors();
		NxActor** actors = gScene->getActors();
		while(nbActors--)
		{
			NxActor* actor = *actors++;

			if(!actor->userData)	continue;

			glPushMatrix();
			float glmat[16];
			actor->getGlobalPose().getColumnMajor44(glmat);		
			glMultMatrixf(glmat);
			glutSolidCube(float(size_t(actor->userData))*2.0f);
			glPopMatrix();
		}
	}

	RenderTerrain();

#ifdef __PPCGEKKO__	
	char buf[256];
    sprintf(buf,"FPS: %d\nRender time:%d ms \nPhysic time:%d ms \nTotal  time:%d ms\n"
    "Use the arrow keys to move the camera.\n"
	"Press the keys b,HOME,1 and 2 to create various things.\n"
	"Press a to reset the scene.\n");

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
	printf("Use the arrow keys to move the camera.\n");
	printf("Press the keys b,HOME,1 and 2 to create various things.\n");
	printf("Press a to reset the scene.\n");
#else
	printf("Use the arrow keys or 2, 4, 6 and 8 to move the camera.\n");
	printf("Use the mouse to rotate the camera.\n");
	printf("Press p to pause the simulation.\n");
	printf("Press the keys w,space,s,b, and t to create various things.\n");
	printf("Press r to reset the scene.\n");
#endif	
	// Initialize Glut
	glutInit(&argc, argv);
	glutInitWindowSize(512, 512);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	int mainHandle = glutCreateWindow("SampleTerrain");
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
	glutRemapButtonExt('a','r');
	glutRemapButtonExt('b','w');
	glutRemapButtonExt('1','s');
	glutRemapButtonExt('2','b');
	glutRemapButtonExt(GLUT_KEY_HOME,'t');
	glutRemapButtonExt(GLUT_KEY_UP,101);
	glutRemapButtonExt(GLUT_KEY_DOWN,103);
	glutRemapButtonExt(GLUT_KEY_LEFT,100);
	glutRemapButtonExt(GLUT_KEY_RIGHT,102);
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
	float AmbientColor[] = { 0.0f, 0.1f, 0.2f, 0.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientColor);
	float DiffuseColor[] = { 1.0f, 1.0f, 1.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseColor);
	float SpecularColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularColor);
	float Position[] = { -10.0f, 100.0f, -4.0f, 1.0f };
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

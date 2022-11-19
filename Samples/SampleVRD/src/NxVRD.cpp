// ===============================================================================
//
//						   NVIDIA PhysX SDK Sample Program
//
// Title: VRD Connection Sample
// Description: This sample shows different ways to connect to the Visual Remote 
//				Debugger (VRD) and how to define a camera view for the VRD.
//
// Please note that this sample shows advanced use of the VRD, if you just want
// to debug a simple scene, the only thing you need to do is:
//       gPhysicsSDK->getFoundationSDK().getRemoteDebugger()->connect("localhost");
// (Where you need to exchange "localhost" for the IP of the machine running the
// VRD, if you are debugging on two machines, a PS3, or an Xbox)
//
// This sample defines a camera view in the VRD: "Camera 1". Change camera in the
// VRD through the View/Camera menu.
//
// ===============================================================================

#include <stdio.h>
#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "NxPhysics.h"
#include "ErrorStream.h"
#include "PerfRenderer.h"
#include "GLFontRenderer.h"
#include "Utilities.h"
#include "SamplesVRDSettings.h"
#include "UserAllocator.h"

// Physics
static NxPhysicsSDK*	gPhysicsSDK = NULL;
static NxScene*		gScene = NULL;
static PerfRenderer    gPerfRenderer;
static UserAllocator*	gAllocator = NULL;

// Rendering
static NxVec3	gEye(50.0f, 50.0f, 50.0f);
static NxVec3	gDir(-0.6f,-0.2f,-0.7f);
static NxVec3	gViewY;
static int		gMouseX = 0;
static int		gMouseY = 0;
static bool		gConnectToVRD = false;

static int		gAppData = 0;

// This class is so that we more easily can keep track of the camera object
// for the VRD. If we don't make sure that it is created when setting its
// parameters, we will get an error in the VRD.
class KeepTrackOfVRD: public NxRemoteDebuggerEventListener
{
public:
	KeepTrackOfVRD() {}

	void updateCamera()
	{
		gPhysicsSDK->getFoundationSDK().getRemoteDebugger()->writeParameter(gEye, this, false, "Origin", NX_DBG_EVENTMASK_EVERYTHING);
		gPhysicsSDK->getFoundationSDK().getRemoteDebugger()->writeParameter(gEye+gDir, this, false, "Target", NX_DBG_EVENTMASK_EVERYTHING);
	}

//////////////////////////////////////////////////////////////////////////
// Implementation of NxRemoteDebuggerEventListener
	virtual void onConnect() 
	{
		//"this" is just used as an identifier for the camera, could be any value, 
		// as long as it is unique. 
		gPhysicsSDK->getFoundationSDK().getRemoteDebugger()->createObject(this, NX_DBG_OBJECTTYPE_CAMERA, "Camera 1", NX_DBG_EVENTMASK_EVERYTHING);
		gPhysicsSDK->getFoundationSDK().getRemoteDebugger()->writeParameter(gEye, this, true, "Origin", NX_DBG_EVENTMASK_EVERYTHING);
		gPhysicsSDK->getFoundationSDK().getRemoteDebugger()->writeParameter(gEye+gDir, this, true, "Target", NX_DBG_EVENTMASK_EVERYTHING);
		gPhysicsSDK->getFoundationSDK().getRemoteDebugger()->writeParameter(NxVec3(0,1,0), this, true, "Up", NX_DBG_EVENTMASK_EVERYTHING);

		//also tell the application that the appdata object is not available yet
		gAppData = 0;
	}

	virtual void onDisconnect() 
	{
		//Remove the camera object from the VRD
		gPhysicsSDK->getFoundationSDK().getRemoteDebugger()->removeObject(this, NX_DBG_EVENTMASK_EVERYTHING);
	}

	//These calls are not interesting for our use
	virtual void beforeMaskChange(NxU32 oldMask, NxU32 newMask) {}
	virtual void afterMaskChange(NxU32 oldMask, NxU32 newMask) {}
} gKeepTrackOfVRD;


static void connectToVRD()
{
	// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	GLFontRenderer::setScreenResolution(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	GLFontRenderer::setColor(1.0f, 0.0f, 0.0f, 1.0f);
	GLFontRenderer::print(0.33f, 0.5f, 0.075f, "Connecting ...", false, 11, true);
	glutSwapBuffers();

	printf("Connect to Visual Remote Debugger (VRD) running on host \"%s\" ... \n", SAMPLES_VRD_HOST);

	//If you are running the VRD with default settings on the same PC as the application 
	// you are debugging, you could simply start it by calling:
	// gPhysicsSDK->getFoundationSDK().getRemoteDebugger()->connect("localhost");
	//But, this sample can be run on several platforms, thus we use the settings from the
	// SamplesVRDSettings.h file. The following line is the ONLY thing you must add, in
	// order to debug a PhysX application using the Visual Remote Debugger:
	gPhysicsSDK->getFoundationSDK().getRemoteDebugger()->connect(SAMPLES_VRD_HOST, SAMPLES_VRD_PORT, SAMPLES_VRD_EVENTMASK);

	if (gPhysicsSDK->getFoundationSDK().getRemoteDebugger()->isConnected())
	{
		//Also create a VRD object that contains some application-specific information.
		//I have not made sure that this information will be available when connecting to the VRD
		// by pressing 'r'. It could easily be handled through gKeepTrackOfVRD as the camera,
		// and always be available.
		gPhysicsSDK->getFoundationSDK().getRemoteDebugger()->createObject(&gAppData, NX_DBG_OBJECTTYPE_GENERIC, "AppData", NX_DBG_EVENTMASK_EVERYTHING);
		gPhysicsSDK->getFoundationSDK().getRemoteDebugger()->writeParameter("Whatever data would be interesting for debugging", &gAppData, true, "Info", NX_DBG_EVENTMASK_EVERYTHING);
		gPhysicsSDK->getFoundationSDK().getRemoteDebugger()->createObject(&gAppData+1, NX_DBG_OBJECTTYPE_VECTOR, "AVector", NX_DBG_EVENTMASK_EVERYTHING);
		gPhysicsSDK->getFoundationSDK().getRemoteDebugger()->writeParameter(NxVec3(0, 0, 0), &gAppData+1, true, "Origin", NX_DBG_EVENTMASK_EVERYTHING);
		gPhysicsSDK->getFoundationSDK().getRemoteDebugger()->writeParameter(NxVec3(1, 1, 1), &gAppData+1, true, "Extent", NX_DBG_EVENTMASK_EVERYTHING);
		gPhysicsSDK->getFoundationSDK().getRemoteDebugger()->addChild(&gAppData, &gAppData+1, NX_DBG_EVENTMASK_EVERYTHING);
		gAppData = 1; //Tells our application that we have created the VRD object and that it is ok to set information in it

		//We currently need to make the appdata object a child of the scene, since it contains objects that 
		//should be drawn, see below. This will not be required in future versions.

		printf("Visual Remote Debugger connected.\n");
	}
	else
	{
		printf("Error: Visual Remote Debugger connection failed.\n");

#ifdef __CELLOS_LV2__
		if (!strcmp(SAMPLES_VRD_HOST,"localhost"))
			printf("Please replace \"localhost\" with the IP Address of the host PC running the RemoteDebugger\n");
#endif
	}
}

static bool InitNx()
{
	if (!gAllocator)
		gAllocator = new UserAllocator;

	// Initialize PhysicsSDK
	NxPhysicsSDKDesc desc;
	NxSDKCreateError errorCode = NXCE_NO_ERROR;
	gPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, gAllocator, new ErrorStream(), desc, &errorCode);
	if(gPhysicsSDK == NULL) 
	{
		printf("\nSDK create error (%d - %s).\nUnable to initialize the PhysX SDK, exiting the sample.\n\n", errorCode, getNxSDKCreateError(errorCode));
		return false;
	}
	// Register an event listener for the VRD, this is so that we can create the camera before it is used
	gPhysicsSDK->getFoundationSDK().getRemoteDebugger()->registerEventListener(&gKeepTrackOfVRD);
	gPhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.05f);

	connectToVRD();
	
	// Create a scene
	NxSceneDesc sceneDesc;
	sceneDesc.gravity				= NxVec3(0.0f, -9.81f, 0.0f);
	gScene = gPhysicsSDK->createScene(sceneDesc);
	if(gScene == NULL) 
	{
		printf("\nError: Unable to create a PhysX scene, exiting the sample.\n\n");
		return false;
	}
	//Make the appdata object a member of the scene.
	gPhysicsSDK->getFoundationSDK().getRemoteDebugger()->addChild(gScene, &gAppData, NX_DBG_EVENTMASK_EVERYTHING);

	// Set default material
	NxMaterial* defaultMaterial = gScene->getMaterialFromIndex(0);
	defaultMaterial->setRestitution(0.0f);
	defaultMaterial->setStaticFriction(0.5f);
	defaultMaterial->setDynamicFriction(0.5f);

	// Create ground plane
	NxPlaneShapeDesc planeDesc;
	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&planeDesc);
	gScene->createActor(actorDesc);

	return true;
}

static void ExitNx()
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

static void CreateCube(const NxVec3& pos, int size=2, const NxVec3* initialVelocity=NULL)
{
	if(gScene == NULL) return;	

	// Create body
	NxBodyDesc bodyDesc;
	bodyDesc.angularDamping	= 0.5f;
	if(initialVelocity) bodyDesc.linearVelocity = *initialVelocity;

	NxBoxShapeDesc boxDesc;
	boxDesc.dimensions = NxVec3((float)size, (float)size, (float)size);

	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&boxDesc);
	actorDesc.body			= &bodyDesc;
	actorDesc.density		= 10.0f;
	actorDesc.globalPose.t  = pos;
	gScene->createActor(actorDesc)->userData = (void*)size_t(size);
}

static void CreateCubeFromEye(int size)
{
	NxVec3 t = gEye;
	NxVec3 vel = gDir;
	vel.normalize();
	vel*=200.0f;
	CreateCube(t, size, &vel);
}

static void CreateStack(int size)
{
	const float cubeSize = 2.0f;
	const float spacing = 0.0001f;
	NxVec3 pos(0.0f, cubeSize, 0.0f);
	float offset = -size * (cubeSize * 2.0f + spacing) * 0.5f;
	while(size)
	{
		for(int i=0;i<size;i++)
		{
			pos.x = offset + (float)i * (cubeSize * 2.0f + spacing);
			CreateCube(pos, (int)cubeSize);
		}

		offset += cubeSize;
		pos.y += (cubeSize * 2.0f + spacing);
		size--;
	}
}

static void CreateTower(int size)
{
	const float cubeSize = 2.0f;
	const float spacing = 0.01f;
	NxVec3 pos(0.0f, cubeSize, 0.0f);
	while(size)
	{
		CreateCube(pos, (int)cubeSize);
		pos.y += (cubeSize * 2.0f + spacing);
		size--;
	}
}

static void KeyboardCallback(unsigned char key, int x, int y)
{
	switch(key)
	{
	case 27:
		exit(0);
		break;
	case '0':
	{
		bool perfOn = gPerfRenderer.toggleEnable();
		if (gAppData) gPhysicsSDK->getFoundationSDK().getRemoteDebugger()->writeParameter((perfOn?"Performance renderer is on":"Performance renderer is off"), &gAppData, false, "Info", NX_DBG_EVENTMASK_EVERYTHING);
		break;
	}
	case ' ':
		CreateCube(NxVec3(0.0f, 20.0f, 0.0f),1);
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
	case 'w':
	case 'W':
		CreateCubeFromEye(8);
		break;
	case 'q':
	case 'Q':
		{
			NxActor** actors = gScene->getActors();
			if(gScene->getNbActors() > 1){
				gScene->releaseActor(*actors[gScene->getNbActors()-1]);
			}
		}
		break;		
	case 'r':
	case 'R':
		// Can't connect if the the simulation is running, set a flag and wait until 
		// we know that the scene is not simulating.
		gConnectToVRD = true;
		break;
	case GLUT_KEY_UP:	case '8':	gEye += gDir*2.0f; break;
	case GLUT_KEY_DOWN: case '2':	gEye -= gDir*2.0f; break;
	case GLUT_KEY_LEFT:	case '4':	gEye -= gViewY*2.0f; break;
	case GLUT_KEY_RIGHT: case '6':	gEye += gViewY*2.0f; break;
	}
}

static void ArrowKeyCallback(int key, int x, int y)
{
	KeyboardCallback(key,x,y);
}

static void MouseCallback(int button, int state, int x, int y)
{
	gMouseX = x;
	gMouseY = y;
}

static void MotionCallback(int x, int y)
{
	int dx = gMouseX - x;
	int dy = gMouseY - y;
	
	gDir.normalize();
	gViewY.cross(gDir, NxVec3(0,1,0));

	NxQuat qx(NxPiF32 * dx * 20/ 180.0f, NxVec3(0,1,0));
	qx.rotate(gDir);
	NxQuat qy(NxPiF32 * dy * 20/ 180.0f, gViewY);
	qy.rotate(gDir);

	gMouseX = x;
	gMouseY = y;
}

static void RenderCallback()
{
	if(gScene == NULL) return;

	// The user pressed 'r' to connect to the VRD - we need to delay the 
	// connection attempt until we know that no scenes are simulating.
	if (gConnectToVRD)
	{
		//gPhysicsSDK->getFoundationSDK().getRemoteDebugger()->connect(SAMPLES_VRD_HOST, SAMPLES_VRD_PORT, SAMPLES_VRD_EVENTMASK);
		connectToVRD();
		gConnectToVRD = false;
	}

	// Start simulation (non blocking)
	gScene->simulate(1.0f/60.0f);

	// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	// Setup projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, (float)glutGet(GLUT_WINDOW_WIDTH)/(float)glutGet(GLUT_WINDOW_HEIGHT), 1.0f, 10000.0f);
	gluLookAt(gEye.x, gEye.y, gEye.z, gEye.x + gDir.x, gEye.y + gDir.y, gEye.z + gDir.z, 0.0f, 1.0f, 0.0f);

	gKeepTrackOfVRD.updateCamera();

	// Setup modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Render all actors
	int nbActors = gScene->getNbActors();
	NxActor** actors = gScene->getActors();
	while(nbActors--)
	{
		NxActor* actor = *actors++;
		if(!actor->userData) continue;

		// Render actor
		glPushMatrix();
		float glMat[16];
		actor->getGlobalPose().getColumnMajor44(glMat);
		glMultMatrixf(glMat);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glutSolidCube(float(size_t(actor->userData))*2.0f);
		glPopMatrix();

		// Render shadow
		glPushMatrix();
		const static float shadowMat[]={ 1,0,0,0, 0,0,0,0, 0,0,1,0, 0,0,0,1 };
		glMultMatrixf(shadowMat);
		glMultMatrixf(glMat);
		glDisable(GL_LIGHTING);
		glColor4f(0.1f, 0.2f, 0.3f, 1.0f);
		glutSolidCube(float(size_t(actor->userData))*2.0f);
		glEnable(GL_LIGHTING);
		glPopMatrix();
	}

	// Fetch simulation results
	gScene->flushStream();
	gScene->fetchResults(NX_RIGID_BODY_FINISHED, true);
	
	// Print profile results (if enabled)
	gPerfRenderer.render(gScene->readProfileData(true), glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

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
	// Initialize glut
	printf("Use the arrow keys or 2, 4, 6 and 8 to move the camera.\n");
	printf("Use the mouse to rotate the camera.\n");
	printf("Press 0 to toggle frame rate display.\n");
	printf("Press the keys w, t, s, b and space to create various things.\n");
	printf("Press q to destroy the last thing created.\n");
	printf("\nStart the VRD before this sample to automatically get a VRD connection, or press 'r' to connect to the VRD afterwards.\n");
	glutInit(&argc, argv);
	glutInitWindowSize(512, 512);
	glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH);
	int mainHandle = glutCreateWindow("SampleVRD");
	glutSetWindow(mainHandle);
	glutDisplayFunc(RenderCallback);
	glutReshapeFunc(ReshapeCallback);
	glutIdleFunc(IdleCallback);
	glutKeyboardFunc(KeyboardCallback);
	glutSpecialFunc(ArrowKeyCallback);
	glutMouseFunc(MouseCallback);
	glutMotionFunc(MotionCallback);
	MotionCallback(0,0);
	ReshapeCallback(glutGet(GLUT_WINDOW_WIDTH),glutGet(GLUT_WINDOW_HEIGHT));

	// Setup default render states
	glClearColor(0.3f, 0.4f, 0.5f, 1.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);

	// Setup lighting
	glEnable(GL_LIGHTING);
	float ambientColor[]	= { 0.0f, 0.1f, 0.2f, 0.0f };
	float diffuseColor[]	= { 1.0f, 1.0f, 1.0f, 0.0f };		
	float specularColor[]	= { 0.0f, 0.0f, 0.0f, 0.0f };		
	float position[]		= { 100.0f, 100.0f, 400.0f, 1.0f };		
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientColor);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseColor);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularColor);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glEnable(GL_LIGHT0);

	// Initialize physics scene and start the application main loop if scene was created
	if (InitNx())
	{
		atexit(ExitNx);
		glutMainLoop();
	}

	return 0;
}


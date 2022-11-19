/*
	This sample shows how to use the new filtering API.
	The arbitrary filtering rule in this sample is "shapes sharing at least one group should collide".

	The filtering equation is:

		(G0 op0 K0) op2 (G1 op1 K1)==b

	So, we set it up like this:

		K0 = K1 = 0
		op0 = op1 = NX_FILTEROP_OR
		op2 = NX_FILTEROP_AND
		b = true;

	That way we end up with the following filtering equation:

		collision enabled <=> (G0 & G1)==true

	In other words, if the group bitmasks have at least one common bit, we collide.
*/

#include <stdio.h>

#ifdef WIN32
#define NOMINMAX 
#include <windows.h>
#endif

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
// Physics code
#undef random
#include "NxPhysics.h"
#include "Utilities.h"
#include "SamplesVRDSettings.h"
#include "UserAllocator.h"

#ifdef __PPCGEKKO__
#include "GLFontRenderer.h"
#endif

static bool				gPause = false;
static NxPhysicsSDK*	gPhysicsSDK = NULL;
static NxScene*			gScene = NULL;
static NxVec3			gDefaultGravity(0.0f, -9.81f, 0.0f);
static UserAllocator*	gAllocator = NULL;

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

static void CreateCube(const NxVec3& pos, int size=2, const NxVec3* initial_velocity=NULL)
{
	// Create body
	NxBodyDesc BodyDesc;
	BodyDesc.angularDamping	= 0.5f;
//	BodyDesc.maxAngularVelocity	= 10.0f;
	if(initial_velocity)	BodyDesc.linearVelocity = *initial_velocity;

	NxU32 random = rand();
	random &= 1;

	NxBoxShapeDesc BoxDesc;
	BoxDesc.dimensions		= NxVec3(float(size), float(size), float(size));
	if(random)	BoxDesc.groupsMask.bits0 = 1;
	else		BoxDesc.groupsMask.bits1 = 1;

	NxActorDesc ActorDesc;
	ActorDesc.shapes.pushBack(&BoxDesc);
	ActorDesc.body			= &BodyDesc;
	ActorDesc.density		= 10.0f;
	ActorDesc.globalPose.t  = pos;
	NxActor* newActor = gScene->createActor(ActorDesc);

	NxU32 userData = size;
	userData |= random<<16;
	newActor->userData = (void*)userData;
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

static bool CreateScene()
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

	gScene->setFilterOps(NX_FILTEROP_OR, NX_FILTEROP_OR, NX_FILTEROP_AND);
	gScene->setFilterBool(true);
	NxGroupsMask zeroMask;
	zeroMask.bits0 = zeroMask.bits1 = zeroMask.bits2 = zeroMask.bits3 = 0;
	gScene->setFilterConstant0(zeroMask);
	gScene->setFilterConstant1(zeroMask);


	NxMaterialDesc	defaultMaterial;
	defaultMaterial.restitution		= 0.0f;
	defaultMaterial.staticFriction	= 0.5f;
	defaultMaterial.dynamicFriction	= 0.5f;
	NxMaterial *mat = gScene->getMaterialFromIndex(0);
	mat->loadFromDesc(defaultMaterial);


	// Create ground plane
	NxPlaneShapeDesc PlaneDesc;
	PlaneDesc.groupsMask.bits0 = 0xffffffff;
	PlaneDesc.groupsMask.bits1 = 0xffffffff;
	PlaneDesc.groupsMask.bits2 = 0xffffffff;
	PlaneDesc.groupsMask.bits3 = 0xffffffff;

	NxActorDesc ActorDesc;
	ActorDesc.shapes.pushBack(&PlaneDesc);
	gScene->createActor(ActorDesc);

	return true;
}

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
	if (gPhysicsSDK->getFoundationSDK().getRemoteDebugger() && !gPhysicsSDK->getFoundationSDK().getRemoteDebugger()->isConnected())
		gPhysicsSDK->getFoundationSDK().getRemoteDebugger()->connect(SAMPLES_VRD_HOST, SAMPLES_VRD_PORT, SAMPLES_VRD_EVENTMASK);
#endif

	gPhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.025f);

	return CreateScene();
}

static void ReleaseScene()
	{
	if(gPhysicsSDK)
		{
		if(gScene)	gPhysicsSDK->releaseScene(*gScene);
		}
	}

static void ResetScene()
	{
	if(gPhysicsSDK)
		{
		ReleaseScene();
		if (!CreateScene()) exit(0);
		}
	}

static void ExitCallback()
{
	ReleaseScene();
	if(gPhysicsSDK != NULL)
	{
		NxReleasePhysicsSDK(gPhysicsSDK);
		gPhysicsSDK = NULL;
	}

	if (gAllocator)
	{
		delete gAllocator;
		gAllocator = NULL;
	}
}


// Render code

static int gMainHandle;
static NxVec3 Eye(50.0f, 50.0f, 50.0f);
static NxVec3 Dir(-0.6,-0.2,-0.7);
static NxVec3 N;
static int mx = 0;
static int my = 0;

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
	float glmat[16];
	// Physics code
	if(!gPause)	
		{

		if(gScene)
			{
			gScene->simulate(1.0f/60.0f);	//Note: a real application would compute and pass the elapsed time here.
			gScene->flushStream();
			gScene->fetchResults(NX_RIGID_BODY_FINISHED, true);
			}


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

			size_t userData = size_t(actor->userData);
			if(!userData)	continue;

			NxF32 size = NxF32(userData&0xffff);

			NxU32 random = userData>>16;
			if(random & 1)	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			else			glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

			glPushMatrix();
			actor->getGlobalPose().getColumnMajor44(glmat);		
			glMultMatrixf(glmat);
			glutSolidCube(size*2.0f);
			glPopMatrix();
		}
	}
#ifdef __PPCGEKKO__	
	char buf[256];
    sprintf(buf,
	"Press a to reset the scene.\n"
	"Use the arrow keys to move the camera.\n");

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
	printf("Press the keys 1,2,+,-, and b to create various things.\n");
	printf("Press a to reset the scene.\n");
	printf("Use the arrow keys to move the camera.\n");
#else
	printf("Press the keys w,space,s,b, and t to create various things.\n");
	printf("Press r to reset the scene.\n");
	printf("Use the arrow keys or 2, 4, 6 and 8 to move the camera.\n");
	printf("Use the mouse to rotate the camera.\n");
	printf("Press p to pause the simulation.\n");
#endif
	
	// Initialize Glut
	glutInit(&argc, argv);
	glutInitWindowSize(512, 512);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	gMainHandle = glutCreateWindow("SampleFiltering");
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
	glutRemapButtonExt('a','r');
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
//#ifdef __PPCGEKKO__
//	float AmbientColor[]	= { 0.2f, 0.3f, 0.4f, 0.0f };		glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientColor);
//#else	
	float AmbientColor[]	= { 0.0f, 0.1f, 0.2f, 0.0f };		glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientColor);
//#endif	
	float DiffuseColor[]	= { 1.0f, 1.0f, 1.0f, 0.0f };		glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseColor);
	float SpecularColor[]	= { 0.0f, 0.0f, 0.0f, 0.0f };		glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularColor);
	float Position[]		= { -10.0f, 100.0f, -4.0f, 1.0f };		glLightfv(GL_LIGHT0, GL_POSITION, Position);
	glEnable(GL_LIGHT0);

	// Initialize physics scene and start the application main loop if scene was created
	if (InitNx())
	{
		atexit(ExitCallback);
		glutMainLoop();
	}

	return 0;
}

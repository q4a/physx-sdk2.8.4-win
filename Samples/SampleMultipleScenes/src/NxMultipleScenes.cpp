// ===============================================================================
//
//						   NVIDIA PhysX SDK Sample Program
//
// Title: Multiple Scenes
// Description: This sample program shows that you can simulate several scenes
//              simultaneously using the NVIDIA PhysX SDK. The two scenes in this
//              example are rendered at the same spot, but that is mostly to show
//              that they do not interact with each other. In a game scenario you
//              can e.g. use two different scenes to simulate the physics in two
//              separate locations.
//
// Originally written by: Pierre Terdiman (01.01.04)
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
#include "Utilities.h"
#include "SamplesVRDSettings.h"
#include "CustomRandom.h"
#include "UserAllocator.h"

#ifdef __PPCGEKKO__
#include "GLFontRenderer.h"
#endif

static bool				gPause = false;
static NxPhysicsSDK*	gPhysicsSDK = NULL;
static NxScene*			gScenes[2] = { NULL, NULL };
static NxVec3			gDefaultGravity(0.0f, -9.81f, 0.0f);
static UserAllocator*	gAllocator = NULL;

static void CreateCube(NxU32 sceneIndex, const NxVec3& pos, int size=2, const NxVec3* initial_velocity=NULL)
{
	// Create body
	NxBodyDesc BodyDesc;
	BodyDesc.angularDamping	= 0.5f;
	if(initial_velocity)	BodyDesc.linearVelocity = *initial_velocity;

	NxBoxShapeDesc BoxDesc;
	BoxDesc.dimensions		= NxVec3(float(size), float(size), float(size));

	NxActorDesc ActorDesc;
	ActorDesc.shapes.pushBack(&BoxDesc);
	ActorDesc.body			= &BodyDesc;
	ActorDesc.density		= 10.0f;
	ActorDesc.globalPose.t  = pos;
	gScenes[sceneIndex]->createActor(ActorDesc)->userData = (void*)size;
}

static void CreateStack(int sceneIndex, int size)
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
			CreateCube(sceneIndex, Pos, (int)CubeSize);
		}
		Offset += CubeSize;
		Pos.y += (CubeSize * 2.0f + Spacing);
		size--;
	}
}

static void CreateTower(int sceneIndex, int size)
{
	float CubeSize = 2.0f;
	float Spacing = 0.01f;
	NxVec3 Pos(0.0f, CubeSize, 0.0f);
	while(size)
	{
		CreateCube(sceneIndex, Pos, (int)CubeSize);
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
	gPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, gAllocator, NULL, desc, &errorCode);
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

	// Create two scenes
	for(NxU32 i=0;i<2;i++)
	{
		NxSceneDesc sceneDesc;
		sceneDesc.gravity				= gDefaultGravity;

		gScenes[i] = gPhysicsSDK->createScene(sceneDesc);
		if(gScenes[i] == NULL) 
		{
			printf("\nError: Unable to create one of the two PhysX scenes, exiting the sample.\n\n");
			return false;
		}

		// Create ground plane
		NxPlaneShapeDesc PlaneDesc;
		NxActorDesc ActorDesc;
		ActorDesc.shapes.pushBack(&PlaneDesc);
		gScenes[i]->createActor(ActorDesc);

		//materials are no longer shared between scenes so they have to be set up for both:
		NxMaterial * defaultMaterial = gScenes[i]->getMaterialFromIndex(0); 
		defaultMaterial->setRestitution(0.0f);
		defaultMaterial->setStaticFriction(0.5f);
		defaultMaterial->setDynamicFriction(0.5f);
	}
	return true;
}



// Render code

static int gMainHandle;
static bool gShadows = true;
static NxVec3 Eye(50.0f, 50.0f, 50.0f);
static NxVec3 Dir(-0.6,-0.2,-0.7);
static NxVec3 N;
static int mx = 0;
static int my = 0;

static void KeyboardCallback(unsigned char key, int x, int y)
{
	static Random random;
	int sceneIndex = random.rand()&1;

	switch (key)
	{
	case 27:	exit(0); break;
	case ' ':			CreateCube(sceneIndex, NxVec3(0.0f, 20.0f, 0.0f), 1+(rand()&3)); break;
	case 's':			CreateStack(sceneIndex, 10); break;
	case 'b':			CreateStack(sceneIndex, 30); break;
	case 't':			CreateTower(sceneIndex, 30); break;
	case 'x':			gShadows = !gShadows; break;
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
		Vel*=200.0f;
		CreateCube(sceneIndex, t, 8, &Vel);
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
/*	
	static DWORD PreviousTime = 0;
	DWORD CurrentTime = getTime();
	DWORD ElapsedTime = CurrentTime - PreviousTime;
	PreviousTime = CurrentTime;
*/
	
	// Physics code
	if(!gPause)
		{
		NxU32 i;
		//this is an example of allowing two scenes to run in parallell if the implementation supports it:
		for(i=0;i<2;i++)
			{
			if(gScenes[i])	
				{
				gScenes[i]->simulate(1.0f/60.0f);	//Note: a real application would compute and pass the elapsed time here.
				gScenes[i]->flushStream();
				}
			}
		for(i=0;i<2;i++)
			{
			if(gScenes[i])	
				{
				gScenes[i]->fetchResults(NX_RIGID_BODY_FINISHED, true);
				}
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

	// Keep physics & graphics in sync

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	for(NxU32 i=0;i<2;i++)
	{
		int nbActors = gScenes[i]->getNbActors();
		NxActor** actors = gScenes[i]->getActors();
		while(nbActors--)
		{
			NxActor* actor = *actors++;

			glPushMatrix();
			float glmat[16];
			actor->getGlobalPose().getColumnMajor44(glmat);
			glMultMatrixf(glmat);
			if(i==0)	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			else		glColor4f(0.5f, 0.5f, 0.5f, 1.0f);
			glutSolidCube(float(size_t(actor->userData))*2.0f);
			glPopMatrix();
			

			// Handle shadows
			
			if(gShadows)
			{
				glPushMatrix();

				const static float ShadowMat[]={ 1,0,0,0, 0,0,0,0, 0,0,1,0, 0,0,0,1 };

				glMultMatrixf(ShadowMat);
				glMultMatrixf(glmat);
				glDisable(GL_LIGHTING);
				glColor4f(0.3f, 0.2f, 0.3f, 1.0f);
				glutSolidCube(float(size_t(actor->userData))*2.0f);
				glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
				glEnable(GL_LIGHTING);

				glPopMatrix();
			}
			
		}
	}
	
#ifdef __PPCGEKKO__	
	char buf[256];
    sprintf(buf,
    "\nPress the keys 1,2,+,-, and b to create various things.\nThe object will appear in randomly in one of two scenes.\n"
	"\nUse the arrow keys to move around."
	"\nPress a to pause the simulation.\n" );

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
		if (gScenes[0]) gPhysicsSDK->releaseScene(*gScenes[0]);
		if (gScenes[1]) gPhysicsSDK->releaseScene(*gScenes[1]);
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
	printf("Press the keys 1,2,+,-, and b to create various things.\nThe object will appear in randomly in one of two scenes.\n");
	printf("Use the arrow keys to move around.\n");
	printf("Press a to pause the simulation.\n");
#else
	printf("Press the keys w,space,s,b, and t to create various things.\nThe object will appear in randomly in one of two scenes.\n");
	printf("Use the arrow keys or 2, 4, 6 and 8 or d, f, e and g to move around.\nUse the mouse to look around.\n");
	printf("Press p to pause the simulation.\n");
#endif	
	// Initialize Glut
	glutInit(&argc, argv);
	glutInitWindowSize(512, 512);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	gMainHandle = glutCreateWindow("SampleMultipleScenes");
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
	glClearColor(0.5f, 0.4f, 0.5f, 1.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_CULL_FACE);

	// Setup lighting
	glEnable(GL_LIGHTING);
	float AmbientColor[]	= { 0.1f, 0.1f, 0.1f, 0.0f };		glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientColor);
	float DiffuseColor[]	= { 1.0f, 1.0f, 1.0f, 0.0f };		glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseColor);
	float SpecularColor[]	= { 0.0f, 0.0f, 0.0f, 0.0f };		glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularColor);
	float Position[]		= { 0.0f, 10.0f, 40.0f, 1.0f };		glLightfv(GL_LIGHT0, GL_POSITION, Position);
	glEnable(GL_LIGHT0);

	// Initialize physics scene and start the application main loop if scene was created
	if (InitNx())
	{
		atexit(ExitCallback);
		glutMainLoop();
	}

	return 0;
}

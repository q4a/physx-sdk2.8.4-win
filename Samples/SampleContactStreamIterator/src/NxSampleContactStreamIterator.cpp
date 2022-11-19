// ===============================================================================
//
//						   NVIDIA PhysX SDK Sample Program
//
// Title: Contact Stream Iterator
// Description: This sample program shows how to use contact report and the 
//              contact stream iterator.
//              Toggle between per pair or per point contact forces with 'c'. Per 
//              point forces are only provided for box-plane contacts in this example.
//
// Originally written by: Pierre Terdiman (04-02-04)
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
#include "ErrorStream.h"
#include "Utilities.h"
#include "SamplesVRDSettings.h"
#include "UserAllocator.h"
#include "Timing.h"

#ifdef __PPCGEKKO__
#include "GLFontRenderer.h"
#endif

static bool				gPause = false;
static bool				showPointContacts = false;
static NxPhysicsSDK*	gPhysicsSDK = NULL;
static NxScene*			gScene = NULL;
static NxVec3			gDefaultGravity(0.0f, -9.81f, 0.0f);
static ErrorStream		gErrorStream;
static UserAllocator*	gAllocator = NULL;

static NxActor*			gGroundActor = NULL;
static NxU32			gNbReports = 0;

struct MyCubeObject
{
	int		size;
	NxU32	events;
};

static NxActor* lastActor = NULL;

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
	ActorDesc.density		= 0.1f;
	ActorDesc.globalPose.t  = pos;

	// Binds a user-defined structure to the Nx object
	MyCubeObject* Object = new MyCubeObject;		// Note those ones are never released in this sample
	Object->size	= size;
	Object->events	= 0;

	NxActor * a = gScene->createActor(ActorDesc);
	a->userData = Object;

	lastActor = a;


#ifdef TEST_SPECIFIC_SHAPES
	if(gGroundActor && a)
		{
		gScene->setActorPairFlags(*a, *gGroundActor, NX_NOTIFY_ON_START_TOUCH|NX_NOTIFY_ON_TOUCH|NX_NOTIFY_ON_END_TOUCH);
		}
#endif
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
	virtual void  onContactNotify(NxContactPair& pair, NxU32 events)
	{
		gNbReports++;

		if(!pair.isDeletedActor[0])
			{
			MyCubeObject* Object = (MyCubeObject*)pair.actors[0]->userData;
			if(Object)	Object->events = events;
			}

		if(!pair.isDeletedActor[1])
			{
			MyCubeObject* Object = (MyCubeObject*)pair.actors[1]->userData;
			if(Object)	Object->events = events;
			}

//		if(events & NX_NOTIFY_ON_START_TOUCH)	printf("Start touch\n");
//		if(events & NX_NOTIFY_ON_END_TOUCH)		printf("End touch\n");

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

//					printf("%f  - %f\n", i.getPointNormalForce(), pair.sumNormalForce.magnitude());
					NxVec3 contactForce = (showPointContacts /*&& i.getShapeFlags()&NX_SF_POINT_CONTACT_FORCE*/) ?  contactNormal * i.getPointNormalForce() : pair.sumNormalForce; 
					NxVec3 contactArrowForceTip = contactPoint + contactForce * 0.1f;
					NxVec3 contactArrowFrictionTip = contactPoint + pair.sumFrictionForce* 0.1f;
					NxVec3 contactArrowPenetrationTip = contactPoint - contactNormal * i.getSeparation() * 20.0f;

					glDisable(GL_LIGHTING);

#ifdef __PPCGEKKO__
					// Draw the contact
					glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
					glBegin(GL_LINES,2);
					glVertex3f(contactPoint.x,contactPoint.y,contactPoint.z);
					glVertex3f(contactArrowForceTip.x,contactArrowForceTip.y,contactArrowForceTip.z);
					glEnd();
					
					// Draw friction
					glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
					glBegin(GL_LINES,2);
					glVertex3f(contactPoint.x,contactPoint.y,contactPoint.z);
					glVertex3f(contactArrowPenetrationTip.x,contactArrowPenetrationTip.y,contactArrowPenetrationTip.z);
					glEnd();
					
					// Draw the penetration part
					glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
					glBegin(GL_LINES,2);
					glVertex3f(contactPoint.x,contactPoint.y,contactPoint.z);
					glVertex3f(contactArrowForceTip.x,contactArrowForceTip.y,contactArrowForceTip.z);
					glEnd();				

#else
					// Draw the contact
					glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
					float line1[2][3] = { {contactPoint.x,contactPoint.y,contactPoint.z}, {contactArrowForceTip.x,contactArrowForceTip.y,contactArrowForceTip.z}};
    				glEnableClientState(GL_VERTEX_ARRAY);
					glVertexPointer(3, GL_FLOAT, 0, line1);
					glDrawArrays(GL_LINES, 0, 2);

					// Draw friction
					glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
					float line2[2][3] = { {contactPoint.x,contactPoint.y,contactPoint.z}, {contactArrowFrictionTip.x,contactArrowFrictionTip.y,contactArrowFrictionTip.z}};
					glVertexPointer(3, GL_FLOAT, 0, line2);
					glDrawArrays(GL_LINES, 0, 2);

					// Draw the penetration part
					glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
					float line3[2][3] = { {contactPoint.x,contactPoint.y,contactPoint.z}, {contactArrowPenetrationTip.x,contactArrowPenetrationTip.y,contactArrowPenetrationTip.z}};
					glVertexPointer(3, GL_FLOAT, 0, line3);
					glDrawArrays(GL_LINES, 0, 2);
					glDisableClientState(GL_VERTEX_ARRAY);
#endif					
					}
				}
			}
	}

}gMyContactReport;

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
	sceneDesc.userContactReport		= &gMyContactReport;
	gScene = gPhysicsSDK->createScene(sceneDesc);
	if(gScene == NULL) 
	{
		printf("\nError: Unable to create a PhysX scene, exiting the sample.\n\n");
		return false;
	}

#ifndef TEST_SPECIFIC_SHAPES
	gScene->setActorGroupPairFlags(0,0,NX_NOTIFY_ON_START_TOUCH|NX_NOTIFY_ON_TOUCH|NX_NOTIFY_ON_END_TOUCH);
#endif

	NxMaterial * defaultMaterial = gScene->getMaterialFromIndex(0); 
	defaultMaterial->setRestitution(0.0f);
	defaultMaterial->setStaticFriction(0.5f);
	defaultMaterial->setDynamicFriction(0.5f);

	// Create ground plane
	NxPlaneShapeDesc PlaneDesc;
	PlaneDesc.shapeFlags |= NX_SF_POINT_CONTACT_FORCE;
	NxActorDesc ActorDesc;
	ActorDesc.shapes.pushBack(&PlaneDesc);
	gGroundActor = gScene->createActor(ActorDesc);

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
	switch (key)
	{
	case 27:
		exit(0);
		break;
	case ' ':
		CreateCube(NxVec3(0.0f, 20.0f, 0.0f), 1+(rand()&3));
		break;
	case 'o':
	case 'O':
		{
		if(lastActor) gScene->releaseActor(*lastActor);
		lastActor = (gScene->getNbActors() > 1) ? gScene->getActors()[gScene->getNbActors()-1] : NULL;
		}
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
	case 'x':
	case 'X':
		gShadows = !gShadows;
		break;
	case 'p':
	case 'P':
		gPause = !gPause;
		break;
	case 101: case '8':	Eye += Dir * 2.0f; break;
	case 103: case '2':	Eye -= Dir * 2.0f; break;
	case 100: case '4':	Eye -= N * 2.0f; break;
	case 102: case '6':	Eye += N * 2.0f; break;
	case 'c': 
	case 'C': 
		showPointContacts = !showPointContacts;
		break;
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
	//some timing code to prevent it from going too fast on new PCs.
	//Here we don't just skip the physics update because we also do some 
	//rendering in physics which would get cleared off the screen too fast.
#ifdef WIN32
	static DWORD PreviousTime = 0;
	DWORD CurrentTime = getTime();
	DWORD ElapsedTime = CurrentTime - PreviousTime;
	if (ElapsedTime < 10.0f)
		return;

	PreviousTime = CurrentTime;
#endif

	if(gScene && !gPause)
		{
		gNbReports = 0;
		gScene->simulate(1.0f/60.0f);		//Note: a real application would compute and pass the elapsed time here.
		gScene->flushStream();
		}


	float glmat[16];
	// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Setup camera
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, (float)glutGet(GLUT_WINDOW_WIDTH)/(float)glutGet(GLUT_WINDOW_HEIGHT), 1.0f, 10000.0f);
	gluLookAt(Eye.x, Eye.y, Eye.z, Eye.x + Dir.x, Eye.y + Dir.y, Eye.z + Dir.z, 0.0f, 1.0f, 0.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Physics code
	// We moved the physics code here because we want to render the contacts in contact report callbacks, and the callbacks are
	// fired when we execute the following call. Note that this is bad practice however, the simulation shouldn't be performed
	// in the middle of the rendering loop.

	if(gScene && !gPause)
		{
		gScene->fetchResults(NX_RIGID_BODY_FINISHED, true);
//		printf("%d\n", gNbReports);
		}

	// ~Physics code

	glEnable(GL_LIGHTING);

	// Keep physics & graphics in sync
	int nbActors = gScene->getNbActors();
	NxActor** actors = gScene->getActors();
	while(nbActors--)
	{
		NxActor* actor = *actors++;

		// Get our object back
		MyCubeObject* Object = (MyCubeObject*)actor->userData;
		if(!Object)	continue;

		// Setup object's color according to events
		if(Object->events & NX_NOTIFY_ON_START_TOUCH)
			{
			glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
			}
		else if(Object->events & NX_NOTIFY_ON_END_TOUCH)
			{
			glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
			}
		else
			{
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			}

		// Render cube
		glPushMatrix();
		actor->getGlobalPose().getColumnMajor44(glmat);
		glMultMatrixf(glmat);
		glutSolidCube(float(Object->size)*2.0f);
		glPopMatrix();

		// Handle shadows
		if(gShadows)
		{
			glPushMatrix();

			const static float ShadowMat[]={ 1,0,0,0, 0,0,0,0, 0,0,1,0, 0,0,0,1 };

			glMultMatrixf(ShadowMat);
			glMultMatrixf(glmat);
			glDisable(GL_LIGHTING);
			glColor4f(0.1f, 0.2f, 0.3f, 1.0f);
			glutSolidCube(float(Object->size)*2.0f);
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			glEnable(GL_LIGHTING);

			glPopMatrix();
		}

		// Reset events. If we don't do that the events bitmask is not updated as soon as objects don't collide
		// anymore, so the bitmask stay red (comment that line below, to see this behaviour).
		Object->events = 0;
	}

#ifdef __PPCGEKKO__
	char buf[256];
    sprintf(buf,
		"Use the arrow keys to move the camera.\n"
		"Press the keys b, 1, 2 create various things.\n");

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
	printf("Use the arrow keys to move the camera.\n");
	printf("Press the keys b, 1, 2 create various things.\n");
#else
	printf("Use the arrow keys or 2, 4, 6 and 8 to move the camera.\n");
	printf("Use the mouse to rotate the camera.\n");
	printf("Press p to pause the simulation.\n");
	printf("Press x to toggle shadows.\n");
	printf("Press c to toggle contact normal force display for each contact or only ground plane contacts.\n");
	printf("Press the keys w,space,s,b, and t to create various things.\n");
	printf("Press o to remove the last created thing.\n");
#endif	
#if defined(_XBOX) || defined(__CELLOS_LV2__)
	glutRemapButtonExt(9, 'c', false); //Right shoulder to display normals
#endif
#ifdef __PPCGEKKO__
	glutRemapButtonExt('1','s');
	glutRemapButtonExt('b','w');
	glutRemapButtonExt('a','0');
	glutRemapButtonExt('2','t');
	glutRemapButtonExt(GLUT_KEY_HOME,'0');
#endif
	// Initialize Glut
	glutInit(&argc, argv);
	glutInitWindowSize(512, 512);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	gMainHandle = glutCreateWindow("SampleContactStreamIterator");
	glutSetWindow(gMainHandle);
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
	glEnable(GL_CULL_FACE);

	// Setup lighting
	glEnable(GL_LIGHTING);
	float AmbientColor[]	= { 0.0f, 0.1f, 0.2f, 0.0f };		glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientColor);
	float DiffuseColor[]	= { 1.0f, 1.0f, 1.0f, 0.0f };		glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseColor);
	float SpecularColor[]	= { 0.0f, 0.0f, 0.0f, 0.0f };		glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularColor);
	float Position[]		= { 100.0f, 100.0f, 400.0f, 1.0f };		glLightfv(GL_LIGHT0, GL_POSITION, Position);
	glEnable(GL_LIGHT0);

	// Initialize physics scene and start the application main loop if scene was created
	if (InitNx())
	{
		atexit(ExitCallback);
		glutMainLoop();
	}

	return 0;
}

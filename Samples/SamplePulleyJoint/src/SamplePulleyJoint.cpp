// ===============================================================================
//
//						   NVIDIA PhysX SDK Sample Program
//
// Title: Motorized Pulley Joint
// Description: This sample program shows how to use the pulley joint.
//
// Originally written by: Bob Schade & Matthias Müller-Fischer (02-21-05)
//
// ===============================================================================

#include "SamplePulleyJoint.h"
#include "GLFontRenderer.h"
#include "Utilities.h"
#include "SamplesVRDSettings.h"
#include "UserAllocator.h"
#include <stdarg.h>

#ifdef __PPCGEKKO__
#include "PerfRenderer.h"
#endif

// Physics SDK globals
NxPhysicsSDK*     gPhysicsSDK = NULL;
NxScene*          gScene = NULL;
NxVec3            gDefaultGravity(0,-9.8,0);
UserAllocator*	gAllocator = NULL;
DebugRenderer gDebugRenderer;

// Time globals
NxReal gTime;
NxReal gLastTime;
 
// Display globals
int mx = 0;
int my = 0;
char gDisplayString[512] = "";

// Camera globals
NxVec3 gCameraPos(0,5,-8);
NxVec3 gCameraForward(0,0,1);
NxVec3 gCameraRight(-1,0,0);
const NxReal gCameraSpeed = 0.02;

// Force globals
NxVec3 gForceVec(0,0,0);
NxReal gForceStrength = 1000;
bool bForceMode = true;

// Limit globals
NxReal gLinearLimit = 1.0f;
NxReal gSwing1Limit = NxPiF32 / 180.0f * 30.0f;
NxReal gSwing2Limit = NxPiF32 / 180.0f * 70.0f;
NxReal gTwistLowLimit = NxPiF32 / 180.0f * -90.0f;
NxReal gTwistHighLimit = NxPiF32 / 180.0f * 45.0f;

// Keyboard globals
#define MAX_KEYS 256
bool gKeys[MAX_KEYS];

// Simulation globals
bool bPause = false;
bool bShadows = true;
bool bDebugWireframeMode = true;
bool bWireframeMode = false;

// Motor
NxReal gMotorVelocity = 1.0f;
NxMotorDesc gMotorDesc;

// Actor globals
NxActor* groundPlane = NULL;
NxActor* capsule1 = NULL;
NxActor* capsule2 = NULL;

// Joint globals
NxPulleyJoint* pulleyJoint = NULL;

// Focus actor
NxActor* gSelectedActor = NULL;


void PrintControls()
{
#ifdef __PPCGEKKO__
	printf("\n Force Controls:\n ---------------\n UP = +z, DOWN = -z\n LEFT = +x, RIGHT = -x\n a = +y, b = -y\n");
	printf("\n Motor Control:\n --------------\n 1 = stop, + = forward, - = backward\n");
	printf("\n Miscellaneous:\n --------------\n HOME = Select Next Actor\n 2 = Toggle Force Mode\n");
#else
	printf("\n Flight Controls:\n ----------------\n w = forward, s = back\n a = strafe left, d = strafe right\n q = up, z = down\n");
    printf("\n Force Controls:\n ---------------\n i = +z, k = -z\n j = +x, l = -x\n u = +y, m = -y\n");
	printf("\n Motor Control:\n --------------\n 0 = stop, 1 = forward, 2 = backward\n");
	printf("\n Miscellaneous:\n --------------\n p = Pause\n r = Select Next Actor\n f = Toggle Force Mode\n b = Toggle Debug Wireframe Mode\n x = Toggle Shadows\n");
#endif	
}


float PrintfTex(float y, const char* fmt, ... )
{
	va_list  vlist;
    char     buf[1024];

    // Get output string
    va_start( vlist, fmt );
    vsprintf( buf, fmt, vlist );
    va_end( vlist );
	
	int len = strlen(buf);
	len = (len < 1024)?len:1024;
	int start = 0;
	char textBuffer[1024];
	for(int i=0;i<len;i++)
	{
		if(buf[i] == '\n' || i == len-1)
		{
			int offset = i;
			if(i == len-1) offset= i+1;
			memcpy(textBuffer, buf+start, offset-start);
			textBuffer[offset-start]=0;
			GLFontRenderer::print(0.01, y, 0.03f, textBuffer);
			y -= 0.035f;
			start = offset+1;
		}
	}
	return y;
}
void DisplayText()
{
	float y = 0.95f;
#ifdef __PPCGEKKO__	
    y = PrintfTex(0.95f,
    	"\n Force Controls:\n ---------------\n UP = +z, DOWN = -z\n LEFT = +x, RIGHT = -x\n a = +y, b = -y\n"
		"\n Motor Control:\n --------------\n 1 = stop, + = forward, - = backward\n"
		"\n Miscellaneous:\n --------------\n HOME = Select Next Actor\n 2 = Toggle Force Mode\n"
    	); 
#endif    	 	
    PrintfTex(y,gDisplayString);	
}


void RefreshDisplayString()
{
#ifndef __PPCGEKKO__
	sprintf(gDisplayString, "PULLEY JOINT subject to %s\nMotor control: 0 stop 1 forward 2 backward", 
		bForceMode ? "force" : "torque");
#else
	sprintf(gDisplayString, "PULLEY JOINT subject to %s", 
		bForceMode ? "force" : "torque");
#endif		
}

void setMotor(NxReal vel) 
{
	gMotorDesc.velTarget = vel;
	pulleyJoint->setMotor(gMotorDesc);
}

NxVec3 ApplyForceToActor(NxActor* actor, const NxVec3& forceDir, const NxReal forceStrength, bool forceMode)
{
	NxVec3 forceVec = forceStrength*forceDir;

	if (forceMode)
		actor->addForce(forceVec);
	else 
		actor->addTorque(forceVec);

	return forceVec;
}

void SelectNextActor()
{
	NxU32 nbActors = gScene->getNbActors();
	NxActor** actors = gScene->getActors();
	for(NxU32 i = 0; i < nbActors; i++)
	{
		if (actors[i] == gSelectedActor)
		{
			gSelectedActor = actors[(i+1)%nbActors];
			if (gSelectedActor == groundPlane) // skip the ground plane
			{
				gSelectedActor = actors[(i+2)%nbActors];
			}
			break;
		}
	}
}

void ProcessKeys()
{
	// Process keys
	for (int i = 0; i < MAX_KEYS; i++)
	{	
		if (!gKeys[i])  { continue; }

		switch (i)
		{
			// Camera controls
			case 'w':{ gCameraPos += gCameraForward*gCameraSpeed; break; }
			case 's':{ gCameraPos -= gCameraForward*gCameraSpeed; break; }
			case 'a':{ gCameraPos -= gCameraRight*gCameraSpeed; break; }
			case 'd':{ gCameraPos += gCameraRight*gCameraSpeed; break; }
			case 'z':{ gCameraPos -= NxVec3(0,1,0)*gCameraSpeed; break; }
			case 'q':{ gCameraPos += NxVec3(0,1,0)*gCameraSpeed; break; }

			// Force controls
			case 'i': {gForceVec = ApplyForceToActor(gSelectedActor,NxVec3(0,0,1),gForceStrength,bForceMode); break; }
			case 'k': {gForceVec = ApplyForceToActor(gSelectedActor,NxVec3(0,0,-1),gForceStrength,bForceMode); break; }
			case 'j': {gForceVec = ApplyForceToActor(gSelectedActor,NxVec3(+1,0,0),gForceStrength,bForceMode); break; }
			case 'l': {gForceVec = ApplyForceToActor(gSelectedActor,NxVec3(-1,0,0),gForceStrength,bForceMode); break; }
			case 'u': {gForceVec = ApplyForceToActor(gSelectedActor,NxVec3(0,+1,0),gForceStrength,bForceMode); break; }
			case 'm': {gForceVec = ApplyForceToActor(gSelectedActor,NxVec3(0,-1,0),gForceStrength,bForceMode); break; }
		}
	}
}

void SetupCamera()
{
	// Setup camera
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0f, (float)glutGet(GLUT_WINDOW_WIDTH)/(float)glutGet(GLUT_WINDOW_HEIGHT), 1.0f, 10000.0f);
	gluLookAt(gCameraPos.x,gCameraPos.y,gCameraPos.z,gCameraPos.x + gCameraForward.x, gCameraPos.y + gCameraForward.y, gCameraPos.z + gCameraForward.z, 0.0f, 1.0f, 0.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void RenderActors(bool shadows)
{
    // Render all the actors in the scene
    int nbActors = gScene->getNbActors();
    NxActor** actors = gScene->getActors();
    while (nbActors--)
    {
        NxActor* actor = *actors++;
        DrawActor(actor, gSelectedActor);

        // Handle shadows
        if (shadows)
        {
			DrawActorShadow(actor);
        }
    }
}

void DrawForce(NxActor* actor, NxVec3& forceVec, const NxVec3& color)
{
	// draw only if the force is large enough
	NxReal force = forceVec.magnitude();
	if (force < 0.1f)  return;

	forceVec = 3*forceVec/force;

	NxVec3 pos = actor->getCMassGlobalPosition();
	DrawArrow(pos, pos + forceVec, color);
}

void RenderCallback()
{
    if (gScene && !bPause)    
        RunPhysics();

    // Clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ProcessKeys();
	SetupCamera();
 	RenderActors(bShadows);

	if (bForceMode)
		DrawForce(gSelectedActor, gForceVec, NxVec3(1,1,0));
	else
		DrawForce(gSelectedActor, gForceVec, NxVec3(0,1,1));
	gForceVec = NxVec3(0,0,0);

	// Show debug wireframes
	if(bDebugWireframeMode) 
	{
		const NxDebugRenderable * p = gScene->getDebugRenderable();
		if(p != NULL)
		{
			glDisable(GL_LIGHTING);
			gDebugRenderer.renderData(*p);
			glEnable(GL_LIGHTING);
		}
	}

	DisplayText();
	
    glutSwapBuffers();
}

void ReshapeCallback(int width, int height)
{
    glViewport(0, 0, width, height);
}

void IdleCallback()
{
    glutPostRedisplay();
}

void KeyboardCallback(unsigned char key, int x, int y)
{
	gKeys[key] = true;

	switch (key)
	{
		case 'r': { SelectNextActor(); break; }		
		case 27 : { exit(0); break; }
		default : { break; }
		case 'p': { bPause = !bPause; UpdateTime(); break; }
		case 'x': { bShadows = !bShadows; break; }
		case 'b': { bDebugWireframeMode = !bDebugWireframeMode; break; }		
		case 'f': { bForceMode = !bForceMode; RefreshDisplayString(); break; }
		case '0': { setMotor(0.0f); break; }
		case '1': { setMotor(gMotorVelocity); break; }
		case '2': { setMotor(-gMotorVelocity); break; }
	}
}

void KeyboardUpCallback(unsigned char key, int x, int y)
{
	gKeys[key] = false;
}

void MouseCallback(int button, int state, int x, int y)
{
    mx = x;
    my = y;
}

void MotionCallback(int x, int y)
{
    int dx = mx - x;
    int dy = my - y;
    
    gCameraForward.normalize();
    gCameraRight.cross(gCameraForward,NxVec3(0,1,0));

    NxQuat qx(NxPiF32 * dx * 20 / 180.0f, NxVec3(0,1,0));
    qx.rotate(gCameraForward);
    NxQuat qy(NxPiF32 * dy * 20 / 180.0f, gCameraRight);
    qy.rotate(gCameraForward);

    mx = x;
    my = y;
}

static void ExitCallback()
{
	ReleaseNx();
}

void InitGlut(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitWindowSize(512, 512);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	int mainHandle = glutCreateWindow("SamplePullyJoint");
	glutSetWindow(mainHandle);
    glutDisplayFunc(RenderCallback);
    glutReshapeFunc(ReshapeCallback);
    glutIdleFunc(IdleCallback);
    glutKeyboardFunc(KeyboardCallback);
    glutKeyboardUpFunc(KeyboardUpCallback);
    glutMouseFunc(MouseCallback);
    glutMotionFunc(MotionCallback);
    MotionCallback(0,0);

	// Setup default render states
	glClearColor(0.52f, 0.60f, 0.71f, 1.0f);  
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_CULL_FACE);

    // Setup lighting
    glEnable(GL_LIGHTING);
    float AmbientColor[] = { 0.0f, 0.1f, 0.2f, 0.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientColor);
    float DiffuseColor[] = { 0.2f, 0.2f, 0.2f, 0.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseColor);
    float SpecularColor[] = { 0.5f, 0.5f, 0.5f, 0.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularColor);
    float Position[] = { 100.0f, 100.0f, -400.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, Position);
    glEnable(GL_LIGHT0);
}


NxPulleyJoint* CreatePulleyJoint(NxActor* a0, NxActor* a1, const NxVec3& pulley0, const NxVec3& pulley1, const NxVec3& globalAxis, NxReal distance, NxReal ratio)
{
	NxPulleyJointDesc pulleyDesc;
	pulleyDesc.actor[0] = a0;
	pulleyDesc.actor[1] = a1;
	pulleyDesc.localAnchor[0] = NxVec3(0,2,0);
	pulleyDesc.localAnchor[1] = NxVec3(0,2,0);
	pulleyDesc.setGlobalAxis(globalAxis);

	pulleyDesc.pulley[0] = pulley0; 	// suspension points of two bodies in world space.
	pulleyDesc.pulley[1] = pulley1; 	// suspension points of two bodies in world space.
	pulleyDesc.distance = distance;		// the rest length of the rope connecting the two objects.  The distance is computed as ||(pulley0 - anchor0)|| +  ||(pulley1 - anchor1)|| * ratio.
	pulleyDesc.stiffness = 1.0f;		// how stiff the constraint is, between 0 and 1 (stiffest)
	pulleyDesc.ratio = ratio;			// transmission ratio
	pulleyDesc.flags = NX_PJF_IS_RIGID;	// This is a combination of the bits defined by ::NxPulleyJointFlag. 
	pulleyDesc.motor = gMotorDesc;

//	pulleyDesc.projectionMode = NX_JPM_NONE;
//	pulleyDesc.projectionMode = NX_JPM_POINT_MINDIST;

//	pulleyDesc.jointFlags |= NX_JF_COLLISION_ENABLED;

	NxJoint* pulleyJoint = gScene->createJoint(pulleyDesc);

	return (NxPulleyJoint*)pulleyJoint->is(NX_JOINT_PULLEY);
}

bool InitNx()
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
#if SAMPLES_USE_VRD
	// The settings for the VRD host and port are found in SampleCommonCode/SamplesVRDSettings.h
	if (gPhysicsSDK->getFoundationSDK().getRemoteDebugger() && !gPhysicsSDK->getFoundationSDK().getRemoteDebugger()->isConnected())
		gPhysicsSDK->getFoundationSDK().getRemoteDebugger()->connect(SAMPLES_VRD_HOST, SAMPLES_VRD_PORT, SAMPLES_VRD_EVENTMASK);
#endif

	// Set the physics parameters
	gPhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.005f);

	// Set the debug visualization parameters
	gPhysicsSDK->setParameter(NX_VISUALIZATION_SCALE, 1);
	gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1);
	gPhysicsSDK->setParameter(NX_VISUALIZE_JOINT_LIMITS, 1);
	gPhysicsSDK->setParameter(NX_VISUALIZE_JOINT_LOCAL_AXES, 1);

    // Create the scene
    NxSceneDesc sceneDesc;
    sceneDesc.gravity               = gDefaultGravity;
    gScene = gPhysicsSDK->createScene(sceneDesc);
	if(gScene == NULL) 
	{
		printf("\nError: Unable to create a PhysX scene, exiting the sample.\n\n");
		return false;
	}
    
    // Create the default material
	NxMaterialDesc matDesc;
	matDesc.restitution		= 0.5;
	matDesc.staticFriction	= 0.5;
	matDesc.dynamicFriction	= 0.5;
	NxMaterial *mat = gScene->getMaterialFromIndex(0);
	mat->loadFromDesc(matDesc);

	// Create the default material
	NxMaterial* defaultMaterial = gScene->getMaterialFromIndex(0); 
	defaultMaterial->setRestitution(0.5);
	defaultMaterial->setStaticFriction(0.5);
	defaultMaterial->setDynamicFriction(0.5);

	// Create the objects in the scene
	groundPlane = CreateGroundPlane();

	capsule1 = CreateCapsule(NxVec3(-1,4,0), 1, 0.5, 10);

	capsule1->setLinearDamping(1.5f);
	capsule2 = CreateCapsule(NxVec3(1,4,0), 1, 0.5, 20);
	capsule2->setLinearDamping(1.5f);

	// Motor specs
	gMotorDesc.maxForce = NX_MAX_REAL;		
	gMotorDesc.freeSpin = false;
	gMotorDesc.velTarget = 0.0f;

	// Test pulley Joint
	NxVec3 pulley1 = NxVec3(-1,7,0);
	NxVec3 pulley2 = NxVec3(1,7,0);
	NxVec3 globalAxis = NxVec3(0,-1,0);
	pulleyJoint = CreatePulleyJoint(capsule1, capsule2, pulley1, pulley2, globalAxis, 4.0f, 2.0f);

	gSelectedActor = capsule1;
	UpdateTime();
	RefreshDisplayString();

	return true;
}

void ReleaseNx()
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

NxReal UpdateTime()
{
    NxReal deltaTime;
    gTime = getTime()*0.001f;  // Get current time in seconds
    deltaTime = gTime - gLastTime;
	if (deltaTime > 0)
	{
		gLastTime = gTime;
		return deltaTime;
	}
	else
		return 0.0f;
}

void RunPhysics()
{
	// Update the time step
#ifdef __PPCGEKKO__
	NxReal deltaTime = 1.0f/60.0f;
#else	
	NxReal deltaTime = UpdateTime();
#endif
	// Run collision and dynamics for delta time since the last frame
	gScene->simulate(deltaTime);	
	gScene->flushStream();
	gScene->fetchResults(NX_RIGID_BODY_FINISHED, true);
}

int main(int argc, char** argv)
{
	PrintControls();
#if defined(_XBOX) || defined(__CELLOS_LV2__)
	glutRemapButtonExt(4, 'k', false); //Up to force forward
	glutRemapButtonExt(5, 'i', false); //Down to force backward
	glutRemapButtonExt(6, 'l', false); //Left to force left
	glutRemapButtonExt(7, 'j', false); //Right to force right
	glutRemapButtonExt(8, 'u', false); //Left shoulder to force up
	glutRemapButtonExt(9, 'm', false); //Right shoulder to force down
	glutRemapButtonExt(4, 'w', true); //Shift+Up to camera forward
	glutRemapButtonExt(5, 's', true); //Shift+Down to camera backward
	glutRemapButtonExt(6, 'a', true); //Shift+Left to camera left
	glutRemapButtonExt(7, 'd', true); //Shift+Right to camera right
	glutRemapButtonExt(8, 'q', true); //Shift+Left shoulder to camera up
	glutRemapButtonExt(9, 'z', true); //Shift+Right shoulder to camera down
	glutRemapButtonExt(1, '1', false); //B/O to motor forward
	glutRemapButtonExt(2, '2', false); //X/S to motor backward
	glutRemapButtonExt(3, '0', false); //Y/T to motor stop
	glutRemapButtonExt(0, 'x', true); //Shift+A/X to toggle shadows
	glutRemapButtonExt(1, 'r', true); //Shift+B/O to select next actor
	glutRemapButtonExt(2, 'f', true); //Shift+X/S to force mode
	glutRemapButtonExt(3, 'b', true); //Shift+Y/T to toggle wireframe
#endif
#ifdef __PPCGEKKO__
	glutRemapButtonExt(GLUT_KEY_UP, 	'i');
	glutRemapButtonExt(GLUT_KEY_DOWN,	'k');
	glutRemapButtonExt(GLUT_KEY_LEFT,	'j');
	glutRemapButtonExt(GLUT_KEY_RIGHT,	'l');
	glutRemapButtonExt(GLUT_KEY_HOME,	'r');
	glutRemapButtonExt('a', 'u');
	glutRemapButtonExt('b', 'm');
	glutRemapButtonExt('+', '1');
	glutRemapButtonExt('-', '2');
	glutRemapButtonExt('1', '0');
	glutRemapButtonExt('2', 'f');
#endif
    InitGlut(argc, argv);

	// Initialize physics scene and start the application main loop if scene was created
	if (InitNx())
	{
		atexit(ExitCallback);
		glutMainLoop();
	}

	return 0;
}

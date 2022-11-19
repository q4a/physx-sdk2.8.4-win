// ===============================================================================
//
//						   NVIDIA PhysX SDK Sample Program
//
// Title: Six Degree of Freedom Joints
// Description: This sample program shows how to use the D6 joints. For example it
//              is possible to use this joint to switch from one type of joint to
//              another without recreating the joint.
//
// Originally written by: Bob Schade (10-15-04)
//
// ===============================================================================

#include "SampleD6Joint.h"
#include "GLFontRenderer.h"
#include "Timing.h"
#include "Utilities.h"
#include "SamplesVRDSettings.h"
#include "UserAllocator.h"
#include <stdarg.h>

#ifdef __PPCGEKKO__
#include "GLFontRenderer.h"
#endif

// Physics SDK globals
NxPhysicsSDK*     gPhysicsSDK = NULL;
NxScene*          gScene = NULL;
//NxVec3            gDefaultGravity(0,-9.8,0);
NxVec3            gDefaultGravity(0,0,0);
DebugRenderer gDebugRenderer;
UserAllocator*	gAllocator = NULL;

// Time globals
NxReal gTime;
NxReal gLastTime;
 
// Display globals
int gMainHandle;
int mx = 0;
int my = 0;

// Camera globals
NxVec3 gCameraPos(0,5,-8);
NxVec3 gCameraForward(0,0,1);
NxVec3 gCameraRight(-1,0,0);
const NxReal gCameraSpeed = 0.02;

// HUD globals
char gDisplayString[512] = "";
NxD6JointMotion gJointMotion[6] = 
{ 
    NX_D6JOINT_MOTION_LOCKED,
    NX_D6JOINT_MOTION_LOCKED,
    NX_D6JOINT_MOTION_LOCKED,
    NX_D6JOINT_MOTION_LOCKED,
    NX_D6JOINT_MOTION_LOCKED,
    NX_D6JOINT_MOTION_LOCKED
};
const char* gJointMotionString[3] = {"Locked","Limited","Free"}; 

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
bool bDebugWireframeMode = false;


// Actor globals
NxActor* groundPlane = NULL;
NxActor* capsule1 = NULL;
NxActor* capsule2 = NULL;

// Joint globals
NxD6Joint* d6Joint = NULL;
NxD6Joint* d6JointUpper = NULL;

// Focus actor
NxActor* gSelectedActor = NULL;

// Upper Joint mode
int bUpperJointLocked = true;


void toggle(NxD6JointMotion &motion) 
{
	switch(motion) {
		case NX_D6JOINT_MOTION_LOCKED : motion = NX_D6JOINT_MOTION_LIMITED; break;
		case NX_D6JOINT_MOTION_LIMITED : motion = NX_D6JOINT_MOTION_FREE; break;
		case NX_D6JOINT_MOTION_FREE : motion = NX_D6JOINT_MOTION_LOCKED; break;
	}
}

void PrintControls()
{
#ifdef __PPCGEKKO__
    printf("\n Force Controls:\n ---------------\n UP = +z, DOWN = -z\n LEFT = +x, RIGHT = -x\n a = +y, b = -y\n");
	printf("\n Miscellaneous:\n --------------\n + - = toggle DOF\n  HOME = Select Next Actor\n 1 = Toggle Force Mode\n 2 = Toggle Upper Joint Mode\n");
#else
	printf("\n Flight Controls:\n ----------------\n w = forward, s = back\n a = strafe left, d = strafe right\n q = up, z = down\n");
    printf("\n Force Controls:\n ---------------\n i = +z, k = -z\n j = +x, l = -x\n u = +y, m = -y\n");
	printf("\n Miscellaneous:\n --------------\n 1-6 = toggle DOF\n p = Pause\n r = Select Next Actor\n f = Toggle Force Mode\n t = Toggle Upper Joint Mode\n b = Toggle Debug Wireframe Mode\n x = Toggle Shadows\n Q = Reset Scene\n");
#endif	
}

float PrintfTex(float y, const char* fmt, ... )
{
	va_list  vlist;
    char     buf[512];

    // Get output string
    va_start( vlist, fmt );
    vsprintf( buf, fmt, vlist );
    va_end( vlist );
	
	int len = strlen(buf);
	len = (len < 256)?len:255;
	int start = 0;
	char textBuffer[256];
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
	
    y = PrintfTex(y,gDisplayString);
#ifdef __PPCGEKKO__
    y = PrintfTex(y,"\n Force Controls:\n UP = +z, DOWN = -z\n LEFT = +x, RIGHT = -x\n a = +y, b = -y\n");
	PrintfTex(y,"\n Miscellaneous:\n + = select DOF\n - = toggle DOF\n  HOME = Select Next Actor\n 1 = Toggle Force Mode\n 2 = Toggle Upper Joint Mode\n");
#endif	
}

//#endif  
#ifdef __PPCGEKKO__
static int curMotionIndex = 3;
#define LCUR(x) ((x)== curMotionIndex)?"[":" "
#define RCUR(x) ((x)== curMotionIndex)?"]":" " 
void RefreshDisplayString()
{
	sprintf(gDisplayString, "CONFIGURABLE JOINT subject to %s:\nROTATION \n   Axis: %s%s%s   Normal: %s%s%s   Binormal: %s%s%s\nTRANSLATION \n   Axis: %s%s%s   Normal: %s%s%s   Binormal: %s%s%s", 
		bForceMode ? "force" : "torque", 
		LCUR(3),gJointMotionString[gJointMotion[3]],RCUR(3),
	    LCUR(4),gJointMotionString[gJointMotion[4]],RCUR(4),
	    LCUR(5),gJointMotionString[gJointMotion[5]],RCUR(5),
	    LCUR(0),gJointMotionString[gJointMotion[0]],RCUR(0),
	    LCUR(1),gJointMotionString[gJointMotion[1]],RCUR(1),
	    LCUR(2),gJointMotionString[gJointMotion[2]],RCUR(2));
}
#else	
void RefreshDisplayString()
{
	sprintf(gDisplayString, "CONFIGURABLE JOINT subject to %s:\nROTATION 1, 2, 3\n   Axis: %s   Normal: %s   Binormal: %s\nTRANSLATION 4, 5, 6\n   Axis: %s   Normal: %s   Binormal: %s", 
		bForceMode ? "force" : "torque", 
		gJointMotionString[gJointMotion[3]],
	    gJointMotionString[gJointMotion[4]],
	    gJointMotionString[gJointMotion[5]],
	    gJointMotionString[gJointMotion[0]],
	    gJointMotionString[gJointMotion[1]],
	    gJointMotionString[gJointMotion[2]]);
}
#endif

// Configurable joint - roll joint
void ReconfigureD6Joint()
{
	NxActor* a0 = capsule1;
	NxActor* a1 = capsule2;

    NxD6JointDesc d6Desc;

	d6Joint->saveToDesc(d6Desc);

	// reset actor 1
/*
	NxMat33 orient;
	orient.id();
	a1->raiseBodyFlag(NX_BF_KINEMATIC);
	a1->setGlobalOrientation(orient);
	a1->setGlobalPosition(NxVec3(0,3,0));
	a1->clearBodyFlag(NX_BF_KINEMATIC);
*/
	// set the DOF
	d6Desc.xMotion = gJointMotion[0];
	d6Desc.yMotion = gJointMotion[1];
	d6Desc.zMotion = gJointMotion[2];

	d6Desc.twistMotion = gJointMotion[3];
	d6Desc.swing1Motion = gJointMotion[4];
	d6Desc.swing2Motion = gJointMotion[5];

	d6Joint->loadFromDesc(d6Desc);

	// reconfig upper joint
	d6JointUpper->saveToDesc(d6Desc);
	if (bUpperJointLocked) {
		d6Desc.twistMotion = NX_D6JOINT_MOTION_LOCKED;
		d6Desc.swing1Motion = NX_D6JOINT_MOTION_LOCKED;
		d6Desc.swing2Motion = NX_D6JOINT_MOTION_LOCKED;
	}
	else {
		d6Desc.twistMotion = NX_D6JOINT_MOTION_FREE;
		d6Desc.swing1Motion = NX_D6JOINT_MOTION_FREE;
		d6Desc.swing2Motion = NX_D6JOINT_MOTION_FREE;
	}
	d6JointUpper->loadFromDesc(d6Desc);
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
	if (bDebugWireframeMode) 
		{
		glDisable(GL_LIGHTING);
		gDebugRenderer.renderData(*gScene->getDebugRenderable());
		glEnable(GL_LIGHTING);
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
#ifdef __PPCGEKKO__
		case '+':
		{
			curMotionIndex = (curMotionIndex + 1) % 6;
			RefreshDisplayString();
//			toggle(gJointMotion[curMotionIndex]); ReconfigureD6Joint(); RefreshDisplayString(); 
			break;
		}
		case '-':
		{
//			curMotionIndex = (curMotionIndex + 5) % 6;
			toggle(gJointMotion[curMotionIndex]); ReconfigureD6Joint(); RefreshDisplayString(); 
			break;
		}
#endif		
		case 'r': { SelectNextActor(); break; }		
		case 27 : { exit(0); break; }
		case 'p': { bPause = !bPause; UpdateTime(); break; }
		case 'x': { bShadows = !bShadows; break; }
		case 'b': { bDebugWireframeMode = !bDebugWireframeMode; break; }		
		case 'f': { bForceMode = !bForceMode; RefreshDisplayString(); break; }
		case 't': { bUpperJointLocked = !bUpperJointLocked; ReconfigureD6Joint(); RefreshDisplayString(); break; }
		case '1': { toggle(gJointMotion[3]); ReconfigureD6Joint(); RefreshDisplayString(); break; }
		case '2': { toggle(gJointMotion[4]); ReconfigureD6Joint(); RefreshDisplayString(); break; }
		case '3': { toggle(gJointMotion[5]); ReconfigureD6Joint(); RefreshDisplayString(); break; }
		case '4': { toggle(gJointMotion[0]); ReconfigureD6Joint(); RefreshDisplayString(); break; }
		case '5': { toggle(gJointMotion[1]); ReconfigureD6Joint(); RefreshDisplayString(); break; }
		case '6': { toggle(gJointMotion[2]); ReconfigureD6Joint(); RefreshDisplayString(); break; }
		case 'Q': { ResetNx(); break; }
		default: { break; }
	}
}

void KeyboardUpCallback(unsigned char key, int x, int y)
{
	gKeys[key] = false;
}

//void SpecialCallback(int key, int x, int y)
//{
//	gKeys[key] = true;
//	
//	switch (key)
//    {
//		// Reset PhysX
//		case GLUT_KEY_F10: ResetNx(); return; 
//	}
//}

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

void ExitCallback()
{
	ReleaseNx();
}

void InitGlut(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitWindowSize(512, 512);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	gMainHandle = glutCreateWindow("SampleD6Joint");
	glutSetWindow(gMainHandle);
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
#ifdef __PPCGEKKO__    
    glEnable(GL_CULL_FACE);
#endif    

    // Setup lighting
    glEnable(GL_LIGHTING);
    float AmbientColor[]    = { 0.0f, 0.1f, 0.2f, 0.0f };         glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientColor);
    float DiffuseColor[]    = { 0.2f, 0.2f, 0.2f, 0.0f };         glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseColor);
    float SpecularColor[]   = { 0.5f, 0.5f, 0.5f, 0.0f };         glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularColor);
    float Position[]        = { 100.0f, 100.0f, -400.0f, 1.0f };  glLightfv(GL_LIGHT0, GL_POSITION, Position);
    glEnable(GL_LIGHT0);
}

/*
enum NxD6JointLockFlags
	{
	NX_D6JOINT_LOCK_X			= 1<<0,
	NX_D6JOINT_LOCK_Y			= 1<<1,
	NX_D6JOINT_LOCK_Z			= 1<<2,
	NX_D6JOINT_LOCK_LINEAR		= 7,

	NX_D6JOINT_LOCK_TWIST		= 1<<3,
	NX_D6JOINT_LOCK_SWING1		= 1<<4,
	NX_D6JOINT_LOCK_SWING2		= 1<<5,
	NX_D6JOINT_LOCK_ANGULAR		= 7<<3,

	};


enum NxD6JointLimitFlags
	{
	NX_D6JOINT_LIMIT_TWIST	= 1<<0,
	NX_D6JOINT_LIMIT_SWING	= 1<<1,
	NX_D6JOINT_LIMIT_LINEAR	= 1<<2
	};

enum NxD6JointDriveType
	{
	NX_D6JOINT_DRIVE_POSITION	= 1<<0,
	NX_D6JOINT_DRIVE_VELOCITY	= 1<<1,
	NX_D6JOINT_DRIVE_SLERP		= 1<<2
	};



///////////////////////////////////////////////////////////


enum NxD6JointMotion
	{
	NX_D6JOINT_MOTION_LOCKED,
	NX_D6JOINT_MOTION_LIMITED,
	NX_D6JOINT_MOTION_FREE
	};
*/

NxD6Joint* CreateD6Joint(NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis)
{
	NxD6JointDesc d6Desc;
	d6Desc.actor[0] = a0;
	d6Desc.actor[1] = a1;
	d6Desc.setGlobalAnchor(globalAnchor);
	d6Desc.setGlobalAxis(globalAxis);

	d6Desc.twistMotion = NX_D6JOINT_MOTION_LOCKED;
	d6Desc.swing1Motion = NX_D6JOINT_MOTION_LOCKED;
	d6Desc.swing2Motion = NX_D6JOINT_MOTION_LOCKED;

	d6Desc.xMotion = NX_D6JOINT_MOTION_LOCKED;
	d6Desc.yMotion = NX_D6JOINT_MOTION_LOCKED;
	d6Desc.zMotion = NX_D6JOINT_MOTION_LOCKED;

	d6Desc.linearLimit.value = gLinearLimit;
	d6Desc.swing1Limit.value = gSwing1Limit;
	d6Desc.swing2Limit.value = gSwing2Limit;
	d6Desc.twistLimit.low.value =  gTwistLowLimit;
	d6Desc.twistLimit.high.value = gTwistHighLimit;

	// drive test
/*
	d6Desc.drivePosition.x = 0.5f;
	d6Desc.xDrive.spring = 1000.0f;
	d6Desc.xDrive.damping = 1.0f;
	d6Desc.xDrive.driveType.raiseFlagMask(NX_D6JOINT_DRIVE_POSITION);

	d6Desc.driveLinearVelocity.z = 0.5f;
	d6Desc.zDrive.forceLimit = 1000.0f;
	d6Desc.zDrive.driveType.raiseFlagMask(NX_D6JOINT_DRIVE_VELOCITY);

	d6Desc.swingDrive.driveType.raiseFlagMask(NX_D6JOINT_DRIVE_POSITION);
	d6Desc.swingDrive.spring = 100.0f;
	d6Desc.swingDrive.damping = 1.0f;
	d6Desc.driveOrientation.fromAngleAxis(-40.0f, NxVec3(0,1,1));

	d6Desc.swingDrive.driveType.raiseFlagMask(NX_D6JOINT_DRIVE_VELOCITY);
	d6Desc.swingDrive.forceLimit = 1000.0f;
	d6Desc.driveAngularVelocity.y = 10.0f;
*/	

//	d6Desc.projectionMode = NX_JPM_NONE;
	d6Desc.projectionMode = NX_JPM_POINT_MINDIST;

//	d6Desc.jointFlags |= NX_JF_COLLISION_ENABLED;

	NxJoint* d6Joint = gScene->createJoint(d6Desc);

	return (NxD6Joint*)d6Joint->is(NX_JOINT_D6);
}

bool InitNx()
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

	NxMaterial * defaultMaterial = gScene->getMaterialFromIndex(0); 
	defaultMaterial->setRestitution(0.0f);
	defaultMaterial->setStaticFriction(0.5f);
	defaultMaterial->setDynamicFriction(0.5f);
		
		
	// Create the objects in the scene
	groundPlane = CreateGroundPlane();

	capsule1 = CreateCapsule(NxVec3(0,5,0), 1, 0.5, 10);
//	capsule1->raiseBodyFlag(NX_BF_KINEMATIC);
	capsule2 = CreateCapsule(NxVec3(0,3,0), 1, 0.5, 10);
	capsule2->setLinearDamping(0.5);

	NxVec3 globalAnchor = NxVec3(0,5,0);
	NxVec3 globalAnchorUpper = NxVec3(0,7,0);
	NxVec3 globalAxis = NxVec3(0,-1,0);
	// upper joint
	d6JointUpper = CreateD6Joint(NULL, capsule1, globalAnchorUpper, globalAxis);
	// lower test joint
	d6Joint = CreateD6Joint(capsule1, capsule2, globalAnchor, globalAxis);

	gSelectedActor = capsule2;
	UpdateTime();
	RefreshDisplayString();

#if defined(_XBOX) || defined(__CELLOS_LV2__)
	glutRemapButtonExt(8, 'f', true); //shift+Left shoulder to toggle force mode
	glutRemapButtonExt(9, 'Q', true); //shift+Right shoulder to reset

	glutRemapButtonExt(4, 'w', true); //Shift+Up to forward
	glutRemapButtonExt(5, 's', true); //Shift+Down to back
	glutRemapButtonExt(6, 'a', true); //Shift+Left to left
	glutRemapButtonExt(7, 'd', true); //Shift+Right to right

	glutRemapButtonExt(4, 'k', false); //Up to force forward
	glutRemapButtonExt(5, 'i', false); //Down to force backward
	glutRemapButtonExt(6, 'l', false); //Left to force left
	glutRemapButtonExt(7, 'j', false); //Right to force right
	glutRemapButtonExt(8, 'u', false); //Left shoulder to force up
	glutRemapButtonExt(9, 'm', false); //Right shoulder to force down

	glutRemapButtonExt(0, 'r', false); //A/X to select next actor
	glutRemapButtonExt(0, 't', true); //Shift+A/X to toggle joint lock
	
	glutRemapButtonExt(1, '1', false); //B/O to toggle joint motion 1
	glutRemapButtonExt(1, '2', true); //Shift+B/O to toggle joint motion 2
	glutRemapButtonExt(2, '3', false); //X/S to toggle joint motion 3
	glutRemapButtonExt(2, '4', true); //Shift+X/S to toggle joint motion 4
	glutRemapButtonExt(3, '5', false); //Y/T to toggle joint motion 5
	glutRemapButtonExt(3, '6', true); //Shift+Y/T to toggle joint motion 6
#endif
#ifdef __PPCGEKKO__
	glutRemapButtonExt(GLUT_KEY_UP,'i');
	glutRemapButtonExt(GLUT_KEY_DOWN,'k');
	glutRemapButtonExt(GLUT_KEY_LEFT,'j');
	glutRemapButtonExt(GLUT_KEY_RIGHT,'l');
	glutRemapButtonExt('a','u');
	glutRemapButtonExt('b','m');
	glutRemapButtonExt(GLUT_KEY_HOME,'r');
	glutRemapButtonExt('1','f');
	glutRemapButtonExt('2','t');
#endif
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

void ResetNx()
{
	ReleaseNx();
	if (!InitNx()) exit(0);
}

NxReal UpdateTime()
{
    NxReal deltaTime;
    gTime = getTime()*0.001f;  // Get current time in seconds
    deltaTime = gTime - gLastTime;
    gLastTime = gTime;
    return deltaTime;
}

void RunPhysics()
{
	// Update the time step
	NxReal deltaTime = UpdateTime();

	// Run collision and dynamics for delta time since the last frame
	gScene->simulate(deltaTime);	
	gScene->flushStream();
	gScene->fetchResults(NX_RIGID_BODY_FINISHED, true);
}

int main(int argc, char** argv)
{
	PrintControls();
    InitGlut(argc, argv);

	// Initialize physics scene and start the application main loop if scene was created
	if (InitNx())
	{
		atexit(ExitCallback);
		glutMainLoop();
	}

	ReleaseNx();
	return 0;
}

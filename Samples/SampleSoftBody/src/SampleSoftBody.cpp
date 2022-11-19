// ===============================================================================
//
//						   NVIDIA PhysX SDK Sample Program
//
// Title: Soft Body Sample
// Description: This sample shows an example of how to use the soft body API.
//
// Originally written by: Matthias Müller-Fischer (10-03-06)
// Revised by:			  Roni Oeschger
//
// ===============================================================================

#define NXU_CORE_DUMP_ENABLED 0

#include "SampleSoftBody.h"
#include "MySoftBody.h"
#include "ObjMesh.h"
#include "Timing.h"
#include "GLFontRenderer.h"
#include "MediaPath.h"
#include "Utilities.h"
#include "SamplesVRDSettings.h"
#include "cooking.h"
#include "ActorPicking.h"
#include "UserAllocator.h"

#if NXU_CORE_DUMP_ENABLED
	#include "NXU_helper.h"
#endif

#ifdef __PPCGEKKO__
#include "GLFontRenderer.h"
#endif

DebugRenderer gDebugRenderer;

// Physics SDK globals
NxPhysicsSDK*     gPhysicsSDK = NULL;
NxScene*          gScene = NULL;
NxVec3            gDefaultGravity(0.0f, -9.8f, 0.0f);
UserAllocator*	gAllocator = NULL;
#if defined(NX_DISABLE_HARDWARE) || defined(_XBOX) || defined(__CELLOS_LV2__)
bool              gHardwareSimulation = false;
#else
bool              gHardwareSimulation = true;
#endif
bool              gTearLines = false;
bool			  gCookingInitialized = false;

// Time globals
NxReal gTime;
NxReal gLastTime;

// Display globals
int  gMainHandle;
bool gShowRigidBodies = false;
bool gShowTetraMesh = false;
bool gShowSoftBodies = true;
int  gInvisible;

// Help text globals
char gDisplayString[512] = "";
char gTitleString[512] = "";
char gHelpString[512] = "";
bool gHelp = false;

// Camera globals
NxReal gCameraFov = 40.0f;
NxVec3 gCameraPos(0.0f, 15.0f, 30.0f);
NxVec3 gCameraForward(0.0f, 0.0f, -1.0f);
NxVec3 gCameraRight(1.0f, 0.0f, 0.0f);
const NxReal gCameraSpeed = 0.2f;
int gViewWidth = 0;
int gViewHeight = 0;

// MouseGlobals
int mx = 0;
int my = 0;
NxDistanceJoint *gMouseJoint = NULL;
NxActor* gMouseSphere = NULL;
NxReal gMouseDepth = 0.0f;

// Keyboard globals
#define MAX_KEYS 256
bool gKeys[MAX_KEYS];

// Simulation globals
bool bPause = false;
bool bShadows = true;
bool bWireframeMode = false;
bool bGravity = false;

// Scene
int      gSceneNr = 1;
NxArray<MySoftBody *> gSoftBodies;
NxArray<ObjMesh *> gObjMeshes;

NxActor *gSelectedActor = NULL;
NxActor *gGroundPlane = NULL;
NxSoftBody *gSelectedSoftBody = NULL;
int		 gSelectedSoftBodyVertex = -1;

// fps
int			gFrameCounter = 0;
float       gPreviousTime = getCurrentTime();
//Stopwatch	gStopWatch;


// ------------------------------------------------------------------------------------
void PrintControls()
{
#ifdef __PPCGEKKO__
	printf("\n Flight Controls:\n ----------------\n UP = forward, DOWN = back\n LEFT = strafe left, RIGHT = strafe right\n");
#else
	printf("\n Flight Controls:\n ----------------\n w = forward, s = back\n a = strafe left, d = strafe right\n q = up, e = down\n");
	printf("\n Miscellaneous:\n --------------\n p = Pause\n m = Single Step\n x = Toggle Shadows\n");
#endif	
}

// ------------------------------------------------------------------------------------

void DisplayStats(float& y)
{
#ifdef WIN32 
	static NxI32 totalHeapIdx = -1;
	static NxI32 deformableHeapIdx = -1;
	static NxI32 utilHeapIdx = -1;

	if (gHardwareSimulation)
	{
		const NxSceneStats2* stats = gScene->getStats2();
		NxU32 numStats = stats->numStats;
		if (totalHeapIdx == -1)
		{
			// Figure out indices for stats. This should only need to be done once.
			for (NxU32 curIdx = 0; curIdx < stats->numStats; ++curIdx)
			{
				if (0 == strncmp(stats->stats[curIdx].name, "GpuHeapUsageTotal", 17))
					totalHeapIdx = curIdx;
				else if (0 == strncmp(stats->stats[curIdx].name, "GpuHeapUsageDeformable", 17))
					deformableHeapIdx = curIdx;
				else if (0 == strncmp(stats->stats[curIdx].name, "GpuHeapUsageUtils", 17))
					utilHeapIdx = curIdx;
			}
		}

		char textBuffer[1024];

		if ((totalHeapIdx >= 0) && ((NxU32)totalHeapIdx < numStats))
		{
			NxSceneStatistic& stat = stats->stats[totalHeapIdx];
			_snprintf(textBuffer, 1024, "%s = %d, max = %d kB", stat.name, stat.curValue, stat.maxValue);
			GLFontRenderer::print(0.01, y, 0.03f, textBuffer);
			y -= 0.035f;
		}

		if ((deformableHeapIdx >= 0) && ((NxU32)deformableHeapIdx < numStats))
		{
			NxSceneStatistic& stat = stats->stats[deformableHeapIdx];
			_snprintf(textBuffer, 1024, "%s = %d, max = %d kB", stat.name, stat.curValue, stat.maxValue);
			GLFontRenderer::print(0.01, y, 0.03f, textBuffer);
			y -= 0.035f;
		}

		if ((utilHeapIdx >= 0) && ((NxU32)utilHeapIdx < numStats))
		{
			NxSceneStatistic& stat = stats->stats[utilHeapIdx];
			_snprintf(textBuffer, 1024, "%s = %d, max = %d kB", stat.name, stat.curValue, stat.maxValue);
			GLFontRenderer::print(0.01, y, 0.03f, textBuffer);
			y -= 0.035f;
		}
	}
#endif
}

// ------------------------------------------------------------------------------------

void DisplayText()
{
	sprintf(gDisplayString, "%s%s", gTitleString, gHelp?gHelpString:"");

	float y = 0.95f;
	int len = (int)strlen(gDisplayString);
	len = (len < 1024)?len:1023;
	int start = 0;
	char textBuffer[1024];
	for(int i=0;i<len;i++)
	{
		if(gDisplayString[i] == '\n' || i == len-1)
		{
			int offset = i;
			if(i == len-1) offset= i+1;
			memcpy(textBuffer, gDisplayString+start, offset-start);
			textBuffer[offset-start]=0;
			GLFontRenderer::print(0.01, y, 0.03f, textBuffer);
			y -= 0.035f;
			start = offset+1;
		}
	}
	DisplayStats(y);
}


// ------------------------------------------------------------------------------------
void SetTitleString(const char *demoName)
{
#ifdef __PPCGEKKO__
	sprintf(gTitleString, "%s (%s)\nHOME for help\n", demoName, gHardwareSimulation ? "hardware" : "software");
#else
	sprintf(gTitleString, "%s (%s)\nF1 for help\n", demoName, gHardwareSimulation ? "hardware" : "software");
#endif	
}

// ------------------------------------------------------------------------------------
void SetHelpString(const char *demoKeyString)
{
#ifdef __PPCGEKKO__
	sprintf(gHelpString, "\nGeneral:\n");
	strcat(gHelpString, "    a: choose scene\n");
	strcat(gHelpString, "    1: show tetrahedra\n");
	strcat(gHelpString, "    2: show hidden rigid bodies\n");
	strcat(gHelpString, "    b: shoot sphere\n");
	strcat(gHelpString, "    HOME: toggle help\n");
#else
	sprintf(gHelpString, "\nGeneral:\n");
	strcat(gHelpString, "    1-7: choose scene\n");
	strcat(gHelpString, "    t: show tetrahedra\n");
	strcat(gHelpString, "    r: show hidden rigid bodies\n");
	strcat(gHelpString, "    p: pause\n");
	strcat(gHelpString, "    m: single step\n");

#if !defined(NX_DISABLE_HARDWARE) && !defined(_XBOX) && !defined(__CELLOS_LV2__)
	strcat(gHelpString, "    h: hardware on/off\n");
#endif

	strcat(gHelpString, "    v: toggle volume preservation\n");
	strcat(gHelpString, "    w,a,s,d: move/strafe\n");
	strcat(gHelpString, "    q,e: move up/down\n");
	strcat(gHelpString, "    mouse right: pick and drag\n");
	strcat(gHelpString, "    F3,F4: pick push/pull\n");
	strcat(gHelpString, "    space: shoot sphere\n");
	strcat(gHelpString, "    F1: toggle help\n");
	strcat(gHelpString, "    x: toggle shadows\n");
	
#if NXU_CORE_DUMP_ENABLED
	strcat(gHelpString, "    c: core dump\n");
#endif
#endif
	if (demoKeyString)
	{
		strcat(gHelpString, "\nDemo specific:\n");
		strcat(gHelpString, demoKeyString);
	}
}


// ------------------------------------------------------------------------------------
void ModifyStiffnesses(NxReal delta)
{
	for (MySoftBody **softBody = gSoftBodies.begin(); softBody != gSoftBodies.end(); softBody++){
		NxSoftBody *nxBody = (*softBody)->getNxSoftBody();
		NxReal stiffness = nxBody->getStretchingStiffness();
		stiffness = NxMath::clamp(stiffness + delta, 1.0f, 0.0f);
		nxBody->setStretchingStiffness(stiffness);
	}
}

// ------------------------------------------------------------------------------------
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
			case 'e':{ gCameraPos -= NxVec3(0,1,0)*gCameraSpeed; break; }
			case 'q':{ gCameraPos += NxVec3(0,1,0)*gCameraSpeed; break; }
			case '+':{ ModifyStiffnesses(0.01f); break; }
			case '-':{ ModifyStiffnesses(-0.01f); break; }
		}
	}
}

// ------------------------------------------------------------------------------------
void SetupCamera()
{
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	gluPerspective(gCameraFov, ((float)glutGet(GLUT_WINDOW_WIDTH)/(float)glutGet(GLUT_WINDOW_HEIGHT)), 1.0f, 10000.0f);
	gluLookAt(gCameraPos.x,gCameraPos.y,gCameraPos.z,gCameraPos.x + gCameraForward.x, gCameraPos.y + gCameraForward.y, gCameraPos.z + gCameraForward.z, 
		0.0f, 1.0f, 0.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


// ------------------------------------------------------------------------------------
void RenderScene(bool shadows)
{
    // Render all the actors in the scene
	int nbActors = gScene->getNbActors();
	NxActor** actors = gScene->getActors();
	while (nbActors--)
	{
		NxActor* actor = *actors++;
		if ((actor->userData == (void*)&gInvisible) && !gShowRigidBodies)
			continue;

		if (bWireframeMode)
			DrawWireActor(actor);
		else
			DrawActor(actor);

		// Handle shadows
		if (shadows)
		{
			DrawActorShadow(actor);
		}
	}

	// possibility to turn off rendering of softbodies completely
	if (gShowSoftBodies) {
		// Render all the soft bodies in the scene
		for (MySoftBody **softBody = gSoftBodies.begin(); softBody != gSoftBodies.end(); softBody++){
			glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
			(*softBody)->simulateAndDraw(shadows, gSceneNr == 5 || gShowTetraMesh, false);
		}
	}
}

// ------------------------------------------------------------------------------------
void RenderCallback()
{
	if (gScene && !bPause)
        RunPhysics();
	
    // Clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ProcessKeys();
	SetupCamera();

	RenderScene(bShadows);

	DisplayText();

    glutSwapBuffers();
}

// ------------------------------------------------------------------------------------
void ReshapeCallback(int width, int height)
{
	gViewWidth = width;
	gViewHeight = height;
    glViewport(0, 0, width, height);
}

// ------------------------------------------------------------------------------------
void IdleCallback()
{
    glutPostRedisplay();

	float time = getCurrentTime();
	float elapsedTime = time - gPreviousTime;
	
	if (elapsedTime > 1.0f) {
		char title[30];
		sprintf(title, "SampleSoftBody %3.1f fps", (float)gFrameCounter / elapsedTime);
		glutSetWindowTitle(title);
		gPreviousTime = time;
		gFrameCounter = 0;
	}

	getElapsedTime();
}

// ------------------------------------------------------------------------------------
void KeyboardCallback(unsigned char key, int x, int y)
{
	gKeys[key] = true;
#ifdef __PPCGEKKO__
	if('e' == key)
	{
		gSceneNr = gSceneNr%7 + 1;
		ResetNx();
	}
#else
	if ('1' <= key && key <= '7') {
		if(bPause) bPause = !bPause;
		gSceneNr = key - '0';
		ResetNx();
	}

	if (key == '0') {
		if(bPause) bPause = !bPause;
		if(++gSceneNr >= 8) gSceneNr = 1;
		ResetNx();
	}
#endif
	switch (key)
	{

#if NXU_CORE_DUMP_ENABLED
	case 'C':
	case 'c':
		{
			char scratch[512];
			static int dumpCount=0;
			sprintf(scratch,"dump_%d.xml", dumpCount);
  		printf("Saving scene to NxuStream file '%s'\r\n", scratch);
			NXU::coreDump(gPhysicsSDK,scratch,NXU::FT_XML,true,false);

			sprintf(scratch,"dump_%d.nxb", dumpCount);
  		printf("Saving scene to NxuStream file '%s'\r\n", scratch);
			NXU::coreDump(gPhysicsSDK,scratch,NXU::FT_BINARY,true,false);

			dumpCount++;
		}
		break;
#endif

	case 'G':
	case 'g': {     
		NxSoftBody **softBodies = gScene->getSoftBodies();
		for (NxU32 i = 0; i < gScene->getNbSoftBodies(); i++) {
			softBodies[i]->setFlags(softBodies[i]->getFlags() ^ NX_SBF_GRAVITY);
		}
		break;
			  }

	case 'p': { bPause = !bPause; UpdateTime(); break; }
	case 'm': { RunPhysics(); glutPostRedisplay(); break; }
	case 'x': { bShadows = !bShadows; break; }
	case 'n': { bWireframeMode = !bWireframeMode; break; }		
	case 'v': {
		NxSoftBody **softBodies = gScene->getSoftBodies();
		for (NxU32 i = 0; i < gScene->getNbSoftBodies(); i++) {
			softBodies[i]->setFlags(softBodies[i]->getFlags() ^ NX_SBF_VOLUME_CONSERVATION);
		}
		break;
			  }
	case ' ': {
		NxActor *sphere = CreateSphere(gCameraPos, 1.0f, 1.0f);
		sphere->setLinearVelocity(gCameraForward * 20.0f);
		break; 
			  }

#if !defined(NX_DISABLE_HARDWARE) && !defined(_XBOX) && !defined(__CELLOS_LV2__)
	case 'h' : {
		gHardwareSimulation = !gHardwareSimulation;
		ResetNx();
		break;
			   }
#endif

	case 't' : {
		gShowTetraMesh = !gShowTetraMesh;
		break;
			   }
	case 'r' : {
		gShowRigidBodies = !gShowRigidBodies;
		break;
			   }
    case 'z' : {
		gShowSoftBodies = !gShowSoftBodies;
		break;
			   }
	case 27 : { exit(0); break; }
	default : { break; }
	}
}

// ------------------------------------------------------------------------------------
void KeyboardUpCallback(unsigned char key, int x, int y)
{
	gKeys[key] = false;

}


// ------------------------------------------------------------------------------------
NxActor* CreateCompoundCapsule(const NxVec3& pos0, const NxReal height0, const NxReal radius0, const NxReal height1, const NxReal radius1)
{
	// Add a single-shape actor to the scene
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	// The actor has two shapes (capsules)
	NxCapsuleShapeDesc capsuleDesc;
	capsuleDesc.height = height0;
	capsuleDesc.radius = radius0;
	capsuleDesc.localPose.t = NxVec3(0,radius0+(NxReal)0.5*height0,0);
	NxMat33 rot0, rot1;
	rot0.rotX(-0.4);
	rot1.rotZ(-0.3);
	rot0 = rot0*rot1;
	capsuleDesc.localPose.M = rot0;
	actorDesc.shapes.pushBack(&capsuleDesc);

	NxCapsuleShapeDesc capsuleDesc1;
	capsuleDesc1.height = height1;
	capsuleDesc1.radius = radius1;
	capsuleDesc1.localPose.t = NxVec3(0.33,3.15,-0.4);
	rot0.rotY(-0.04);
	capsuleDesc1.localPose.M = rot0;
	actorDesc.shapes.pushBack(&capsuleDesc1);

	actorDesc.body = &bodyDesc;
	actorDesc.density = 1.0;

	actorDesc.globalPose.t = pos0;
	return gScene->createActor(actorDesc);	
}


// ------------------------------------------------------------------------------------
void LetGoSoftBody()
{
	if (gSelectedSoftBody && gSelectedSoftBodyVertex >= 0)
		gSelectedSoftBody->freeVertex(gSelectedSoftBodyVertex);
	gSelectedSoftBodyVertex = -1;
}

// ------------------------------------------------------------------------------------
bool PickSoftBody(int x, int y)
{
	NxRay ray; 
	ViewUnProject(x,y, 0.0f, ray.orig);
	ViewUnProject(x,y, 1.0f, ray.dir);
	ray.dir -= ray.orig; ray.dir.normalize();

	NxVec3 hit;
	NxU32 vertexId;
	gSelectedSoftBodyVertex = -1;
	NxReal distance = NX_MAX_REAL;

	NxSoftBody **softBodies = gScene->getSoftBodies();
	for (NxU32 i = 0; i < gScene->getNbSoftBodies(); i++) {
		if (softBodies[i]->raycast(ray, hit, vertexId)) {
			if(hit.magnitude() < distance)
			{
				gSelectedSoftBody = softBodies[i];
				gSelectedSoftBodyVertex = vertexId;
				int hitx, hity;
				ViewProject(hit, hitx, hity, gMouseDepth);
				distance = hit.magnitude();
			}
		}
	}

	return distance < NX_MAX_REAL;
}

// ------------------------------------------------------------------------------------
void MoveSoftBody(int x, int y)
{
	if (gSelectedSoftBody && gSelectedSoftBodyVertex >= 0) {
		NxVec3 pos; 
		ViewUnProject(x,y, gMouseDepth, pos);
		gSelectedSoftBody->attachVertexToGlobalPosition(gSelectedSoftBodyVertex, pos);
	}
}


// ------------------------------------------------------------------------------------
void SpecialCallback(int key, int x, int y)
{
	switch (key)
    {
		// Reset PhysX
		case GLUT_KEY_F10: ResetNx(); return;
		// Show help text
		case GLUT_KEY_F1 :
			gHelp = !gHelp;
			break;
		case GLUT_KEY_F3 : 
			gMouseDepth += 0.02f * (1.0f - gMouseDepth); 
			MoveActor(mx,my); MoveSoftBody(mx,my);
			break;
		case GLUT_KEY_F4 : 
			gMouseDepth -= 0.02 * (1.0f - gMouseDepth);  
			if (gMouseDepth < 0.0f) gMouseDepth = 0.0f;
			MoveActor(mx,my); MoveSoftBody(mx,my);
			break;
	}
}


// ------------------------------------------------------------------------------------
void MouseCallback(int button, int state, int x, int y)
{
    mx = x;
	my = y;

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
#if !defined(_XBOX) && !defined (__CELLOS_LV2__) // no picking on consoles
		if (!PickActor(x,y))
			PickSoftBody(x,y);
#endif
	}
	if (state == GLUT_UP) {
		LetGoActor();
		LetGoSoftBody();
	}
}

// ------------------------------------------------------------------------------------
void MotionCallback(int x, int y)
{
    int dx = mx - x;
    int dy = my - y;
    
	if (gMouseJoint)
		MoveActor(x,y);
	else if (gSelectedSoftBodyVertex >= 0) 
		MoveSoftBody(x,y);
	else {
		gCameraForward.normalize();
		gCameraRight.cross(gCameraForward,NxVec3(0,1,0));

		NxQuat qx(NxPiF32 * dx * 20 / 180.0f, NxVec3(0,1,0));
		qx.rotate(gCameraForward);
		NxQuat qy(NxPiF32 * dy * 20 / 180.0f, gCameraRight);
		qy.rotate(gCameraForward);
	}

    mx = x;
    my = y;
}

// ------------------------------------------------------------------------------------
void InitGlut(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitWindowSize(800, 600);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	gMainHandle = glutCreateWindow("Soft Body Sample");
	glutSetWindow(gMainHandle);
    glutDisplayFunc(RenderCallback);
    glutReshapeFunc(ReshapeCallback);
    glutIdleFunc(IdleCallback);
    glutKeyboardFunc(KeyboardCallback);
    glutKeyboardUpFunc(KeyboardUpCallback);
	glutSpecialFunc(SpecialCallback);
    glutMouseFunc(MouseCallback);
    glutMotionFunc(MotionCallback);
    MotionCallback(0,0);

    // Setup default render states
	glClearColor(0.52f, 0.60f, 0.71f, 1.0f);  
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
	glShadeModel(GL_SMOOTH);
#ifdef WIN32
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
#else
	glDisable(GL_CULL_FACE);
#endif

    // Setup lighting
    glEnable(GL_LIGHTING);
    float AmbientColor[]    = { 0.0f, 0.1f, 0.2f, 0.0 };         glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientColor);
    float DiffuseColor[]    = { 0.8f, 0.8f, 0.8f, 0.0 };         glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseColor);
    float SpecularColor[]   = { 0.7f, 0.7f, 0.7f, 0.0 };         glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularColor);
    float Position[]        = { 10.0f, 10.0f, 10.0f, 0.0f };  glLightfv(GL_LIGHT0, GL_POSITION, Position);
    glEnable(GL_LIGHT0);
}

// ------------------------------------------------------------------------------------
void setupD6Desc(NxD6JointDesc &d6Desc, NxActor* a0, NxActor* a1, NxVec3 &globalAnchor, NxVec3 &globalAxis) 
{
	d6Desc.actor[0] = a0;
	d6Desc.actor[1] = a1;

	d6Desc.xMotion = NX_D6JOINT_MOTION_LOCKED;
	d6Desc.yMotion = NX_D6JOINT_MOTION_LOCKED;
	d6Desc.zMotion = NX_D6JOINT_MOTION_LOCKED;

	d6Desc.setGlobalAnchor(globalAnchor);
	d6Desc.setGlobalAxis(globalAxis);

	d6Desc.projectionMode = NX_JPM_NONE;
}

// ------------------------------------------------------------------------------------
void SetupAnimalScene()
{
	SetTitleString("1. Frog Example");
	SetHelpString(NULL);

	// Create the objects in the scene
	gGroundPlane = CreateGroundPlane();
	
	NxSoftBodyDesc softBodyDesc;
	softBodyDesc.globalPose.t = NxVec3(0.0f, 3.0f, 0.0f);
	softBodyDesc.particleRadius = 0.2f;
	softBodyDesc.volumeStiffness = 0.5f;
	softBodyDesc.stretchingStiffness = 1.0f;
	softBodyDesc.friction = 1.0f;
	softBodyDesc.attachmentResponseCoefficient = 0.1f;
	softBodyDesc.solverIterations = 5;

	if (gHardwareSimulation)
		softBodyDesc.flags |= NX_SBF_HARDWARE;

	gScene->setGravity(NxVec3(0.0f, -10.0f, 0.0f));

	const char *fileName = "froggyNormalized";	

	char tetFileName[256], objFileName[256], s[256];
	sprintf(tetFileName, "%s.tet", fileName);
	sprintf(objFileName, "%s.obj", fileName);

	ObjMesh *objMesh = new ObjMesh();
	objMesh->loadFromObjFile(FindMediaFile(objFileName, s));
	gObjMeshes.push_back(objMesh);

	NxMat33 rot; rot.rotX(NxHalfPiF32);
	
	for (int i = 0; i < 10; i++) {
		softBodyDesc.globalPose.t = NxVec3(0,5+i*3,0);
		MySoftBody *softBody = new MySoftBody(gScene, softBodyDesc, FindMediaFile(tetFileName,s), objMesh);
		if (!softBody->getNxSoftBody())
		{
			printf("Error: Unable to create the SoftBody for the current scene.\n");
			delete softBody;
		} else
		{
			gSoftBodies.push_back(softBody);
			
			NxActor *caps = CreateCapsule(NxVec3(0.0f, 5.0f + i*3.0f, -0.3f), 1.0f, 0.73f, 1.0f);
			
			caps->userData = (void*)&gInvisible;
			caps->setGlobalOrientation(rot);
			softBody->getNxSoftBody()->attachToShape(caps->getShapes()[0], NX_SOFTBODY_ATTACHMENT_TWOWAY);
		}
	}

	// set camera position and direction
	gCameraPos.set(-4.0f, 10.0f, 30.0f);
	gCameraForward.set(0.01f, -0.05f, -0.3f);
	gCameraForward.normalize();
}

NxActor* CreateOrientedCapsule(const NxMat34& globalPose, const NxReal height, const NxReal radius, const NxReal density)
{
	// Add a single-shape actor to the scene
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	// The actor has one shape, a capsule
	NxCapsuleShapeDesc capsuleDesc;
	capsuleDesc.height = height;
	capsuleDesc.radius = radius;
	capsuleDesc.localPose.t = NxVec3(0,radius+(NxReal)0.5*height,0);
	actorDesc.shapes.pushBack(&capsuleDesc);

	if (density)
	{
		actorDesc.body = &bodyDesc;
		actorDesc.density = density;
	}
	else
	{
		actorDesc.body = NULL;
	}
	actorDesc.globalPose = globalPose;
	return gScene->createActor(actorDesc);	
}

// ------------------------------------------------------------------------------------
void SetupWheelScene()
{
	SetTitleString("2. Wheel Example");
	SetHelpString(NULL);

	gScene->setGravity(NxVec3(0.0f, -20.0f, 0.0f));

	// Create the objects in the scene
	gGroundPlane = CreateGroundPlane();
	
	NxSoftBodyDesc softBodyDesc;
	softBodyDesc.particleRadius = 0.2f;
	softBodyDesc.density = 1.0f;
	softBodyDesc.volumeStiffness = 1.0f;
	softBodyDesc.stretchingStiffness = 1.0f;
	softBodyDesc.friction = 1.0f;
	softBodyDesc.collisionResponseCoefficient = 0.9f;
	softBodyDesc.attachmentResponseCoefficient = 0.8f;
	softBodyDesc.solverIterations = 5;
	softBodyDesc.flags |= NX_SBF_COLLISION_TWOWAY;

	if (gHardwareSimulation)
		softBodyDesc.flags |= NX_SBF_HARDWARE;

	const char *fileName = "wheelNormalized";	

	char tetFileName[256], objFileName[256], s[256];
	sprintf(tetFileName, "%s.tet", fileName);
	sprintf(objFileName, "%s.obj", fileName);

	MySoftBody *softBody;

	NxReal carHeight = 7.5f;
	NxReal stiffness = 0.9f;
	NxMat34 capsulePose = NxMat34(NxMat33(NX_IDENTITY_MATRIX), NxVec3(-4,carHeight,-5.0f));
	capsulePose.M.rotX(NxHalfPiF32);
	NxActor *caps1 = CreateOrientedCapsule(capsulePose, 7.1f, 1.3f, 1.0f);
	capsulePose.t = NxVec3(4,carHeight,-5.0f);
	NxActor *caps2 = CreateOrientedCapsule(capsulePose, 7.1f, 1.3f, 1.0f);

	ObjMesh *objMesh = new ObjMesh();
	objMesh->loadFromObjFile(FindMediaFile(objFileName, s));

	NxMat33 rot;
	rot.rotX(NxPiF32);
	softBodyDesc.globalPose.t = NxVec3(4.0f, carHeight, 3.4f);
	softBodyDesc.globalPose.M = rot;
	softBodyDesc.stretchingStiffness = stiffness;
	softBody = new MySoftBody(gScene, softBodyDesc, FindMediaFile(tetFileName,s), objMesh);
	if (!softBody->getNxSoftBody())
	{
		printf("Error: Unable to create the SoftBody for the current scene.\n");
		delete softBody;
	} else
	{
		gObjMeshes.push_back(objMesh);

		softBody->getNxSoftBody()->attachToCollidingShapes(NX_SOFTBODY_ATTACHMENT_TWOWAY);
		gSoftBodies.push_back(softBody);

		softBodyDesc.globalPose.t = NxVec3(-4.0f ,carHeight, 3.4f);
		softBodyDesc.globalPose.M = rot;
		softBodyDesc.stretchingStiffness = stiffness;
		softBody = new MySoftBody(gScene, softBodyDesc, FindMediaFile(tetFileName,s), objMesh);
		softBody->getNxSoftBody()->attachToCollidingShapes(NX_SOFTBODY_ATTACHMENT_TWOWAY);
		gSoftBodies.push_back(softBody);

		softBodyDesc.globalPose.t = NxVec3(4.0f, carHeight, -3.4f);
		softBodyDesc.globalPose.M.id();
		softBodyDesc.stretchingStiffness = stiffness;
		softBody = new MySoftBody(gScene, softBodyDesc, FindMediaFile(tetFileName,s), objMesh);
		softBody->getNxSoftBody()->attachToCollidingShapes(NX_SOFTBODY_ATTACHMENT_TWOWAY);
		gSoftBodies.push_back(softBody);

		softBodyDesc.globalPose.t = NxVec3(-4.0f, carHeight, -3.4f);
		softBodyDesc.globalPose.M.id();
		softBodyDesc.stretchingStiffness = stiffness;
		softBody = new MySoftBody(gScene, softBodyDesc, FindMediaFile(tetFileName,s), objMesh);
		softBody->getNxSoftBody()->attachToCollidingShapes(NX_SOFTBODY_ATTACHMENT_TWOWAY);
		gSoftBodies.push_back(softBody);
		NxActor *box = CreateBox(NxVec3(0,carHeight,0), NxVec3(4.6f, 0.5f, 1.0f), 1.0f);
		CreateRevoluteJoint(box, caps1, NxVec3(-4,carHeight,-3.5f), NxVec3(0,0,1));
		CreateRevoluteJoint(box, caps2, NxVec3( 4,carHeight,-3.5f), NxVec3(0,0,1));
		NxActor *ground = CreateBoxOriented(NxVec3(0,1,0), NxQuat(-5.f, NxVec3(0,0,1)), NxVec3(10.0f, 1.0f, 8.0f), 0.0f);
	}

	// set camera position and direction
	gCameraPos.set(-17.0f, 20.0f, 20.0f);
	gCameraForward.set(1.0f, -0.8f, -0.90f);
	gCameraForward.normalize();
	gCameraForward.normalize();
}

// ------------------------------------------------------------------------------------
void SetupPalmScene()
{
	SetTitleString("3. Palm Example");
	SetHelpString("    a wind force was added to this scene");

	// Create the objects in the scene
	gGroundPlane = CreateGroundPlane();
	
	NxSoftBodyDesc softBodyDesc;
	softBodyDesc.globalPose.t = NxVec3(0,3,0);
	softBodyDesc.particleRadius = 0.2f;
	softBodyDesc.volumeStiffness = 0.5f;
	softBodyDesc.stretchingStiffness = 1.0f;
	softBodyDesc.friction = 1.0f;
	softBodyDesc.collisionResponseCoefficient = 0.9f;
	softBodyDesc.solverIterations = 5;
	softBodyDesc.flags |= NX_SBF_COLLISION_TWOWAY;

	if (gHardwareSimulation)
		softBodyDesc.flags |= NX_SBF_HARDWARE;

	gScene->setGravity(NxVec3(0.0f, -1.0f, 0.0f));

	const char *fileName = "palmNormalized";	

	char tetFileName[256], objFileName[256], s[256];
	sprintf(tetFileName, "%s.tet", fileName);
	sprintf(objFileName, "%s.obj", fileName);

	ObjMesh *objMesh = new ObjMesh();
	objMesh->loadFromObjFile(FindMediaFile(objFileName, s));

	MySoftBody *softBody = new MySoftBody(gScene, softBodyDesc, FindMediaFile(tetFileName,s), objMesh);
	if (!softBody->getNxSoftBody())
	{
		printf("Error: Unable to create the SoftBody for the current scene.\n");
		delete softBody;
	} else
	{
		gSoftBodies.push_back(softBody);
		gObjMeshes.push_back(objMesh);

		softBody->getNxSoftBody()->setExternalAcceleration(NxVec3(5,0,0));

		NxActor *caps0 = CreateCompoundCapsule(NxVec3(-0.46f, -0.15f, 0.68f), 1.7f, 0.21f, 2.0f, 0.18f);
		caps0->userData = (void*)&gInvisible;

		CreateRopeSphericalJoint(NULL, caps0, NxVec3(-0.46f, -0.0f, 0.68f), NxVec3(0,0,1));

		softBody->getNxSoftBody()->attachToCollidingShapes(0);
	}

	// set camera position and direction
	gCameraPos.set(0.0f, 7.5f, 9.5f);
	gCameraForward.set(-0.1f, -0.22f, -0.5f);
	gCameraForward.normalize();
}

// ------------------------------------------------------------------------------------
void SetupGrassScene()
{
	SetTitleString("4. Grass Example");
	SetHelpString("    shoot spheres with space");

	// Create the objects in the scene
	gGroundPlane = CreateGroundPlane();
	
	NxSoftBodyDesc softBodyDesc;
	softBodyDesc.globalPose.t = NxVec3(0,1.4,0);
	softBodyDesc.particleRadius = 0.2f;
	softBodyDesc.volumeStiffness = 0.5f;
	softBodyDesc.stretchingStiffness = 0.01f;
	softBodyDesc.friction = 1.0f;
	softBodyDesc.solverIterations = 5;


	if (gHardwareSimulation)
		softBodyDesc.flags |= NX_SBF_HARDWARE;

	// adjust gravity
	gScene->setGravity(NxVec3(0.0f, -1.0f, 0.0f));

	const char *fileName = "grassNormalized";	

	char tetFileName[256], objFileName[256], s[256];
	sprintf(tetFileName, "%s.tet", fileName);
	sprintf(objFileName, "%s.obj", fileName);

	ObjMesh *objMesh = new ObjMesh();
	objMesh->loadFromObjFile(FindMediaFile(objFileName, s));

	MySoftBody *softBody = new MySoftBody(gScene, softBodyDesc, FindMediaFile(tetFileName,s), objMesh);
	if (!softBody->getNxSoftBody())
	{
		printf("Error: Unable to create the SoftBody for the current scene.\n");
		delete softBody;
	} else
	{
		gSoftBodies.push_back(softBody);
		gObjMeshes.push_back(objMesh);

		CreateBox(NxVec3(0,0,0), NxVec3(5,0.2,5), 0);
		softBody->getNxSoftBody()->attachToCollidingShapes(0);

		NxActor *sphere = CreateSphere(NxVec3(0,7,7), 1.0, 1.0);
		sphere->setLinearVelocity(NxVec3(0,-4.0,-6.0));
	}

	// set camera position and direction
	gCameraPos.set(5.0f, 10.0f, 15.0f);
	gCameraForward.set(-0.4f, -0.55f, -0.866f);
	gCameraForward.normalize();
}

// ------------------------------------------------------------------------------------
void SetupRegularScene()
{
	SetTitleString("5. Regular Mesh Example");
	SetHelpString(NULL);

	// Create the objects in the scene
	gGroundPlane = CreateGroundPlane();
	
	NxSoftBodyDesc softBodyDesc;
	softBodyDesc.globalPose.t = NxVec3(0,3,0);
	softBodyDesc.particleRadius = 0.2f;
	softBodyDesc.volumeStiffness = 1.0f;
	softBodyDesc.stretchingStiffness = 0.2f;
	softBodyDesc.friction = 1.0f;
	softBodyDesc.solverIterations = 5;

	if (gHardwareSimulation)
		softBodyDesc.flags |= NX_SBF_HARDWARE;

	gScene->setGravity(NxVec3(0.0f, -10.0f, 0.0f));
	MySoftBody *softBody;

	int i;
	NxReal offset = 0.5f;
	NxVec3 softDim(3.0f, 2.0f, 2.0f);
	NxReal boxH = 1.0f;
	NxReal softRadius = 2.0f;
	NxReal sphereH = 0.2f;

	NxReal hardRadius = softRadius - offset;
	NxVec3 hardDim = softDim * 0.8f; hardDim -= NxVec3(offset);
	NxActor *a;

	for (i = 0; i < 20; i++) {
		NxVec3 pos(0.0f, 5.0f + 5.0f*i, 0.0f);
		softBodyDesc.globalPose.t = pos;
		softBody = new MySoftBody(gScene, softBodyDesc, SOFTBODY_BOX, softDim.x, softDim.y, softDim.z, boxH);
		if (!softBody->getNxSoftBody())
		{
			printf("Error: Unable to create the SoftBody for the current scene.\n");
			delete softBody;
		} else
		{
			gSoftBodies.push_back(softBody);
			a = CreateBox(NxVec3(pos.x, pos.y-hardDim.y, pos.z), hardDim, 0.2f);
			a->userData = (void*)&gInvisible;
			softBody->getNxSoftBody()->attachToCollidingShapes(NX_SOFTBODY_ATTACHMENT_TWOWAY);

			pos.x += 10.0f;
			softBodyDesc.globalPose.t = pos;
			softBody = new MySoftBody(gScene, softBodyDesc, SOFTBODY_SPHERE, softRadius, softRadius, softRadius, sphereH);
			gSoftBodies.push_back(softBody);
			a = CreateSphere(NxVec3(pos.x, pos.y - hardRadius, pos.z), hardRadius, 0.2f);
			a->userData = (void*)&gInvisible;
			softBody->getNxSoftBody()->attachToCollidingShapes(NX_SOFTBODY_ATTACHMENT_TWOWAY);
		}
	}

	// set camera position and direction
	gCameraPos.set(0.0f, 20.0f, 50.0f);
	gCameraForward.set(0,-1,-4);
	gCameraForward.normalize();
}

// ------------------------------------------------------------------------------------
void SetupBunnyScene()
{
	SetTitleString("6. Soft Bunny Example");
	SetHelpString("    adjust stretching stiffness\n    with + and -");

	// Create the objects in the scene
	gGroundPlane = CreateGroundPlane();
	
	NxSoftBodyDesc softBodyDesc;
	softBodyDesc.globalPose.t = NxVec3(0,35,-1);
	softBodyDesc.particleRadius = 0.2f;
	softBodyDesc.volumeStiffness = 1.0f;
	softBodyDesc.stretchingStiffness = 0.07f;
	softBodyDesc.friction = 1.0f;
	softBodyDesc.solverIterations = 5;


	if (gHardwareSimulation)
		softBodyDesc.flags |= NX_SBF_HARDWARE;

	gScene->setGravity(NxVec3(0.0f, -10.0f, 0.0f));

	const char *fileName = "bunnyNormalized";	

	char tetFileName[256], objFileName[256], s[256];
	sprintf(tetFileName, "%s.tet", fileName);
	sprintf(objFileName, "%s.obj", fileName);

	ObjMesh *objMesh = new ObjMesh();
	objMesh->loadFromObjFile(FindMediaFile(objFileName, s));

	MySoftBody *softBody = new MySoftBody(gScene, softBodyDesc, FindMediaFile(tetFileName,s), objMesh);
	if (!softBody->getNxSoftBody())
	{
		printf("Error: Unable to create the SoftBody for the current scene.\n");
		delete softBody;
	} else
	{
		gSoftBodies.push_back(softBody);
		gObjMeshes.push_back(objMesh);
	}

	// rad to degree
	NxF32 fDegree = 1.5f * (360.f / (2.f * NxPiF32));
	NxQuat rotation0(-1.f * fDegree, NxVec3(0,0,1));
	NxActor *plate0 = CreateBoxOriented(NxVec3(-2.5,10,0), rotation0, NxVec3(20.0f, 1.0f, 10.0f), 0.0f);
	NxQuat rotation1(fDegree, NxVec3(0,0,1));
	NxActor *plate1 = CreateBoxOriented(NxVec3(2.5,10,0), rotation1, NxVec3(20.0f, 1.0f, 10.0f), 0.0f);

	// set camera position and direction
	gCameraPos.set(-10.0f, 50.0f, 45.0f);
	gCameraForward.set(0,-1.5,-2);
	gCameraForward.normalize();
}

// ------------------------------------------------------------------------------------
void SetupPumpkinScene()
{
	SetTitleString("7. Soft Pumpkin Example");
	SetHelpString("    2 pumpkins with different\n    stretching stiffness");

	// Create the objects in the scene
	gGroundPlane = CreateGroundPlane();
	
	NxSoftBodyDesc softBodyDesc;
	softBodyDesc.globalPose.t = NxVec3(4.0,35,0);
	softBodyDesc.particleRadius = 0.2f;
	softBodyDesc.volumeStiffness = 1.0f;
	softBodyDesc.stretchingStiffness = 1.0f;
	softBodyDesc.friction = 1.0f;
	softBodyDesc.solverIterations = 5;


	if (gHardwareSimulation)
		softBodyDesc.flags |= NX_SBF_HARDWARE;

	gScene->setGravity(NxVec3(0.0f, -10.0f, 0.0f));

	const char *fileName = "pumpkinNormalized";	

	char tetFileName[256], objFileName[256], s[256];
	sprintf(tetFileName, "%s.tet", fileName);
	sprintf(objFileName, "%s.obj", fileName);

	ObjMesh *objMesh = new ObjMesh();
	objMesh->loadFromObjFile(FindMediaFile(objFileName, s));
	gObjMeshes.push_back(objMesh);

	// first pumpkin
	MySoftBody *softBody = new MySoftBody(gScene, softBodyDesc, FindMediaFile(tetFileName,s), objMesh);
	if (!softBody->getNxSoftBody())
	{
		printf("Error: Unable to create the SoftBody for the current scene.\n");
		delete softBody;
	} else
	{
		gSoftBodies.push_back(softBody);
	}

	// second softer pumpkin
	softBodyDesc.stretchingStiffness = 0.3f;
	softBodyDesc.globalPose.t = NxVec3(-4.0,35,0);
	MySoftBody *softBody1 = new MySoftBody(gScene, softBodyDesc, FindMediaFile(tetFileName,s), objMesh);
	if (!softBody1->getNxSoftBody())
	{
		printf("Error: Unable to create the SoftBody for the current scene.\n");
		delete softBody;
	} else
	{
		gSoftBodies.push_back(softBody1);
	}

	// constuct a stairway
	CreateBox(NxVec3(0.0, 0.0, -8.0), NxVec3(14.0, 16.0, 6.0), 0.0f);
	CreateBox(NxVec3(0.0, 0.0, 0.0), NxVec3(14.0, 14.0, 2.0), 0.0f);
	CreateBox(NxVec3(0.0, 0.0, 4.0), NxVec3(14.0, 12.0, 2.0), 0.0f);
	CreateBox(NxVec3(0.0, 0.0, 8.0), NxVec3(14.0, 10.0, 2.0), 0.0f);
	CreateBox(NxVec3(0.0, 0.0, 12.0), NxVec3(14.0, 8.0, 2.0), 0.0f);
	CreateBox(NxVec3(0.0, 0.0, 16.0), NxVec3(14.0, 6.0, 2.0), 0.0f);
	CreateBox(NxVec3(0.0, 0.0, 20.0), NxVec3(14.0, 4.0, 2.0), 0.0f);
	CreateBox(NxVec3(0.0, 0.0, 24.0), NxVec3(14.0, 2.0, 2.0), 0.0f);

	// set camera position and direction
	gCameraPos.set(-60.0f, 50.0f, 60.0f);
	gCameraForward.set(6,-3.5,-4);
	gCameraForward.normalize();
}

// ------------------------------------------------------------------------------------
bool InitNx()
{
	if (!gAllocator)
		gAllocator = new UserAllocator;

	// Initialize PhysicsSDK
	NxPhysicsSDKDesc desc;
	desc.flags &= ~NX_SDKF_NO_HARDWARE;
	desc.gpuHeapSize = 8;
	desc.meshCacheSize = 0;
	NxSDKCreateError errorCode = NXCE_NO_ERROR;
	gPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, gAllocator, NULL, desc, &errorCode);
	if(gPhysicsSDK == NULL) 
	{
		printf("\nSDK create error (%d - %s).\nUnable to initialize the PhysX SDK, exiting the sample.\n\n", errorCode, getNxSDKCreateError(errorCode));
		return false;
	}

#if !defined(NX_DISABLE_HARDWARE) && !defined(_XBOX) && !defined(__CELLOS_LV2__) && !defined(__PPCGEKKO__)
	NxHWVersion hwCheck = gPhysicsSDK->getHWVersion();
	if (hwCheck == NX_HW_VERSION_NONE) 
	{
		printf("\nWarning: Unable to find a PhysX card, or PhysX card used by other application.");
		printf("\nThe soft bodies will be simulated in software.\n\n");
		gHardwareSimulation = false;
	}
#endif
#ifdef __PPCGEKKO__
		gHardwareSimulation = false;
#endif
#if SAMPLES_USE_VRD
	// The settings for the VRD host and port are found in SampleCommonCode/SamplesVRDSettings.h
	if (gPhysicsSDK->getFoundationSDK().getRemoteDebugger() && !gPhysicsSDK->getFoundationSDK().getRemoteDebugger()->isConnected())
		gPhysicsSDK->getFoundationSDK().getRemoteDebugger()->connect(SAMPLES_VRD_HOST, SAMPLES_VRD_PORT, SAMPLES_VRD_EVENTMASK);
#endif

	if (!gCookingInitialized)
	{
		gCookingInitialized = true;
		if (!InitCooking(gAllocator)) 
		{
			printf("\nError: Unable to initialize the cooking library, exiting the sample.\n\n");
			return false;
		}
	}
	// Set the physics parameters
	gPhysicsSDK->setParameter(NX_SKIN_WIDTH, -0.01f);

	// Set the debug visualization parameters
	gPhysicsSDK->setParameter(NX_VISUALIZATION_SCALE, 1);
	gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1);
	gPhysicsSDK->setParameter(NX_VISUALIZE_JOINT_LIMITS, 1);
	gPhysicsSDK->setParameter(NX_VISUALIZE_JOINT_LOCAL_AXES, 1);


    // Create the scene
    NxSceneDesc sceneDesc;
    sceneDesc.gravity               = gDefaultGravity;
    gScene = gPhysicsSDK->createScene(sceneDesc);

	// Create the default material
	NxMaterialDesc       m; 
	m.restitution        = 0.5f;
	m.staticFriction     = 0.2f;
	m.dynamicFriction    = 0.2f;
	NxMaterial * mat = gScene->getMaterialFromIndex(0);
	mat->loadFromDesc(m); 

	switch(gSceneNr) {
		case 1 : SetupAnimalScene(); break;
		case 2 : SetupWheelScene(); break;
		case 3 : SetupPalmScene(); break;
		case 4 : SetupGrassScene(); break;
		case 5 : SetupRegularScene(); break;
		case 6 : SetupBunnyScene(); break;
		case 7 : SetupPumpkinScene(); break;

		// Insert new cases here
	}

	if (gSoftBodies.size() > 0)
		gSelectedSoftBody = gSoftBodies[0]->getNxSoftBody();
	else
		gSelectedSoftBody = NULL;

	if (gScene->getNbActors() > 0)
		gSelectedActor = *gScene->getActors();
	else
		gSelectedActor = NULL;

	UpdateTime();

#if defined(_XBOX) || defined(__CELLOS_LV2__)
	glutRemapButtonExt(1, 'b', false);//Bending
	glutRemapButtonExt(1, 'o', true);//Orthogonal bending
	glutRemapButtonExt(2, 'G', false);//Gravity
	glutRemapButtonExt(2, 't', true);//Tear lines
	glutRemapButtonExt(3, 'x', false);//Shadows
	glutRemapButtonExt(3, 'n', true);//Debug wireframe
#endif
#if defined(__PPCGEKKO__)
	glutRemapButtonExt(GLUT_KEY_UP, 'w');
	glutRemapButtonExt(GLUT_KEY_DOWN, 's');
	glutRemapButtonExt(GLUT_KEY_LEFT, 'a');
	glutRemapButtonExt(GLUT_KEY_RIGHT, 'd');
	glutRemapButtonExt(GLUT_KEY_HOME, GLUT_KEY_F1);
	glutRemapButtonExt('b', ' ');
	glutRemapButtonExt('1', 't');
	glutRemapButtonExt('2', 'r');
	glutRemapButtonExt('a', 'e');
#endif
	return true;
}


// ------------------------------------------------------------------------------------
void ReleaseNx()
{
	for (MySoftBody **softBody = gSoftBodies.begin(); softBody != gSoftBodies.end(); softBody++)
		delete *softBody;
	gSoftBodies.clear();

	for (ObjMesh **objMesh = gObjMeshes.begin(); objMesh != gObjMeshes.end(); objMesh++)
		delete *objMesh;
	gObjMeshes.clear();

	if (gScene) gPhysicsSDK->releaseScene(*gScene);
	if (gCookingInitialized) CloseCooking();
	gCookingInitialized = false;
	if (gPhysicsSDK)  gPhysicsSDK->release();

	if (gAllocator)
	{
		delete gAllocator;
		gAllocator = NULL;
	}
}

// ------------------------------------------------------------------------------------
void ResetNx()
{
	LetGoActor();
	LetGoSoftBody();
	ReleaseNx();
	if (!InitNx()) exit(0);
}

// ------------------------------------------------------------------------------------
NxReal UpdateTime()
{
    NxReal deltaTime;
    gTime = getTime()*0.001f;  // Get current time in seconds
    deltaTime = gTime - gLastTime;
    gLastTime = gTime;
    return deltaTime;
}

// ------------------------------------------------------------------------------------
void RunPhysics()
{
	// Update the time step
	NxReal deltaTime = UpdateTime();

	//deltaTime = 0.02f;	// todo: may go back to real time at some point
deltaTime = 1.0/60.0;

	// Run collision and dynamics for delta time since the last frame

	gScene->simulate(deltaTime);	
	gScene->flushStream();
	gScene->fetchResults(NX_RIGID_BODY_FINISHED, true);

	// adding a wind force to scene 3 (palm scene)
	if (gSceneNr == 3) {
		NxSoftBody **softbodies = gScene->getSoftBodies();
		NxReal range = 2.5f;
		softbodies[0]->setExternalAcceleration(NxVec3(3.4f + NxMath::rand(-range, range), NxMath::rand(-range, range), NxMath::rand(-range, range)));
	}

	gFrameCounter++;
}

// ------------------------------------------------------------------------------------
int main(int argc, char** argv)
{
	PrintControls();
    InitGlut(argc, argv);

	// Initialize physics scene and start the application main loop if scene was created
	if (InitNx())
	{
		atexit(ReleaseNx);
		glutMainLoop();
	}

	return 0;
}

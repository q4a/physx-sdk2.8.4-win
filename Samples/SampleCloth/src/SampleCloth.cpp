// ===============================================================================
//
//						   NVIDIA PhysX SDK Sample Program
//
// Title: Cloth Sample
// Description: This sample shows an example of how to use the cloth API.
//
// Originally written by: Matthias Müller-Fischer (03-08-05)
//
// ===============================================================================
#ifndef __PPCGEKKO__
#include "SampleCloth.h"
#include "MyCloth.h"
#include "Timing.h"
#include "GLFontRenderer.h"
#include "Utilities.h"
#include "SamplesVRDSettings.h"
#include "cooking.h"
#include "ActorPicking.h"
#include "ErrorStream.h"
#include "UserAllocator.h"

#define CLOTH_GRID_STEP_CURTAIN		0.15f
#define CLOTH_GRID_STEP_FLAG		0.15f
#define CLOTH_GRID_STEP_DAMPING		0.2f
#define CLOTH_GRID_STEP_TEARING		0.1f
#define CLOTH_GRID_STEP_ATTACHMENT	0.4f

//
//	Values for gSceneNr
//
//	1 : Curtain - SetupCurtainScene
//	2 : Flag SetupFlagScene
//	3 : Damping SetupDampingScene
//	4 : Pipe SetupPipeScene
//	5 : Attachment SetupAttachmentScene
//	6 : Tearing SetupTearingScene
//	7 : Pressure SetupPressureScene
//	8 : RemapMesh SetupRemapMeshScene
//
int gSceneNr = 1; // default start scene

DebugRenderer gDebugRenderer;

// Physics SDK globals
NxPhysicsSDK*     gPhysicsSDK = NULL;
NxScene*          gScene = NULL;
NxVec3            gDefaultGravity(0,-9.8,0);
UserAllocator*    gAllocator = NULL;
#if defined(NX_DISABLE_HARDWARE) || defined(_XBOX) || defined(__CELLOS_LV2__)
bool              gHardwareSimulation = false;
#else
bool              gHardwareSimulation = true;
#endif
bool              gTearLines = false;
bool              gCookingInitialized = false;

ErrorStream       gErrorStream;

// Time globals
NxReal gTime = 0.0f;
 
// Display globals
int  gMainHandle;
char gDisplayString[512] = "";

// Camera globals
NxReal gCameraFov = 40.0f;
NxVec3 gCameraPos(0,5,15);
NxVec3 gCameraForward(0,0,-1);
NxVec3 gCameraRight(1,0,0);
const NxReal gCameraSpeed = 0.2;
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
bool bLeftMouseButtonPressed = false;

// Scene
NxArray<MyCloth *> gCloths;

NxActor *gSelectedActor = NULL;
NxActor *gGroundPlane = NULL;
NxCloth *gSelectedCloth = NULL;
int		 gSelectedClothVertex = -1;
// fps
int      gFrameCounter = 0;
float    gPreviousTime = getCurrentTime();
//Stopwatch	gStopWatch;

// ------------------------------------------------------------------------------------
void PrintControls()
{
	printf("\n Flight Controls:\n ----------------\n w = forward, s = back\n a = strafe left, d = strafe right\n q = up, e = down\n");
	printf("\n Miscellaneous:\n --------------\n p = Pause\n m = Single Step\n x = Toggle Shadows\n");
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
	float y = 0.95f;	

	int len = (int)strlen(gDisplayString);
	len = (len < 256)?len:255;
	int start = 0;
	char textBuffer[256];
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
void RefreshDisplayString(const char *demoName, bool bTearDemo=false)
{
#ifdef __CELLOS_LV2__
	if(bTearDemo)
		sprintf(gDisplayString, "%s (%s/%s)\nUse 1-9 to choose scene\nUse 'b' for bending, 't' for tear lines\nPress space to shoot", 
			demoName, gHardwareSimulation ? "hardware" : "software", gTearLines ? "tear lines" : "full tear");
	else
		sprintf(gDisplayString, "%s (%s)\nUse 1-9 to choose scene\nUse 'b' for bending\nPress space to shoot", 
			demoName, gHardwareSimulation ? "hardware" : "software");
#elif NX_DISABLE_HARDWARE
	if(bTearDemo)
		sprintf(gDisplayString, "%s (%s/%s)\nUse 1-9 to choose scene\nUse right mouse button to drag objects\nUse 'b' for bending, 't' for tear lines\nPress space to shoot", 
			demoName, gHardwareSimulation ? "hardware" : "software", gTearLines ? "tear lines" : "full tear");
	else
		sprintf(gDisplayString, "%s (%s)\nUse 1-9 to choose scene\nUse right mouse button to drag objects\nUse 'b' for bending\nPress space to shoot", 
			demoName, gHardwareSimulation ? "hardware" : "software");	
#else
	if(bTearDemo)
	{
		sprintf(gDisplayString, "%s (%s/%s)\nUse 1-9 to choose scene\nUse right mouse button to drag objects\nUse 'h' for hardware on/off, 'b' for bending, 't' for tear lines\nPress space to shoot", 
			demoName, gHardwareSimulation ? "hardware" : "software", gTearLines ? "tear lines" : "full tear");
	}
	else
	{
		sprintf(gDisplayString, "%s (%s)\nUse 1-9 to choose scene\nUse right mouse button to drag objects\nUse 'h' for hardware on/off, 'b' for bending\nPress space to shoot", 
			demoName, gHardwareSimulation ? "hardware" : "software");
	}
#endif
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
		}
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

// ------------------------------------------------------------------------------------
void RenderScene(bool shadows)
{
	// Render all the actors in the scene
	int nbActors = gScene->getNbActors();
	NxActor** actors = gScene->getActors();
	while (nbActors--)
	{
		NxActor* actor = *actors++;
		
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
	// Render all the cloths in the scene
	for (MyCloth **cloth = gCloths.begin(); cloth != gCloths.end(); cloth++){
		glColor4f(1.0f, 0.0f, 0.0f,1.0f);
		(*cloth)->draw(shadows);
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
	
	gDebugRenderer.renderData(*(gScene->getDebugRenderable()));

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

	//gStopWatch.Stop();
	float time = getCurrentTime();
	float elapsedTime = time - gPreviousTime;
	
	if (elapsedTime > 1.0f) {
		char title[30];
		sprintf(title, "SampleCloth %3.1f fps", (float)gFrameCounter / elapsedTime);
		glutSetWindowTitle(title);
		gPreviousTime = time;
		gFrameCounter = 0;
		//gStopWatch.Reset();
	}

	getElapsedTime();
	//gStopWatch.Start();

}

// ------------------------------------------------------------------------------------
void KeyboardCallback(unsigned char key, int x, int y)
{
	gKeys[key] = true;

	if ('1' <= key && key <= '9') {
		gSceneNr = key - '0';
		ResetNx();
	}

	if (key == '0') {
		if(++gSceneNr == 10) gSceneNr = 1;
		ResetNx();
	}

	switch (key)
	{
	case 'o': {     
		NxCloth **cloths = gScene->getCloths();
		for (NxU32 i = 0; i < gScene->getNbCloths(); i++) {
			cloths[i]->setFlags(cloths[i]->getFlags() ^ NX_CLF_BENDING_ORTHO);
		}
		break;
			  }

	case 'G':
	case 'g': {     
		NxCloth **cloths = gScene->getCloths();
		for (NxU32 i = 0; i < gScene->getNbCloths(); i++) {
			cloths[i]->setFlags(cloths[i]->getFlags() ^ NX_CLF_GRAVITY);
		}
		break;
			  }

	case 'p': { bPause = !bPause; break; }
	case 'm': { RunPhysics(); glutPostRedisplay(); break; }
	case 'x': { bShadows = !bShadows; break; }
	case 'n': { bWireframeMode = !bWireframeMode; break; }		
	case 'b': {
		NxCloth **cloths = gScene->getCloths();
		for (NxU32 i = 0; i < gScene->getNbCloths(); i++) {
			cloths[i]->setFlags(cloths[i]->getFlags() ^ NX_CLF_BENDING);
		}
		break;
			  }
	case ' ': {
		{
		NxActor *sphere = CreateSphere(gCameraPos, 1.0f, 1.0f);
		sphere->setLinearVelocity(gCameraForward * 20.0f);
		break; 
		}
		 
			  }
#if !defined(NX_DISABLE_HARDWARE) && !defined(_XBOX) && !defined(__CELLOS_LV2__)
	case 'h' : {
		gHardwareSimulation = !gHardwareSimulation;
		ResetNx();
		break;
			   }
#endif
	case 't' : {		
		gTearLines = !gTearLines;	
		ResetNx();
		break;
			   }
	case 'v' : {
		toggleVSync();
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
void LetGoCloth()
{
	if (gSelectedCloth && gSelectedClothVertex >= 0)
		gSelectedCloth->freeVertex(gSelectedClothVertex);
	gSelectedClothVertex = -1;
}


//-------------------------------------------------------------------------------------
bool Pick(int x, int y)
{
	//LetGoActor();

	NxRay ray; 
	ViewUnProject(x,y,0.0f, ray.orig);
	ViewUnProject(x,y,1.0f, ray.dir);
	ray.dir -= ray.orig; ray.dir.normalize();

	NxRaycastHit hit;
	NxShape* closestShape = gScene->raycastClosestShape(ray, NX_ALL_SHAPES, hit);

	if (!closestShape) return true;
	if (!closestShape->getActor().isDynamic()) return true;

	NxVec3 origin = ray.orig;
	NxVec3 impact = hit.worldImpact;
	float distRB = (impact.x - origin.x) * (impact.x - origin.x) + (impact.y - origin.y) * (impact.y - origin.y) + (impact.z - origin.z) * (impact.z - origin.z);


	NxVec3 hitcloth;
	NxU32 vertexId;
	gSelectedClothVertex = -1;
	float distCloth, closest= 1e9;

	NxCloth **cloths = gScene->getCloths();
	for (NxU32 i = 0; i < gScene->getNbCloths(); i++) 
	{
		if (cloths[i]->raycast(ray, hitcloth, vertexId))
		{
			distCloth = (hitcloth.x - origin.x) * (hitcloth.x - origin.x) + (hitcloth.y - origin.y) * (hitcloth.y - origin.y) + (hitcloth.z - origin.z) * (hitcloth.z - origin.z);
			if(distCloth < closest)	closest = distCloth;
		}
	}
	distCloth = closest;
	
	return (distCloth < distRB);
}


// ------------------------------------------------------------------------------------
bool PickCloth(int x, int y)
{
	NxRay ray; 
	ViewUnProject(x,y,0.0f, ray.orig);
	ViewUnProject(x,y,1.0f, ray.dir);
	ray.dir -= ray.orig; ray.dir.normalize();

	NxVec3 hit;
	NxU32 vertexId;
	gSelectedClothVertex = -1;
	NxReal distance = NX_MAX_REAL;

	NxCloth **cloths = gScene->getCloths();
	for (NxU32 i = 0; i < gScene->getNbCloths(); i++) {
		if (cloths[i]->raycast(ray, hit, vertexId)) {
			if(hit.magnitude() < distance)
			{
#if 1
				gSelectedCloth = cloths[i];
				gSelectedClothVertex = vertexId;
				int hitx, hity;
				ViewProject(hit, hitx, hity, gMouseDepth);
				distance = hit.magnitude();
#else
				cloths[i]->wakeUp();
				cloths[i]->tearVertex(vertexId, NxVec3(0,1,0));
				cloths[i]->tearVertex(vertexId, NxVec3(1,0,0));
#endif
			}
		}
	}

	return distance < NX_MAX_REAL;
}

// ------------------------------------------------------------------------------------
void MoveCloth(int x, int y)
{
	if (gSelectedCloth && gSelectedClothVertex >= 0) {
		NxVec3 pos; 
		ViewUnProject(x,y, gMouseDepth, pos);
		gSelectedCloth->attachVertexToGlobalPosition(gSelectedClothVertex, pos);
	}
}


// ------------------------------------------------------------------------------------
void SpecialCallback(int key, int x, int y)
{
	switch (key)
	{
		// Reset PhysX
		case GLUT_KEY_F10: ResetNx(); return; 
		case GLUT_KEY_F1 : 
			gMouseDepth += 0.02f * (1.0f - gMouseDepth); 
			MoveActor(mx,my); MoveCloth(mx,my);
			break;
		case GLUT_KEY_F2 : 
			gMouseDepth -= 0.02f * (1.0f - gMouseDepth);  
			if (gMouseDepth < 0.0f) gMouseDepth = 0.0f;
			MoveActor(mx,my); MoveCloth(mx,my);
			break;
	}
}


// ------------------------------------------------------------------------------------
void MouseCallback(int button, int state, int x, int y)
{
	mx = x;
	my = y;

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
#if !defined(_XBOX) && !defined (__CELLOS_LV2__) // no picking on consoles
		//if (!PickActor(x,y))
		//{
		//	PickCloth(x,y);

			// this prevents from only grabbing it when the mouse moves
		//	MotionCallback(x,y);
		//}

		bool bSelectedCloth = Pick(x, y);
		
		if(bSelectedCloth)
		{
			PickCloth(x,y);

			// this prevents from only grabbing it when the mouse moves
			MotionCallback(x,y);
		}
		else
		{
			PickActor(x,y);
		}
#endif
	}
	else if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		bLeftMouseButtonPressed = true;
	}
	else if (state == GLUT_UP)
	{
		LetGoActor();
		LetGoCloth();
		bLeftMouseButtonPressed = false;
	}
}

// ------------------------------------------------------------------------------------
void MotionCallback(int x, int y)
{
	int dx = mx - x;
	int dy = my - y;

	if (gMouseJoint)
	{
		MoveActor(x,y);
	}
	else if (gSelectedClothVertex >= 0) 
	{
		MoveCloth(x,y);
	}
	else if (bLeftMouseButtonPressed)
	{
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
void ExitCallback()
{
	ReleaseNx();

	if (gAllocator)
	{
		delete gAllocator;
		gAllocator = NULL;
	}
}

// ------------------------------------------------------------------------------------
void InitGlut(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(800, 600);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	gMainHandle = glutCreateWindow("Cloth Sample");
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
	//glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);

#ifdef WIN32
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
#endif

    // Setup lighting
    glEnable(GL_LIGHTING);
    float AmbientColor[]    = { 0.0f, 0.1f, 0.2f, 0.0f };         glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientColor);
    float DiffuseColor[]    = { 0.8f, 0.8f, 0.8f, 0.0f };         glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseColor);
    float SpecularColor[]   = { 0.7f, 0.7f, 0.7f, 0.0f };         glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularColor);
//    float Position[]        = { 100.0f, 100.0f, -400.0f, 1.0f };  glLightfv(GL_LIGHT0, GL_POSITION, Position);
    float Position[]        = { 10.0f, 10.0f, 10.0f, 0.0f };  glLightfv(GL_LIGHT0, GL_POSITION, Position);
    glEnable(GL_LIGHT0);
}

// ------------------------------------------------------------------------------------
void SetupCurtainScene(NxU32 dim)
{
	RefreshDisplayString("Cloth and Shape Interaction Sample");

	gGroundPlane = CreateGroundPlane();

	// generate a dim x dim grid of curtains for stress testing
	NxU32 xx, zz;
	for (xx = 0; xx < dim; xx++) 
	{
		for (zz = 0; zz < dim; zz++) 
		{
			NxVec3 offset(xx * 12.0f, 0.0f, zz * 12.0f);

			// Create the objects in the scene
			NxActor *sphere1 = CreateSphere(NxVec3(-1,0,-0.5) + offset, 1, 10);
			NxActor *box1 = CreateBox(NxVec3(1,0,-1) + offset, NxVec3(1,1,1), 10);
			NxActor *box2 = CreateBox(NxVec3(1,6.5,0) + offset, NxVec3(5,0.5,0.5), 10); 
			box2->setLinearDamping(5.0);

			NxD6JointDesc d6Desc;
			d6Desc.actor[0] = NULL;
			d6Desc.actor[1] = box2;
			NxVec3 globalAnchor(0,7,0);
			globalAnchor += offset;
			d6Desc.localAnchor[0] = globalAnchor;
			box2->getGlobalPose().multiplyByInverseRT(globalAnchor, d6Desc.localAnchor[1]);

			d6Desc.localAxis[0]   = NxVec3(1,0,0);
			d6Desc.localNormal[0] = NxVec3(0,1,0);
			d6Desc.localAxis[1]   = NxVec3(1,0,0);
			d6Desc.localNormal[1] = NxVec3(0,1,0);

			d6Desc.twistMotion = NX_D6JOINT_MOTION_LOCKED;
			d6Desc.swing1Motion = NX_D6JOINT_MOTION_LOCKED;
			d6Desc.swing2Motion = NX_D6JOINT_MOTION_LOCKED;
			d6Desc.xMotion = NX_D6JOINT_MOTION_FREE;
			d6Desc.yMotion = NX_D6JOINT_MOTION_LOCKED;
			d6Desc.zMotion = NX_D6JOINT_MOTION_LOCKED;

			NxJoint* d6Joint = gScene->createJoint(d6Desc);

			NxClothDesc clothDesc;
			clothDesc.globalPose.t = NxVec3(-3,7,0) + offset;
			clothDesc.thickness = 0.2f;
			//clothDesc.density = 1.0f;
			clothDesc.bendingStiffness = 1.0f;
			clothDesc.stretchingStiffness = 1.0f;
			//clothDesc.dampingCoefficient = 0.50f;
			clothDesc.friction = 0.5f;
			//clothDesc.collisionResponseCoefficient = 1.0f;
			//clothDesc.attachmentResponseCoefficient = 1.0f;
			//clothDesc.solverIterations = 5;
			//clothDesc.flags |= NX_CLF_STATIC;
			//clothDesc.flags |= NX_CLF_DISABLE_COLLISION;
			//clothDesc.flags |= NX_CLF_VISUALIZATION;
			//clothDesc.flags |= NX_CLF_GRAVITY;
			clothDesc.flags |= NX_CLF_BENDING;
			//clothDesc.flags |= NX_CLF_BENDING_ORTHO;
			//clothDesc.flags |= NX_CLF_DAMPING;
			//clothDesc.flags |= NX_CLF_COMDAMPING;
			clothDesc.flags |= NX_CLF_COLLISION_TWOWAY;

			if (gHardwareSimulation)
				clothDesc.flags |= NX_CLF_HARDWARE;

			MyCloth *regularCloth = new MyCloth(gScene, clothDesc, 8.0f, 7.0f, CLOTH_GRID_STEP_CURTAIN, "ageia.bmp");

			if (!regularCloth->getNxCloth())
			{
				printf("Error: Unable to create the cloth for the current scene.\n");
				delete regularCloth;
			} else
			{
				gCloths.push_back(regularCloth);
				regularCloth->getNxCloth()->attachToShape(*box2->getShapes(), NX_CLOTH_ATTACHMENT_TWOWAY);
			}
		}
	}

	gCameraPos.set(1.43f,10.96f,17.9f + 20.0 * (dim-1));
	gCameraForward.set(0.0f,-0.29f,-0.96f);
	gCameraForward.normalize();
}

// ------------------------------------------------------------------------------------
void SetupFlagScene()
{
	RefreshDisplayString("External acceleration demo");

	// Create the objects in the scene
	gGroundPlane = CreateGroundPlane();

	NxActor *capsule = CreateCapsule(NxVec3(0.0,-0.2,0.0),10.0f, 0.15f, 0.0f); 

	NxClothDesc clothDesc;
	clothDesc.globalPose.t = NxVec3(0.0f,10.0f,0.0f);
	clothDesc.globalPose.M.rotX(NxHalfPiF32); 
	clothDesc.thickness = 0.2f;
	//clothDesc.density = 1.0f;
	//clothDesc.bendingStiffness = 1.0f;
	//clothDesc.stretchingStiffness = 1.0f;
	//clothDesc.dampingCoefficient = 0.50f;
	clothDesc.friction = 0.5f;
	//clothDesc.collisionResponseCoefficient = 1.0f;
	//clothDesc.attachmentResponseCoefficient = 1.0f;
	//clothDesc.solverIterations = 5;
	//clothDesc.flags |= NX_CLF_STATIC;
	//clothDesc.flags |= NX_CLF_DISABLE_COLLISION;
	//clothDesc.flags |= NX_CLF_VISUALIZATION;
//	clothDesc.flags &= ~NX_CLF_GRAVITY;
	clothDesc.flags |= NX_CLF_BENDING;
	//clothDesc.flags |= NX_CLF_BENDING_ORTHO;
	//clothDesc.flags |= NX_CLF_DAMPING;
	//clothDesc.flags |= NX_CLF_COMDAMPING;
	clothDesc.flags |= NX_CLF_COLLISION_TWOWAY;
	clothDesc.flags |= NX_CLF_SELFCOLLISION;


	if (gHardwareSimulation)
		clothDesc.flags |= NX_CLF_HARDWARE;
	
	MyCloth *regularCloth = new MyCloth(gScene, clothDesc, 4.5f, 4.5f, CLOTH_GRID_STEP_FLAG, "ageia.bmp");

	if (!regularCloth->getNxCloth())
	{
		printf("Error: Unable to create the cloth for the current scene.\n");
		delete regularCloth;
	} else
	{
		gCloths.push_back(regularCloth);
		regularCloth->getNxCloth()->attachToShape(*capsule->getShapes(), 0);
	}

	gCameraPos.set(1.43f,10.96f,17.9f);
	gCameraForward.set(0.0f,-0.29f,-0.96f);
	gCameraForward.normalize();
}

// ------------------------------------------------------------------------------------
void SetupDampingScene()
{
	RefreshDisplayString("Damping Demo: Undamped, normally damped, COM damped");

	// Create objects in scene
	gGroundPlane = CreateGroundPlane();
	
	NxActor *box1 = CreateBox(NxVec3(6,6.5,0), NxVec3(12,0.5,0.5), 0); 
	NxActor *box2 = CreateBox(NxVec3(-2,0,1), NxVec3(1,1,1), 10);
	NxActor *box3 = CreateBox(NxVec3(6,0,1), NxVec3(1,1,1), 10);
	NxActor *box4 = CreateBox(NxVec3(14,0,1), NxVec3(1,1,1), 10);

	// First Cloth

	NxClothDesc clothDesc;
	clothDesc.globalPose.t = NxVec3(-6,7,0);
	clothDesc.thickness = 0.2f;
	//clothDesc.density = 1.0f;
	//clothDesc.bendingStiffness = 1.0f;
	//clothDesc.stretchingStiffness = 1.0f;
	//clothDesc.dampingCoefficient = 0.50f;
	clothDesc.friction = 0.5f;
	//clothDesc.collisionResponseCoefficient = 1.0f;
	//clothDesc.attachmentResponseCoefficient = 1.0f;
	//clothDesc.solverIterations = 5;
	//clothDesc.flags |= NX_CLF_STATIC;
	//clothDesc.flags |= NX_CLF_DISABLE_COLLISION;
	//clothDesc.flags |= NX_CLF_VISUALIZATION;
	//clothDesc.flags |= NX_CLF_GRAVITY;
	clothDesc.flags |= NX_CLF_BENDING;
	//clothDesc.flags |= NX_CLF_BENDING_ORTHO;
	//clothDesc.flags |= NX_CLF_DAMPING;
	//clothDesc.flags |= NX_CLF_COMDAMPING;
	clothDesc.flags |= NX_CLF_COLLISION_TWOWAY;

	if (gHardwareSimulation)
		clothDesc.flags |= NX_CLF_HARDWARE;

	MyCloth *regularCloth1 = new MyCloth(gScene, clothDesc, 8.0f, 7.0f, CLOTH_GRID_STEP_DAMPING, "ageia.bmp");
	
	if (!regularCloth1->getNxCloth())
	{
		printf("Error: Unable to create the cloth for the current scene.\n");
		delete regularCloth1;
	} else
	{
		gCloths.push_back(regularCloth1);
		regularCloth1->getNxCloth()->attachToShape(*box1->getShapes(), NX_CLOTH_ATTACHMENT_TWOWAY);
	}

	// Second cloth
	clothDesc.globalPose.t = NxVec3(2,7,0);
	clothDesc.flags |= NX_CLF_DAMPING;
	clothDesc.dampingCoefficient = 1.0f;

	MyCloth *regularCloth2 = new MyCloth(gScene, clothDesc, 8.0f, 7.0f, CLOTH_GRID_STEP_DAMPING, "ageia.bmp");
	if (!regularCloth2->getNxCloth())
	{
		printf("Error: Unable to create the cloth for the current scene.\n");
		delete regularCloth2;
	} else
	{
		gCloths.push_back(regularCloth2);
		regularCloth2->getNxCloth()->attachToShape(*box1->getShapes(), NX_CLOTH_ATTACHMENT_TWOWAY);
	}

	// Third cloth
	clothDesc.globalPose.t = NxVec3(10,7,0);
	clothDesc.flags |= NX_CLF_DAMPING;
	clothDesc.flags |= NX_CLF_COMDAMPING;
	clothDesc.dampingCoefficient = 1.0f;

	MyCloth *regularCloth3 = new MyCloth(gScene, clothDesc, 8.0f, 7.0f, CLOTH_GRID_STEP_DAMPING, "ageia.bmp");
	if (!regularCloth3->getNxCloth())
	{
		printf("Error: Unable to create the cloth for the current scene.\n");
		delete regularCloth3;
	} else
	{
		gCloths.push_back(regularCloth3);
		regularCloth3->getNxCloth()->attachToShape(*box1->getShapes(), NX_CLOTH_ATTACHMENT_TWOWAY);
	}

	gCameraPos.set(6.0f,5.8f,26.0f);
	gCameraForward.set(0.0f,0.0f,-1.0f);
	gCameraForward.normalize();
}

// ------------------------------------------------------------------------------------

void SetupPipeScene()
{
	RefreshDisplayString("Two Way Body Cloth Interaction Demo");

	// Create objects in scene
	gGroundPlane = CreateGroundPlane();
	NxActor *box1 = CreateBox(NxVec3(0,10.5,-1), NxVec3(1.0f,1.0f,0.1f), 0.0f);
	NxActor *box2 = CreateBox(NxVec3(0,13,1.5), NxVec3(2.0f,2.0f,0.5f), 0.0f);
	NxQuat q;
	q.fromAngleAxis(20.0f,NxVec3(1.0f,0.0f,0.0f));
	NxActor *box3 = CreateBoxOriented(NxVec3(0,15.4,-1), q, NxVec3(1.0f,0.1f,1.0f), 0.0f);
	q.fromAngleAxis(20.0f,NxVec3(1.0f,0.0f,0.0f));
	NxActor *box4 = CreateBoxOriented(NxVec3(0,19,-11), q, NxVec3(1.0f,0.1f,1.0f), 0.0f);
	q.fromAngleAxis(20.0f,NxVec3(1.0f,0.0f,0.0f));
	NxActor *box5 = CreateBoxOriented(NxVec3(0,17.5,-13), q, NxVec3(3.0f,0.1f,2.0f), 0.0f);

	NxActor *box6 = CreateBox(NxVec3(0,20,-12), NxVec3(1.0f,5.0f,0.1f), 0.0f);
	NxActor *box7 = CreateBox(NxVec3(0,18.5,-14), NxVec3(2.0f,5.0f,0.1f), 0.0f);
	NxActor *box8 = CreateBox(NxVec3(0,10.5,1.0), NxVec3(1.0f,1.0f,0.1f), 0.0f);

#ifdef __PPCGEKKO__
	for (NxU32 i = 0; i < 12; i++)
#else
	for (NxU32 i = 0; i < 16; i++)
#endif	
		NxActor *sphere = CreateSphere( NxVec3(0,22.0f+4.0f*i,-13),0.7,1.0f);

	NxClothDesc clothDesc;
	clothDesc.globalPose.t = NxVec3(0,18.2,-11);
	clothDesc.globalPose.M.rotX(0.35f);
	clothDesc.thickness = 0.4f;	
	clothDesc.density = 0.05f;
	clothDesc.bendingStiffness = 1.0f;
	clothDesc.stretchingStiffness = 1.0f;
	clothDesc.dampingCoefficient = 0.10f;
	clothDesc.friction = 0.3f;
	clothDesc.collisionResponseCoefficient = 0.4f;
	//clothDesc.attachmentResponseCoefficient = 1.0f;
	//clothDesc.solverIterations = 5;
	//clothDesc.flags |= NX_CLF_STATIC;
	//clothDesc.flags |= NX_CLF_DISABLE_COLLISION;
	//clothDesc.flags |= NX_CLF_VISUALIZATION;
	//clothDesc.flags |= NX_CLF_GRAVITY;
	clothDesc.flags |= NX_CLF_BENDING;
	//clothDesc.flags |= NX_CLF_BENDING_ORTHO;
	clothDesc.flags |= NX_CLF_DAMPING;
	//clothDesc.flags |= NX_CLF_COMDAMPING;
	clothDesc.flags |= NX_CLF_COLLISION_TWOWAY;

	if (gHardwareSimulation)
		clothDesc.flags |= NX_CLF_HARDWARE;

	MyCloth *objCloth1 = new MyCloth(gScene, clothDesc, "pipe.obj", 1.0f);
	if (!objCloth1->getNxCloth())
	{
		printf("Error: Unable to create the cloth for the current scene.\n");
		delete objCloth1;
	} else
	{
		gCloths.push_back(objCloth1);
		objCloth1->getNxCloth()->attachToCollidingShapes(0);
	}

	clothDesc.globalPose.t = NxVec3(0,12.2,0);
	clothDesc.globalPose.M.rotX(NxHalfPiF32);

	MyCloth *objCloth2 = new MyCloth(gScene, clothDesc, "pipe.obj", 1.0f);
	if (!objCloth2->getNxCloth())
	{
		printf("Error: Unable to create the cloth for the current scene.\n");
		delete objCloth2;
	} else
	{
		gCloths.push_back(objCloth2);
		objCloth2->getNxCloth()->attachToCollidingShapes(0);
	}

	gCameraPos.set(29.4f,12.93f,9.79f);
	gCameraForward.set(-0.88f,-0.11f,-0.43f);
	gCameraForward.normalize();
}
// ------------------------------------------------------------------------------------

void SetupAttachmentScene()
{
	RefreshDisplayString("Attachment Demo");

	// Create objects in scene
	gGroundPlane = CreateGroundPlane();
	NxActor *box1 = CreateBox(NxVec3(-7,12.25,0), NxVec3(2.5f,1.0f,1.0f), 0.0f);
	NxActor *box2 = CreateBox(NxVec3(0,12.25,0), NxVec3(2.5f,1.0f,1.0f), 0.0f);
	NxActor *box3 = CreateBox(NxVec3(7,12.25,0), NxVec3(2.5f,1.0f,1.0f), 0.0f);	

	NxActor *attachedBox = CreateBox(NxVec3(-6.25,3.5,2.0), NxVec3(1.25f,1.0f,1.0f), 0.5f);
	NxActor *attachedSphere = CreateSphere(NxVec3(0.0,3.5,2.0), 1.3f, 0.5f);
	NxActor *attachedCapsule = CreateCapsule(NxVec3(9.0,4.5,2.0),2.0f, 1.0f, 1.0f); 

	NxReal damping = 0.3f;
	attachedBox->setAngularDamping(damping);
	attachedBox->setLinearDamping(damping);
	attachedSphere->setAngularDamping(damping);
	attachedSphere->setLinearDamping(damping);
	attachedCapsule->setAngularDamping(damping);
	attachedCapsule->setLinearDamping(damping);

	NxQuat q;
	q.fromAngleAxis(90,NxVec3(0.0f,0.0f,1.0f));
	attachedCapsule->setGlobalOrientationQuat(q);

	NxClothDesc clothDesc;
	clothDesc.globalPose.M.rotX(1.3);
	clothDesc.thickness = 0.3f;	
	clothDesc.density = 1.0f;
	clothDesc.bendingStiffness = 1.0f;
	clothDesc.stretchingStiffness = 1.0f;
	clothDesc.dampingCoefficient = 0.5f;
	clothDesc.friction = 0.5f;
	//clothDesc.collisionResponseCoefficient = 1.0f;
	clothDesc.attachmentResponseCoefficient = 1.0f;
	clothDesc.attachmentTearFactor = 2.0f;
	clothDesc.solverIterations = 5;
	//clothDesc.flags |= NX_CLF_STATIC;
	//clothDesc.flags |= NX_CLF_DISABLE_COLLISION;
	//clothDesc.flags |= NX_CLF_VISUALIZATION;
	//clothDesc.flags |= NX_CLF_GRAVITY;
	clothDesc.flags |= NX_CLF_BENDING;
	clothDesc.flags |= NX_CLF_BENDING_ORTHO;
	clothDesc.flags |= NX_CLF_DAMPING;
	//clothDesc.flags |= NX_CLF_COMDAMPING;
	//clothDesc.flags |= NX_CLF_COLLISION_TWOWAY;

	if (gHardwareSimulation)
		clothDesc.flags |= NX_CLF_HARDWARE;

	clothDesc.globalPose.t = NxVec3(-7.2,13.0,0);

	MyCloth *regularCloth1 = new MyCloth(gScene, clothDesc, 2.0f, 8.0f, CLOTH_GRID_STEP_ATTACHMENT);

	if (!regularCloth1->getNxCloth())
	{
		printf("Error: Unable to create the cloth for the current scene.\n");
		delete regularCloth1;
	} else
	{
		gCloths.push_back(regularCloth1);
		regularCloth1->getNxCloth()->attachToShape(box1->getShapes()[0], NX_CLOTH_ATTACHMENT_TWOWAY | NX_CLOTH_ATTACHMENT_TEARABLE);
		regularCloth1->getNxCloth()->attachToShape(attachedBox->getShapes()[0], NX_CLOTH_ATTACHMENT_TWOWAY);
	}

	clothDesc.globalPose.t = NxVec3(-1.0,13.0,0);

	MyCloth *regularCloth2 = new MyCloth(gScene, clothDesc, 2.0f, 8.0f, CLOTH_GRID_STEP_ATTACHMENT);

	if (!regularCloth2->getNxCloth())
	{
		printf("Error: Unable to create the cloth for the current scene.\n");
		delete regularCloth2;
	} else
	{
		gCloths.push_back(regularCloth2);
		regularCloth2->getNxCloth()->attachToShape(box2->getShapes()[0], NX_CLOTH_ATTACHMENT_TWOWAY | NX_CLOTH_ATTACHMENT_TEARABLE);
		regularCloth2->getNxCloth()->attachToShape(attachedSphere->getShapes()[0], NX_CLOTH_ATTACHMENT_TWOWAY);
	}

	clothDesc.globalPose.t = NxVec3(6.1,13.0,0);

	MyCloth *regularCloth3 = new MyCloth(gScene, clothDesc, 2.0f, 8.0f, CLOTH_GRID_STEP_ATTACHMENT);

	if (!regularCloth3->getNxCloth())
	{
		printf("Error: Unable to create the cloth for the current scene.\n");
		delete regularCloth3;
	} else
	{
		gCloths.push_back(regularCloth3);
		regularCloth3->getNxCloth()->attachToCollidingShapes(NX_CLOTH_ATTACHMENT_TWOWAY);
	}

	gCameraPos.set(3.25f,13.37f,26.62f);
	gCameraForward.set(-0.07f,-0.21f,-0.96f);
	gCameraForward.normalize();
}
// ------------------------------------------------------------------------------------
void SetupTearingScene()
{
	RefreshDisplayString("Tearing Demo", true);

	// Create the objects in the scene
	gGroundPlane = CreateGroundPlane();
	NxActor *box1 = CreateBox(NxVec3(1,11,0), NxVec3(3,0.5,0.5), 0);
	NxActor *box2 = CreateBox(gTearLines ? NxVec3(3.3,3,0) : NxVec3(3.3,7,0), NxVec3(0.5,0.5,0.5), 10);

	NxClothDesc clothDesc;
	clothDesc.globalPose.t = NxVec3(-1.5,11,0);
	clothDesc.globalPose.M.rotX(NxHalfPiF32);
	clothDesc.thickness = 0.1f;
	//clothDesc.density = 1.0f;
	//clothDesc.bendingStiffness = 1.0f;
	//clothDesc.stretchingStiffness = 1.0f;
	//clothDesc.dampingCoefficient = 0.50f;
	clothDesc.friction = 0.5f;
	//clothDesc.pressure = 0.0f;
	clothDesc.tearFactor = 2.0f;
	//clothDesc.collisionResponseCoefficient = 1.0f;
	//clothDesc.attachmentResponseCoefficient = 1.0f;
	//clothDesc.solverIterations = 5;
	//clothDesc.flags |= NX_CLF_PRESSURE;
	//clothDesc.flags |= NX_CLF_STATIC;
	//clothDesc.flags |= NX_CLF_DISABLE_COLLISION;
	//clothDesc.flags |= NX_CLF_SELFCOLLISION;
	//clothDesc.flags |= NX_CLF_VISUALIZATION;
	//clothDesc.flags |= NX_CLF_GRAVITY;
	clothDesc.flags |= NX_CLF_BENDING;
	//clothDesc.flags |= NX_CLF_BENDING_ORTHO;
	//clothDesc.flags |= NX_CLF_DAMPING;
	//clothDesc.flags |= NX_CLF_COMDAMPING;
	clothDesc.flags |= NX_CLF_COLLISION_TWOWAY;
	clothDesc.flags |= NX_CLF_TEARABLE;

	if (gHardwareSimulation)
		clothDesc.flags |= NX_CLF_HARDWARE;

	MyCloth *regularCloth = new MyCloth(gScene, clothDesc, 5.0f, 8.0f, CLOTH_GRID_STEP_TEARING, "rug512.bmp", gTearLines);

	if (!regularCloth->getNxCloth())
	{
		printf("Error: Unable to create the cloth for the current scene.\n");
		delete regularCloth;
	} else
	{
		gCloths.push_back(regularCloth);
		regularCloth->getNxCloth()->attachToShape(*box1->getShapes(), 0);
		regularCloth->getNxCloth()->attachToShape(*box2->getShapes(), NX_CLOTH_ATTACHMENT_TWOWAY);
	}

	gCameraPos.set(1.43f,10.96f,17.9f);
	gCameraForward.set(0.0f,-0.29f,-0.96f);
	gCameraForward.normalize();
}

// ------------------------------------------------------------------------------------
void SetupPressureScene()
{
	RefreshDisplayString("Pressure Demo");

	// Create the objects in the scene
	gGroundPlane = CreateGroundPlane();
	NxActor *box1 = CreateBox(NxVec3(0,0,-5), NxVec3(10,5,0.5), 0);
	NxActor *box2 = CreateBox(NxVec3(-10,0,0), NxVec3(0.5,5,5), 0); 
	NxActor *box3 = CreateBox(NxVec3( 10,0,0), NxVec3(0.5,5,5), 0); 

	NxReal r = 1.0;
	NxReal d = 1.0;
	CreateSphere(NxVec3(0,20,0), r, d);
	CreateSphere(NxVec3(-3,35, 0), r, d);
	CreateSphere(NxVec3(3,50, 0), r, d);
	CreateSphere(NxVec3(0,40, 0), r, d);
	CreateSphere(NxVec3(-3,55, 0), r, d);
	CreateSphere(NxVec3(3,60, 0), r, d);
	CreateSphere(NxVec3(0,60, 0), r, d);
	CreateSphere(NxVec3(-3,75, 0), r, d);
	CreateSphere(NxVec3(3,80, 0), r, d);

	NxClothDesc clothDesc;
	clothDesc.globalPose.t = NxVec3(0.3,6,0);
	clothDesc.thickness = 0.1f;
	//clothDesc.density = 1.0f;
	//clothDesc.bendingStiffness = 1.0f;
	//clothDesc.stretchingStiffness = 1.0f;
	//clothDesc.dampingCoefficient = 0.50f;
	clothDesc.friction = 0.5f;
	clothDesc.pressure = 1.5f;
	clothDesc.tearFactor = 2.2f;
	//clothDesc.collisionResponseCoefficient = 1.0f;
	//clothDesc.attachmentResponseCoefficient = 1.0f;
	//clothDesc.solverIterations = 5;
	clothDesc.flags |= NX_CLF_PRESSURE;
	//clothDesc.flags |= NX_CLF_STATIC;
	//clothDesc.flags |= NX_CLF_DISABLE_COLLISION;
	//clothDesc.flags |= NX_CLF_SELFCOLLISION;
	//clothDesc.flags |= NX_CLF_VISUALIZATION;
	//clothDesc.flags |= NX_CLF_GRAVITY;
	clothDesc.flags |= NX_CLF_BENDING;
	//clothDesc.flags |= NX_CLF_BENDING_ORTHO;
	//clothDesc.flags |= NX_CLF_DAMPING;
	clothDesc.flags |= NX_CLF_COLLISION_TWOWAY;
	//clothDesc.flags |= NX_CLF_TEARABLE;

	if (gHardwareSimulation)
		clothDesc.flags |= NX_CLF_HARDWARE;

	MyCloth *objCloth = new MyCloth(gScene, clothDesc, "cow.obj", 1.0f);
	gCloths.push_back(objCloth);

	gCameraPos.set(-4.98f,17.8f,25.6f);
	gCameraForward.set(0.17f,-0.47f,-0.87f);
	gCameraForward.normalize();
}


void SetupRemapMeshScene()
{
	RefreshDisplayString("Remap Mesh Demo");

	// Create the objects in the scene
	gGroundPlane = CreateGroundPlane();
	NxVec3 box1Pos(-1.5,11,-5);

	NxVec3 boxDim(5,0.2,0.2);

	NxActor *box1 = CreateBox(box1Pos, boxDim, 0);

	NxClothDesc clothDesc;
	clothDesc.globalPose.t = NxVec3(-1.5,11,0);
	clothDesc.globalPose.M.rotX(NxHalfPiF32);
	NxMat33 rot;
	rot.rotZ(NxPiF32);
	clothDesc.globalPose.M = rot * clothDesc.globalPose.M;
	clothDesc.thickness = 0.1f;
	//clothDesc.density = 1.0f;
	//clothDesc.bendingStiffness = 1.0f;
	//clothDesc.stretchingStiffness = 1.0f;
	//clothDesc.dampingCoefficient = 0.50f;
	clothDesc.friction = 0.5f;
	//clothDesc.pressure = 0.0f;
	clothDesc.tearFactor = 2.4f;
	//clothDesc.collisionResponseCoefficient = 1.0f;
	//clothDesc.attachmentResponseCoefficient = 1.0f;
	//clothDesc.solverIterations = 5;
	//clothDesc.flags |= NX_CLF_PRESSURE;
	//clothDesc.flags |= NX_CLF_STATIC;
	//clothDesc.flags |= NX_CLF_DISABLE_COLLISION;
	//clothDesc.flags |= NX_CLF_SELFCOLLISION;
	//clothDesc.flags |= NX_CLF_VISUALIZATION;
	//clothDesc.flags |= NX_CLF_GRAVITY;
	clothDesc.flags |= NX_CLF_BENDING;
	//clothDesc.flags |= NX_CLF_BENDING_ORTHO;
	//clothDesc.flags |= NX_CLF_DAMPING;
	//clothDesc.flags |= NX_CLF_COMDAMPING;
	clothDesc.flags |= NX_CLF_COLLISION_TWOWAY;
	clothDesc.flags |= NX_CLF_TEARABLE;
	clothDesc.flags |= NX_CLF_VALIDBOUNDS;

	clothDesc.validBounds.include(clothDesc.globalPose.t);
	clothDesc.validBounds.min -= NxVec3(30,30,30);
	clothDesc.validBounds.max += NxVec3(30,30,30);

	if (gHardwareSimulation)
		clothDesc.flags |= NX_CLF_HARDWARE;

	// Curtain
	NxVec3 offset(0,0,0);
	MyCloth *curtain = new MyCloth(gScene, clothDesc, "curtain.obj", 1.0f, &offset, "rug512.bmp");
	if (!curtain->getNxCloth())
	{
		printf("Error: Unable to create the curtain for the current scene.\n");
		delete curtain;
	}
	else
	{
		gCloths.push_back(curtain);
		curtain->getNxCloth()->attachToShape(*box1->getShapes(), 0);
	}

	clothDesc.globalPose.M.rotX(NxHalfPiF32);
	clothDesc.flags &= ~NX_CLF_VALIDBOUNDS;
	clothDesc.pressure = 1.5f;
	clothDesc.flags |= NX_CLF_PRESSURE;
	clothDesc.tearFactor = 1.7f;

	NxActor* ramp1 = CreateBox(NxVec3(15, 10, -7.5), NxVec3(10,4,0.2), 1);
	ramp1->raiseBodyFlag(NX_BF_KINEMATIC);	
	rot.rotX(NxMath::degToRad(-60.f));
	ramp1->setGlobalOrientation(rot);

	NxActor* pole1 = CreateCapsule(NxVec3(13.5, 0, 5), 5, 0.3f, 1);
	NxActor* pole2 = CreateCapsule(NxVec3(19.0, 0, 10), 5, 0.6f, 1);
	pole1->raiseBodyFlag(NX_BF_KINEMATIC);
	pole2->raiseBodyFlag(NX_BF_KINEMATIC);
	CreateBox(NxVec3(9, 0, 8), NxVec3(1,0.8,0.3), 0);
	rot.rotX(NxMath::degToRad(-30.f));
	pole1->setGlobalOrientation(rot);
	rot.rotX(NxMath::degToRad(-30.f));
	pole2->setGlobalOrientation(rot);

	// Sphere
	offset.set(20,-10,-20);
	MyCloth* sphere = new MyCloth(gScene, clothDesc, "icoSphere.obj", 0.2f, &offset, "rug512.bmp");
	if (!sphere->getNxCloth())
	{
		printf("Error: Unable to create the sphere for the current scene.\n");
		delete sphere;
	}
	else
	{
		gCloths.push_back(sphere);
	}

	// Sphere
	offset.set(15,-10,-20);
	sphere = new MyCloth(gScene, clothDesc, "icoSphere.obj", 0.1f, &offset, "rug512.bmp");
	if (!sphere->getNxCloth())
	{
		printf("Error: Unable to create the sphere for the current scene.\n");
		delete sphere;
	}
	else
	{
		gCloths.push_back(sphere);
	}

	offset.set(10,-10,-20);
	sphere = new MyCloth(gScene, clothDesc, "icoSphere.obj", 0.15f, &offset, "rug512.bmp");
	if (!sphere->getNxCloth())
	{
		printf("Error: Unable to create the sphere for the current scene.\n");
		delete sphere;
	}
	else
	{
		gCloths.push_back(sphere);
	}

	gCameraPos.set(3.f, 15.f,17.9f);
	gCameraForward.set(0.0f,-0.29f,-0.96f);
	gCameraForward.normalize();
}



// ------------------------------------------------------------------------------------
bool InitNx()
{
	if (!gAllocator)
		gAllocator = new UserAllocator;

	// Initialize PhysicsSDK
	NxPhysicsSDKDesc desc;
	desc.gpuHeapSize = 8;
	desc.meshCacheSize = 0;
	desc.flags &= ~NX_SDKF_NO_HARDWARE;
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

#if !defined(NX_DISABLE_HARDWARE) && !defined(_XBOX) && !defined(__CELLOS_LV2__)
	NxHWVersion hwCheck = gPhysicsSDK->getHWVersion();
	if (hwCheck == NX_HW_VERSION_NONE) 
	{
		printf("\nWarning: Unable to find a PhysX card, the cloth will be simulated in software.\n\n");
		gHardwareSimulation = false;
	}
#endif

	if (!gCookingInitialized)
	{
		gCookingInitialized = true;
		if (!InitCooking(gAllocator, &gErrorStream)) 
		{
			printf("\nError: Unable to initialize the cooking library, exiting the sample.\n\n");
			return false;
		}
	}
	// Set the physics parameters
	gPhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.005f);

	// Set the debug visualization parameters
	//gPhysicsSDK->setParameter(NX_VISUALIZATION_SCALE, 1);
	//gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1);
	//gPhysicsSDK->setParameter(NX_VISUALIZE_JOINT_LIMITS, 1);
	//gPhysicsSDK->setParameter(NX_VISUALIZE_JOINT_LOCAL_AXES, 1);
	//gPhysicsSDK->setParameter(NX_VISUALIZE_CLOTH_MESH, 1);
	//gPhysicsSDK->setParameter(NX_VISUALIZE_CLOTH_TEARING, 1);
	//gPhysicsSDK->setParameter(NX_VISUALIZE_CLOTH_TEARABLE_VERTICES, 1);
	//gPhysicsSDK->setParameter(NX_VISUALIZE_CLOTH_VALIDBOUNDS, 1);

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
	NxMaterialDesc       m; 
	m.restitution        = 0.5f;
	m.staticFriction     = 0.2f;
	m.dynamicFriction    = 0.2f;
	NxMaterial * mat = gScene->getMaterialFromIndex(0);
	mat->loadFromDesc(m); 

	switch(gSceneNr) {
		case 1 : SetupCurtainScene(1); break;
		case 2 : SetupFlagScene(); break;
		case 3 : SetupDampingScene(); break;
		case 4 : SetupPipeScene(); break;
		case 5 : SetupAttachmentScene(); break;
		case 6 : SetupTearingScene(); break;
		case 7 : SetupPressureScene(); break;
		case 8 : SetupRemapMeshScene(); break;
	}
	
	if (gCloths.size() > 0)
		gSelectedCloth = gCloths[0]->getNxCloth();
	else
		gSelectedCloth = NULL;

	if (gScene->getNbActors() > 0)
		gSelectedActor = *gScene->getActors();
	else
		gSelectedActor = NULL;

#if defined(_XBOX) | defined(__CELLOS_LV2__)
	glutRemapButtonExt(1, 'b', false);//Bending
	glutRemapButtonExt(1, 'o', true);//Orthogonal bending
	glutRemapButtonExt(2, 'G', false);//Gravity
	glutRemapButtonExt(2, 't', true);//Tear lines
	glutRemapButtonExt(3, 'x', false);//Shadows
	glutRemapButtonExt(3, 'n', true);//Debug wireframe
#endif
	return true;
}


// ------------------------------------------------------------------------------------
void ReleaseNx()
{
	for (MyCloth **cloth = gCloths.begin(); cloth != gCloths.end(); cloth++)
		delete *cloth;
	gCloths.clear();

	if (gCookingInitialized) CloseCooking();
	gCookingInitialized = false;
	if(gPhysicsSDK != NULL)
	{
		if(gScene != NULL) gPhysicsSDK->releaseScene(*gScene);
		gScene = NULL;
		NxReleasePhysicsSDK(gPhysicsSDK);
		gPhysicsSDK = NULL;
	}
}

// ------------------------------------------------------------------------------------
void ResetNx()
{
	LetGoActor();
	LetGoCloth();
	ReleaseNx();
	if (!InitNx()) exit(0);
}

// ------------------------------------------------------------------------------------
void RunPhysics()
{
	NxReal deltaTime;
	NxU32 maxSubSteps;
	NxTimeStepMethod method;
	gScene->getTiming(deltaTime, maxSubSteps, method);

	if(0) // set to 1 for deltaTime different than maxTimeStep
	{
		// delta time = substep size * number of substeps
		gTime += 0.02f;
		deltaTime *= NxU32(gTime / deltaTime);
		gTime -= deltaTime;
	}

	deltaTime = 0.02f;	// todo: may go back to real time at some point

	// Run collision and dynamics for delta time since the last frame
	gScene->simulate(deltaTime);	
	gScene->flushStream();
	gScene->fetchResults(NX_RIGID_BODY_FINISHED, true);

	if (gSceneNr == 2 && gScene->getNbCloths() == 1) {
		NxCloth **cloths = gScene->getCloths();
		NxReal range = 5.0f;
		cloths[0]->setWindAcceleration(NxVec3(
			25.0f + NxMath::rand(-range, range),
			NxMath::rand(-range, range),
			NxMath::rand(-range, range)
		));
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
		atexit(ExitCallback);
		glutMainLoop();
	}
	else
		ReleaseNx();
}
#endif

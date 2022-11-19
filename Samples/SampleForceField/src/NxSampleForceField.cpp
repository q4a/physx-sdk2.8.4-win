// ===============================================================================
//
//						   NVIDIA PhysX SDK Sample Program
//
// Title: Force Fields
// Description: This sample program shows how to use force fields
//
// This file contains the main application, the interesting parts (for learning
// about Force Fields) are the following sample files:
// * VortexSample.cpp    - A vortex that can be moved around in a scene. Exclude shapes
//                         and NX_FFC_CYLINDRICAL coordinates are used.
// * WindSample.cpp      - A straight blowing wind with a small animation. 
//                         Exclude shapes and NX_FFC_CARTESIAN coordinates.
// * ExplosionSample.cpp - A simple explosion effect. NX_FFC_SPHERICAL coordinates
//                         are used in this sample.
//
// ===============================================================================

#define NOMINMAX
#ifdef WIN32 
#include <windows.h>
#else __linux__
#endif

// Physics code
#undef random

#include "NxSampleForceField.h"
#include "Utilities.h"
#include "cooking.h"
#include "SamplesVRDSettings.h"
#include "MySoftBody.h"
#include "NXU_helper.h"
#include "UserAllocator.h"

#ifdef __PPCGEKKO__
#include "GLFontRenderer.h"
#endif

DebugRenderer		gDebugRenderer;
int					gDebugRenderState = 0;
ErrorStream			gErrorStream;

// Physics SDK globals
NxPhysicsSDK*		gPhysicsSDK = NULL;
NxScene*			gScene = NULL;
NxVec3				gDefaultGravity(0,-9.8f,0);
bool				gSceneRunning = false;
UserAllocator*		gAllocator = NULL;

#ifdef NX_DISABLE_HARDWARE
bool				gHardwareSimulation = false;
#else
bool				gHardwareSimulation = true;
#endif

bool				gCookingInitialized = false;

unsigned			gSampleIndex = 0;
NxArray<ForceFieldSample*> gSamples;

// Display globals
int					gMainHandle;
bool				gHelp = false;
char				gDisplayString[512] = "";
char				gTitleString[512] = "";
char				gHelpString[512] = "";

// Camera globals
NxReal				gCameraFov = 40.0f;
NxVec3				gCameraPos(0,8,25);
NxVec3				gCameraForward(0,0,-1);
NxVec3				gCameraRight(1,0,0);
NxReal				gCameraSpeed = 0.2;
int					gViewWidth = 0;
int					gViewHeight = 0;
bool				gRotateCamera = false;

// MouseGlobals
int					mx = 0;
int					my = 0;

// Keyboard globals
#define				MAX_KEYS 256
bool				gKeys[MAX_KEYS];

// Simulation globals
bool				gPause = false;
bool				gShadows = true;
bool				gWireframeMode = false;

// fps
int					gFrameCounter = 0;
float				gPreviousTime = getCurrentTime();

NxForceField*		gForceField = NULL;
NxActor*			gForceFieldActor = NULL;
MySoftBody*			gSoftBody = NULL;

bool InitNx()
{
	if (!gAllocator)
		gAllocator = new UserAllocator;

	// Initialize PhysicsSDK
	NxPhysicsSDKDesc desc;
	desc.flags &= ~NX_SDKF_NO_HARDWARE;
	desc.gpuHeapSize = 8;
	desc.meshCacheSize = 1;
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

	gPhysicsSDK->setParameter(NX_VISUALIZE_FORCE_FIELDS, 1.0f);

#if !defined(__PPCGEKKO__)
#ifndef NX_DISABLE_HARDWARE
	NxHWVersion hwCheck = gPhysicsSDK->getHWVersion();
	if (hwCheck == NX_HW_VERSION_NONE) 
	{
		printf("\nWarning: Unable to find a PhysX card, or PhysX card used by other application.");
		printf("\nThe fluids will be simulated in software.\n\n");
		gHardwareSimulation = false;
	}
#endif
#endif
#ifdef __PPCGEKKO__
		gHardwareSimulation = false;
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
	gPhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.005f);

	// Set the debug visualization parameters
	gPhysicsSDK->setParameter(NX_VISUALIZATION_SCALE, 1);

#ifdef __PPCGEKKO__	
	//Make a list of the demos in this sample
	gSamples.pushBack(new SampleVortex());
	gSamples.pushBack(new SampleWind());
	gSamples.pushBack(new SampleExplosion());
#endif

	gSamples[gSampleIndex]->setup();
	if (gScene == NULL)
	{
		for (NxU32 i = 0; i < gPhysicsSDK->getNbScenes(); i++)
		{
			gScene = gPhysicsSDK->getScene(i);
			break;
		}
	}
		
#if defined(_XBOX) | defined(__CELLOS_LV2__)
	glutRemapButtonExt(3, 'x', false); //Demo-specific usage
#endif
#if defined(__PPCGEKKO__)
	glutRemapButtonExt('a', 'i');
	glutRemapButtonExt('b', ' ');
	glutRemapButtonExt('1', GLUT_KEY_F5);
	glutRemapButtonExt(GLUT_KEY_HOME, GLUT_KEY_F1);
	glutRemapButtonExt('2', 'x');
#endif

	// simulate one frame to make fluids and other things initialized
	RunPhysics();
	return true;
}

void WaitForPhysics()
{
	if (gSceneRunning)
	{
		NxU32 error;
		gScene->flushStream();
		gScene->fetchResults(NX_ALL_FINISHED, true, &error);
		assert(error == 0);
		gSceneRunning = false;
	}
}

void ReleaseNx()
{
	WaitForPhysics();

	if (gCookingInitialized) 
	{
		CloseCooking();
		gCookingInitialized = false;
	}

	if(gPhysicsSDK != NULL)
	{
		if(gScene != NULL) gPhysicsSDK->releaseScene(*gScene);
		gScene = NULL;
		NxReleasePhysicsSDK(gPhysicsSDK);
		gPhysicsSDK = NULL;
	}

	NXU::releasePersistentMemory();

	if (gAllocator)
	{
		delete gAllocator;
		gAllocator = NULL;
	}
}

void ResetNx(NxU32 loadSample)
{
	WaitForPhysics();
	gSamples[gSampleIndex]->cleanup();
	ReleaseNx();
	gSampleIndex = loadSample;
	if (!InitNx()) exit(0);
}

void RunPhysics()
{
	if (gSceneRunning)
		return;

	NxReal deltaTime = 0.02f;	//We are not simulating real time here, you should meassure the
								// difference in real time since the last call and use that.

	//Let the current sample update, if it needs to change anything before simulating
	gSamples[gSampleIndex]->update();

	gScene->simulate(deltaTime);
	gSceneRunning = true;
	WaitForPhysics();

	gFrameCounter++;
}

// ------------------------------------------------------------------------------------

void DisplayStats(float& y)
{
#ifdef WIN32 
	static NxI32 totalHeapIdx = -1;
	static NxI32 deformableHeapIdx = -1;
	static NxI32 fluidHeapIdx = -1;
	static NxI32 utilHeapIdx = -1;
	static NxI32 meshCacheIdx = -1;

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
				else if (0 == strncmp(stats->stats[curIdx].name, "GpuHeapUsageFluid", 17))
					fluidHeapIdx = curIdx;
				else if (0 == strncmp(stats->stats[curIdx].name, "GpuHeapUsageUtils", 17))
					utilHeapIdx = curIdx;
				else if (0 == strncmp(stats->stats[curIdx].name, "GpuMeshCacheUsage", 17))
					meshCacheIdx = curIdx;
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

		if ((fluidHeapIdx >= 0) && ((NxU32)fluidHeapIdx < numStats))
		{
			NxSceneStatistic& stat = stats->stats[fluidHeapIdx];
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

		if ((meshCacheIdx >= 0) && ((NxU32)meshCacheIdx < numStats))
		{
			NxSceneStatistic& stat = stats->stats[meshCacheIdx];
			_snprintf(textBuffer, 1024, "%s = %d, max = %d kB", stat.name, stat.curValue, stat.maxValue);
			GLFontRenderer::print(0.01, y, 0.03f, textBuffer);
			y -= 0.035f;
		}	}
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
	char tempString[512];	
	sprintf(gHelpString, "\nGeneral:\n");
	strcat(gHelpString, "    + -: choose scene\n");
	strcat(gHelpString, "    b: shoot sphere\n");
	strcat(gHelpString, "    HOME: toggle help\n");
	strcat(gHelpString, "    1: toggle debug rendering\n");
#else
	char tempString[512];
	sprintf(gHelpString, "\nGeneral:\n");
	sprintf(tempString, "    1-%d: choose scene\n", gSamples.size());
	strcat(gHelpString, tempString);
	strcat(gHelpString, "    p: pause\n");
	strcat(gHelpString, "    o: single step\n");
#ifndef NX_DISABLE_HARDWARE
	strcat(gHelpString, "    h: hardware on/off\n");
#endif
	strcat(gHelpString, "    w,a,s,d: move/strafe\n");
	strcat(gHelpString, "    q,e: move up/down\n");
	strcat(gHelpString, "    space: shoot sphere\n");
	strcat(gHelpString, "    F1: toggle help\n");
	strcat(gHelpString, "    F5: toggle debug rendering\n");
	strcat(gHelpString, "    F10: Reset scene\n");
#endif

	if (demoKeyString)
	{
		strcat(gHelpString, "\nDemo specific:\n");
		strcat(gHelpString, demoKeyString);
	}
}

// ------------------------------------------------------------------------------------

void ProcessKeys()
{
	// Process keys
	for (int i = 0; i < MAX_KEYS; i++)
	{	
		if (!gKeys[i])  { continue; }

		gSamples[gSampleIndex]->processKey(i);

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

inline void MyDrawArrow(const NxVec3& posA, const NxVec3& posB)
{
#ifdef __PPCGEKKO__
	glBegin(GL_LINES, 2);
	glVertex3d(posA.x, posA.y, posA.z);
	glVertex3d(posB.x, posB.y, posB.z);
	glEnd();
#else	
	NxVec3 v3ArrowShape[] = {
		NxVec3(posA), NxVec3(posB)
	};
	glVertexPointer(3, GL_FLOAT, sizeof(NxVec3), &v3ArrowShape[0].x);
	glDrawArrays(GL_LINES, 0, sizeof(v3ArrowShape)/sizeof(NxVec3));
#endif	
	
}


void SampleAndVisualizeForceFieldShapeGroup(NxForceField* field, NxForceFieldShapeGroup* group)
{
	group->resetShapesIterator();
	NxU32 nbShapes = group->getNbShapes();
	while (nbShapes--)
	{
		NxForceFieldShape* shape = group->getNextShape();
		NxBoxForceFieldShape* boxShape = shape->isBox();
		if (!boxShape) continue;

		NxVec3 dim = boxShape->getDimensions();
	

		NxMat34 pose = boxShape->getPose();
		if (boxShape->getForceField() != NULL)
		{
  			pose = field->getPose() * pose;
  			if (gForceFieldActor != NULL)
  				pose = gForceFieldActor->getGlobalPose() * pose;
		}
			
		NxVec3 start = pose.t - (pose.M*dim);
		int resolution = 10;
		//NxVec3 step = dim * (2.0f / (float) resolution);
		NxVec3 stepX = pose.M.getColumn(0) * dim.x * (2.0f / (float) resolution);
		NxVec3 stepY = pose.M.getColumn(1) * dim.y * (2.0f / (float) resolution);
		NxVec3 stepZ = pose.M.getColumn(2) * dim.z * (2.0f / (float) resolution);
		NxVec3 samplePoint;
		NxVec3 sampleForce;
		NxVec3 sampleTorque;
		for (int i = 0; i < resolution; i++)
		{
			for (int j = 0; j < resolution; j++)
			{
				for (int k = 0; k < resolution; k++)
				{
					samplePoint = start + stepX*i + stepY*j + stepZ*k;

					field->samplePoints(1, &samplePoint, NULL, &sampleForce, &sampleTorque);
					MyDrawArrow(samplePoint, samplePoint + sampleForce * 0.005f);
				}
			}
		}
	}
}

void SampleAndVisualizeForceField(NxForceField* field)
{
	if (field == NULL) return;

#if !(defined(__CELLOS_LV2__) || defined(_XBOX) || defined(__PPCGEKKO__))
	glPushAttrib(GL_ALL_ATTRIB_BITS);
#endif
	glDisable(GL_LIGHTING);
	glLineWidth(1.0f);
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	glEnableClientState(GL_VERTEX_ARRAY);
	glLoadIdentity();

	// visualize all shape groups
	field->resetShapeGroupsIterator();
	NxU32 nbGroups = field->getNbShapeGroups();
	while (nbGroups--)
	{
		NxForceFieldShapeGroup* group = field->getNextShapeGroup();
		SampleAndVisualizeForceFieldShapeGroup(field, group);
	}
	SampleAndVisualizeForceFieldShapeGroup(field, &field->getIncludeShapeGroup());

	glDisableClientState(GL_VERTEX_ARRAY);
#if !(defined(__CELLOS_LV2__) || defined(_XBOX) || defined(__PPCGEKKO__))
	glPopAttrib();
#else
	glEnable(GL_LIGHTING);
	glColor4f(1.0f,1.0f,1.0f,1.0f);
#endif
}


void RenderForceFieldGroup(NxForceFieldShapeGroup* group)
{
		const NxReal excludeCol[4] = {1.0f, 0.0f, 0.0f, 0.3f};
		const NxReal includeCol[4] = {0.0f, 1.0f, 0.0f, 0.3f};
		const NxReal* col = (group->getFlags() & NX_FFSG_EXCLUDE_GROUP) ? excludeCol : includeCol;
	
		group->resetShapesIterator();
		NxU32 nbShapes = group->getNbShapes();

		NxMat34 ffPose;
		ffPose.id();
		NxForceField* ff = group->getForceField();
		if(ff)
		{
			ffPose = ff->getPose();
			NxActor* actor =  ff->getActor();
			if(actor)
				ffPose = actor->getGlobalPose() * ffPose;
		}
		while (nbShapes--)
		{
			NxForceFieldShape* shape = group->getNextShape();
			NxMat34 pose = ffPose * shape->getPose();
			
			glPushMatrix();
			SetupGLMatrix(pose.t, pose.M);
			glColor4fv(col);

			switch(shape->getType())
			{
			case NX_SHAPE_BOX:
				{
				NxBoxForceFieldShape* boxShape = shape->isBox();
				NxVec3 boxDim = boxShape->getDimensions();
				glScalef(boxDim.x, boxDim.y, boxDim.z);
				glutSolidCube(2);
				break;
				}
			case NX_SHAPE_SPHERE:
				{
				NxSphereForceFieldShape* sphereShape = shape->isSphere();
				NxReal radius = sphereShape->getRadius();
				glScalef(radius, radius, radius);
				glutSolidSphere(1.0f, 12, 12);
				break;
				}
			}
			glPopMatrix();
		}
}

void RenderForceField(NxForceField* field)
{
		// render groups
		field->resetShapeGroupsIterator();
		NxU32 nbGroups = field->getNbShapeGroups();
		while (nbGroups--)
		{
			NxForceFieldShapeGroup* group = field->getNextShapeGroup();
			RenderForceFieldGroup(group);
		}
		RenderForceFieldGroup(&field->getIncludeShapeGroup());
}

void RenderScene(bool shadows)
{
    // Render all the actors in the scene
	int nbActors = gScene->getNbActors();
    NxActor** actors = gScene->getActors();
    while (nbActors--)
    {
        NxActor* actor = *actors++;
		if (actor->userData == (void*)1) continue;
		DrawActor(actor);
    }

	NxCloth** cloths = gScene->getCloths();
	int nbCloths = gScene->getNbCloths();
	while (nbCloths--)
	{
		NxCloth* cloth = *cloths++;
		DrawCloth(cloth, true);
	}

#if NX_USE_FLUID_API
#ifdef __PPCGEKKO__
	glDisable(GL_LIGHTING);
	glColor4d(1.0, 0.5, 0.25, 1.0);
#endif
	NxFluid** fluids = gScene->getFluids();
	int nbFluids = gScene->getNbFluids();
	while (nbFluids--)
	{
		NxFluid* fluid = *fluids++;
		DrawFluid(fluid);
	}
#ifdef __PPCGEKKO__
	glEnable(GL_LIGHTING);
	glColor4d(1.0, 1.0, 1.0, 1.0);
#endif	
#endif

	if (gSoftBody != NULL)
	{
		//void simulateAndDraw(bool shadows, bool tetraMesh, bool tetraWireframe);
		gSoftBody->simulateAndDraw(false, true, false);
	}

	if (gForceField != NULL && gDebugRenderState > 0)
	{
		//draw transparent boxes for where the force field shapes are
#if !defined(__CELLOS_LV2__) && !defined(__PPCGEKKO__)
		glPushAttrib(GL_ALL_ATTRIB_BITS);
#endif
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthMask(false);

		NxU32 nbForceFields = gScene->getNbForceFields();
		while(nbForceFields--)
		{
			RenderForceField(gScene->getForceFields()[nbForceFields]);
		}
		glDisable(GL_BLEND);
		glDepthMask(true);

#if !defined(__CELLOS_LV2__) && !defined(__PPCGEKKO__)
		glPopAttrib();
#endif
	}
}

void RenderCallback()
{
	ProcessKeys();
	
	if (gScene && !gPause)
        RunPhysics();
	
    // Clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	SetupCamera();

	if (gDebugRenderState >= 2)
	{
		NxU32 nbForceFields = gScene->getNbForceFields();
		while(nbForceFields--)
		{
			SampleAndVisualizeForceField(gScene->getForceFields()[nbForceFields]);
		}
	}

	RenderScene(gShadows);

	if (gDebugRenderState == 3)
		gDebugRenderer.renderData(*(gScene->getDebugRenderable()));
	
	DisplayText();
    glutSwapBuffers();
}

void ReshapeCallback(int width, int height)
{
	gViewWidth = width;
	gViewHeight = height;
    glViewport(0, 0, width, height);
}

void IdleCallback()
{
    glutPostRedisplay();

	float time = getCurrentTime();
	float elapsedTime = time - gPreviousTime;
	
	if (elapsedTime > 1.0f) {
		char title[30];
		sprintf(title, "SampleForceField %3.1f fps", (float)gFrameCounter / elapsedTime);
		glutSetWindowTitle(title);
		gPreviousTime = time;
		gFrameCounter = 0;
	}

	getElapsedTime();
}

#ifdef __PPCGEKKO__
static int gSceneIndex = 0;
#endif

void KeyboardCallback(unsigned char key, int x, int y)
{
	gKeys[key] = true;
#ifndef __PPCGEKKO__	
	if ('1' <= key && key <= '8') 
	{
		unsigned index = key - '0' - 1;
		if (index < gSamples.size())
		{
			ResetNx(index);
		}
	}
#endif
	if (key == '0') 
	{
		ResetNx((gSampleIndex + 1) % gSamples.size());
	}

	switch (key)
	{
#ifdef __PPCGEKKO__
	case '+':
	{
		gSceneIndex = (gSceneIndex+1)%gSamples.size();
		ResetNx(gSceneIndex);
		break;
	}
	case '-':
	{
		gSceneIndex = (gSceneIndex+gSamples.size()-1)%gSamples.size();
		ResetNx(gSceneIndex);
		break;
	}
#endif	
	case 'p': { gPause = !gPause; break; }
	case 'o': { if (!gPause) gPause = true; RunPhysics(); glutPostRedisplay(); break; }
	case 'n': { gWireframeMode = !gWireframeMode; break; }		
	case ' ': {
		NxActor *sphere = CreateSphere(gCameraPos, 1.0f, 1.0f);
		sphere->setLinearVelocity(gCameraForward * 20.0f);
		break; 
			  }
#ifndef NX_DISABLE_HARDWARE
	case 'h' : {
		gHardwareSimulation = !gHardwareSimulation;
		ResetNx(gSampleIndex);
		break;
			   }
#endif
	case 27 : { ReleaseNx(); exit(0); break; }
	default : { break; }
	}

	gSamples[gSampleIndex]->onKeyPress(key, x, y);
}

void KeyboardUpCallback(unsigned char key, int x, int y)
{
	gKeys[key] = false;
}

void SpecialCallback(int key, int x, int y)
{
	switch (key)
    {
		// Reset PhysX
		case GLUT_KEY_F9: toggleVSync(); return;
		case GLUT_KEY_F10: ResetNx(gSampleIndex); return; 
		case GLUT_KEY_F1: 
			gHelp = !gHelp;
			break;
		case GLUT_KEY_F5:
		{
			gDebugRenderState++;
			if (gDebugRenderState > 3) gDebugRenderState = 0;

			WaitForPhysics();
			if (gDebugRenderState == 3)
				gPhysicsSDK->setParameter(NX_VISUALIZATION_SCALE, 1.0f);
			else
				gPhysicsSDK->setParameter(NX_VISUALIZATION_SCALE, 0.0f);

			break;
		}
	}
	gSamples[gSampleIndex]->onVirtualKeyPress(key, x, y);
}

void MouseCallback(int button, int state, int x, int y)
{
    mx = x;
	my = y;
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		gRotateCamera = true;
	}
	if (state == GLUT_UP) 
	{
		gRotateCamera = false;
	}
}

void MotionCallback(int x, int y)
{
    int dx = mx - x;
    int dy = my - y;

	if (gRotateCamera)
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

int main(int argc, char** argv)
{
#if !defined(__PPCGEKKO__)
	//Make a list of the demos in this sample
	gSamples.pushBack(new SampleVortex());
	gSamples.pushBack(new SampleWind());
	gSamples.pushBack(new SampleExplosion());
#endif

	//Initialize GLUT
	glutInit(&argc, argv);
	glutInitWindowSize(800, 600);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	gMainHandle = glutCreateWindow("Force Field Sample");
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
	glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
#endif

	// Setup lighting
	glEnable(GL_LIGHTING);
	float AmbientColor[]  = { 0.0f, 0.0f, 0.0f, 0.0f };		glLightfv(GL_LIGHT0, GL_AMBIENT, AmbientColor);
	float DiffuseColor[]  = { 0.6f, 0.6f, 0.6f, 0.0f };		glLightfv(GL_LIGHT0, GL_DIFFUSE, DiffuseColor);
	float SpecularColor[] = { 0.7f, 0.7f, 0.7f, 0.0f };		glLightfv(GL_LIGHT0, GL_SPECULAR, SpecularColor);
	float Position[]      = { 10.0f, 200.0f, 15.0f, 0.0f };	glLightfv(GL_LIGHT0, GL_POSITION, Position);
	glEnable(GL_LIGHT0);

	// Initialize physics scene and start the application main loop if scene was created
	if (InitNx())
	{
		atexit(ReleaseNx);
		glutMainLoop();
	}
	else
		ReleaseNx();
}

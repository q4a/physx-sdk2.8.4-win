#include "ActorPicking.h"

extern NxScene*				gScene;
extern NxDistanceJoint*		gMouseJoint;
extern NxActor*				gMouseSphere;
extern NxReal				gMouseDepth;
extern NxActor*				gSelectedActor;

#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif


// ------------------------------------------------------------------------------------

void ViewProject(NxVec3 &v, int &xi, int &yi, float &depth)
{
//We cannot do picking easily on the xbox/PS3/WII anyway
#if defined(_XBOX) || defined (__CELLOS_LV2__ )|| defined(__PPCGEKKO__)
	xi=yi=0;
	depth=0.0f;
#else
	GLint viewPort[4];
	GLdouble modelMatrix[16];
	GLdouble projMatrix[16];
	glGetIntegerv(GL_VIEWPORT, viewPort);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
	GLdouble winX, winY, winZ;
	gluProject((GLdouble) v.x, (GLdouble) v.y, (GLdouble) v.z,
		modelMatrix, projMatrix, viewPort, &winX, &winY, &winZ);
	xi = (int)winX; yi = viewPort[3] - (int)winY - 1; depth = (float)winZ;
#endif
}

// ------------------------------------------------------------------------------------

void ViewUnProject(int xi, int yi, float depth, NxVec3 &v)
{
//We cannot do picking easily on the xbox/PS3/WII anyway
#if defined(_XBOX) || defined (__CELLOS_LV2__) || defined(__PPCGEKKO__)
	v=NxVec3(0.0f,0.0f,0.0f);
#else
	GLint viewPort[4];
	GLdouble modelMatrix[16];
	GLdouble projMatrix[16];
	glGetIntegerv(GL_VIEWPORT, viewPort);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
	yi = viewPort[3] - yi - 1;
	GLdouble wx, wy, wz;
	gluUnProject((GLdouble) xi, (GLdouble) yi, (GLdouble) depth,
	modelMatrix, projMatrix, viewPort, &wx, &wy, &wz);
	v.set((NxReal)wx, (NxReal)wy, (NxReal)wz);
#endif
}

// ------------------------------------------------------------------------------------

void LetGoActor()
{
	if (gMouseJoint) 
		gScene->releaseJoint(*gMouseJoint);
	gMouseJoint = NULL;
	if (gMouseSphere)
		gScene->releaseActor(*gMouseSphere);
	gMouseSphere = NULL;
}

// ------------------------------------------------------------------------------------

bool PickActor(int x, int y)
{
#if !defined(_XBOX) && !defined (__CELLOS_LV2__) && !defined(__PPCGEKKO__)// no picking on consoles
	LetGoActor();

	NxRay ray; 
	ViewUnProject(x,y,0.0f, ray.orig);
	ViewUnProject(x,y,1.0f, ray.dir);
	ray.dir -= ray.orig; ray.dir.normalize();

	NxRaycastHit hit;
	NxShape* closestShape = gScene->raycastClosestShape(ray, NX_ALL_SHAPES, hit);
	if (!closestShape) return false;
	if (!closestShape->getActor().isDynamic()) return false;
	int hitx, hity;
	ViewProject(hit.worldImpact, hitx, hity, gMouseDepth);
	gMouseSphere = CreateSphere(hit.worldImpact, 0.1f, 1.0f);

	gMouseSphere->raiseBodyFlag(NX_BF_KINEMATIC);
	gMouseSphere->raiseActorFlag(NX_AF_DISABLE_COLLISION);
	NxDistanceJointDesc desc;
	gSelectedActor = &closestShape->getActor();
	gSelectedActor->wakeUp();

	desc.actor[0] = gMouseSphere;
#ifdef USE_NX_DOUBLE_BUFFERED
	desc.actor[1] = static_cast<NxdActor*>(gSelectedActor->userData);
#else
	desc.actor[1] = gSelectedActor;
#endif

	gMouseSphere->getGlobalPose().multiplyByInverseRT(hit.worldImpact, desc.localAnchor[0]);
	gSelectedActor->getGlobalPose().multiplyByInverseRT(hit.worldImpact, desc.localAnchor[1]);
	desc.maxDistance = 0.0f;
	desc.minDistance = 0.0f;
	desc.spring.damper = 1.0f;
	desc.spring.spring = 200.0f;
	desc.flags |= NX_DJF_MAX_DISTANCE_ENABLED | NX_DJF_SPRING_ENABLED;
	NxJoint* joint = gScene->createJoint(desc);
	gMouseJoint = (NxDistanceJoint*)joint->is(NX_JOINT_DISTANCE);

	return true;
#else
    return false; // do this for xbox and PS3
#endif
}

// ------------------------------------------------------------------------------------

void MoveActor(int x, int y)
{
	if (!gMouseSphere) return;
	NxVec3 pos;
	ViewUnProject(x,y, gMouseDepth, pos);
	gMouseSphere->setGlobalPosition(pos);
	gSelectedActor->wakeUp();
}


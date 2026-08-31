#ifndef PHYSICSTHREAD_H
#define PHYSICSTHREAD_H
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/


#include "Allocateable.h"
#include "NxThread.h"

class NpScene;
class NxSync;
class NxMutex;


class PhysicsThread: public NxThread, public NxFoundation::NxAllocateable
{
public:
	/* should set up the physics thread & wait on the sync */

	PhysicsThread(NxSync &physicsStart,NpScene *scene);
	
	virtual void execute();

private:
	NxSync &mPhysicsStart; 
	NpScene* mScene;
};

#endif

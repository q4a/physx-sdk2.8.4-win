#ifndef NX_COLLISION_CORE_ACTOR
#define NX_COLLISION_CORE_ACTOR
/*----------------------------------------------------------------------------*\
|
|							  NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "PxActor.h"

class Scene;

class CoreActor : public PxActor
{
public:
	CoreActor(Scene& scene, NxU32 actorType);
	~CoreActor();
	NX_INLINE virtual void initialize();
	NX_INLINE virtual void destroy();
	NX_INLINE Scene& getScene() const;
private:
	Scene& mScene;
};

//////////////////////////////////////////////////////////////////////////

NX_INLINE void CoreActor::initialize()
{
	PxActor::initialize();
}

NX_INLINE void CoreActor::destroy()
{
	PxActor::destroy();
}

NX_INLINE Scene& CoreActor::getScene() const
{
	return mScene;
}

#endif

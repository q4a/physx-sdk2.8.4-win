#ifndef NX_PHYSICS_STATIC_ACTOR
#define NX_PHYSICS_STATIC_ACTOR
/*----------------------------------------------------------------------------*\
|
|							  NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/
#include "NxAllocateable.h"
#include "RbActor.h"
#include "Nxp.h"

class NxScene;
class NxActor;
class Shape;

class StaticActor : public RbActor, public NxFoundation::NxAllocateable
{
public:
	StaticActor(Scene &scene, NxActor* nxActor);

	virtual bool canDeactivate() const;
	virtual bool canActivate() const;
	virtual bool isIslandGenerating() const;

protected:
	virtual void onActivate();
	virtual void onDeactivate();

private:
	~StaticActor();
};


#endif

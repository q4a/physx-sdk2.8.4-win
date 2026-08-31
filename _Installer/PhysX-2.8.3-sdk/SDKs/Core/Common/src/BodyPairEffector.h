#ifndef NX_PHYSICS_BODYPAIREFFECTOR
#define NX_PHYSICS_BODYPAIREFFECTOR
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Effector.h"

//namespace NxRigidBody
//	{

class Body;
class Scene;
/**
 This class doesn't expose an interface.
*/
class BodyPairEffector : public Effector
	{
	public:
	BodyPairEffector(Scene & s);
	virtual ~BodyPairEffector();

	virtual void apply();
	virtual void applyToPair(Body * b1, Body * b2) = 0;

	virtual void event(NxU32 e, NxFoundation::Observable & o);

	protected:
	void setBodies(Body *, Body *);

	Body * body1;
	Body * body2;
	};
//	}

#endif

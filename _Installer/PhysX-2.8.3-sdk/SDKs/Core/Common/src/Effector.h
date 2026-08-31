#ifndef NX_PHYSICS_EFFECTOR
#define NX_PHYSICS_EFFECTOR
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Physics.h"
#include "Observable.h"
#include "Allocateable.h"
#include "NvSpringAndDamperEffector.h"

//namespace NxRigidBody
//	{

class NvScene;
class SpringAndDamperEffector;
class NpEffector;
class Scene;

class Effector : public NvSpringAndDamperEffector, public NxFoundation::Observable, public NxFoundation::NxAllocateable
	{
	public:	
	NX_INLINE	NvSpringAndDamperEffector*	getNext()	{ return next; }

	//group access:
											Effector(Scene& ownerScene);
	virtual									~Effector();
	virtual		NvScene&					getNvScene();

	virtual		void						apply() = 0;
	NX_INLINE	void						setNpEffector(NpSpringAndDamperEffector* npJ)	{ mNpEffector = npJ;	}
	NX_INLINE	NpSpringAndDamperEffector*	getNpEffector()									{ return mNpEffector;	}
				Scene&						getScene() const;
	public:
				Effector*					next;		//the scene's effector linked list
	private:
				Scene&						ownerScene;		//this used to be in our observer list.
				NpSpringAndDamperEffector*	mNpEffector;
	};

#endif

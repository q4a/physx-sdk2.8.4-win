#ifndef NX_PHYSICS_NP_JOINT
#define NX_PHYSICS_NP_JOINT
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/
#include "Physics.h"
#include "ReadWriteMutex.h"
#include "NxJoint.h"
#include "NvJoint.h"
#include "DebugRenderable.h"
class NxJointDesc;
class Joint;
class NvBody;
class NpActor;
class NpScene;
class NvScene;

class NpJoint : public ReadWriteMutex
	{
	public:
	NpJoint();
	virtual ~NpJoint();		//virtual destructor so we can delete Np*Joint objects ezly.
	virtual void initialize(const NxJointDesc&);	//load from desc on startup.

	//void loadFromDesc(const NxJointDesc&, NvJointDesc & nvDesc);
	
	void loadFromDesc(const NxJointDesc&, NvJointDesc & nvDesc, const NvJointPrismaticDesc &);
	void loadFromDesc(const NxJointDesc&, NvJointDesc & nvDesc, const NvJointCylindricalDesc &);
	void loadFromDesc(const NxJointDesc&, NvJointDesc & nvDesc, const NvJointPointOnLineDesc &);
	void loadFromDesc(const NxJointDesc&, NvJointDesc & nvDesc, const NvJointPointOnPlaneDesc &);
	void loadFromDesc(const NxJointDesc&, NvJointDesc & nvDesc, const NvJointFixedDesc &);
	void loadFromDesc(const NxJointDesc&, NvJointDesc & nvDesc, const NvJointRevoluteDesc &);
	void loadFromDesc(const NxJointDesc&, NvJointDesc & nvDesc, const NvJointSphereicalDesc &);
	void loadFromDesc(const NxJointDesc&, NvJointDesc & nvDesc, const NvJointDistanceDesc &);
	void loadFromDesc(const NxJointDesc&, NvJointDesc & nvDesc, const NvJointPulleyDesc &);
	void loadFromDesc(const NxJointDesc&, NvJointDesc & nvDesc, const NvJointD6Desc &);
	
	
	void saveToDesc(NxJointDesc &, void * userData);

	void getGlobalAxis(NxVec3 & dest) const;
	void getGlobalAnchor(NxVec3& d) const;
	void setGlobalAxis(const NxVec3& axisIn);
	void setGlobalAnchor(const NxVec3& d);

	void setBreakable(NxReal l, NxReal a);
	void setSolverExtrapolationFactor(NxReal solverExtrapolationFactor);
	void setUseAccelerationSpring(bool b);
	bool addLimitPlane(const NxVec3 & normal, const NxVec3 & pointInPlane, NxReal restitution);
	void wakeUp();
	NpScene * getNpScene() const;									//only works if we have at least one actor (i.e. not broken)!

	NX_INLINE	NxJoint	* getNxJoint()		{ return reinterpret_cast<NxJoint *>(reinterpret_cast<char *>(this) - sizeof(NxJoint)); }	//AM: the types are always related, but this isn't coded that way because of stupid C++ virtual inheritance issues. There is an assert in the joint ctors (loadFromDesc) that make sure that this cast is legit.

	void is(NxJointType type);
	virtual void visualize(NxFoundation::DebugRenderable &);
	virtual	void sendToVRD(bool create, bool fromCtor, bool callBase);
			void removeFromVRD();
	
	NvScene * creationScene; //needed for HSM/when removing joint
	private:
	void loadFromDesc(const NxJointDesc&, NvJointDesc & nvDesc);

	protected:
		NX_INLINE	NX_BOOL			isBroken()	{ return (joint->getState() == NX_JS_BROKEN);	}

	NpActor * actors[2];
	NvJoint	* joint;
	NpScene * scene;
	
	NxU32 limitPlaneCount;
	};

#endif

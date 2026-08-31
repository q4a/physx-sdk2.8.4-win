#ifndef NX_PHYSICS_HINGEJOINT
#define NX_PHYSICS_HINGEJOINT
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Joint.h"
#include "NxJointLimitPairDesc.h"
#include "NxMotorDesc.h"
#include "NxSpringDesc.h"

class RevoluteJoint : public Joint/*, public NxRevoluteJoint*/
	{
	public:
	virtual void release();
	virtual		void						loadFromDesc(const NvJointDesc &, const NvJointRevoluteDesc &);

	virtual		void						setSolverExtrapolationFactor(NxReal sef);
	virtual		void						setUseAccelerationSpring(bool b);

			void setLimits(const NxJointLimitPairDesc &);
			bool getLimits(NxJointLimitPairDesc &);
			void setMotor(const NxMotorDesc &);
			bool getMotor(NxMotorDesc &) const;
			void setSpring(const NxSpringDesc &);
			bool getSpring(NxSpringDesc &) const;

	NX_INLINE void	setProjection(NxReal dist, NxReal angle) { projectionDistance = dist;  projectionAngleCosine = NxMath::cos(angle);	projectionAngleSine = NxMath::sin(angle);	/*please don't optimize this, its only for loading...*/ }
	NX_INLINE void	getProjection(NxReal & dist, NxReal & angle) const { dist = projectionDistance; angle = NxMath::acos(projectionAngleCosine); }


			NxReal	getAngle() const;
			NxReal	getVelocity() const;
	// double buffering
			NxReal	getAngleExternal() const;
//			NxReal	getVelocityExternal();
			void	setFlags(NxU32 flags);
	NX_INLINE NxU32	getFlags() const { return revoluteFlags; }

	RevoluteJoint(const NvJointDesc & desc, const NvJointRevoluteDesc &);
	~RevoluteJoint();

	virtual void processToAxisConstraints(NxReal dt);
	virtual void visualize(NxFoundation::DebugRenderable &);
	virtual	void projectPose(Body * childBody);

	virtual void disableAxCons();

	private:
	NxJointLimitPairDesc limit;		
	NxMotorDesc			 motor;		
	NxSpringDesc		 spring;	
	NxReal	projectionDistance;	
	NxReal	projectionAngleCosine;
	NxReal	projectionAngleSine;
	NxU32	revoluteFlags;					// This is a combination of the bits defined by ::NxRevoluteJointFlag . 

	private:
		enum
		{
			D6_CON = 0,
			ANG_CON_MS,
			LAST_CON,
		};
	PxdHandle constraints[LAST_CON];

	
	};

#endif

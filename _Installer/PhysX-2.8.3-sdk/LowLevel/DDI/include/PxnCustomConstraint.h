#ifndef PXN_CUSTOMCONSTRAINT_H
#define PXN_CUSTOMCONSTRAINT_H

#include "px_dynamics.h"

class PxnBodyAtom;

class PxnCustomConstraint
{
public:
	// General methods
	virtual PxdHandle				getHandleV()									const		= 0;
	virtual void					destroyV()													= 0;

	// General properties
	virtual void					getAtomsV(PxnBodyAtom*& a0, PxnBodyAtom*& a1)	const		= 0;

	virtual PxdConstraintType		getTypeV()										const		= 0;

	virtual const PxdVector&		getPoint0V()									const		= 0;
	virtual void					setPoint0V(const PxdVector& v)								= 0;

	virtual const PxdVector&		getPoint1V()									const		= 0;
	virtual void					setPoint1V(const PxdVector& v)								= 0;

	virtual const PxdVector&		getAxis0V()										const		= 0;
	virtual void					setAxis0V(const PxdVector& v)								= 0;

	virtual const PxdVector&		getAxis1V()										const		= 0;
	virtual void					setAxis1V(const PxdVector& v)								= 0;

	virtual PxFloat					getErrorV()										const		= 0;
	virtual	void					setErrorV(PxFloat f)										= 0;

	virtual PxFloat					getTargetVelocityV()							const		= 0;
	virtual void					setTargetVelocityV(PxFloat f)								= 0;

	virtual PxFloat					getMinImpulseV()								const		= 0;
	virtual void					setMinImpulseV(PxFloat f)									= 0;

	virtual PxFloat					getMaxImpulseV()								const		= 0;
	virtual void					setMaxImpulseV(PxFloat f)									= 0;

	virtual PxFloat					getImpulseMagnitudeV()							const		= 0;
    
	virtual PxFloat					getImplicitSpringV()							const		= 0;
	virtual void					setImplicitSpringV(PxFloat f)								= 0;

	virtual PxFloat					getImplicitDamperV()							const		= 0;
	virtual void					setImplicitDamperV(PxFloat f)								= 0;

	virtual bool					getImpulseReportV()								const		= 0;
	virtual void					setImpulseReportV(bool b)									= 0;

	virtual PxFloat					getDominance0V()								const		= 0;
	virtual void					setDominance0V(PxFloat)										= 0;

	virtual PxFloat					getDominance1V()								const		= 0;
	virtual void					setDominance1V(PxFloat)										= 0;

};


class PxnContactConstraint
{
public:
	virtual PxdHandle				getHandleV()									const		= 0;
	virtual void					destroyV()													= 0;

	virtual void					getAtomsV(PxnBodyAtom*& a0, PxnBodyAtom*& a1)	const		= 0;

	virtual PxdTransform			getFrame0V()									const		= 0;
	virtual void					setFrame0V(const PxdTransform& t)							= 0;

	virtual PxdTransform			getFrame1V()									const		= 0;
	virtual void					setFrame1V(const PxdTransform& t)							= 0;

	virtual PxdVector				getErrorV()										const		= 0;
	virtual void					setErrorV(const PxdVector& v)								= 0;

	virtual PxdVector				getTargetVelocityV()							const		= 0;
	virtual void					setTargetVelocityV(const PxdVector& v)						= 0;

	virtual PxdVector				getImpulseV()									const		= 0;

	virtual PxFloat					getMinImpulseV()								const		= 0;
	virtual void					setMinImpulseV(PxFloat f)									= 0;

	virtual PxFloat					getMaxImpulseV()								const		= 0;
	virtual void					setMaxImpulseV(PxFloat f)									= 0;

	virtual PxFloat					getStaticFriction0V()							const		= 0;
	virtual void					setStaticFriction0V(PxFloat f)								= 0;

	virtual PxFloat					getStaticFriction1V()							const		= 0;
	virtual void					setStaticFriction1V(PxFloat f)								= 0;

	virtual PxFloat					getDynamicFriction0V()							const		= 0;
	virtual void					setDynamicFriction0V(PxFloat f)								= 0;

	virtual PxFloat					getDynamicFriction1V()							const		= 0;
	virtual void					setDynamicFriction1V(PxFloat f)								= 0;

	virtual PxFloat					getRestitutionV()								const		= 0;
	virtual void					setRestitutionV(PxFloat f)									= 0;

	virtual bool					getAnisotropicV()								const		= 0;
	virtual void					setAnisotropicV(bool b)										= 0;

	virtual bool					getImpulseReportV()								const		= 0;
	virtual void					setImpulseReportV(bool b)									= 0;

	virtual PxInt					getFeature0V()									const		= 0;
	virtual PxInt					getFeature1V()									const		= 0;

	virtual PxFloat					getDominance0V()								const		= 0;
	virtual void					setDominance0V(PxFloat)										= 0;

	virtual PxFloat					getDominance1V()								const		= 0;
	virtual void					setDominance1V(PxFloat)										= 0;
};


class PxnD6Joint
{
public:
	virtual PxdHandle				getHandleV()									const		= 0;
	virtual void					destroyV()													= 0;

	virtual void					getAtomsV(PxnBodyAtom*& a0, PxnBodyAtom*& a1)	const		= 0;

	virtual const PxdTransform&		getFrame0V()									const		= 0;
	virtual void					setFrame0V(const PxdTransform& t)							= 0;

	virtual const PxdTransform&		getFrame1V()									const		= 0;
	virtual void					setFrame1V(const PxdTransform& t)							= 0;

	virtual PxInt					getJointTypeV()									const		= 0;
	virtual void					setJointTypeV(PxInt i)										= 0;

	virtual PxInt					getDriveTypeV()									const		= 0;
	virtual void					setDriveTypeV(PxInt i)										= 0; 

	virtual PxFloat					getLimitValueV(PxdD6JointSubProperty p)			const		= 0;
	virtual void					setLimitValueV(PxdD6JointSubProperty p, PxFloat f)			= 0;

	virtual PxFloat					getLimitSpringV(PxdD6JointSubProperty p)		const		= 0;
	virtual void					setLimitSpringV(PxdD6JointSubProperty p, PxFloat f)			= 0;

	virtual PxFloat					getLimitDampingV(PxdD6JointSubProperty p)		const		= 0;
	virtual void					setLimitDampingV(PxdD6JointSubProperty p, PxFloat f)		= 0;

	virtual PxFloat					getLimitRestitutionV(PxdD6JointSubProperty p)	const		= 0;
	virtual void					setLimitRestitutionV(PxdD6JointSubProperty p, PxFloat f)	= 0;

	virtual PxFloat					getDriveSpringV(PxdD6JointSubProperty p)		const		= 0;
	virtual void					setDriveSpringV(PxdD6JointSubProperty p, PxFloat f)			= 0;

	virtual PxFloat					getDriveDampingV(PxdD6JointSubProperty p)		const		= 0;
	virtual void					setDriveDampingV(PxdD6JointSubProperty p, PxFloat f)		= 0;

	virtual PxFloat					getDriveLimitV(PxdD6JointSubProperty p)			const		= 0;
	virtual void					setDriveLimitV(PxdD6JointSubProperty p, PxFloat f)			= 0;

	virtual PxFloat					getDriveGearV()									const		= 0;
	virtual void					setDriveGearV(PxFloat f)									= 0;

	virtual const PxdTransform&		getDrivePositionV()								const		= 0;
	virtual void					setDrivePositionV(const PxdTransform& t)					= 0;

	virtual void					getDriveVelocityV(PxdVector& lin, PxdVector& ang) const		= 0;
	virtual void					setDriveVelocityV(const PxdVector& lin, const PxdVector& ang)= 0;

	virtual PxFloat					getMaxLinearImpulseV()							const		= 0;
	virtual void					setMaxLinearImpulseV(PxFloat f)								= 0;

	virtual PxFloat					getMaxAngularImpulseV()							const		= 0;
	virtual void					setMaxAngularImpulseV(PxFloat f)							= 0;

	virtual void					getImpulseV(PxdVector& lin, PxdVector& ang) const			= 0;

	virtual bool					getImpulseReportV()								const		= 0;
	virtual void					setImpulseReportV(bool b)									= 0;

	virtual PxFloat					getSolverExtrapolationFactorV()					const		= 0;
	virtual void					setSolverExtrapolationFactorV(PxFloat f)					= 0;

	virtual bool					getUseAccelerationSpringsV()					const		= 0;
	virtual void					setUseAccelerationSpringsV(bool b)							= 0;

	virtual bool					getConstrainPerpendicularDirsV()				const		= 0;
	virtual void					setConstrainPerpendicularDirsV(bool b)						= 0;

	virtual bool					getJointBrokenV()								const		= 0;

	virtual PxFloat					getDominance0V()								const		= 0;
	virtual void					setDominance0V(PxFloat)										= 0;

	virtual PxFloat					getDominance1V()								const		= 0;
	virtual void					setDominance1V(PxFloat)										= 0;

};


#endif

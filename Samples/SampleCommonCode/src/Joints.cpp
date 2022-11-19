/*----------------------------------------------------------------------------*\
|
|						    NVIDIA PhysX Technology
|
|							    www.nvidia.com
|
\*----------------------------------------------------------------------------*/

//Joint creation code for sample programs

#include "Joints.h"

extern NxScene* gScene;

NxFixedJoint* CreateFixedJoint(NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis)
{
	NxFixedJointDesc fixedDesc;
	fixedDesc.actor[0] = a0;
	fixedDesc.actor[1] = a1;
	fixedDesc.setGlobalAnchor(globalAnchor);
	fixedDesc.setGlobalAxis(globalAxis);

	return static_cast<NxFixedJoint*>(gScene->createJoint(fixedDesc));
}

NxRevoluteJoint* CreateRevoluteJoint(NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis)
{
	NxRevoluteJointDesc revDesc;
	revDesc.actor[0] = a0;
	revDesc.actor[1] = a1;
	revDesc.setGlobalAnchor(globalAnchor);
	revDesc.setGlobalAxis(globalAxis);

	return static_cast<NxRevoluteJoint*>(gScene->createJoint(revDesc));
}

NxSphericalJoint* CreateSphericalJoint(NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis)
{
	NxSphericalJointDesc sphericalDesc;
	sphericalDesc.actor[0] = a0;
	sphericalDesc.actor[1] = a1;
	sphericalDesc.setGlobalAnchor(globalAnchor);
	sphericalDesc.setGlobalAxis(globalAxis);

	return static_cast<NxSphericalJoint*>(gScene->createJoint(sphericalDesc));
}

NxPrismaticJoint* CreatePrismaticJoint(NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis)
{
	NxPrismaticJointDesc prismaticDesc;
	prismaticDesc.actor[0] = a0;
	prismaticDesc.actor[1] = a1;
	prismaticDesc.setGlobalAnchor(globalAnchor);
	prismaticDesc.setGlobalAxis(globalAxis);

	NxJoint* joint = gScene->createJoint(prismaticDesc);
	
//	joint->setLimitPoint(globalAnchor);
//	joint->addLimitPlane(-globalAxis, globalAnchor + 1.5*globalAxis);
//	joint->addLimitPlane(globalAxis, globalAnchor - 1.5*globalAxis);

	return static_cast<NxPrismaticJoint*>(joint);
}

NxCylindricalJoint* CreateCylindricalJoint(NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis)
{
	NxCylindricalJointDesc cylDesc;
	cylDesc.actor[0] = a0;
	cylDesc.actor[1] = a1;
	cylDesc.setGlobalAnchor(globalAnchor);
	cylDesc.setGlobalAxis(globalAxis);

	NxJoint* joint = gScene->createJoint(cylDesc);

	return static_cast<NxCylindricalJoint*>(joint);
}

NxPointOnLineJoint* CreatePointOnLineJoint(NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis)
{
	NxPointOnLineJointDesc polDesc;
	polDesc.actor[0] = a0;
	polDesc.actor[1] = a1;
	polDesc.setGlobalAnchor(globalAnchor);
	polDesc.setGlobalAxis(globalAxis);
	polDesc.jointFlags |= NX_JF_COLLISION_ENABLED;

	NxJoint* joint = gScene->createJoint(polDesc);

	return static_cast<NxPointOnLineJoint*>(joint);
}

NxPointInPlaneJoint* CreatePointInPlaneJoint(NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis)
{
	NxPointInPlaneJointDesc pipDesc;
	pipDesc.actor[0] = a0;
	pipDesc.actor[1] = a1;
	pipDesc.setGlobalAnchor(globalAnchor);
	pipDesc.setGlobalAxis(globalAxis);
	pipDesc.jointFlags |= NX_JF_COLLISION_ENABLED;

	NxJoint* joint = gScene->createJoint(pipDesc);

	return static_cast<NxPointInPlaneJoint*>(joint);
}

NxSphericalJoint* CreateRopeSphericalJoint(NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis)
{
	NxSphericalJointDesc sphericalDesc;
	sphericalDesc.actor[0] = a0;
	sphericalDesc.actor[1] = a1;
	sphericalDesc.setGlobalAnchor(globalAnchor);
	sphericalDesc.setGlobalAxis(globalAxis);

	sphericalDesc.flags |= NX_SJF_TWIST_LIMIT_ENABLED;
	sphericalDesc.twistLimit.low.value = -(NxReal)0.1*NxPi;
	sphericalDesc.twistLimit.high.value = (NxReal)0.1*NxPi;

	sphericalDesc.flags |= NX_SJF_TWIST_SPRING_ENABLED;
	NxSpringDesc ts;
	ts.spring = 500;
	ts.damper = 0.5;
	ts.targetValue = 0;
	sphericalDesc.twistSpring = ts;

	sphericalDesc.flags |= NX_SJF_SWING_LIMIT_ENABLED;
	sphericalDesc.swingLimit.value = (NxReal)0.3*NxPi;

	sphericalDesc.flags |= NX_SJF_SWING_SPRING_ENABLED;
	NxSpringDesc ss;
	ss.spring = 500;
	ss.damper = 0.5;
	ss.targetValue = 0;
	sphericalDesc.swingSpring = ss;

	return static_cast<NxSphericalJoint*>(gScene->createJoint(sphericalDesc));
}

NxSphericalJoint* CreateClothSphericalJoint(NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis)
{
	NxSphericalJointDesc sphericalDesc;
	sphericalDesc.actor[0] = a0;
	sphericalDesc.actor[1] = a1;
	sphericalDesc.setGlobalAnchor(globalAnchor);
	sphericalDesc.setGlobalAxis(globalAxis);

	sphericalDesc.flags |= NX_SJF_TWIST_LIMIT_ENABLED;
	sphericalDesc.twistLimit.low.value = -(NxReal)0.025*NxPi;
	sphericalDesc.twistLimit.low.hardness = 0.5;
	sphericalDesc.twistLimit.low.restitution = 0.5;
	sphericalDesc.twistLimit.high.value = (NxReal)0.025*NxPi;
	sphericalDesc.twistLimit.high.hardness = 0.5;
	sphericalDesc.twistLimit.high.restitution = 0.5;

	sphericalDesc.flags |= NX_SJF_SWING_LIMIT_ENABLED;
	sphericalDesc.swingLimit.value = (NxReal)0.25*NxPi;
	sphericalDesc.swingLimit.hardness = 0.5;
	sphericalDesc.swingLimit.restitution = 0.5;

	sphericalDesc.flags |= NX_SJF_TWIST_SPRING_ENABLED;
	sphericalDesc.twistSpring.spring = 0.5;
	sphericalDesc.twistSpring.damper = 1;

	sphericalDesc.flags |= NX_SJF_SWING_SPRING_ENABLED;
	sphericalDesc.swingSpring.spring = 0.5;
	sphericalDesc.swingSpring.damper = 1;

//	sphericalDesc.flags |= NX_SJF_JOINT_SPRING_ENABLED;
//	sphericalDesc.jointSpring.spring = 0.5;
//	sphericalDesc.jointSpring.damper = 1;

	sphericalDesc.projectionDistance = (NxReal)0.15;
	sphericalDesc.projectionMode = NX_JPM_POINT_MINDIST;

	return static_cast<NxSphericalJoint*>(gScene->createJoint(sphericalDesc));
}

NxSphericalJoint* CreateBodySphericalJoint(NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis)
{
	NxSphericalJointDesc sphericalDesc;
	sphericalDesc.actor[0] = a0;
	sphericalDesc.actor[1] = a1;
	sphericalDesc.setGlobalAnchor(globalAnchor);
	sphericalDesc.setGlobalAxis(globalAxis);

	sphericalDesc.flags |= NX_SJF_TWIST_LIMIT_ENABLED;
	sphericalDesc.twistLimit.low.value = -(NxReal)0.025*NxPi;
	sphericalDesc.twistLimit.low.hardness = 0.5;
	sphericalDesc.twistLimit.low.restitution = 0.5;
	sphericalDesc.twistLimit.high.value = (NxReal)0.025*NxPi;
	sphericalDesc.twistLimit.high.hardness = 0.5;
	sphericalDesc.twistLimit.high.restitution = 0.5;

	sphericalDesc.flags |= NX_SJF_SWING_LIMIT_ENABLED;
	sphericalDesc.swingLimit.value = (NxReal)0.25*NxPi;
	sphericalDesc.swingLimit.hardness = 0.5;
	sphericalDesc.swingLimit.restitution = 0.5;

	sphericalDesc.flags |= NX_SJF_TWIST_SPRING_ENABLED;
	sphericalDesc.twistSpring.spring = 0.5;
	sphericalDesc.twistSpring.damper = 1;

	sphericalDesc.flags |= NX_SJF_SWING_SPRING_ENABLED;
	sphericalDesc.swingSpring.spring = 0.5;
	sphericalDesc.swingSpring.damper = 1;

//	sphericalDesc.flags |= NX_SJF_JOINT_SPRING_ENABLED;
//	sphericalDesc.jointSpring.spring = 0.5;
//	sphericalDesc.jointSpring.damper = 1;

	sphericalDesc.projectionDistance = (NxReal)0.15;
	sphericalDesc.projectionMode = NX_JPM_POINT_MINDIST;

	return static_cast<NxSphericalJoint*>(gScene->createJoint(sphericalDesc));
}

NxRevoluteJoint* CreateWheelJoint(NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis)
{
	NxRevoluteJointDesc revDesc;
	revDesc.actor[0] = a0;
	revDesc.actor[1] = a1;
	revDesc.setGlobalAnchor(globalAnchor);
	revDesc.setGlobalAxis(globalAxis);

	return static_cast<NxRevoluteJoint*>(gScene->createJoint(revDesc));
}

NxRevoluteJoint* CreateStepJoint(NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis)
{
	NxRevoluteJointDesc revDesc;
	revDesc.actor[0] = a0;
	revDesc.actor[1] = a1;
	revDesc.setGlobalAnchor(globalAnchor);
	revDesc.setGlobalAxis(globalAxis);

	return static_cast<NxRevoluteJoint*>(gScene->createJoint(revDesc));
}

NxRevoluteJoint* CreateChassisJoint(NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis)
{
	NxRevoluteJointDesc revDesc;
	revDesc.actor[0] = a0;
	revDesc.actor[1] = a1;
	revDesc.setGlobalAnchor(globalAnchor);
	revDesc.setGlobalAxis(globalAxis);

	revDesc.flags |= NX_RJF_LIMIT_ENABLED;

	NxJointLimitPairDesc limitDesc;
	limitDesc.high.value = (NxReal)0.01*NxPi;
	limitDesc.low.value = -(NxReal)0.01*NxPi;;

	revDesc.limit = limitDesc;

	revDesc.flags |= NX_RJF_SPRING_ENABLED;
	NxSpringDesc springDesc;
	springDesc.targetValue = 0;
	springDesc.spring = 5000;

//	motorDesc.maxForce = 100;
//	motorDesc.freeSpin = 0;
//
//	revDesc.motor = motorDesc;

	return static_cast<NxRevoluteJoint*>(gScene->createJoint(revDesc));
}

NxFixedJoint* CreateCannonJoint(NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis)
{
	NxFixedJointDesc fixDesc;

	fixDesc.actor[0] = a0;
	fixDesc.actor[1] = a1;
	fixDesc.setGlobalAnchor(globalAnchor);
	fixDesc.setGlobalAxis(globalAxis);

	return static_cast<NxFixedJoint*>(gScene->createJoint(fixDesc));
}

NxSphericalJoint* CreateBladeLink(NxActor* a0, NxActor* a1, const NxVec3& globalAnchor, const NxVec3& globalAxis)
{
	NxSphericalJointDesc sphericalDesc;
	sphericalDesc.actor[0] = a0;
	sphericalDesc.actor[1] = a1;
	sphericalDesc.setGlobalAnchor(globalAnchor);
	sphericalDesc.setGlobalAxis(globalAxis);

	sphericalDesc.flags |= NX_SJF_SWING_LIMIT_ENABLED;
	sphericalDesc.swingLimit.value = (NxReal)0.05*NxPi;
	sphericalDesc.swingLimit.restitution = 0.75;
	sphericalDesc.swingLimit.hardness = 0.5;

	sphericalDesc.flags |= NX_SJF_SWING_SPRING_ENABLED;
	sphericalDesc.swingSpring.spring = 0.75;
	sphericalDesc.swingSpring.damper = 1;
	
	sphericalDesc.flags |= NX_SJF_TWIST_LIMIT_ENABLED;
	sphericalDesc.twistLimit.low.value = -(NxReal)0.05*NxPi;
	sphericalDesc.twistLimit.low.restitution = 0.75;
	sphericalDesc.twistLimit.low.hardness = 0.5;
	sphericalDesc.twistLimit.high.value = (NxReal)0.05*NxPi;
	sphericalDesc.twistLimit.high.restitution = 0.75;
	sphericalDesc.twistLimit.high.hardness = 0.5;

	sphericalDesc.flags |= NX_SJF_TWIST_SPRING_ENABLED;
	sphericalDesc.twistSpring.spring = 0.75;
	sphericalDesc.twistSpring.damper = 1;

	return static_cast<NxSphericalJoint*>(gScene->createJoint(sphericalDesc));
}




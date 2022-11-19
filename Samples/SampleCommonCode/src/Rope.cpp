#include "rope.h"

Rope::Rope(NxScene& nxScene,NxVec3& startPosition, int numSegments, float segLength, float segMass,float segCollisionRadius,
			float projectionRatio)
			:scene(&nxScene),
			numSegs(numSegments),
			segmentLength(segLength),
			baseMassOfRopeSegment(segMass),
			segmentCollisionRadius(segCollisionRadius),
			startingPos(startPosition),
			projectionDistanceRampRate(0.0005),
			baseProjectionDistance(segLength * projectionRatio),
			randomMassCoefficient(0.003),
			internalSpringDamping(50.0f),
			segmentLinearDamping(0.1f),
			segmentAngularDamping(0.1f),
			momentOfInertiaConstant(2.0f)

{
	


	NxVec3 segmentPosDelta(segmentLength, 0.0f, 0.0);
    NxBodyDesc segmentBodyDesc;
	segmentBodyDesc.angularDamping   = 0.1f;
    
	NxSphereShapeDesc SphereDesc;
    SphereDesc.radius         = segCollisionRadius;

    NxActorDesc ActorDesc;
    ActorDesc.shapes.pushBack(&SphereDesc);
    ActorDesc.body            = &segmentBodyDesc; 
	ActorDesc.density		  = 1.0f;
	 
	ropeSegments = new NxActor*[numSegments];
	for(int i = 0; i < numSegments; i++)
	{
		ActorDesc.globalPose.t    = startingPos + i * segmentPosDelta;
		ropeSegments[i]  = scene->createActor(ActorDesc);
		int randMass = (rand() % 100);
		ropeSegments[i]->setMass(baseMassOfRopeSegment + randMass * randomMassCoefficient);
		ropeSegments[i]->setMassSpaceInertiaTensor(NxVec3(momentOfInertiaConstant, momentOfInertiaConstant, momentOfInertiaConstant));
		ropeSegments[i]->setAngularDamping(segmentAngularDamping);
		ropeSegments[i]->setLinearDamping(segmentLinearDamping);
		ropeSegments[i]->wakeUp(1e10);

		NxSphericalJointDesc sphericalDesc;			
		// some damping on twist and swing springs helps smooth out the rope motion
		sphericalDesc.flags |= NX_SJF_SWING_SPRING_ENABLED;
		sphericalDesc.flags |= NX_SJF_TWIST_SPRING_ENABLED;
		
		sphericalDesc.swingSpring.spring = 0.0f;
		sphericalDesc.swingSpring.damper = internalSpringDamping;
		sphericalDesc.twistSpring.spring = 1.0f;
		sphericalDesc.twistSpring.damper = internalSpringDamping;
		
		// projection is critical to manage stretch; if it is too tight, energy dissipates quickly.
		sphericalDesc.projectionMode = NX_JPM_POINT_MINDIST;
		sphericalDesc.projectionDistance = baseProjectionDistance + (i * projectionDistanceRampRate);		
		
		if(i>0)
		{
			sphericalDesc.actor[0] = ropeSegments[i];
			sphericalDesc.actor[1] =  ropeSegments[i-1];
			sphericalDesc.setGlobalAnchor(startingPos + (i-1) * segmentPosDelta);
			scene->createJoint(sphericalDesc);
		}
	}		
}
Rope::~Rope()
{
	for(int i = 0; i < numSegs; i++)
	{
		scene->releaseActor(*ropeSegments[i]);
	}
	delete[] ropeSegments;
}

NxActor*	Rope::getSegment(int segIndex)	
{
	if(segIndex < numSegs)
		return ropeSegments[segIndex];
	else
		return NULL;
}
int			Rope::getNumSegments()
{
	return numSegs;
}
NxActor*	Rope::getFirstSegment()
{
	if(ropeSegments != NULL)
		return ropeSegments[0];
	else
		return NULL;
}

NxActor*    Rope::getLastSegment()
{
	if(ropeSegments != NULL)
	{
		return ropeSegments[numSegs - 1];
	}
	else
		return NULL;
	
}

void		Rope::setUniformVelocity(NxVec3& vel)
{
}

NxJoint*		Rope::pinFirstSegmentInPlace()
{
	NxSphericalJointDesc sphericalDesc;			
	// some damping on twist and swing springs helps smooth out the rope motion
	//sphericalDesc.flags |= NX_SJF_SWING_SPRING_ENABLED;
	//sphericalDesc.flags |= NX_SJF_TWIST_SPRING_ENABLED;
	
	sphericalDesc.swingSpring.spring = 0.0f;
	sphericalDesc.swingSpring.damper = 10.0f;
	sphericalDesc.twistSpring.spring = 1.0f;
	sphericalDesc.twistSpring.damper = 10.0f;
	
	// projection is critical to manage stretch; if it is too tight, energy dissipates quickly.
	sphericalDesc.projectionMode = NX_JPM_POINT_MINDIST;
	sphericalDesc.projectionDistance = baseProjectionDistance;				
	sphericalDesc.actor[0] = ropeSegments[0];
	sphericalDesc.actor[1] = 0;
	sphericalDesc.setGlobalAnchor(ropeSegments[0]->getGlobalPosition());
	return scene->createJoint(sphericalDesc);
			

}
NxJoint*		Rope::pinLastSegmentInPlace()
{
	NxSphericalJointDesc sphericalDesc;			
	// some damping on twist and swing springs helps smooth out the rope motion
	//sphericalDesc.flags |= NX_SJF_SWING_SPRING_ENABLED;
	//sphericalDesc.flags |= NX_SJF_TWIST_SPRING_ENABLED;
	
	sphericalDesc.swingSpring.spring = 0.0f;
	sphericalDesc.swingSpring.damper = 10.0f;
	sphericalDesc.twistSpring.spring = 1.0f;
	sphericalDesc.twistSpring.damper = 10.0f;
	
	// projection is critical to manage stretch; if it is too tight, energy dissipates quickly.
	sphericalDesc.projectionMode = NX_JPM_POINT_MINDIST;
	sphericalDesc.projectionDistance = baseProjectionDistance;				
	sphericalDesc.actor[0] = ropeSegments[numSegs-1];
	sphericalDesc.actor[1] = 0;
	sphericalDesc.setGlobalAnchor(ropeSegments[numSegs-1]->getGlobalPosition());
	return scene->createJoint(sphericalDesc);

}
NxJoint*		Rope::pinSegmentToPosition(int segIndex, NxVec3& pos)
{
	if(segIndex < numSegs)
	{
	NxSphericalJointDesc sphericalDesc;			
	// some damping on twist and swing springs helps smooth out the rope motion
	sphericalDesc.flags |= NX_SJF_SWING_SPRING_ENABLED;
	sphericalDesc.flags |= NX_SJF_TWIST_SPRING_ENABLED;
	
	sphericalDesc.swingSpring.spring = 0.0f;
	sphericalDesc.swingSpring.damper = 10.0f;
	sphericalDesc.twistSpring.spring = 1.0f;
	sphericalDesc.twistSpring.damper = 10.0f;
	
	// projection is critical to manage stretch; if it is too tight, energy dissipates quickly.
	sphericalDesc.projectionMode = NX_JPM_NONE;
	sphericalDesc.projectionDistance = baseProjectionDistance;				
	sphericalDesc.actor[0] = ropeSegments[segIndex];
	sphericalDesc.actor[1] = 0;
	sphericalDesc.setGlobalAnchor(ropeSegments[segIndex]->getGlobalPosition());
	return scene->createJoint(sphericalDesc);
	}
	else
		return NULL;
}
	


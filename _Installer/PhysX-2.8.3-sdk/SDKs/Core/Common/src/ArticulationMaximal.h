#ifndef NX_RIGIDBODY_ARTICULATIONMAXIMAL
#define NX_RIGIDBODY_ARTICULATIONMAXIMAL
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/
#include "ArticulationNode.h"
//namespace NxRigidBody
//	{

/**
 This is the interface to full articulations including
 even maximal coordinate (lagrange) joints for the purpose
 of an acyclic projection order.
*/

class ArticulationMaximal : public ArticulationNode
		{
		public:
		ArticulationMaximal(ArticulationNode * , Body *, Joint *);
		virtual ~ArticulationMaximal();

		//physics interface:
		virtual void projectPose(); 
		virtual void processToAxisConstraintsInProjectionOrder(NxReal dt);

		};
	//}
#endif

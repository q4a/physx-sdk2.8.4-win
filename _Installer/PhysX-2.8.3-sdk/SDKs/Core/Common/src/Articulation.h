#ifndef NX_PHYSICS_ARTICULATION
#define NX_PHYSICS_ARTICULATION
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Physics.h"
#include "Array.h"
#include "Allocateable.h"
#include "DebugRenderable.h"
//namespace NxRigidBody
//	{

class Body;
class BodyRank;
class Joint;
class ArticulationNode;
/**
 This class serves both the static administration of an articulation and the actual articulation itself.
 An Articulation object holds several articulation root nodes which make up a simulation island that
 is further connected with lagrange joints.
*/

class Articulation : public NxFoundation::NxArraySDK<ArticulationNode*>
	{
	public:
	//creation:
	static	void				processJointGroup(Body* root);	

								Articulation();
								~Articulation();
#ifdef LINKLESS_ARTIC_TEST
			void				processToAxisConstraints(NxReal dt);
#endif
	//maximal methods:
			void				projectPose();
			void				processToAxisConstraintsInProjectionOrder(NxReal dt);

			void				visualize(NxFoundation::DebugRenderable & dr);
	
	private:
	static	Articulation*		buildArticulations(Body* root);
	static	ArticulationNode*	buildOneArticulation(Body* root, Joint* jointToWorld);
	static	ArticulationNode*	buildBodyTree(ArticulationNode* parent, Body* b, Joint* toParent, bool & cyclic);

	static	void				purgeBodyTree();
	static	unsigned			countJointsToWorld(Body* b);
	static	void				rankJoint(Joint&, BodyRank&);

	NxFoundation::NxArraySDK<ArticulationNode*>	projectionTrees;	//this is a special articulation that includes all joints, even non-articulateable ones.
												//its used for storing the joint projection sequence.
												//we need several per group only if kinematics are involved!  Because a kinematic doesn't split
												//two joint trees into different groups as the static world does.
	};

//	}
#endif

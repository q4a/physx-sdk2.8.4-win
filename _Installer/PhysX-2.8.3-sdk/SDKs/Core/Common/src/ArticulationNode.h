#ifndef NX_PHYSICS_ARTICULATIONNODE
#define NX_PHYSICS_ARTICULATIONNODE
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/
#include "Nx.h"
#include "Physics.h"
#include "PhysicsSDK.h"
#include "DebugRenderable.h"

//namespace NxRigidBody
//	{

class Body;
class Joint;
class Articulation1Dof;
/**
 This class encapsulates the connection between an articulated link and a body - joint pair.
 It has methods relevant to the creation and destruction of an articulation tree.
 This class doesn't expose an interface.
*/

	enum ArticulationNodeFlag
	{
		// WARNING: shared with derived classes' flags - don't add new members
		ANF_IS_PRUNED	= (1<<0)
	};

	class ArticulationNode : public NxFoundation::NxAllocateable
		{
		public:
										ArticulationNode(ArticulationNode*, Body*, Joint*);
		virtual							~ArticulationNode();
		NX_INLINE	ArticulationNode*	getUnprunedChild()
										{
									//	NX_ASSERT(unprunedChild);//if we can't compute this incrementally, all we have to do here is search the list
										return unprunedChild;
										}

				int						pruneSubtrees();
				void					makeRoot();
				void					removeChild(ArticulationNode *);
				void					DEBUGprintTree(unsigned indent = 0);

		//physics interface:
		virtual	void					initializeTree(Body* root, unsigned& i, ArticulationNode* rootLink = 0);
		virtual	void					integratePose(NxReal dt);
		virtual	void					projectPose();
		virtual	void					processToAxisConstraintsInProjectionOrder(NxReal dt);
		virtual	void					visualize(NxFoundation::DebugRenderable & dr);
		virtual	void					sleepVelCheck(NxReal dt);

		static	ArticulationNode*		makeNode(ArticulationNode*, Body*, Joint*);	//factory method that creates the appropriate subclass based on the type of joint supplied.

		//protected:
		//group access:
		static	const unsigned			supportedJointTypeMask;

				ArticulationNode*		rootLink;			//root node of this articulation
				ArticulationNode*		parent;				//parent of this node
				ArticulationNode*		childList;			//children of this node
				ArticulationNode*		nextInChildList;	//the next sibling / next ptr in parent's child list. NULL if parent is NULL.

				Body*					body;				//the body we represent
				Joint*					jointToParent;		//the joint to our parent

		//stuff only valid during building of tree:
		private:
		NX_INLINE	unsigned			getNumChildren();

				ArticulationNode*		unprunedChild;
				unsigned				numChildren;
		protected:
				NxU32					flags;

		friend class Articulation;
		};

NX_INLINE unsigned ArticulationNode::getNumChildren() 
	{ 
	return numChildren; 
	}
//	}
#endif

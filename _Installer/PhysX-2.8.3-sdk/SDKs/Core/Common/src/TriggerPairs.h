#ifndef __TRIGGER_PAIRS__
#define __TRIGGER_PAIRS__

#include "Array.h"
#include "Shape.h"

class CollisionMap;
class Scene;

	struct ShapePair
		{
		NX_INLINE ShapePair(const Shape* s0, const Shape* s1) : shape0(s0), shape1(s1), isSleeping0(s0->isSleeping()), isSleeping1(s1->isSleeping())		{}

		const Shape* shape0;
		const Shape* shape1;
		bool isSleeping0;
		bool isSleeping1;
		};

	struct ShapePairWTriggerFlag
		{
		NX_INLINE ShapePairWTriggerFlag(const Shape* s0, const Shape* s1, NxTriggerFlag flag): 
		shape0(s0), 
		shape1(s1), 
		triggerFlag(flag)	
			{
			}

		NX_INLINE ShapePairWTriggerFlag&  operator= (const ShapePairWTriggerFlag& s)
			{
			shape0 = (s.shape0);
			shape1 = (s.shape1);
			triggerFlag = s.triggerFlag;
			return *this;
			}
		const Shape*	shape0;
		const Shape*	shape1;
		NxTriggerFlag	triggerFlag;
		};

	typedef	NxFoundation::NxArraySDK<ShapePairWTriggerFlag>	TriggerBuffer;


	// PT: the collision map is a static member of the PhysicsSDK, shared by all scenes. However those
	// pairs of triggers are scene-dependent, so storing them in the collision map directly was wrong.
	class TriggerPairs
		{
		public:
										TriggerPairs();
										~TriggerPairs();

				void					purge(Shape* invalidPtr, Scene *scene);
				
				void					insertPairs(NxFoundation::NxArraySDK<ShapePair> &pairs)
										{
										for(NxU32 i=0;i<pairs.size();i++)
											currentPairs->pushBack(pairs[i]);
										}

				NxFoundation::NxArraySDK<ShapePair>*	previousPairs;
				NxFoundation::NxArraySDK<ShapePair>*	currentPairs;
				NxFoundation::NxArraySDK<ShapePair>	pairs0;
				NxFoundation::NxArraySDK<ShapePair>	pairs1;

		friend class CollisionMap;
		};

#endif

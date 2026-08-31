#ifndef NX_COLLISION_MAP
#define NX_COLLISION_MAP
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/
#include "Physics.h"
#include "Nx.h"
#include "Array.h"
#include "TriggerPairs.h"
#include "RbElementInteraction.h"

class NxUserTriggerReport;
class Scene;
class NPhaseContext;
class ShapeInstancePair;
class PhysicsSDK;
class Shape;
struct TriggerCache;
class ShapeInstancePairHL;
class ShapeInstancePairLL;
class TriggerInteraction;

#define PROFILE_OK 0  // whether or not to perform extreme detailed profiling of contact shape collision in a performance build
//namespace NxCollision {


	//! Contact callback
	typedef void (*ContactDetector) (const Shape*, const Shape*, ShapeInstancePairHL&, NPhaseContext&);

	//! Trigger callback
#if NX_FIX_TTP_1922
	#define TRIGGER_PARAMS	(const Shape*, const Shape*, TriggerCache&, NPhaseContext&)
#else
	#define TRIGGER_PARAMS	(const Shape*, const Shape*, NPhaseContext&)
#endif
	typedef bool (*TriggerCallback) TRIGGER_PARAMS;

	class CollisionMap : public NxFoundation::NxAllocateable
		{
		private:
										CollisionMap();
		virtual							~CollisionMap();
		public:
				PxInteractionType		getInteractionType(const CoreElement* element0, const CoreElement* element1) const;
				PxInteractionType		getRbElementInteractionType(const Shape* element0, const Shape* element1) const;
				void					findContacts(const Shape* primitive0, const Shape* primitive1, ShapeInstancePairHL& callback, NPhaseContext& context) const;
				void					findTriggerContacts(const Shape* primitive0, const Shape* primitive1, TriggerInteraction& callback, NPhaseContext& context) const;
				void					doCCDMotion(const Shape* primitive0, const Shape* primitive1, Scene& scene) const;
				void					updateTriggerCallbackBuffer(TriggerPairs& triggerPairs, TriggerBuffer& triggerBuffer, NPhaseContext& context);
		private:
				ContactDetector			map					[NX_SHAPE_COUNT][NX_SHAPE_COUNT];	// Collision-table (a.k.a. "c-table")
				TriggerCallback			triggerMap			[NX_SHAPE_COUNT][NX_SHAPE_COUNT];	// Same, for triggers
				
				// Hack to be able to handle height field meshes differently than normal meshes. This is the map for non heightfield meshes.
				ContactDetector			nonHFMeshMap		[NX_SHAPE_COUNT];					
		friend class PhysicsSDK;
		};
//}

#endif

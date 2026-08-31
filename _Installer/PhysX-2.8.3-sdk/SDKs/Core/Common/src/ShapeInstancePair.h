#ifndef NX_COLLISION_SHAPEINSTANCEPAIR
#define NX_COLLISION_SHAPEINSTANCEPAIR
/*----------------------------------------------------------------------------*\
|
|							  NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Physics.h"
#include "RbElementInteraction.h"
#include "Shape.h"
#include "NPhaseCore.h"
#include "ActorPair.h"
#include "NxAllocateable.h"
#include "Scene.h" // Only because of scene stats 2!

#define INVALID_SLOW_PAIR_ID	0xffffffff

#undef INC_SCENE_STAT
#undef DEC_SCENE_STAT
#undef ADD_TO_SCENE_STATS
#ifdef NX_ENABLE_SCENE_STATS2
#define INC_SCENE_STAT(x) getScene()->stats2.incStat(x)
#define DEC_SCENE_STAT(x) getScene()->stats2.decStat(x)
#define ADD_TO_SCENE_STATS(x, y) getScene()->stats2.addToStat(x, y)
#else
#define INC_SCENE_STAT(x)
#define DEC_SCENE_STAT(x)
#define ADD_TO_SCENE_STATS(x, y)
#endif

#ifdef NX_ENABLE_SCENE_STATS2
bool getPairStat(Scene* scene, const Shape* shape0, const Shape* shape1, bool total, PxInteractionType type, SceneStatisticEnum& statEnum);
#else
#define getPairStat(a, b, c, d, e)
#endif

class BroadPhase;
class Scene;
class CollisionMap;
class ShapeInstancePairHL;
class ShapeInstancePairLL;
class ContactStream;
class ActorPair;
class NxUserContactModify;
class ShapeInstancePair;

void handleContactCallback(PxdContactConstraint& contactConstraint, NxUserContactModify* callback,
						   const NxShape* shape0, const NxShape* shape1);

extern const PxInteractionType activeShapeInstancePairIteratorTypeList[3];
typedef ActiveInteractionIterator<ShapeInstancePair, PxIterator<PxInteraction*const>,
								  3, activeShapeInstancePairIteratorTypeList> ActiveShapeInstancePairIterator;
typedef ActiveInteractionIterator<ShapeInstancePair, PxBackwardIterator<PxInteraction*const>,
								  3, activeShapeInstancePairIteratorTypeList> ActiveShapeInstancePairBackwardIterator;

class SIPContactReportData
{
public:
	SIPContactReportData()
		: updateStamp(INVALID_ID),
		  sumNormalForce(0, 0, 0),
		  sumFrictionForce(0, 0, 0) {}
	NxVec3 sumNormalForce;
	NxVec3 sumFrictionForce;
	NxU32 updateStamp;
};

/*
Description: A shape instance pair represents a pair of objects which _may_ have contacts. Created by the broadphase
and processed by the NPhaseCore.
*/
class ShapeInstancePair : public RbElementInteraction
{
	friend class CollisionMap;
	friend class NPhaseCore;
public:

	NX_INLINE ShapeInstancePair(Shape* s1, Shape* s2, ActorPair& aPair, PxInteractionType type);
	virtual ~ShapeInstancePair() {};
	NX_INLINE void* operator new(size_t s, void* memory);
	NX_INLINE Scene* getScene() const
	{
		return getShape0().getOwnerSceneFast();
	}
	NX_INLINE ActorPair* getActorPair() const;
	bool userNotificationProcessNeeded() const;
	NxU32 prepareProcessUserNotification();
	NX_INLINE bool isSlowPair() const;
	NX_INLINE bool sumForcesCalculationIsForced() const;
	NX_INLINE bool thisFrameHaveContacts() const;
	void visualize(NxFoundation::DebugRenderable&);
	//////////////////////// Virtuals ////////////////////////
	virtual void updateState();
	// Submits to contact stream, calculates sumNormalForce, sumFrictionForce and submits the contact point force when needed
	virtual	void processUserNotification() = 0;
	virtual	bool lastFrameHadContacts() const = 0;
	// Only valid after processUserNotification has been run and thisFrameHaveContacts() is true.
	NX_INLINE NxVec3 getSumNormalForce() const;
	NX_INLINE NxVec3 getSumFrictionForce() const;

	//////////////////////// PxInteraction ////////////////////////
	NX_INLINE virtual void initialize();
	NX_INLINE virtual void destroy();
	NX_INLINE virtual void onActivate();
	NX_INLINE virtual void onDeactivate();
	virtual	bool isActive() const;
	virtual	void onActorActiveChange(PxActor* actor);

	NX_INLINE static bool isShapeInstancePair(const PxInteraction& interaction);

protected:	
	enum SipFlag
	{
		TRIANGLE_MATERIAL_SHAPE0		= (1 << 0),
		TRIANGLE_MATERIAL_SHAPE1		= (1 << 1),
		TRIANGLE_MATERIAL_MASK			= (TRIANGLE_MATERIAL_SHAPE0 | TRIANGLE_MATERIAL_SHAPE1),
		HAS_ANISOTROPIC_MATERIAL		= (1 << 2),

		DISABLE_FRICTION				= (1 << 3),
		DISABLE_STRONG_FRICTION			= (1 << 4),
		HAVE_CONTACTS_THIS_FRAME		= (1 << 5),

		CONTACTS_NEED_UPDATE			= (1 << 6),		// LL contacts need to be updated before solver (per triangle material or anisotropic material)
		CONTACTS_SUM_FORCES				= (1 << 7),		// The user needs sum normal force and friction force (i.e. NX_NOTIFY_FORCES are set)
		CONTACTS_COLLECT_POINTS			= (1 << 8),		// The user has any contact notification flag set
		CONTACTS_COLLECT_POINT_FORCES	= (1 << 9),		// The user has enabled per contact force notification on one or more shapes
		CONTACTS_RESPONSE_DISABLED		= (1 << 10),	// Collision response disabled
		CONTACTS_CALLBACK				= (1 << 11),	// Handle contact callbacks
		CONTACTS_FORCE_SUM_FORCES		= (1 << 12),	// Force the sum forces to be calculated even if not needed externally
		CONTACTS_FORCE_CONE_FRICTION	= (1 << 13),	// Force the friction model to cone friction

		ACTIVATE_ACTORS_ON_DELETE		= (1 << 15),	// The SIP will wake the actors when deleted (i.e. if the SIP has had contact during its lifetime)

		SHAPE0_IS_KINEMATIC				= (1 << 16),	// Cached for performance reasons
		SHAPE1_IS_KINEMATIC				= (1 << 17),	// Cached for performance reasons

		FILTERED_ACTOR_PAIR				= (1 << 18),	// The actor pair is filtered (i.e. no shapes should collide)
		DISABLE_ACTIVITY_TRANSFER		= FILTERED_ACTOR_PAIR,  // Do not let the SIP transfer activity (filtering or triggers)
		EARLY_FILTERING_DISABLED		= (1 << 19),	// The actor pair can not be filtered using SIP markers (i.e. includes a mesh with pages)
	};

	void updateActivityTransfer();
	NX_INLINE void addNormalForce(const NxVec3& f);
	NX_INLINE void addFrictionForce(const NxVec3& f);
	const NxQP getQP0() const;
	const NxQP getQP1() const;

	NX_INLINE bool readFlag(SipFlag flag) const;
	NX_INLINE void clearFlag(SipFlag flag);
	NX_INLINE void raiseFlag(SipFlag flag);
	NX_INLINE void setFlag(SipFlag flag, bool value);
	NX_INLINE NxU32 getFlags(NxU32 filter) const;

private:
	NxU32 mFlags;
	ActorPair& mActorPair;
	NxU32 mSlowPairIndex;			// Owned by NPhaseCore for its slow pair list
	SIPContactReportData* mReportData;
};


////////////////////////////////////////////////////////////////////////////
NX_INLINE ShapeInstancePair::ShapeInstancePair(Shape* s1, Shape* s2, ActorPair& aPair, PxInteractionType type)
	: RbElementInteraction(*s1, *s2, type),
	  mFlags(0),
	  mActorPair(aPair),
	  mSlowPairIndex(INVALID_SLOW_PAIR_ID),
	  mReportData(NULL) {}

NX_INLINE void* ShapeInstancePair::operator new(size_t s, void* memory)
{
	return memory;
}

NX_INLINE ActorPair* ShapeInstancePair::getActorPair() const
{
	return &mActorPair;
}

NX_INLINE bool ShapeInstancePair::sumForcesCalculationIsForced() const
{
	return readFlag(CONTACTS_FORCE_SUM_FORCES);
}

NX_INLINE bool ShapeInstancePair::thisFrameHaveContacts()	const
{
	return readFlag(HAVE_CONTACTS_THIS_FRAME);
}

NX_INLINE bool ShapeInstancePair::isSlowPair() const
{
	return readFlag(CONTACTS_COLLECT_POINTS) || readFlag(CONTACTS_NEED_UPDATE) || readFlag(CONTACTS_SUM_FORCES);
}

NX_INLINE bool ShapeInstancePair::isShapeInstancePair(const PxInteraction& interaction)
{
	switch (interaction.getType())
	{
	case PX_INTERACTION_TYPE_OVERLAP_HL:
	case PX_INTERACTION_TYPE_OVERLAP_LL:
	case PX_INTERACTION_TYPE_PAGEBOUNDS:
		return true;
		break;
	case PX_INTERACTION_TYPE_MARKER:
	case PX_INTERACTION_TYPE_TRIGGER:
	case PX_INTERACTION_TYPE_JOINT:
	case PX_INTERACTION_TYPE_FLUID_BODY:
		return false;
		break;
	default:
		// Unknown type
		NX_ASSERT(0);
		return false;
	};
	return false;
}

NX_INLINE NxVec3 ShapeInstancePair::getSumNormalForce() const
{
	if (mReportData != NULL)
		return mReportData->sumNormalForce;
	else
		return NxVec3(0, 0, 0);
}

NX_INLINE NxVec3 ShapeInstancePair::getSumFrictionForce() const
{
	if (mReportData != NULL)
		return mReportData->sumFrictionForce;
	else
		return NxVec3(0, 0, 0);
}

NX_INLINE void ShapeInstancePair::onActivate()
{
	RbElementInteraction::onActivate();
#ifdef NX_ENABLE_SCENE_STATS2
	SceneStatisticEnum statEnum;
	if (getPairStat(getShape0().getOwnerSceneFast(), &getShape0(), &getShape1(), true, interactionType, statEnum))
		INC_SCENE_STAT(statEnum);
	if (readFlag(CONTACTS_COLLECT_POINTS))
		INC_SCENE_STAT(StatActiveContactNotificationPairs);
	if (readFlag(CONTACTS_CALLBACK))
		INC_SCENE_STAT(StatActiveContactModificationPairs);
#endif
	if (isSlowPair() && (mSlowPairIndex == INVALID_SLOW_PAIR_ID))
		getShape0().getOwnerSceneFast()->getNPhaseCore()->addToActiveSlowPairList(this);
}

NX_INLINE void ShapeInstancePair::onDeactivate()
{
#ifdef NX_ENABLE_SCENE_STATS2
	SceneStatisticEnum statEnum;
	if (getPairStat(getShape0().getOwnerSceneFast(), &getShape0(), &getShape1(), true, interactionType, statEnum))
		DEC_SCENE_STAT(statEnum);
	if (readFlag(CONTACTS_COLLECT_POINTS))
		DEC_SCENE_STAT(StatActiveContactNotificationPairs);
	if (readFlag(CONTACTS_CALLBACK))
		DEC_SCENE_STAT(StatActiveContactModificationPairs);
#endif
	if (mSlowPairIndex != INVALID_SLOW_PAIR_ID)
		getShape0().getOwnerSceneFast()->getNPhaseCore()->removeFromActiveSlowPairList(this);
	RbElementInteraction::onDeactivate();
}

NX_INLINE void ShapeInstancePair::initialize()
{
	RbElementInteraction::initialize();
	NX_ASSERT(getShape0().getTypeFast()!=NX_SHAPE_COMPOUND && getShape1().getTypeFast()!=NX_SHAPE_COMPOUND);
	NX_ASSERT((&getShape0()) && (&getShape1()));

	getActorPair()->incRefCount();
#ifdef NX_ENABLE_SCENE_STATS2
	INC_SCENE_STAT(StatTotalColliders);
	SceneStatisticEnum statEnum;
	if (getPairStat(getShape0().getOwnerSceneFast(), &getShape0(), &getShape1(), false, interactionType, statEnum))
		INC_SCENE_STAT(statEnum);
#endif
}

NX_INLINE void ShapeInstancePair::destroy()
{
	// This will remove the interaction from the actors list, which will prevent
	// update calls to this actor because of Body::wakeUp below.
	RbElementInteraction::destroy();

	if (thisFrameHaveContacts())
	{
		// The SIP is removed explicitly because we still have contact
		Body* body0 = getShape0().getBodyFast();
		Body* body1 = getShape1().getBodyFast();
		if (!body0 || !body1)
		{
			if (body0)
				body0->decStaticTouch();
			if (body1)
				body1->decStaticTouch();
		}
		if (body0)
			body0->decrementBodyConstraintCounter();
		if (body1)
			body1->decrementBodyConstraintCounter();
		getActorPair()->decTouchCount(getScene()->getTimeStamp());
	}
	if (readFlag(ACTIVATE_ACTORS_ON_DELETE))
	{
		Body* body0 = getShape0().getBodyFast();
		Body* body1 = getShape1().getBodyFast();
		if (body0 && !body0->wasForcedToSleepLastFrame() && !readFlag(SHAPE0_IS_KINEMATIC))
			body0->wakeUp();
		if (body1 && !body1->wasForcedToSleepLastFrame() && !readFlag(SHAPE1_IS_KINEMATIC))
			body1->wakeUp();
	}

	getActorPair()->decRefCount();

	if (mReportData)
	{
		getScene()->getNPhaseCore()->releaseSIPContactReportData(mReportData);
		mReportData = NULL;
	}

#ifdef NX_ENABLE_SCENE_STATS2
	// Updating scene stats
	if (readFlag(CONTACTS_COLLECT_POINTS))
		DEC_SCENE_STAT(StatTotalContactNotificationPairs);
	if (readFlag(CONTACTS_CALLBACK))
		DEC_SCENE_STAT(StatTotalContactModificationPairs);
	DEC_SCENE_STAT(StatTotalColliders);
	SceneStatisticEnum statEnum;
	if (getPairStat(getShape0().getOwnerSceneFast(), &getShape0(), &getShape1(), false, interactionType, statEnum))
		DEC_SCENE_STAT(statEnum);
#endif
}

NX_INLINE void ShapeInstancePair::addNormalForce(const NxVec3& f)
{
	NX_ASSERT(mReportData != NULL);
	mReportData->sumNormalForce += f;
}

NX_INLINE void ShapeInstancePair::addFrictionForce(const NxVec3& f)
{
	NX_ASSERT(mReportData != NULL);
	mReportData->sumFrictionForce += f;
}

NX_INLINE bool ShapeInstancePair::readFlag(SipFlag flag) const
{
	return (mFlags & flag) != 0;
}

NX_INLINE void ShapeInstancePair::clearFlag(SipFlag flag)
{
	mFlags &= ~flag;
}

NX_INLINE void ShapeInstancePair::raiseFlag(SipFlag flag)
{
	mFlags |= flag;
}

NX_INLINE void ShapeInstancePair::setFlag(SipFlag flag, bool value)
{
	if (value)
		raiseFlag(flag);
	else
		clearFlag(flag);
}

NX_INLINE NxU32 ShapeInstancePair::getFlags(NxU32 filter) const
{
	return mFlags & filter;
}

#undef INC_SCENE_STAT
#undef DEC_SCENE_STAT
#endif

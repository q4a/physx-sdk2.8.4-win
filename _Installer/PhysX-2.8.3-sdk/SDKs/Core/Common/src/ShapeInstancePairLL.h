#ifndef NX_COLLISION_SHAPEINSTANCEPAIR_LL
#define NX_COLLISION_SHAPEINSTANCEPAIR_LL
/*----------------------------------------------------------------------------*\
|
|							  NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "ShapeInstancePair.h"
#include "Body.h" // OK, because not many headers include ShapeInstancePairLL.h
#include "TriangleMeshShape.h"
#include "ActorPair.h"

extern const PxInteractionType activeShapeInstancePairIteratorTypeListLL[2];
typedef ActiveInteractionIterator<ShapeInstancePairLL, PxIterator<PxInteraction*const>,
								  2, activeShapeInstancePairIteratorTypeListLL> ActiveShapeInstancePairIteratorLL;

class ShapeInstancePairLL : public ShapeInstancePair
{
public:
	NX_INLINE ShapeInstancePairLL(Shape& s1, Shape& s2, ActorPair& aPair);
	virtual ~ShapeInstancePairLL() {};
	NX_INLINE virtual void initialize();
	NX_INLINE virtual void destroy();

	NX_INLINE void activateManager();
	NX_INLINE void deactivateManager();
	void processContacts();
	NX_INLINE bool managerLostTouch();
	NX_INLINE void managerNewTouch();
	NX_INLINE NxU32 getManagerContactState() const;

	//////////////////////// Virtuals ////////////////////////	
	virtual void processUserNotification();
	virtual void updateState();
	NX_INLINE virtual bool lastFrameHadContacts() const;
	virtual	void onActorActiveChange(PxActor* actor);

	//////////////////////// Statics ////////////////////////	
	NX_INLINE static bool isManagerController(const PxInteraction& interaction);

protected:
	enum SipFlagLL
	{
		LL_MANAGER_HAS_TOUCH	= (1 << 0),
		LL_MANAGER_FORCE_SW		= (1 << 1),
		LL_MANAGER_DEACTIVATED	= (1 << 2),  // Manager is needed, but currently deactivated
		LL_MANAGER_RECREATED	= (1 << 3),  // Valid in updateState. Set if manager was recreated
		LL_MANAGER_FORCE_RESET	= (1 << 4),  // This is set to force the managers to be reset
		LL_MANAGER_FLAG_LAST	= LL_MANAGER_FORCE_RESET
	};

	NX_INLINE ShapeInstancePairLL(Shape& s1, Shape& s2, ActorPair& aPair, PxInteractionType type);
	virtual void beforeManagerDestroyed() {};
	virtual void afterManagerCreated() {};

	void createManager();
	NX_INLINE void resetManager();
	NX_INLINE void updateManager();
	NX_INLINE void destroyManager();
	NX_INLINE bool activeManagerAllowed() const;
	NX_INLINE PxdManager getManager() const;
	void forceManagerSW(bool sw);
	bool isManagerForcedSW() const;
	NxU32 getMaterialIndexPair(const Shape** shapes) const;
	void updateAnisotropicContactConstraint(Scene& scene, PxdContactConstraint contactConstraint, const Shape** shapes,
											const NxMaterialDesc** materials);

	NX_INLINE bool readFlagLL(SipFlagLL flag) const;
	NX_INLINE void clearFlagLL(SipFlagLL flag);
	NX_INLINE void raiseFlagLL(SipFlagLL flag);
	NX_INLINE void setFlagLL(SipFlagLL flag, bool value);

private:
	PxdManager mManager;
	NxU32 mNewTouchTimeStamp;
	NxU32 mLostTouchTimeStamp;
	NxU8 mFlagsLL;
};



//////////////////////////////////////////////////////////////////////////
ShapeInstancePairLL::ShapeInstancePairLL(Shape& s1, Shape& s2, ActorPair& aPair)
	: ShapeInstancePair(&s1, &s2, aPair, PX_INTERACTION_TYPE_OVERLAP_LL),
	  mNewTouchTimeStamp(0xffffffff),
	  mLostTouchTimeStamp(0xffffffff),
	  mFlagsLL(0),
	  mManager(0)
{
	// Make sure there is room in the field
	NX_COMPILE_TIME_ASSERT(LL_MANAGER_FLAG_LAST == NxU8(LL_MANAGER_FLAG_LAST));
}

void ShapeInstancePairLL::initialize()
{
	ShapeInstancePair::initialize();
}

void ShapeInstancePairLL::destroy()
{
	// We need to destroy managers before deinit() because otherwise onAllInteractionsInactiveChange
	// will deactivate bodies, but the current manager may still be active and alive.
	destroyManager();
	ShapeInstancePair::destroy();
}

ShapeInstancePairLL::ShapeInstancePairLL(Shape& s1, Shape& s2, ActorPair& aPair, PxInteractionType type)
	: ShapeInstancePair(&s1, &s2, aPair, type),
	  mNewTouchTimeStamp(0xffffffff),
	  mLostTouchTimeStamp(0xffffffff),
	  mFlagsLL(0),
	  mManager(0) {}

NX_INLINE bool ShapeInstancePairLL::lastFrameHadContacts()	const	
{
	return (readFlag(HAVE_CONTACTS_THIS_FRAME) && (mNewTouchTimeStamp != getScene()->getTimeStamp())) ||
		(!readFlag(HAVE_CONTACTS_THIS_FRAME) && (mLostTouchTimeStamp == getScene()->getTimeStamp()));
}

NX_INLINE void ShapeInstancePairLL::activateManager()
{
	if (mManager != 0 && activeManagerAllowed())
	{
		PxdManagerActivate(mManager);
		if (readFlagLL(LL_MANAGER_DEACTIVATED))
			clearFlagLL(LL_MANAGER_DEACTIVATED);
	}
	else
		raiseFlagLL(LL_MANAGER_DEACTIVATED);
}

NX_INLINE void ShapeInstancePairLL::deactivateManager()
{
	if (mManager != 0 && !readFlagLL(LL_MANAGER_DEACTIVATED))
	{
		PxdManagerDeactivate(mManager);
		raiseFlagLL(LL_MANAGER_DEACTIVATED);
	}
}

NX_INLINE bool ShapeInstancePairLL::isManagerController(const PxInteraction& interaction)
{
	return (interaction.getType() == PX_INTERACTION_TYPE_OVERLAP_LL ||
			interaction.getType() == PX_INTERACTION_TYPE_PAGEBOUNDS);
}

NX_INLINE PxdManager ShapeInstancePairLL::getManager() const
{
	return mManager;
}

NX_INLINE void ShapeInstancePairLL::resetManager()
{
	bool destroyed = false;
	if (mManager != 0)
	{
		destroyManager();
		destroyed = true;
	}
	createManager();
	if (destroyed)
		raiseFlagLL(LL_MANAGER_RECREATED);
}

NX_INLINE void ShapeInstancePairLL::updateManager()
{
	if (mManager != 0)
		activateManager();
	else
		createManager();
}

NX_INLINE void ShapeInstancePairLL::destroyManager()
{
	if (mManager != 0)
	{
		beforeManagerDestroyed();
		PxdManagerDestroy(mManager);
		mManager = 0;
	}
}

NX_INLINE bool ShapeInstancePairLL::activeManagerAllowed() const
{
	return ((!getShape0().getBodyFast() || getActor0().isActive() ||
			 ((getShape0().getBodyFast()->getPublicFlagsFast() & NX_BF_KINEMATIC) &&
			  getShape0().getBodyFast()->hasActiveInteraction())) &&
			(!getShape1().getBodyFast() || getActor1().isActive() ||
			 ((getShape1().getBodyFast()->getPublicFlagsFast() & NX_BF_KINEMATIC) &&
			  getShape1().getBodyFast()->hasActiveInteraction())));
}

// Note that LL will not send end touch events for managers that are destroyed while having contact
NX_INLINE void ShapeInstancePairLL::managerNewTouch()
{
	if (readFlagLL(LL_MANAGER_HAS_TOUCH))
		return; // Do not count the touch twice (for instance when recreating a manager with touch)
	raiseFlagLL(LL_MANAGER_HAS_TOUCH);
	mNewTouchTimeStamp = getScene()->getTimeStamp();
	getActorPair()->incTouchCount(mNewTouchTimeStamp);

	Body* body0 = getShape0().getBodyFast();
	Body* body1 = getShape1().getBodyFast();
	if (!body0 || !body1)
	{
		if (body0)
			body0->incStaticTouch();
		if (body1)
			body1->incStaticTouch();
	}

	if (body0)
		body0->incrementBodyConstraintCounter();

	if (body1)
		body1->incrementBodyConstraintCounter();

	// We have contact this frame
	raiseFlag(HAVE_CONTACTS_THIS_FRAME);
	raiseFlag(ACTIVATE_ACTORS_ON_DELETE);
}

NX_INLINE bool ShapeInstancePairLL::managerLostTouch()
{
	if (!readFlagLL(LL_MANAGER_HAS_TOUCH))
		return false;
	clearFlagLL(LL_MANAGER_HAS_TOUCH);

	// We do not have LL contacts this frame and also we lost LL contact this frame
	clearFlag(HAVE_CONTACTS_THIS_FRAME);
	mLostTouchTimeStamp = getScene()->getTimeStamp();
	getActorPair()->decTouchCount(mLostTouchTimeStamp);

	Body* body0 = getShape0().getBodyFast();
	Body* body1 = getShape1().getBodyFast();
	if (body0)
		body0->decrementBodyConstraintCounter();
	if (body1)
		body1->decrementBodyConstraintCounter();

	if (!body0 || !body1)
	{
		if (body0)
		{
			body0->wakeUp();
			body0->decStaticTouch();
		}
		if (body1)
		{
			body1->wakeUp();
			body1->decStaticTouch();
		}

		return true;
	}
	return false;
}

NX_INLINE NxU32 ShapeInstancePairLL::getManagerContactState() const
{
	return getFlags(CONTACTS_NEED_UPDATE | CONTACTS_SUM_FORCES | CONTACTS_COLLECT_POINTS | CONTACTS_FORCE_CONE_FRICTION |
					CONTACTS_COLLECT_POINT_FORCES | CONTACTS_RESPONSE_DISABLED | CONTACTS_CALLBACK);
}

NX_INLINE bool ShapeInstancePairLL::readFlagLL(SipFlagLL flag) const
{
	return (mFlagsLL & flag) != 0;
}

NX_INLINE void ShapeInstancePairLL::clearFlagLL(SipFlagLL flag)
{
	mFlagsLL &= ~flag;
}

NX_INLINE void ShapeInstancePairLL::raiseFlagLL(SipFlagLL flag)
{
	mFlagsLL |= flag;
}

NX_INLINE void ShapeInstancePairLL::setFlagLL(SipFlagLL flag, bool value)
{
	if (value)
		raiseFlagLL(flag);
	else
		clearFlagLL(flag);
}

#endif

#ifndef NX_COLLISION_SHAPEINSTANCEPAIR_HL
#define NX_COLLISION_SHAPEINSTANCEPAIR_HL
/*----------------------------------------------------------------------------*\
|
|							  NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "ShapeInstancePair.h"

#ifdef NX_ENABLE_SCENE_STATS2
#define INC_SCENE_STAT(x) getScene()->stats2.incStat(x)
#define DEC_SCENE_STAT(x) getScene()->stats2.decStat(x)
#define ADD_TO_SCENE_STATS(x, y) getScene()->stats2.addToStat(x, y)
#else
#define INC_SCENE_STAT(x)
#define DEC_SCENE_STAT(x)
#define ADD_TO_SCENE_STATS(x, y)
#endif

//////////////////////////////////////////////////////////////////////////

class ShapeInstancePairHL : public ShapeInstancePair
{
public:
	NX_INLINE ShapeInstancePairHL(Shape& s1, Shape& s2, ActorPair& aPair);
	virtual ~ShapeInstancePairHL();
	virtual void initialize();
	virtual void destroy();

	NX_INLINE void beginContacts();
	void finishContacts();
	void setupSolverConstraints(NxReal dt);
	void contact(const Shape& s0, const Shape& s1, NxReal separation, const NxVec3& contactPoint,
				 const NxVec3& contactNormal, NxU16 ms1=0xffff, NxU16 ms2=0xffff);
	void contact(const Shape& s0, const Shape& s1, NxReal separation, const NxVec3& contactPoint,
				 const NxVec3& contactNormal, NxU16 ms1, NxU16 ms2, NxU32 featureIndex0, NxU32 featureIndex1);

	virtual void processUserNotification();
	virtual void updateState();
	virtual void onActivate();
	NX_INLINE virtual bool lastFrameHadContacts() const;
protected:
	enum SipFlagHL
	{
		HL_HAD_CONTACTS_LAST_FRAME	= (1 << 0),
		HL_WHEEL_CONTACT			= (1 << 1),  // At least one wheel shape exist and have had contact
		HL_FLAG_LAST				= HL_WHEEL_CONTACT
	};

	void setupNewPersistentConstraints();
	void pruneOldContacts();
	void clearPersistentContacts();
	void processWheelContact(const Shape& s0, const Shape& s1, NxReal separation, const NxVec3& contactPoint,
							 const NxVec3& contactNormal, NxU16 ms1, NxU16 ms2, NxU32 featureIndex0,
							 NxU32 featureIndex1);
	void resetWheelConstraints();

	NX_INLINE bool readFlagHL(SipFlagHL flag) const;
	NX_INLINE void clearFlagHL(SipFlagHL flag);
	NX_INLINE void raiseFlagHL(SipFlagHL flag);
	NX_INLINE void setFlagHL(SipFlagHL flag, bool value);

	// Persistent data used by HL contacts
	struct PersistentContact
	{
		// Sub shapes
		const Shape* shape0;
		const Shape* shape1;
		NxVec3 worldPoint;
		NxVec3 localPoint[2];
		NxVec3 localNormal[2];
		NxVec3 error;
		NxQuat worldContactFrame;
		NxReal staticFriction[2];
		NxReal dynamicFriction[2];
		NxReal restitution;
		NxU32 featureIndex0;
		NxU32 featureIndex1;
		PxdContactConstraint constraint;
		bool contactTouched;
		bool anisotropic;
		bool disableStrongFriction;
	};
	typedef NxFoundation::NxArraySDK<PersistentContact> PersistentContactArrayType;

	// Contact point used by HL contacts
	struct ContactPoint
	{
		// Sub shapes
		const Shape* shape0;
		const Shape* shape1;
		NxVec3 worldPoint;
		NxVec3 worldNormal;
		NxReal separation;
		NxU32 featureIndex0;
		NxU32 featureIndex1;
		NxU16 materialIndex0;
		NxU16 materialIndex1;
	};

	PersistentContactArrayType mPersistentContacts;
	typedef NxFoundation::NxArraySDK<ContactPoint> ContactPointArrayType;
	ContactPointArrayType mTemporalContacts;

	NxU8 mFlagsHL;
	NxU8 mCollisionData;
};

//////////////////////////////////////////////////////////////////////////
ShapeInstancePairHL::ShapeInstancePairHL(Shape& s1, Shape& s2, ActorPair& aPair)
	: ShapeInstancePair(&s1, &s2, aPair, PX_INTERACTION_TYPE_OVERLAP_HL),
	  mFlagsHL(0),
	  mCollisionData(0xff)
{
	// Make sure there is room in the field
	NX_COMPILE_TIME_ASSERT(HL_FLAG_LAST == NxU8(HL_FLAG_LAST));
}

NX_INLINE void ShapeInstancePairHL::beginContacts()
{
	setFlagHL(HL_HAD_CONTACTS_LAST_FRAME, readFlag(HAVE_CONTACTS_THIS_FRAME));
	clearFlag(HAVE_CONTACTS_THIS_FRAME);

	mTemporalContacts.clear();
	pruneOldContacts();
}

NX_INLINE bool ShapeInstancePairHL::lastFrameHadContacts() const
{
	return readFlagHL(HL_HAD_CONTACTS_LAST_FRAME);
}

NX_INLINE bool ShapeInstancePairHL::readFlagHL(SipFlagHL flag) const
{
	return (mFlagsHL & flag) != 0;
}

NX_INLINE void ShapeInstancePairHL::clearFlagHL(SipFlagHL flag)
{
	mFlagsHL &= ~flag;
}

NX_INLINE void ShapeInstancePairHL::raiseFlagHL(SipFlagHL flag)
{
	mFlagsHL |= flag;
}

NX_INLINE void ShapeInstancePairHL::setFlagHL(SipFlagHL flag, bool value)
{
	if (value)
		raiseFlagHL(flag);
	else
		clearFlagHL(flag);
}

#undef INC_SCENE_STAT
#undef DEC_SCENE_STAT
#undef ADD_TO_SCENE_STATS
#endif

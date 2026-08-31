#ifndef NX_COLLISION_CORE_INTERACTION
#define NX_COLLISION_CORE_INTERACTION
/*----------------------------------------------------------------------------*\
|
|							  NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Nx.h"

class ElementInteraction;
class ActorInteraction;
class NPhaseCore;
class PxInteraction;

class CoreInteraction
{
public:
	enum DirtyFlag
	{
		CIF_DIRTY_FILTER_STATE	= (1 << 0), // All changes filtering related
		CIF_DIRTY_NOTIFICATION	= (1 << 1),
		CIF_DIRTY_MATERIAL		= (1 << 2),
		CIF_DIRTY_BODY			= (1 << 3) | CIF_DIRTY_FILTER_STATE | CIF_DIRTY_NOTIFICATION,
		CIF_DIRTY_ACTOR			= (1 << 4) | CIF_DIRTY_FILTER_STATE | CIF_DIRTY_NOTIFICATION,
		CIF_DIRTY_ELEMENT		= (1 << 5) | CIF_DIRTY_FILTER_STATE | CIF_DIRTY_NOTIFICATION,
		CIF_DIRTY_DOMINANCE		= (1 << 6),
		CIF_DIRTY_SKIN_WIDTH	= (1 << 7),
		CIF_DIRTY_LAST			= CIF_DIRTY_SKIN_WIDTH,
		CIF_DIRTY_ALL			= 0xffff
	};

	enum Flag
	{
		IS_ELEMENT_INTERACTION				= (1 << 0),  // This interaction is a ElementInteraction and not a ActorInteraction
		USER_CALLBACK_FILTERING_ENABLED		= (1 << 1),  // The interaction uses user callback filtering
		USER_CALLBACK_FILTERING_FILTERED	= (1 << 2),  // The interaction is filtered using user callback filtering
		IN_USER_CALLBACK_FILTERED_LIST		= (1 << 3),  // The interaction is in the user callback filtered list
		IN_DIRTY_LIST						= (1 << 4),  // The interaction is in the dirty list
	};

	NX_INLINE CoreInteraction(bool bElementInteraction);
	virtual ~CoreInteraction() {};
	NX_INLINE virtual void updateState();
	NX_INLINE void setDirty(DirtyFlag dirtyFlag);
	NX_INLINE void setClean();
	NX_INLINE bool isElementInteraction() const;
	NX_INLINE bool isActorInteraction() const;
	virtual NPhaseCore* getNPhaseCore() const = 0;

	NX_INLINE bool readCoreFlag(Flag flag) const;
	NX_INLINE void raiseCoreFlag(Flag flag);
	NX_INLINE void clearCoreFlag(Flag flag);

	static CoreInteraction* isCoreInteraction(PxInteraction* interaction);
protected:
	NX_INLINE NxU16 getDirtyFlags() const;
	NX_INLINE static bool readDirtyFlag(NxU16 dirtyFlags, DirtyFlag flag);
private:
	void addToDirtyList();
	void removeFromDirtyList();

	NxU16 mDirtyFlags;
	NxU16 mFlags;
};

//////////////////////////////////////////////////////////////////////////
NX_INLINE CoreInteraction::CoreInteraction(bool bElementInteraction)
	: mDirtyFlags(CIF_DIRTY_ALL), mFlags(0)
{
	if (bElementInteraction)
		raiseCoreFlag(IS_ELEMENT_INTERACTION);
}

NX_INLINE void CoreInteraction::updateState()
{
	setClean();
	mDirtyFlags = 0;
}

NX_INLINE void CoreInteraction::setDirty(DirtyFlag dirtyFlag)
{
	mDirtyFlags |= dirtyFlag;
	if (!readCoreFlag(IN_DIRTY_LIST))
	{
		addToDirtyList();
		raiseCoreFlag(IN_DIRTY_LIST);
	}
}

NX_INLINE void CoreInteraction::setClean()
{
	if (readCoreFlag(IN_DIRTY_LIST))
	{
		removeFromDirtyList();
		clearCoreFlag(IN_DIRTY_LIST);
	}
}

NX_INLINE bool CoreInteraction::isElementInteraction() const
{
	return readCoreFlag(IS_ELEMENT_INTERACTION);
}

NX_INLINE bool CoreInteraction::isActorInteraction() const
{
	return !readCoreFlag(IS_ELEMENT_INTERACTION);
}

NX_INLINE NxU16 CoreInteraction::getDirtyFlags() const
{
	return mDirtyFlags;
}

NX_INLINE void CoreInteraction::raiseCoreFlag(Flag flag)
{
	mFlags |= flag;
}

NX_INLINE void CoreInteraction::clearCoreFlag(Flag flag)
{
	mFlags &= ~flag;
}

NX_INLINE bool CoreInteraction::readCoreFlag(Flag flag) const
{
	return ((mFlags & flag) == flag);
}

NX_INLINE bool CoreInteraction::readDirtyFlag(NxU16 dirtyFlags, DirtyFlag flag)
{
	return ((dirtyFlags & flag) == flag);
}

#endif

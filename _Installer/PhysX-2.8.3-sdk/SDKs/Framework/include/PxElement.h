#ifndef NX_FRAMEWORK_PXELEMENT
#define NX_FRAMEWORK_PXELEMENT
/*----------------------------------------------------------------------------*\
|
|							  NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/
#include "PxActor.h"
#include "PxVolume.h"

class PxScene;

/*
A PxElement is PxVolume that is a part of a PxActor. It contributes to the activation
framework by adding its interactions to the actor. 
*/

class PxElementInteraction;

class PxElement : public PxVolume
{
	friend class PxElementInteraction;
	friend class PxScene;
public:
	NX_INLINE PxElement(PxActor& actor, PxVolumeType type);
	virtual ~PxElement();

	virtual void initialize();
	virtual void destroy();

	// Get an iterator to the interactions connected to the element
	NX_INLINE PxIterator<PxElementInteraction*const> getInteractions() const;
	NX_INLINE NxU32 getInteractionCount() const;

	NX_INLINE PxActor& getPxActor() const;
	NX_INLINE PxScene& getPxScene() const;
	NX_INLINE bool isActive() const;

	// Create the low level volume of the element
	NX_INLINE void createLowLevelVolume(PxdVolumeDesc& volumeDesc);

protected:

private:

	//These are called from interaction creation/destruction
	NX_INLINE NxU16 addInteraction(PxElementInteraction* interaction);
	NX_INLINE void removeInteraction(NxU16 id);
	NX_INLINE PxElementInteraction* getInteraction(NxU16 id) const;

	NxFoundation::NxArraySDK<PxElementInteraction*>	mInteractions;
	PxActor& mActor;
};

//////////////////////////////////////////////////////////////////////////
/*
Helper class that will iterate over the elements of an actor of a 
specific element type.
*/
template<class T, NxU32 typeEnum>
class PxActorElementIterator
{
public:
	PxActorElementIterator(const PxActor& actor) : it(actor.getElements()) {}
	~PxActorElementIterator() {}
	T* getNext()
	{
		PxElement*const* current = *it;
		while (current != it.end())
		{
			PxElement* element = *current;
			++it;
			if (element->getPxVolumeType() == typeEnum)
			{
				T* retElement = static_cast<T*>(element);
				return retElement;
			}
			current = *it;
		}
		return NULL;
	}
private:
	PxIterator<PxElement*const> it;
};

//////////////////////////////////////////////////////////////////////////
NX_INLINE PxElement::PxElement(PxActor& actor, PxVolumeType type) : 
PxVolume(type),
mActor(actor)
{
}

void PxElement::createLowLevelVolume(PxdVolumeDesc& volumeDesc)
{
	PxVolume::createLowLevelVolume(volumeDesc, getPxScene());
}

// Get an iterator to the interactions connected to the element
NX_INLINE PxIterator<PxElementInteraction*const> PxElement::getInteractions() const 
{ 
	return PxIterator<PxElementInteraction*const>(mInteractions.begin(), mInteractions.end()); 
}

NX_INLINE NxU32 PxElement::getInteractionCount() const
{
	return mInteractions.size();
}

NX_INLINE PxActor& PxElement::getPxActor() const
{
	return mActor;
}

NX_INLINE PxScene& PxElement::getPxScene() const
{
	return mActor.getPxScene();
}

NX_INLINE bool PxElement::isActive() const
{
	return mActor.isActive();
}

//These are called from interaction creation/destruction
NX_INLINE NxU16 PxElement::addInteraction(PxElementInteraction* interaction)
{
	mInteractions.pushBack(interaction);
	return mInteractions.size()-1;
}

NX_INLINE void PxElement::removeInteraction(NxU16 id)
{
	mInteractions.replaceWithLast(id);
}

NX_INLINE PxElementInteraction* PxElement::getInteraction(NxU16 id) const
{
	NX_ASSERT(id<mInteractions.size());
	return mInteractions[id];
}

#endif

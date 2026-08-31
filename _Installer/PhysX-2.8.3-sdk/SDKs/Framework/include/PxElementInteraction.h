#ifndef NX_FRAMEWORK_PXELEMENTINTERACTION
#define NX_FRAMEWORK_PXELEMENTINTERACTION
/*----------------------------------------------------------------------------*\
|
|							  NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "PxInteraction.h"
#include "PxElement.h"

#define PX_INVALID_INTERACTION_ELEMENT_ID 0xffff

class PxElementInteraction : public PxInteraction
{
	friend class PxElement;
public:
	virtual ~PxElementInteraction() {};
	NX_INLINE PxElement& getElement0() const { return mElement0; }
	NX_INLINE PxElement& getElement1() const { return mElement1; }

	NX_INLINE virtual void initialize();
	NX_INLINE virtual void destroy();

protected:
	NX_INLINE PxElementInteraction(PxElement& element0, PxElement& element1, PxInteractionType type);
	NX_INLINE void setElementId(PxElement* element, NxU16 id);
	NX_INLINE NxU16 getElementId(PxElement* element) const;

private:
	PxElement& mElement0;
	PxElement& mElement1;
	NxU16 mElementId0;
	NxU16 mElementId1;
};

//////////////////////////////////////////////////////////////////////////
NX_INLINE PxElementInteraction::PxElementInteraction(PxElement& element0, PxElement& element1, PxInteractionType type)
: PxInteraction(element0.getPxActor(), element1.getPxActor(), type), 
mElement0(element0), mElement1(element1), 
mElementId0(PX_INVALID_INTERACTION_ELEMENT_ID), mElementId1(PX_INVALID_INTERACTION_ELEMENT_ID)
{
}

NX_INLINE void PxElementInteraction::initialize()
{
	PxInteraction::initialize();
	setElementId(&mElement0, mElement0.addInteraction(this));
	setElementId(&mElement1, mElement1.addInteraction(this));
}

NX_INLINE void PxElementInteraction::destroy()
{
	NxU16 id0 = getElementId(&mElement0);
	mElement0.removeInteraction(id0);
	if (id0<mElement0.getInteractionCount())
		mElement0.getInteraction(id0)->setElementId(&mElement0, id0);
	mElementId0 = PX_INVALID_INTERACTION_ELEMENT_ID;

	NxU16 id1 = getElementId(&mElement1);
	mElement1.removeInteraction(id1);
	if (id1<mElement1.getInteractionCount())
		mElement1.getInteraction(id1)->setElementId(&mElement1, id1);
	mElementId1 = PX_INVALID_INTERACTION_ELEMENT_ID;

	PxInteraction::destroy();
}

NX_INLINE void PxElementInteraction::setElementId(PxElement* element, NxU16 id) 
{ 
	NX_ASSERT(id != PX_INVALID_INTERACTION_ELEMENT_ID); 
	if (&mElement0 == element) 
		mElementId0 = id; 
	else 
		mElementId1 = id; 
}

NX_INLINE NxU16 PxElementInteraction::getElementId(PxElement* element) const 
{ 
	if (&mElement0 == element) 
		return mElementId0; 
	else 
		return mElementId1; 
}

#endif

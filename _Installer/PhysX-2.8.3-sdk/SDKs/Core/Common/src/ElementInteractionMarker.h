#ifndef NX_COLLISION_ELEMENT_INTERACTION_MARKER
#define NX_COLLISION_ELEMENT_INTERACTION_MARKER

#include "ElementInteraction.h"

class ElementInteractionMarker : public ElementInteraction
{
public:
	NX_INLINE ElementInteractionMarker(CoreElement& element0, CoreElement& element1);
	virtual ~ElementInteractionMarker() {};
	virtual void initialize();
	virtual void destroy();

	virtual void updateState();
	virtual void onActivate();
	virtual void onDeactivate();
	NX_INLINE virtual bool isActive() const;

	//Implementation of NxAllocatable
	NX_INLINE void* operator new(size_t s, void* memory);
};

NX_INLINE ElementInteractionMarker::ElementInteractionMarker(CoreElement& element0, CoreElement& element1)
	: ElementInteraction(element0, element1, PX_INTERACTION_TYPE_MARKER)
{
}

//////////////////////////////////////////////////////////////////////////
NX_INLINE bool ElementInteractionMarker::isActive() const
{
	return false;
}

NX_INLINE void* ElementInteractionMarker::operator new(size_t s, void* memory)
{
	return memory;
}


#endif //NX_COLLISION_SHAPEINTERACTIONMARKER

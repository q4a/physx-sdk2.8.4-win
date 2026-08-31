#ifndef NX_PHYSICS_FORCE_FIELD_MANAGER
#define NX_PHYSICS_FORCE_FIELD_MANAGER

#include "NvScene.h"
#include "NxSpinMutex.h"

class NpForceField;

class ForceFieldManager : public NvUserRawBoundsReport
{
public:
	virtual void onEnterBounds(NvRawBounds&, NvShape&);
	virtual void onLeaveBounds(NvRawBounds&, NvShape&);
	virtual void onEnterBounds(NvRawBounds&, NvRawBounds&);
	virtual void onLeaveBounds(NvRawBounds&, NvRawBounds&);

	NxSpinMutex spinMutex;
};

#endif

#ifndef PXN_BODYSHAPE_H
#define PXN_BODYSHAPE_H

#include "px_config.h"
#include "px_geometry.h"

class PxnBodyAtom;
class PxnShape;

class PxnBodyShape
{
public:
	virtual void				setTransformV(const PxdTransform& v)			= 0;
	virtual const PxdTransform&	getTransformV()							const	= 0;

	virtual void				setBodyAtomV(PxnBodyAtom* b)					= 0;
	virtual PxnBodyAtom*		getBodyAtomV()							const	= 0;

	virtual PxnShape*			getShapeV()								const	= 0;

	virtual void				setUserDataV(void* data)						= 0;
	virtual void*				getUserDataV()							const	= 0;

	virtual PxdHandle			getHandleV()							const	= 0;
	virtual void				destroyV()										= 0;
protected:
	virtual						~PxnBodyShape()									{}
};

#endif

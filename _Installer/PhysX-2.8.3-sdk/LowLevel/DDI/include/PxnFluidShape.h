#ifndef PXN_FLUIDSHAPE_H
#define PXN_FLUIDSHAPE_H

#include "px_config.h"
#include "px_geometry.h"


class PxnFluid;

class PxnFluidShape
{
public:
	virtual PxnFluid*			getFluidV()								const	= 0;

	virtual	const PxdVector&	getOriginV()							const	= 0;

	virtual	const PxdVector&	getExtentV()							const	= 0;

	virtual	PxU16				getParticleOffsetV()					const	= 0;

	virtual	PxU16				getParticleCountV()						const	= 0;

	virtual void				setUserDataV(void* data)						= 0;
	virtual void*				getUserDataV()							const	= 0;

	virtual PxdHandle			getHandleV()							const	= 0;
	virtual void				destroyV()										= 0;
protected:
	virtual						~PxnFluidShape()									{}
};


#endif

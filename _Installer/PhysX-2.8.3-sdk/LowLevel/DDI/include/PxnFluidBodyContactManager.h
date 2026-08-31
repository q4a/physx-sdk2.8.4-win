#ifndef PXN_FLUIDBODY_CONTACT_MANAGER_H
#define PXN_FLUIDBODY_CONTACT_MANAGER_H

#include "px_config.h"
#include "px_manager.h"


class PxnBodyShape;
class PxnFluidShape;

class PxnFluidBodyContactManager
{
public:
	virtual void		activateV()													= 0;
	virtual void		deactivateV()												= 0;
	virtual bool		isActiveV()											const	= 0;

	virtual void*		getUserDataV()										const	= 0;		
	virtual void		setUserDataV(void* data)									= 0;

	virtual PxdHandle	getHandleV()										const	= 0;
	virtual void		destroyV()													= 0;

	virtual void		getShapesV(PxnFluidShape*& s0, PxnBodyShape*& s1)	const	= 0;

	virtual bool		getIsDrainV()										const	= 0;
	virtual void		setIsDrainV(PxInt isDrain)									= 0;

protected:
	virtual				~PxnFluidBodyContactManager()								{}
};


#endif

#ifndef PXN_CONTACT_MANAGER_H
#define PXN_CONTACT_MANAGER_H

#include "px_config.h"
#include "px_manager.h"

class PxnBodyShape;

class PxnContactManager
{
public:
	virtual void		setDynamicFrictionV(PxFloat v)								= 0;
	virtual PxFloat		getDynamicFrictionV()								const	= 0;

	virtual void		setStaticFrictionV(PxFloat v)								= 0;
	virtual PxFloat		getStaticFrictionV()								const	= 0;

	virtual void		setRestitutionV(PxFloat v)									= 0;
	virtual PxFloat		getRestitutionV()									const	= 0;

	virtual bool		getInfoReportingV()									const	= 0;
	virtual bool		getImpulseReportingV()								const	= 0;
	virtual bool		getChangeableContactsV()							const	= 0;

	virtual void		setMaxConstraintCountV(PxU32 v)								= 0;
	virtual PxU32		getMaxConstraintCountV()							const	= 0;

	virtual bool		getTouchStatusV()									const	= 0;

	virtual PxU32		getConstraintCountV()								const	= 0;
	virtual void		getConstraintsV(PxdConstraint* clist)				const	= 0;

	virtual void		getShapesV(PxnBodyShape*& s0, PxnBodyShape*& s1)	const	= 0;

	virtual PxFloat		getSkinWidthV()										const	= 0;
	virtual void		setSkinWidthV(PxFloat v)									= 0;

	virtual PxFloat		getCorrelationDistanceV()							const	= 0;
	virtual void		setCorrelationDistanceV(PxFloat v)							= 0;
	
	virtual PxInt		mapPageV(PxdShape s)										= 0;
	virtual void		unmapPageV(PxdShape s)										= 0;

	virtual void		activateV()													= 0;
	virtual void		deactivateV()												= 0;
	virtual bool		isActiveV()											const	= 0;

	virtual void*		getUserDataV()										const	= 0;		
	virtual void		setUserDataV(void* data)									= 0;

	virtual PxdHandle	getHandleV()										const	= 0;
	virtual void		destroyV()													= 0;

	virtual bool		getDisableResponseV()								const	= 0;

	virtual PxFloat		getDominance0V()									const	= 0;
	virtual void		setDominance0V(PxFloat)										= 0;

	virtual PxFloat		getDominance1V()									const	= 0;
	virtual void		setDominance1V(PxFloat)										= 0;
protected:
	virtual				~PxnContactManager()										{}
};

#endif

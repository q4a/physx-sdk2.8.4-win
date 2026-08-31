#ifndef PXN_VOLUME_H
#define PXN_VOLUME_H

#include "px_config.h"
#include "px_broadphase.h"

class PxnBodyShape;

class PxnVolume
{
public:
	virtual const PxdVector&	getOriginV()					const	= 0;
	virtual void				setOriginV(const PxdVector& v)			= 0;

	virtual const PxdVector&	getExtentV()					const	= 0;
	virtual void				setExtentV(const PxdVector& v)			= 0;

	virtual PxU32				getGroupV()						const	= 0;
	virtual void				setGroupV(PxU32 group)					= 0;

	virtual void*				getUserDataV()					const	= 0;
	virtual void				setUserDataV(void* data)				= 0;

	virtual PxnBodyShape*		getShapeV()						const	= 0;
	virtual PxdVolumeType		getTypeV()						const	= 0;

	virtual PxdHandle			getHandleV()					const	= 0;
	virtual void				destroyV()								= 0;

protected:
	virtual						~PxnVolume()							{}
};

#endif


#ifndef PXN_BROADPHASE_H
#define PXN_BROADPHASE_H

#include "px_config.h"
#include "px_broadphase.h"

class PxnVolume;

class PxnBroadPhase
{
public:
	virtual PxUInt				getCreatedOverlapsCountV()						const	= 0;
	virtual PxUInt				getDestroyedOverlapsCountV()					const	= 0;

	virtual bool				fillCreatedOverlapsV(PxdBroadPhaseOverlap* overlaps)	= 0;
	virtual bool				fillDestroyedOverlapsV(PxdBroadPhaseOverlap* overlaps)	= 0;

	virtual void				setReportCreatedOverlapsV(bool)							= 0;
	virtual void				setReportDestroyedOverlapsV(bool)						= 0;
	virtual void				setReportIdentifierV(bool)								= 0;
	virtual void				setReportUserDataV(bool)								= 0;

	virtual bool				getReportCreatedOverlapsV()						const	= 0;
	virtual bool				getReportDestroyedOverlapsV()					const	= 0;
	virtual bool				getReportIdentifierV()							const	= 0;
	virtual bool				getReportUserDataV()							const	= 0;

	// PT: functions added to complete the interface and make it usable. Mandatory to support multiple implementations at the same time.
	virtual	void				destroyV()												= 0;
	virtual	PxnVolume*			createVolumeV(const PxdVolumeDesc& desc)				= 0;
	virtual	void				releaseVolumeV(PxnVolume* volume)						= 0;
	virtual	PxnVolume*			findVolumeV(PxdHandle handle)							= 0;
	virtual	bool				updateBroadPhaseV()										= 0;

	// PT: only used for multi SAP broadphase type. Silently ignored by others.
	virtual void				setUpAxis(PxU32 axis)									{						}
	virtual PxU32				getUpAxis()										const	{ return 0xffffffff;	}
	virtual void				setNbX(PxU32 nb)										{						}
	virtual PxU32				getNbX()										const	{ return 0xffffffff;	}
	virtual void				setNbY(PxU32 nb)										{						}
	virtual PxU32				getNbY()										const	{ return 0xffffffff;	}
	virtual void				setSizeX(PxFloat val)									{						}
	virtual PxFloat				getSizeX()										const	{ return 0.0f;			}
	virtual void				setSizeY(PxFloat val)									{						}
	virtual PxFloat				getSizeY()										const	{ return 0.0f;			}
};

#endif

#ifndef PXN_BODYATOM_H
#define PXN_BODYATOM_H

#include "px_dynamics.h"

class PxnBodyAtom
{
public:
	virtual const PxdTransform&		getPoseV()										const		= 0;
	virtual void	 				setPoseV(const PxdTransform& v)								= 0;

	virtual const PxdSpatialVector&	getVelocityV()									const		= 0;
	virtual void	 				setVelocityV(const PxdSpatialVector& v)						= 0;

	virtual const PxdSpatialVector&	getAccelerationV()								const		= 0;
	virtual void	 				setAccelerationV(const PxdSpatialVector& v)					= 0;

	virtual void					getDampingV(PxFloat& lin, PxFloat& ang)			const		= 0;
	virtual void					setDampingV(PxFloat lin, PxFloat ang)						= 0;

	virtual void					getVelocityCapV(PxFloat& lin, PxFloat& ang)		const		= 0;
	virtual void					setVelocityCapV(PxFloat lin, PxFloat ang)					= 0;

	virtual void					getInverseMassV(PxdVector& i, PxFloat& m)		const		= 0;
	virtual void					setInverseMassV(const PxdVector& i, PxFloat m)				= 0;

	virtual bool					getRestingV()									const		= 0;
	virtual void					setRestingV(bool resting)									= 0;

	virtual void					activateV()													= 0;
	virtual void					deactivateV()												= 0;
	virtual bool					isActiveV()										const		= 0;

	virtual void					setIterationCountV(PxInt i)									= 0;
	virtual PxInt					getIterationCountV()							const		= 0;

	virtual void					setHalfPiClampV(bool clamp)									= 0;
	virtual bool					getHalfPiClampV()								const		= 0;

	virtual	void					setReportThresholdV(PxFloat threshold)						= 0;
	virtual float					getReportThresholdV()							const		= 0;

	virtual void					destroyV()													= 0;
	virtual PxdHandle				getHandleV()									const		= 0;
protected:
	virtual							~PxnBodyAtom()												{}
};

#endif

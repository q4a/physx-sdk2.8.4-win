#if 0
#ifndef PXS_CONSTRAINTHELPER_H
#define PXS_CONSTRAINTHELPER_H

#include "px_types.h"

PXD_INLINE void PxcImplicitSpringHelper(PxFloat realSCoeff, PxFloat realDCoeff, PxFloat dt,
										PxFloat& implicitSCoeff, PxFloat& implicitDCoeff)
{
	realDCoeff = PxcMax(realDCoeff, 0.00001f);

	implicitSCoeff = realSCoeff*dt + realDCoeff;
	implicitDCoeff = dt * realSCoeff / implicitSCoeff;
	implicitSCoeff = 1.0f/(dt*implicitSCoeff);
}


#endif
#endif
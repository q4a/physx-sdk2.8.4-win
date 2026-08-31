#ifndef PXC_MAKEFATEDGE_H
#define PXC_MAKEFATEDGE_H

#include "px_config.h"
#include "PxcVector.h"

/*!
Extend an edge along its length by a factor
*/
PXD_FORCE_INLINE static void PxcMakeFatEdge(PxcVector& p0, PxcVector& p1, PxFloat coef = 0.01f)
{
	PxcVector delta = p1-p0;
	PxFloat length = delta.magnitude();
	delta *= coef/length;
	p0 -= delta;
	p1 += delta;
}

#endif

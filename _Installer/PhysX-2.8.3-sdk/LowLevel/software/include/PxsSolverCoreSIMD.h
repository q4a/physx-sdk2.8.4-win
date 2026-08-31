#ifndef PXS_SOLVERCORESIMD_H
#define PXS_SOLVERCORESIMD_H

#include "PxsSolverCore.h"

class PxsSolverCoreSIMD : public PxsSolverCore
{
public:
	static PxsSolverCoreSIMD* create();

	// Implements PxsSolverCore
	virtual void destroyV();

	//! Solve
	virtual void solveV(PxFloat dt, PxU32 maxIterations,
		PxsSolverBodyAtom* atomListStart, PxU32 atomListSize,
		PxU8* constraintListStart, PxU32 constraintListSize,
		PxcStreamedThresholdTable<PxsBodyAtom*>& thresholdTable) const;
	//~Implements PxsSolverCore
};

#endif

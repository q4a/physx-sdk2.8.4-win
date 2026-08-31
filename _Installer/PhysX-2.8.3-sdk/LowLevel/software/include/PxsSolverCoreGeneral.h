#ifndef PXS_SOLVERCOREGENERAL_H
#define PXS_SOLVERCOREGENERAL_H

#include "PxsSolverCore.h"

class PxsSolverCoreGeneral : public PxsSolverCore
{
public:
	static PxsSolverCoreGeneral* create();

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

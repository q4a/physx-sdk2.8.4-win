#ifndef PXS_SOLVERCORE_H
#define PXS_SOLVERCORE_H

#include "px_config.h"
template <class BodyPtr> class PxcStreamedThresholdTable;

class PxsSolverBodyAtom;
class PxsSolverConstraint;
class PxsBodyAtom;

/*!
Interface to constraint solver cores

*/
class PxsSolverCore
{
public:
	virtual void destroyV() = 0;

	/*
	solves dual problem exactly by GS-iterating until convergence stops
	only uses regular velocity vector for storing results, and backs up initial state, which is restored.
	the solution forces are saved in a vector.

	state should not be stored, this function is safe to call from multiple threads.
	*/
	virtual void solveV(PxFloat dt, PxU32 maxIterations,
		PxsSolverBodyAtom* atomListStart, PxU32 atomListSize,
		PxU8* constraintListStart, PxU32 constraintListSize, PxcStreamedThresholdTable<PxsBodyAtom *> &thresholdTable) const = 0;
};

#endif

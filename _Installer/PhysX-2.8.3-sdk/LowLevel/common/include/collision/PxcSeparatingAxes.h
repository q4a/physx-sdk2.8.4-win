#ifndef PXC_SEPARATINGAXES_H
#define PXC_SEPARATINGAXES_H

#include "PxcArray.h"
#include "PxcVector.h"

// ### TODO better later

// This class holds a list of potential separating axes.
// - the orientation is irrelevant so V and -V should be the same vector
// - the scale is irrelevant so V and n*V should be the same vector
// - a given separating axis should appear only once in the class
class PxcSeparatingAxes
{
public:
	PXD_INLINE PxcSeparatingAxes()
	{}

	bool addAxis(const PxcVector& axis);

	PXD_INLINE const PxcVector* getAxes() const
	{
		return mAxes.getPtr();
	}

	PXD_INLINE PxU32 getNumAxes() const
	{
		return mAxes.getSize();
	}

	PXD_INLINE void reset()
	{
		mAxes.clear();
	}

private:
	PxcArray<PxcVector> mAxes;
};

#endif

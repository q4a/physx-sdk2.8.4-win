#ifndef SHAPE_MATCHER_H
#define SHAPE_MATCHER_H

#include "NxFoundation.h"

//---------------------------------------------------------------------
class ShapeMatcher 
{
public:
	void doShapeMatching(NxU32 numPoints, const void *origPos, NxU32 origByteStride, const void *newPos, NxU32 newByteStride,
									float *invMasses, NxU32 invMassByteStride, NxU32 *pointSubset,
									void *displacements, NxU32 displacementByteStride);
private :
	void jacobiRotate(NxMat33 &A, NxMat33 &R, int p, int q);
	void eigenDecomposition(NxMat33 &A, NxMat33 &R);
	void polarDecomposition(const NxMat33 &A, NxMat33 &R);
};


#endif

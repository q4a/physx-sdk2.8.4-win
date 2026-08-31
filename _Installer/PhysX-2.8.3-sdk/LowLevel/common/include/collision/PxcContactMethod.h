#ifndef PXC_CONTACTMETHOD_H
#define PXC_CONTACTMETHOD_H

#include "px_config.h"

#include "PxcTransform.h"
#include "PxcVector.h"
#include "PxcCachedTransforms.h"

class PxnBodyShape;
struct PxcShapeData;
class PxcContactCacheObject;

/*!
Callback interface for contact reports
*/
class PxcContactCallback
{
public:
	// PT: do we really have a virtual call per contact now ?
	virtual void		contact(const PxcVector& worldPoint,
						const PxcVector& worldNormal,
						PxFloat separation,
						PxU32 featureIndex0 = 0,
						PxU32 featureIndex1 = 0)						= 0;
};



/*!
Method prototype for contact generation routines
*/
typedef bool (*PxcContactMethod) (const PxcShapeData& shape0, const PxcShapeData& shape1, 
								  const PxcCachedTransforms& transform0, const PxcCachedTransforms& transform1,
								  PxcContactCallback& callback, PxcContactCacheObject& cache);

#endif

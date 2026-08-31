#ifndef NX_PHYSICS_SOFTBODY
#define NX_PHYSICS_SOFTBODY

/*----------------------------------------------------------------------------*\
|
| NVIDIA PhysX Technology
|
| www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Array.h"

class NvShape;

// ------------------------------------------------------------------------------

class SoftBodyCore
{
public:
	void addShape(const NvShape *shape);
	void removeShape(const NvShape *shape);
	bool containsShape(const NvShape *shape);

private:
	struct ShapeRef {
		const NvShape *shape;
		NxU32 counter;
	};

	NxFoundation::NxArraySDK<ShapeRef> mShapeRefs;
};


// ------------------------------------------------------------------------------

#endif

#ifndef NX_FOUNDATION_ARRAY
#define NX_FOUNDATION_ARRAY
/*----------------------------------------------------------------------------*\
|
|						Public Interface to NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "NxArray.h"
#include "UserAllocatorAccess.h"
#include "Allocateable.h"
namespace NxFoundation
	{

/**
 Same as NxArray, but uses the global SDK allocator
*/

template<class ElemType>
class NxArraySDK: public NxArray<ElemType, UserAllocatorAccess>, public NxAllocateable
	{
	//nothing
	};

/**
 Same as NxArray, but uses the global SDK allocator. Plus allows an alignment to be specified.

 A == Alignment (should be a power of 2).
*/
template<class ElemType, NxU32 A>
class NxArraySDKAlign : public NxArray<ElemType, UserAllocAccessAlign<A> >, public NxAllocateable
	{
	//nothing
	};

	}	

#endif

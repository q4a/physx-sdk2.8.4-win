#ifndef PXC_INTERSECTIONRAYTRIANGLE_H
#define PXC_INTERSECTIONRAYTRIANGLE_H

#include "px_config.h"
#include "PxcVector.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
*	Computes a ray-triangle intersection test.
*	From Tomas Möller's "Fast Minimum Storage Ray-Triangle Intersection"
*	Could be optimized and cut into 2 methods (culled or not). Should make a batch one too to avoid the call overhead, or make it inline.
*
*	\param		orig	[in] ray origin
*	\param		dir		[in] ray direction
*	\param		vert0	[in] triangle vertex
*	\param		vert1	[in] triangle vertex
*	\param		vert2	[in] triangle vertex
*	\param		t		[out] distance
*	\param		u		[out] impact barycentric coordinate
*	\param		v		[out] impact barycentric coordinate
*	\param		cull	[in] true to use backface culling
*	\return		true on overlap
*/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool PxcIntersectionRayTriangle(const PxcVector& orig, 
								const PxcVector& dir, 
								const PxcVector& vert0, 
								const PxcVector& vert1, 
								const PxcVector& vert2, 
								PxFloat& t,
								PxFloat& u, 
								PxFloat& v, 
								bool cull);

#endif

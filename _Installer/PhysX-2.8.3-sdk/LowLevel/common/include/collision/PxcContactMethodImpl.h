#ifndef PXC_CONTACTMETHODIMPL_H
#define PXC_CONTACTMETHODIMPL_H

#include "PxcContactMethod.h"

class PxcConvexInterface;
struct PxcShapeMesh;

/*!\file
This file contains forward declarations of all implemented contact methods.
*/

// Sphere - other
bool PxcContactSphereSphere(const PxcShapeData& shape0, const PxcShapeData& shape1, 
							const PxcCachedTransforms& transform0, const PxcCachedTransforms& transform1,
							PxcContactCallback& callback, PxcContactCacheObject& cache);


bool PxcContactSpherePlane(const PxcShapeData& shape0, const PxcShapeData& shape1, 
						   const PxcCachedTransforms& transform0, const PxcCachedTransforms& transform1,
						   PxcContactCallback& callback, PxcContactCacheObject& cache);

bool PxcContactSphereCapsule(const PxcShapeData& shape0, const PxcShapeData& shape1, 
							 const PxcCachedTransforms& transform0, const PxcCachedTransforms& transform1,
							 PxcContactCallback& callback, PxcContactCacheObject& cache);


bool PxcContactSphereBox(const PxcShapeData& shape0, const PxcShapeData& shape1, 
						 const PxcCachedTransforms& transform0, const PxcCachedTransforms& transform1,
						 PxcContactCallback& callback, PxcContactCacheObject& cache);

bool PxcContactSphereConvex(const PxcShapeData& shape0, const PxcShapeData& shape1, 
							const PxcCachedTransforms& transform0, const PxcCachedTransforms& transform1,
							PxcContactCallback& callback, PxcContactCacheObject& cache);

bool PxcContactSphereMesh(const PxcShapeData& shape0, const PxcShapeData& shape1, 
						  const PxcCachedTransforms& transform0, const PxcCachedTransforms& transform1,
						  PxcContactCallback& callback, PxcContactCacheObject& cache);

bool PxcContactSphereHeightField(const PxcShapeData& shape0, const PxcShapeData& shape1, 
								 const PxcCachedTransforms& transform0, const PxcCachedTransforms& transform1,
								 PxcContactCallback& callback, PxcContactCacheObject& cache);


// Plane - other
bool PxcContactPlaneCapsule(const PxcShapeData& shape0, const PxcShapeData& shape1, 
							const PxcCachedTransforms& transform0, const PxcCachedTransforms& transform1,
							PxcContactCallback& callback, PxcContactCacheObject& cache);

bool PxcContactPlaneBox(const PxcShapeData& shape0, const PxcShapeData& shape1, 
						const PxcCachedTransforms& transform0, const PxcCachedTransforms& transform1,
						PxcContactCallback& callback, PxcContactCacheObject& cache);

bool PxcContactPlaneConvex(const PxcShapeData& shape0, const PxcShapeData& shape1, 
						   const PxcCachedTransforms& transform0, const PxcCachedTransforms& transform1,
						   PxcContactCallback& callback, PxcContactCacheObject& cache);

bool PxcContactPlaneMesh(const PxcShapeData& shape0, const PxcShapeData& shape1, 
						 const PxcCachedTransforms& transform0, const PxcCachedTransforms& transform1,
						 PxcContactCallback& callback, PxcContactCacheObject& cache);



// Capsule - other
bool PxcContactCapsuleCapsule(const PxcShapeData& shape0, const PxcShapeData& shape1,
							  const PxcCachedTransforms& transform0, const PxcCachedTransforms& transform1,
							  PxcContactCallback& callback, PxcContactCacheObject& cache);

bool PxcContactCapsuleCapsuleSingleNormal(const PxcShapeData& shape0, const PxcShapeData& shape1,
										  const PxcCachedTransforms& transform0, const PxcCachedTransforms& transform1,
										  PxcContactCallback& callback, PxcContactCacheObject& cache);

bool PxcContactCapsuleBox(const PxcShapeData& shape0, const PxcShapeData& shape1,
						  const PxcCachedTransforms& transform0, const PxcCachedTransforms& transform1,
						  PxcContactCallback& callback, PxcContactCacheObject& cache);

bool PxcContactCapsuleConvex(const PxcShapeData& shape0, const PxcShapeData& shape1, 
							 const PxcCachedTransforms& transform0, const PxcCachedTransforms& transform1,
							 PxcContactCallback& callback, PxcContactCacheObject& cache);

bool PxcContactCapsuleMesh(const PxcShapeData& shape0, const PxcShapeData& shape1,
						   const PxcCachedTransforms& cacheTrans0, const PxcCachedTransforms& cacheTrans1,
						   PxcContactCallback& callback, PxcContactCacheObject& cache);

bool PxcContactCapsuleHeightField(const PxcShapeData& shape0, const PxcShapeData& shape1, 
								  const PxcCachedTransforms& transform0, const PxcCachedTransforms& transform1,
								  PxcContactCallback& callback, PxcContactCacheObject& cache);

// Box - other
bool PxcContactBoxBox(const PxcShapeData& shape0, const PxcShapeData& shape1,
					  const PxcCachedTransforms& transform0, const PxcCachedTransforms& transform1,
					  PxcContactCallback& callback, PxcContactCacheObject& cache);

bool PxcContactBoxConvex(const PxcShapeData& shape0, const PxcShapeData& shape1, 
						 const PxcCachedTransforms& transform0, const PxcCachedTransforms& transform1,
						 PxcContactCallback& callback, PxcContactCacheObject& cache);

bool PxcContactBoxMesh(const PxcShapeData& shape0, const PxcShapeData& shape1, 
					   const PxcCachedTransforms& transform0, const PxcCachedTransforms& transform1,
					   PxcContactCallback& callback, PxcContactCacheObject& cache);

bool PxcContactBoxHeightField(const PxcShapeData& shape0, const PxcShapeData& shape1,
							  const PxcCachedTransforms& transform0, const PxcCachedTransforms& transform1,
							  PxcContactCallback& callback, PxcContactCacheObject& cache);


// Convex - other
bool PxcContactConvexConvex(const PxcShapeData& shape0, const PxcShapeData& shape1, 
							const PxcCachedTransforms& transform0, const PxcCachedTransforms& transform1,
							PxcContactCallback& callback, PxcContactCacheObject& cache);

bool PxcContactConvexMesh(const PxcShapeData& shape0, const PxcShapeData& shape1, 
						  const PxcCachedTransforms& transform0, const PxcCachedTransforms& transform1,
						  PxcContactCallback& callback, PxcContactCacheObject& cache);

bool PxcContactConvexHeightField(const PxcShapeData& shape0, const PxcShapeData& shape1, 
								 const PxcCachedTransforms& transform0, const PxcCachedTransforms& transform1,
								 PxcContactCallback& callback, PxcContactCacheObject& cache);

// Matrix of types
extern const PxcContactMethod g_ContactMethodTable[][9];


// Helper for convex - convex
bool PxcContactHullHull(const PxcConvexInterface& hull0, const PxcConvexInterface& hull1, 
						const PxcCachedTransforms& transform0, const PxcCachedTransforms& transform1, 
						PxcContactCallback& callback, PxcContactCacheObject& cache);

// Helper for convex - mesh
bool PxcContactHullMesh(const PxcConvexInterface& convex0, const PxcShapeMesh& shape1, 
						const PxcCachedTransforms& cacheTrans0, const PxcCachedTransforms& cacheTrans1,
						PxcContactCallback& callback, PxcContactCacheObject& cache);

#endif

#ifndef __NP_INTERSECTION__
#define __NP_INTERSECTION__


/*
** From NxIntersectionBoxBox.h
*/
/******************************************************************/

bool NX_CALL_CONV NpBoxBoxIntersect(	const NxVec3& extents0, const NxVec3& center0, const NxMat33& rotation0,
										const NxVec3& extents1, const NxVec3& center1, const NxMat33& rotation1,
										bool fullTest);

bool NX_CALL_CONV NpTriBoxIntersect(const NxVec3 & vertex0, const NxVec3 & vertex1, const NxVec3 & vertex2, const NxVec3 & center, const NxVec3& extents);

NxSepAxis NX_CALL_CONV NpSeparatingAxis(	const NxVec3& extents0, const NxVec3& center0, const NxMat33& rotation0,
											const NxVec3& extents1, const NxVec3& center1, const NxMat33& rotation1,
											bool fullTest=true);

	/*
	** From NxIntersectionRayPlane.h
	*/
/*************************************************************************/

void NX_CALL_CONV	NpSegmentPlaneIntersect(const NxVec3& v1, const NxVec3& v2, 
								const NxPlane& plane, NxReal& dist, NxVec3& pointOnPlane);

bool NX_CALL_CONV	NpRayPlaneIntersect(const NxRay& ray, const NxPlane& plane, 
								NxReal& dist, NxVec3& pointOnPlane);

/*
** From NxIntersectionRaySphere.h
*/
/**************************************************************************/

bool NX_CALL_CONV NpRaySphereIntersect(const NxVec3& origin, const NxVec3& dir, NxReal length, const NxVec3& center, NxReal radius, NxReal& hit_time, NxVec3& hit_pos);

	/*
** From NxIntersectionSegmentBox
*/
/**************************************************************************/

bool NX_CALL_CONV NpSegmentBoxIntersect(const NxVec3& p1, const NxVec3& p2,
									const NxVec3& bbox_min,const NxVec3& bbox_max, NxVec3& intercept);

bool NX_CALL_CONV NpRayAABBIntersect(const NxVec3& min, const NxVec3& max, 
									const NxVec3& origin, const NxVec3& dir, NxVec3& coord);

NxU32 NX_CALL_CONV NpRayAABBIntersect2(const NxVec3& min, const NxVec3& max, 
										const NxVec3& origin, const NxVec3& dir, NxVec3& coord, NxReal & t);

bool NX_CALL_CONV NpSegmentOBBIntersect(const NxVec3& p0, const NxVec3& p1, 
										const NxVec3& center, const NxVec3& extents, const NxMat33& rot);

bool NX_CALL_CONV NpSegmentAABBIntersect(const NxVec3& p0, const NxVec3& p1, 
										const NxVec3& min, const NxVec3& max);

bool NX_CALL_CONV NpRayOBBIntersect(const NxRay& ray, const NxVec3& center, 
									const NxVec3& extents, const NxMat33& rot);

/*
** From NxIntersectionSegmentCapsule.h
*/
/*************************************************************************/

NxU32 NX_CALL_CONV NpRayCapsuleIntersect(const NxVec3& origin, const NxVec3& dir, 
										const NxCapsule& capsule, NxReal t[2]);

/*
** From NxIntersectionSweptSpheres.h
*/
/***************************************************************************/

bool NX_CALL_CONV NpSweptSpheresIntersect(	const NxSphere& sphere0, const NxVec3& velocity0,
											const NxSphere& sphere1, const NxVec3& velocity1);

/*
** From NxRayTriIntersect.h
*/
bool NX_CALL_CONV NpRayTriIntersect(const NxVec3& orig, const NxVec3& dir, const NxVec3& vert0, const NxVec3& vert1, const NxVec3& vert2, float& t, float& u, float& v, bool cull);

#endif

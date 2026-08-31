
#include "OPC_Simd.h"

//! This macro quickly finds the min & max values among 3 variables
#define FINDMINMAX(x0, x1, x2, min, max)	\
	min = max = x0;							\
	if(x1<min) min=x1;						\
	if(x1>max) max=x1;						\
	if(x2<min) min=x2;						\
	if(x2>max) max=x2;

//! TO BE DOCUMENTED
inline_ BOOL planeBoxOverlap(const Point& normal, const float d, const Point& maxbox)
{
	Point vmin, vmax;
	for(udword q=0;q<=2;q++)
	{
		if(normal[q]>0.0f)	{ vmin[q]=-maxbox[q]; vmax[q]=maxbox[q]; }
		else				{ vmin[q]=maxbox[q]; vmax[q]=-maxbox[q]; }
	}
	if((normal|vmin)+d>0.0f) return FALSE;
	if((normal|vmax)+d>=0.0f) return TRUE;

	return FALSE;
}

//! TO BE DOCUMENTED
#define AXISTEST_X01(a, b, fa, fb)							\
	min = a*v0.y - b*v0.z;									\
	max = a*v2.y - b*v2.z;									\
	if(min>max) {const float tmp=max; max=min; min=tmp;	}	\
	rad = fa * extents.y + fb * extents.z;					\
	if(min>rad || max<-rad) return FALSE;

//! TO BE DOCUMENTED
#define AXISTEST_X2(a, b, fa, fb)							\
	min = a*v0.y - b*v0.z;									\
	max = a*v1.y - b*v1.z;									\
	if(min>max) {const float tmp=max; max=min; min=tmp;	}	\
	rad = fa * extents.y + fb * extents.z;					\
	if(min>rad || max<-rad) return FALSE;

//! TO BE DOCUMENTED
#define AXISTEST_Y02(a, b, fa, fb)							\
	min = b*v0.z - a*v0.x;									\
	max = b*v2.z - a*v2.x;									\
	if(min>max) {const float tmp=max; max=min; min=tmp;	}	\
	rad = fa * extents.x + fb * extents.z;					\
	if(min>rad || max<-rad) return FALSE;

//! TO BE DOCUMENTED
#define AXISTEST_Y1(a, b, fa, fb)							\
	min = b*v0.z - a*v0.x;									\
	max = b*v1.z - a*v1.x;									\
	if(min>max) {const float tmp=max; max=min; min=tmp;	}	\
	rad = fa * extents.x + fb * extents.z;					\
	if(min>rad || max<-rad) return FALSE;

//! TO BE DOCUMENTED
#define AXISTEST_Z12(a, b, fa, fb)							\
	min = a*v1.x - b*v1.y;									\
	max = a*v2.x - b*v2.y;									\
	if(min>max) {const float tmp=max; max=min; min=tmp;	}	\
	rad = fa * extents.x + fb * extents.y;					\
	if(min>rad || max<-rad) return FALSE;

//! TO BE DOCUMENTED
#define AXISTEST_Z0(a, b, fa, fb)							\
	min = a*v0.x - b*v0.y;									\
	max = a*v1.x - b*v1.y;									\
	if(min>max) {const float tmp=max; max=min; min=tmp;	}	\
	rad = fa * extents.x + fb * extents.y;					\
	if(min>rad || max<-rad) return FALSE;

// compute triangle edges
// - edges lazy evaluated to take advantage of early exits
// - fabs precomputed (half less work, possible since extents are always >0)
// - customized macros to take advantage of the null component
// - axis vector discarded, possibly saves useless movs
#define IMPLEMENT_CLASS3_TESTS						\
	float rad;										\
	float min, max;									\
													\
	const float fey0 = fabsf(e0.y);					\
	const float fez0 = fabsf(e0.z);					\
	AXISTEST_X01(e0.z, e0.y, fez0, fey0);			\
	const float fex0 = fabsf(e0.x);					\
	AXISTEST_Y02(e0.z, e0.x, fez0, fex0);			\
	AXISTEST_Z12(e0.y, e0.x, fey0, fex0);			\
													\
	const float fey1 = fabsf(e1.y);					\
	const float fez1 = fabsf(e1.z);					\
	AXISTEST_X01(e1.z, e1.y, fez1, fey1);			\
	const float fex1 = fabsf(e1.x);					\
	AXISTEST_Y02(e1.z, e1.x, fez1, fex1);			\
	AXISTEST_Z0(e1.y, e1.x, fey1, fex1);			\
													\
	const Point e2 = v0 - v2;						\
	const float fey2 = fabsf(e2.y);					\
	const float fez2 = fabsf(e2.z);					\
	AXISTEST_X2(e2.z, e2.y, fez2, fey2);			\
	const float fex2 = fabsf(e2.x);					\
	AXISTEST_Y1(e2.z, e2.x, fez2, fex2);			\
	AXISTEST_Z12(e2.y, e2.x, fey2, fex2);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Triangle-Box overlap test using the separating axis theorem.
 *	This is the code from Tomas Möller, a bit optimized:
 *	- with some more lazy evaluation (faster path on PC)
 *	- with a tiny bit of assembly
 *	- with "SAT-lite" applied if needed
 *	- and perhaps with some more minor modifs...
 *
 *	\param		center		[in] box center
 *	\param		extents		[in] box extents
 *	\return		true if triangle & box overlap
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*inline_ BOOL AABBTreeCollider::TriBoxOverlap(const Point& center, const Point& extents)
{
	// Stats
	mNbBVPrimTests++;

	// use separating axis theorem to test overlap between triangle and box 
	// need to test for overlap in these directions: 
	// 1) the {x,y,z}-directions (actually, since we use the AABB of the triangle 
	//    we do not even need to test these) 
	// 2) normal of the triangle 
	// 3) crossproduct(edge from tri, {x,y,z}-directin) 
	//    this gives 3x3=9 more tests 

	// move everything so that the boxcenter is in (0,0,0) 
	Point v0, v1, v2;
	v0.x = mLeafVerts[0].x - center.x;
	v1.x = mLeafVerts[1].x - center.x;
	v2.x = mLeafVerts[2].x - center.x;

	// First, test overlap in the {x,y,z}-directions
#ifdef OPC_USE_FCOMI
	// find min, max of the triangle in x-direction, and test for overlap in X
	if(FCMin3(v0.x, v1.x, v2.x)>extents.x)	return FALSE;
	if(FCMax3(v0.x, v1.x, v2.x)<-extents.x)	return FALSE;

	// same for Y
	v0.y = mLeafVerts[0].y - center.y;
	v1.y = mLeafVerts[1].y - center.y;
	v2.y = mLeafVerts[2].y - center.y;

	if(FCMin3(v0.y, v1.y, v2.y)>extents.y)	return FALSE;
	if(FCMax3(v0.y, v1.y, v2.y)<-extents.y)	return FALSE;

	// same for Z
	v0.z = mLeafVerts[0].z - center.z;
	v1.z = mLeafVerts[1].z - center.z;
	v2.z = mLeafVerts[2].z - center.z;

	if(FCMin3(v0.z, v1.z, v2.z)>extents.z)	return FALSE;
	if(FCMax3(v0.z, v1.z, v2.z)<-extents.z)	return FALSE;
#else
	float min,max;
	// Find min, max of the triangle in x-direction, and test for overlap in X
	FINDMINMAX(v0.x, v1.x, v2.x, min, max);
	if(min>extents.x || max<-extents.x) return FALSE;

	// Same for Y
	v0.y = mLeafVerts[0].y - center.y;
	v1.y = mLeafVerts[1].y - center.y;
	v2.y = mLeafVerts[2].y - center.y;

	FINDMINMAX(v0.y, v1.y, v2.y, min, max);
	if(min>extents.y || max<-extents.y) return FALSE;

	// Same for Z
	v0.z = mLeafVerts[0].z - center.z;
	v1.z = mLeafVerts[1].z - center.z;
	v2.z = mLeafVerts[2].z - center.z;

	FINDMINMAX(v0.z, v1.z, v2.z, min, max);
	if(min>extents.z || max<-extents.z) return FALSE;
#endif
	// 2) Test if the box intersects the plane of the triangle
	// compute plane equation of triangle: normal*x+d=0
	// ### could be precomputed since we use the same leaf triangle several times
	const Point e0 = v1 - v0;
	const Point e1 = v2 - v1;
	const Point normal = e0 ^ e1;
	const float d = -normal|v0;
	if(!planeBoxOverlap(normal, d, extents)) return FALSE;

	// 3) "Class III" tests
	if(mFullPrimBoxTest)
	{
		IMPLEMENT_CLASS3_TESTS
	}
	return TRUE;
}*/

#ifndef OPC_SUPPORT_SIMD

//! A dedicated version where the box is constant
inline_ BOOL OBBCollider::TriBoxOverlap(const Point& vert0, const Point& vert1, const Point& vert2)
{
	// Stats
	mNbVolumePrimTests++;

	Point v0, v1, v2;

	/* Transform them in a common space */
	TransformPoint(v0, vert0, mRModelToBox, mTModelToBox);
	TransformPoint(v1, vert1, mRModelToBox, mTModelToBox);
	TransformPoint(v2, vert2, mRModelToBox, mTModelToBox);

	const Point& extents = mBoxExtents;

	// use separating axis theorem to test overlap between triangle and box 
	// need to test for overlap in these directions: 
	// 1) the {x,y,z}-directions (actually, since we use the AABB of the triangle 
	//    we do not even need to test these) 
	// 2) normal of the triangle 
	// 3) crossproduct(edge from tri, {x,y,z}-directin) 
	//    this gives 3x3=9 more tests 

	// Box center is already in (0,0,0)

	// First, test overlap in the {x,y,z}-directions
#ifdef OPC_USE_FCOMI
	// find min, max of the triangle in x-direction, and test for overlap in X
	if(FCMin3(v0.x, v1.x, v2.x)>mBoxExtents.x)	return FALSE;
	if(FCMax3(v0.x, v1.x, v2.x)<-mBoxExtents.x)	return FALSE;

	if(FCMin3(v0.y, v1.y, v2.y)>mBoxExtents.y)	return FALSE;
	if(FCMax3(v0.y, v1.y, v2.y)<-mBoxExtents.y)	return FALSE;

	if(FCMin3(v0.z, v1.z, v2.z)>mBoxExtents.z)	return FALSE;
	if(FCMax3(v0.z, v1.z, v2.z)<-mBoxExtents.z)	return FALSE;
#else
	float min,max;
	// Find min, max of the triangle in x-direction, and test for overlap in X
	FINDMINMAX(v0.x, v1.x, v2.x, min, max);
	if(min>mBoxExtents.x || max<-mBoxExtents.x) return FALSE;

	FINDMINMAX(v0.y, v1.y, v2.y, min, max);
	if(min>mBoxExtents.y || max<-mBoxExtents.y) return FALSE;

	FINDMINMAX(v0.z, v1.z, v2.z, min, max);
	if(min>mBoxExtents.z || max<-mBoxExtents.z) return FALSE;
#endif
	// 2) Test if the box intersects the plane of the triangle
	// compute plane equation of triangle: normal*x+d=0
	// ### could be precomputed since we use the same leaf triangle several times
	const Point e0 = v1 - v0;
	const Point e1 = v2 - v1;
	const Point normal = e0 ^ e1;
	const float d = -normal|v0;
	if(!planeBoxOverlap(normal, d, mBoxExtents)) return FALSE;

	// 3) "Class III" tests - here we always do full tests since the box is a primitive (not a BV)
	{
		IMPLEMENT_CLASS3_TESTS
	}
	return TRUE;
}
#else
//! A dedicated version where the box is constant
// using vmx128
//TODO: we should try and unify this code with that in the loose primitive test for LSS

inline_ BOOL OBBCollider::TriBoxOverlap(const Point& vert0, const Point& vert1, const Point& vert2)
{
	OpcSimd::Vector4 rot_0 = OpcSimd::load(mRModelToBox[0]);
	OpcSimd::Vector4 rot_1 = OpcSimd::load(mRModelToBox[1]);
	OpcSimd::Vector4 rot_2 = OpcSimd::load(mRModelToBox[2]);

	OpcSimd::Vector4 tran = OpcSimd::load(mTModelToBox);
	OpcSimd::Vector4 extents = OpcSimd::load(mBoxExtents);

	return TriBoxOverlap(vert0, vert1, vert2, rot_0, rot_1, rot_2, tran, extents);
}

inline_ BOOL OBBCollider::TriBoxOverlap(const Point& vert0, const Point& vert1, const Point& vert2,
										const OpcSimd::Vector4 &tran, const OpcSimd::Vector4 &extents)
{
	mNbVolumePrimTests++;

	//transform obb to origin
	OpcSimd::Vector4 v0 = OpcSimd::load(vert0);
	OpcSimd::Vector4 v1 = OpcSimd::load(vert1);
	OpcSimd::Vector4 v2 = OpcSimd::load(vert2);

	//Note this transform is different to the LSS loose version.
	v0 += tran;
	v1 += tran;
	v2 += tran;

	// Test triangle AABB
	OpcSimd::Vector4 triMin = OpcSimd::min4(v0, OpcSimd::min4(v1, v2));
	OpcSimd::Vector4 triMax = OpcSimd::max4(v0, OpcSimd::max4(v1, v2));

	OpcSimd::Vector4 mask = OpcSimd::or4(OpcSimd::greater(triMin, extents), OpcSimd::greater(-extents, triMax));
	
	return !OpcSimd::any3TrueBool(mask);
		
}

BOOL OBBCollider::TriBoxOverlap(const Point& vert0, const Point& vert1, const Point& vert2,
										const OpcSimd::Vector4 &rot_0, const OpcSimd::Vector4 &rot_1, const OpcSimd::Vector4 &rot_2, 
										const OpcSimd::Vector4 &tran, const OpcSimd::Vector4 &extents)
{
	// Stats
	mNbVolumePrimTests++;

	//transform obb to origin
	OpcSimd::Vector4 v0 = OpcSimd::load(vert0);
	OpcSimd::Vector4 v1 = OpcSimd::load(vert1);
	OpcSimd::Vector4 v2 = OpcSimd::load(vert2);

	//Note this transform is different to the LSS loose version.
	v0 = OpcSimd::transform(rot_0, rot_1, rot_2, tran, v0);
	v1 = OpcSimd::transform(rot_0, rot_1, rot_2, tran, v1);
	v2 = OpcSimd::transform(rot_0, rot_1, rot_2, tran, v2);

	OpcSimd::Vector4 minusExtents =  -extents;

	// Test triangle AABB
	OpcSimd::Vector4 triMin = OpcSimd::min4(v0, OpcSimd::min4(v1, v2));
	OpcSimd::Vector4 triMax = OpcSimd::max4(v0, OpcSimd::max4(v1, v2));

	OpcSimd::Vector4 mask = OpcSimd::or4(OpcSimd::greater(triMin, extents), OpcSimd::greater(minusExtents, triMax));
	if(OpcSimd::any3TrueBool(mask))
		return FALSE;

	// test tri plane.
	OpcSimd::Vector4 edge0 = OpcSimd::subtract(v1, v0);
	OpcSimd::Vector4 edge2 = OpcSimd::subtract(v0, v2);

	OpcSimd::Vector4 normal = OpcSimd::cross(edge2, edge0);
	OpcSimd::Vector4 dist = OpcSimd::dot(normal, v0);
	OpcSimd::Vector4 maxDist = OpcSimd::dot(extents, OpcSimd::abs(normal));


	if(OpcSimd::outBounds4Bool(dist, maxDist))
		return FALSE;

	//////// test edge axis
	OpcSimd::Vector4 radius = extents + extents;

	OpcSimd::Vector4 axis, p0, p1, p2, pMin, pMax, axisRadius;

	/*
		y*other.z - z*other.y,
		z*other.x - x*other.z,
		x*other.y - y*other.x

		0 - 0
		0 - e0.z
		e0.y - 0
	*/


	// axis == [1,0,0] x e0 == [0, -e0.z, e0.y]
	// x, y, z, w,    x, y, z, w
	// 0, 1, 2, 3,    4, 5, 6, 7

	OPC_SIMD_PERMUTE(PERMUTE_0W1Z0Y0W, OPC_SIMD_0W, OPC_SIMD_1Z, OPC_SIMD_0Y, OPC_SIMD_0W);

	OpcSimd::Vector4 zero = OpcSimd::zero();
	edge0 = OpcSimd::insertW(edge0, zero);
	OpcSimd::Vector4 minusEdge0 = -edge0;

	
	axis = OpcSimd::permute(edge0, minusEdge0, PERMUTE_0W1Z0Y0W );
	p0 = OpcSimd::dot(v0, axis);
	p2 = OpcSimd::dot(v2, axis);
	axisRadius = OpcSimd::dot(radius, OpcSimd::abs(axis));

	pMin = p0 + p2;
	pMax = OpcSimd::abs(p0-p2)+axisRadius;

	if(OpcSimd::outBounds4Bool(pMin, pMax))
		return FALSE;
	

	OpcSimd::Vector4 edge1 = OpcSimd::subtract(v2, v1);
	edge1 = OpcSimd::insertW(edge1, zero);
	OpcSimd::Vector4 minusEdge1 = -edge1;
	
	
	// axis == [1,0,0] x e1 == [0, -e1.z, e1.y]
	axis = OpcSimd::permute(edge1, minusEdge1, PERMUTE_0W1Z0Y0W );
	p0 = OpcSimd::dot(v0, axis);
	p1 = OpcSimd::dot(v1, axis);
	axisRadius = OpcSimd::dot(radius, OpcSimd::abs(axis));
	pMin = p0 + p1;
	pMax = OpcSimd::abs(p0-p1)+axisRadius;

	if(OpcSimd::outBounds4Bool(pMin, pMax))
		return FALSE;
	

	edge2 = OpcSimd::insertW(edge2, zero);
	OpcSimd::Vector4 minusEdge2 = -edge2;

	
	// axis == [1,0,0] x e2 == [0, -e2.z, e2.y]
	axis = OpcSimd::permute(edge2, minusEdge2, PERMUTE_0W1Z0Y0W);
	p0 = OpcSimd::dot(v0, axis);
	p1 = OpcSimd::dot(v1, axis);
	axisRadius = OpcSimd::dot(radius, OpcSimd::abs(axis));

	pMin = p0 + p1;
	pMax = OpcSimd::abs(p0-p1)+axisRadius;

	if(OpcSimd::outBounds4Bool(pMin, pMax))
		return FALSE;
	

	/*
		y*other.z - z*other.y,
		z*other.x - x*other.z,
		x*other.y - y*other.x

		e0.z - 0
		0 - 0
		0 - e0.x
	*/
	// axis == [0,1,0] x e0 == [e0.z, 0, -e0.x]
	// x, y, z, w,    x, y, z, w
	// 0, 1, 2, 3,    4, 5, 6, 7

	OPC_SIMD_PERMUTE(PERMUTE_0Z0W1X0W, OPC_SIMD_0Z, OPC_SIMD_0W, OPC_SIMD_1X, OPC_SIMD_0W);

	axis = OpcSimd::permute(edge0, minusEdge0, PERMUTE_0Z0W1X0W);
	p0 = OpcSimd::dot(v0, axis);
	p2 = OpcSimd::dot(v2, axis);
	axisRadius = OpcSimd::dot(radius, OpcSimd::abs(axis));
	pMin = p0 + p2;
	pMax = OpcSimd::abs(p0-p2)+axisRadius;

	if(OpcSimd::outBounds4Bool(pMin, pMax))
		return FALSE;

	// axis == [0,1,0] x e1 == [e1.z, 0, -e1.x]
	axis = OpcSimd::permute(edge1, minusEdge1, PERMUTE_0Z0W1X0W);
	p0 = OpcSimd::dot(v0, axis);
	p1 = OpcSimd::dot(v1, axis);
	axisRadius = OpcSimd::dot(radius, OpcSimd::abs(axis));

	pMin = p0 + p1;
	pMax = OpcSimd::abs(p0-p1)+axisRadius;

	if(OpcSimd::outBounds4Bool(pMin, pMax))
		return FALSE;

	// axis == [0, 1, 0] x e2 == [e2.z, 0, -e2.x]
	axis = OpcSimd::permute(edge2, minusEdge2, PERMUTE_0Z0W1X0W);
	p0 = OpcSimd::dot(v0, axis);
	p1 = OpcSimd::dot(v1, axis);
	axisRadius = OpcSimd::dot(radius, OpcSimd::abs(axis));

	pMin = p0 + p1;
	pMax = OpcSimd::abs(p0-p1)+axisRadius;

	if(OpcSimd::outBounds4Bool(pMin, pMax))
		return FALSE;


	/*
		y*other.z - z*other.y,
		z*other.x - x*other.z,
		x*other.y - y*other.x

		0 - e0.y
		e0.x - 0
		0 - 0
	*/

	// axis == [0, 0, 1] x e0 == [-e0.y, e0.x, 0]
	// x, y, z, w,    x, y, z, w
	// 0, 1, 2, 3,    4, 5, 6, 7

	OPC_SIMD_PERMUTE(PERMUTE_1Y0X0W0W, OPC_SIMD_1Y, OPC_SIMD_0X, OPC_SIMD_0W, OPC_SIMD_0W);

	axis = OpcSimd::permute(edge0, minusEdge0, PERMUTE_1Y0X0W0W);
	p0 = OpcSimd::dot(v0, axis);
	p2 = OpcSimd::dot(v2, axis);
	axisRadius = OpcSimd::dot(radius, OpcSimd::abs(axis));

	pMin = p0 + p2;
	pMax = OpcSimd::abs(p0-p2)+axisRadius;

	if(OpcSimd::outBounds4Bool(pMin, pMax))
		return FALSE;


	// axis == [0, 0, 1] x e1 == [-e1.y, e1.x, 0]
	axis = OpcSimd::permute(edge1, minusEdge1, PERMUTE_1Y0X0W0W);
	p0 = OpcSimd::dot(v0, axis);
	p1 = OpcSimd::dot(v1, axis);
	axisRadius = OpcSimd::dot(radius, OpcSimd::abs(axis));

	pMin = p0 + p1;
	pMax = OpcSimd::abs(p0-p1)+axisRadius;

	if(OpcSimd::outBounds4Bool(pMin, pMax))
		return FALSE;

	// axis == [0, 0, 1] x e2 == [-e2.y, e2.x, 0]
	axis = OpcSimd::permute(edge2, minusEdge2, PERMUTE_1Y0X0W0W);
	p0 = OpcSimd::dot(v0, axis);
	p1 = OpcSimd::dot(v1, axis);
	axisRadius = OpcSimd::dot(radius, OpcSimd::abs(axis));

	pMin = p0 + p1;
	pMax = OpcSimd::abs(p0-p1)+axisRadius;

	if(OpcSimd::outBounds4Bool(pMin, pMax))
		return FALSE;

	return TRUE;

}

inline_ BOOL AABBCollider::TriBoxOverlap(const OpcSimd::Vector4 &leafVerts0,const OpcSimd::Vector4 &leafVerts1,const OpcSimd::Vector4 &leafVerts2,
										 const OpcSimd::Vector4 &center,const OpcSimd::Vector4 &extents)
{
	// Stats
	mNbVolumePrimTests++;

	// use separating axis theorem to test overlap between triangle and box 
	// need to test for overlap in these directions: 
	// 1) the {x,y,z}-directions (actually, since we use the AABB of the triangle 
	//    we do not even need to test these) 
	// 2) normal of the triangle 
	// 3) crossproduct(edge from tri, {x,y,z}-directin) 
	//    this gives 3x3=9 more tests 

	// move everything so that the boxcenter is in (0,0,0) 
	/*Point v0, v1, v2;
	v0.x = mLeafVerts[0].x - center.x;
	v1.x = mLeafVerts[1].x - center.x;
	v2.x = mLeafVerts[2].x - center.x;*/
	OpcSimd::Vector4 v0, v1, v2;
	v0 = leafVerts0 - center;
	v1 = leafVerts1 - center;
	v2 = leafVerts2 - center;

	//// First, test overlap in the {x,y,z}-directions
	//float min,max;
	//// Find min, max of the triangle in x-direction, and test for overlap in X
	//FINDMINMAX(v0.x, v1.x, v2.x, min, max);
	//if(min>extents.x || max<-extents.x) return FALSE;

	//// Same for Y
	//v0.y = mLeafVerts[0].y - center.y;
	//v1.y = mLeafVerts[1].y - center.y;
	//v2.y = mLeafVerts[2].y - center.y;

	//FINDMINMAX(v0.y, v1.y, v2.y, min, max);
	//if(min>extents.y || max<-extents.y) return FALSE;

	//// Same for Z
	//v0.z = mLeafVerts[0].z - center.z;
	//v1.z = mLeafVerts[1].z - center.z;
	//v2.z = mLeafVerts[2].z - center.z;

	//FINDMINMAX(v0.z, v1.z, v2.z, min, max);
	//if(min>extents.z || max<-extents.z) return FALSE;	

	OpcSimd::Vector4 minusExtents = -extents;

	// Test triangle AABB
	OpcSimd::Vector4 triMin = OpcSimd::min4(v0, OpcSimd::min4(v1, v2));
	OpcSimd::Vector4 triMax = OpcSimd::max4(v0, OpcSimd::max4(v1, v2));

	OpcSimd::Vector4 mask = OpcSimd::or4(OpcSimd::greater(triMin, extents), OpcSimd::greater(minusExtents, triMax));
	if(OpcSimd::any3TrueBool(mask))
		return FALSE;



	// 2) Test if the box intersects the plane of the triangle
	// compute plane equation of triangle: normal*x+d=0
	// ### could be precomputed since we use the same leaf triangle several times
	/*const Point e0 = v1 - v0;
	const Point e1 = v2 - v1;
	const Point normal = e0 ^ e1;
	const float d = -normal|v0;
	if(!planeBoxOverlap(normal, d, extents)) return FALSE;*/

	// test tri plane.
	OpcSimd::Vector4 edge0 = OpcSimd::subtract(v1, v0);
	OpcSimd::Vector4 edge2 = OpcSimd::subtract(v0, v2);
	OpcSimd::Vector4 normal = OpcSimd::cross(edge2, edge0);
	OpcSimd::Vector4 dist = OpcSimd::dot(normal, v0);
	OpcSimd::Vector4 maxDist = OpcSimd::dot(extents, OpcSimd::abs(normal));
	if(OpcSimd::outBounds4Bool(dist, maxDist))
		return FALSE;

	// 3) "Class III" tests - here we always do full tests since the box is a primitive (not a BV)
	/*{
		IMPLEMENT_CLASS3_TESTS
	}*/

	//////// test edge axis
	OpcSimd::Vector4 radius = extents + extents;

	OpcSimd::Vector4 axis, p0, p1, p2, pMin, pMax, axisRadius;
	/*
		y*other.z - z*other.y,
		z*other.x - x*other.z,
		x*other.y - y*other.x

		e0.z - 0
		0 - 0
		0 - e0.x
	*/
	// axis == [0,1,0] x e0 == [e0.z, 0, -e0.x]
	// x, y, z, w,    x, y, z, w
	// 0, 1, 2, 3,    4, 5, 6, 7

	OPC_SIMD_PERMUTE(PERMUTE_0Z0W1X0W, OPC_SIMD_0Z, OPC_SIMD_0W, OPC_SIMD_1X, OPC_SIMD_0W);

	OpcSimd::Vector4 zero = OpcSimd::zero();
	edge0 = OpcSimd::insertW(edge0, zero);
	OpcSimd::Vector4 minusEdge0 = -edge0;

	axis = OpcSimd::permute(edge0, minusEdge0, PERMUTE_0Z0W1X0W);
	p0 = OpcSimd::dot(v0, axis);
	p2 = OpcSimd::dot(v2, axis);
	axisRadius = OpcSimd::dot(radius, OpcSimd::abs(axis));

	pMin = p0 + p2;
	pMax = OpcSimd::abs(p0-p2)+axisRadius;

	if(OpcSimd::outBounds4Bool(pMin, pMax))
		return FALSE;


	// axis == [0,1,0] x e1 == [e1.z, 0, -e1.x]

	OpcSimd::Vector4 edge1 = OpcSimd::subtract(v2, v1);
	edge1 = OpcSimd::insertW(edge1, zero);
	OpcSimd::Vector4 minusEdge1 = -edge1;

	axis = OpcSimd::permute(edge1, minusEdge1, PERMUTE_0Z0W1X0W);
	p0 = OpcSimd::dot(v0, axis);
	p1 = OpcSimd::dot(v1, axis);
	axisRadius = OpcSimd::dot(radius, OpcSimd::abs(axis));

	pMin = p0 + p1;
	pMax = OpcSimd::abs(p0-p1)+axisRadius;

	if(OpcSimd::outBounds4Bool(pMin, pMax))
		return FALSE;


	// axis == [0, 1, 0] x e2 == [e2.z, 0, -e2.x]
	edge2 = OpcSimd::insertW(edge2, zero);
	OpcSimd::Vector4 minusEdge2 = -edge2;

	axis = OpcSimd::permute(edge2, minusEdge2, PERMUTE_0Z0W1X0W);
	p0 = OpcSimd::dot(v0, axis);
	p1 = OpcSimd::dot(v1, axis);
	axisRadius = OpcSimd::dot(radius, OpcSimd::abs(axis));

	pMin = p0 + p1;
	pMax = OpcSimd::abs(p0-p1)+axisRadius;

	if(OpcSimd::outBounds4Bool(pMin, pMax))
		return FALSE;


	/*
		y*other.z - z*other.y,
		z*other.x - x*other.z,
		x*other.y - y*other.x

		0 - e0.y
		e0.x - 0
		0 - 0
	*/

	// axis == [0, 0, 1] x e0 == [-e0.y, e0.x, 0]
	// x, y, z, w,    x, y, z, w
	// 0, 1, 2, 3,    4, 5, 6, 7

	OPC_SIMD_PERMUTE(PERMUTE_1Y0X0W0W, OPC_SIMD_1Y, OPC_SIMD_0X, OPC_SIMD_0W, OPC_SIMD_0W);

	axis = OpcSimd::permute(edge0, minusEdge0, PERMUTE_1Y0X0W0W);
	p0 = OpcSimd::dot(v0, axis);
	p2 = OpcSimd::dot(v2, axis);
	axisRadius = OpcSimd::dot(radius, OpcSimd::abs(axis));

	pMin = p0 + p2;
	pMax = OpcSimd::abs(p0-p2)+axisRadius;

	if(OpcSimd::outBounds4Bool(pMin, pMax))
		return FALSE;

	// axis == [0, 0, 1] x e1 == [-e1.y, e1.x, 0]
	axis = OpcSimd::permute(edge1, minusEdge1, PERMUTE_1Y0X0W0W);
	p0 = OpcSimd::dot(v0, axis);
	p1 = OpcSimd::dot(v1, axis);
	axisRadius = OpcSimd::dot(radius, OpcSimd::abs(axis));

	pMin = p0 + p1;
	pMax = OpcSimd::abs(p0-p1)+axisRadius;

	if(OpcSimd::outBounds4Bool(pMin, pMax))
		return FALSE;


	// axis == [0, 0, 1] x e2 == [-e2.y, e2.x, 0]
	axis = OpcSimd::permute(edge2, minusEdge2, PERMUTE_1Y0X0W0W);
	p0 = OpcSimd::dot(v0, axis);
	p1 = OpcSimd::dot(v1, axis);
	axisRadius = OpcSimd::dot(radius, OpcSimd::abs(axis));

	pMin = p0 + p1;
	pMax = OpcSimd::abs(p0-p1)+axisRadius;

	if(OpcSimd::outBounds4Bool(pMin, pMax))
		return FALSE;

	return TRUE;
}

#endif //OPC_SUPPORT_SIMD

//! ...and another one, jeez
inline_ BOOL AABBCollider::TriBoxOverlap()
{
	// Stats
	mNbVolumePrimTests++;

	// Hook
	const Point& center		= mBox.mCenter;
	const Point& extents	= mBox.mExtents;

	// use separating axis theorem to test overlap between triangle and box 
	// need to test for overlap in these directions: 
	// 1) the {x,y,z}-directions (actually, since we use the AABB of the triangle 
	//    we do not even need to test these) 
	// 2) normal of the triangle 
	// 3) crossproduct(edge from tri, {x,y,z}-directin) 
	//    this gives 3x3=9 more tests 

	// move everything so that the boxcenter is in (0,0,0) 
	Point v0, v1, v2;
	v0.x = mLeafVerts[0].x - center.x;
	v1.x = mLeafVerts[1].x - center.x;
	v2.x = mLeafVerts[2].x - center.x;

	// First, test overlap in the {x,y,z}-directions
#ifdef OPC_USE_FCOMI
	// find min, max of the triangle in x-direction, and test for overlap in X
	if(FCMin3(v0.x, v1.x, v2.x)>extents.x)	return FALSE;
	if(FCMax3(v0.x, v1.x, v2.x)<-extents.x)	return FALSE;

	// same for Y
	v0.y = mLeafVerts[0].y - center.y;
	v1.y = mLeafVerts[1].y - center.y;
	v2.y = mLeafVerts[2].y - center.y;

	if(FCMin3(v0.y, v1.y, v2.y)>extents.y)	return FALSE;
	if(FCMax3(v0.y, v1.y, v2.y)<-extents.y)	return FALSE;

	// same for Z
	v0.z = mLeafVerts[0].z - center.z;
	v1.z = mLeafVerts[1].z - center.z;
	v2.z = mLeafVerts[2].z - center.z;

	if(FCMin3(v0.z, v1.z, v2.z)>extents.z)	return FALSE;
	if(FCMax3(v0.z, v1.z, v2.z)<-extents.z)	return FALSE;
#else
	float min,max;
	// Find min, max of the triangle in x-direction, and test for overlap in X
	FINDMINMAX(v0.x, v1.x, v2.x, min, max);
	if(min>extents.x || max<-extents.x) return FALSE;

	// Same for Y
	v0.y = mLeafVerts[0].y - center.y;
	v1.y = mLeafVerts[1].y - center.y;
	v2.y = mLeafVerts[2].y - center.y;

	FINDMINMAX(v0.y, v1.y, v2.y, min, max);
	if(min>extents.y || max<-extents.y) return FALSE;

	// Same for Z
	v0.z = mLeafVerts[0].z - center.z;
	v1.z = mLeafVerts[1].z - center.z;
	v2.z = mLeafVerts[2].z - center.z;

	FINDMINMAX(v0.z, v1.z, v2.z, min, max);
	if(min>extents.z || max<-extents.z) return FALSE;
#endif
	// 2) Test if the box intersects the plane of the triangle
	// compute plane equation of triangle: normal*x+d=0
	// ### could be precomputed since we use the same leaf triangle several times
	const Point e0 = v1 - v0;
	const Point e1 = v2 - v1;
	const Point normal = e0 ^ e1;
	const float d = -normal|v0;
	if(!planeBoxOverlap(normal, d, extents)) return FALSE;

	// 3) "Class III" tests - here we always do full tests since the box is a primitive (not a BV)
	{
		IMPLEMENT_CLASS3_TESTS
	}
	return TRUE;
}

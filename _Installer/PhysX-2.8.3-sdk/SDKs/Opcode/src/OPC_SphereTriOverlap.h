#include "OPC_Simd.h"

// This is collision detection. If you do another distance test for collision *response*,
// if might be useful to simply *skip* the test below completely, and report a collision.
// - if sphere-triangle overlap, result is ok
// - if they don't, we'll discard them during collision response with a similar test anyway
// Overall this approach should run faster.

// Original code by David Eberly in Magic.
BOOL SphereCollider::SphereTriOverlap(const Point& vert0, const Point& vert1, const Point& vert2)
{
	// Stats
	mNbVolumePrimTests++;

	// Early exit if one of the vertices is inside the sphere
	Point kDiff = vert2 - mCenter;
	float fC = kDiff.SquareMagnitude();
	if(fC <= mRadius2)	return TRUE;

	kDiff = vert1 - mCenter;
	fC = kDiff.SquareMagnitude();
	if(fC <= mRadius2)	return TRUE;

	kDiff = vert0 - mCenter;
	fC = kDiff.SquareMagnitude();
	if(fC <= mRadius2)	return TRUE;

	// Else do the full distance test
	Point TriEdge0	= vert1 - vert0;
	Point TriEdge1	= vert2 - vert0;

//Point kDiff	= vert0 - mCenter;
	float fA00	= TriEdge0.SquareMagnitude();
	float fA01	= TriEdge0 | TriEdge1;
	float fA11	= TriEdge1.SquareMagnitude();
	float fB0	= kDiff | TriEdge0;
	float fB1	= kDiff | TriEdge1;
//float fC	= kDiff.SquareMagnitude();
	float fDet	= fabsf(fA00*fA11 - fA01*fA01);
	float u		= fA01*fB1-fA11*fB0;
	float v		= fA01*fB0-fA00*fB1;
	float SqrDist;

	if(u + v <= fDet)
	{
		if(u < 0.0f)
		{
			if(v < 0.0f)  // region 4
			{
				if(fB0 < 0.0f)
				{
//					v = 0.0f;
					if(-fB0>=fA00)			{ /*u = 1.0f;*/		SqrDist = fA00+2.0f*fB0+fC;	}
					else					{ u = -fB0/fA00;	SqrDist = fB0*u+fC;			}
				}
				else
				{
//					u = 0.0f;
					if(fB1>=0.0f)			{ /*v = 0.0f;*/		SqrDist = fC;				}
					else if(-fB1>=fA11)		{ /*v = 1.0f;*/		SqrDist = fA11+2.0f*fB1+fC;	}
					else					{ v = -fB1/fA11;	SqrDist = fB1*v+fC;			}
				}
			}
			else  // region 3
			{
//				u = 0.0f;
				if(fB1>=0.0f)				{ /*v = 0.0f;*/		SqrDist = fC;				}
				else if(-fB1>=fA11)			{ /*v = 1.0f;*/		SqrDist = fA11+2.0f*fB1+fC;	}
				else						{ v = -fB1/fA11;	SqrDist = fB1*v+fC;			}
			}
		}
		else if(v < 0.0f)  // region 5
		{
//			v = 0.0f;
			if(fB0>=0.0f)					{ /*u = 0.0f;*/		SqrDist = fC;				}
			else if(-fB0>=fA00)				{ /*u = 1.0f;*/		SqrDist = fA00+2.0f*fB0+fC;	}
			else							{ u = -fB0/fA00;	SqrDist = fB0*u+fC;			}
		}
		else  // region 0
		{
			// minimum at interior point
			if(fDet==0.0f)
			{
//				u = 0.0f;
//				v = 0.0f;
				SqrDist = MAX_FLOAT;
			}
			else
			{
				float fInvDet = 1.0f/fDet;
				u *= fInvDet;
				v *= fInvDet;
				SqrDist = u*(fA00*u+fA01*v+2.0f*fB0) + v*(fA01*u+fA11*v+2.0f*fB1)+fC;
			}
		}
	}
	else
	{
		float fTmp0, fTmp1, fNumer, fDenom;

		if(u < 0.0f)  // region 2
		{
			fTmp0 = fA01 + fB0;
			fTmp1 = fA11 + fB1;
			if(fTmp1 > fTmp0)
			{
				fNumer = fTmp1 - fTmp0;
				fDenom = fA00-2.0f*fA01+fA11;
				if(fNumer >= fDenom)
				{
//					u = 1.0f;
//					v = 0.0f;
					SqrDist = fA00+2.0f*fB0+fC;
				}
				else
				{
					u = fNumer/fDenom;
					v = 1.0f - u;
					SqrDist = u*(fA00*u+fA01*v+2.0f*fB0) + v*(fA01*u+fA11*v+2.0f*fB1)+fC;
				}
			}
			else
			{
//				u = 0.0f;
				if(fTmp1 <= 0.0f)		{ /*v = 1.0f;*/		SqrDist = fA11+2.0f*fB1+fC;	}
				else if(fB1 >= 0.0f)	{ /*v = 0.0f;*/		SqrDist = fC;				}
				else					{ v = -fB1/fA11;	SqrDist = fB1*v+fC;			}
			}
		}
		else if(v < 0.0f)  // region 6
		{
			fTmp0 = fA01 + fB1;
			fTmp1 = fA00 + fB0;
			if(fTmp1 > fTmp0)
			{
				fNumer = fTmp1 - fTmp0;
				fDenom = fA00-2.0f*fA01+fA11;
				if(fNumer >= fDenom)
				{
//					v = 1.0f;
//					u = 0.0f;
					SqrDist = fA11+2.0f*fB1+fC;
				}
				else
				{
					v = fNumer/fDenom;
					u = 1.0f - v;
					SqrDist = u*(fA00*u+fA01*v+2.0f*fB0) + v*(fA01*u+fA11*v+2.0f*fB1)+fC;
				}
			}
			else
			{
//				v = 0.0f;
				if(fTmp1 <= 0.0f)		{ /*u = 1.0f;*/		SqrDist = fA00+2.0f*fB0+fC;	}
				else if(fB0 >= 0.0f)	{ /*u = 0.0f;*/		SqrDist = fC;				}
				else					{ u = -fB0/fA00;	SqrDist = fB0*u+fC;			}
			}
		}
		else  // region 1
		{
			fNumer = fA11 + fB1 - fA01 - fB0;
			if(fNumer <= 0.0f)
			{
//				u = 0.0f;
//				v = 1.0f;
				SqrDist = fA11+2.0f*fB1+fC;
			}
			else
			{
				fDenom = fA00-2.0f*fA01+fA11;
				if(fNumer >= fDenom)
				{
//					u = 1.0f;
//					v = 0.0f;
					SqrDist = fA00+2.0f*fB0+fC;
				}
				else
				{
					u = fNumer/fDenom;
					v = 1.0f - u;
					SqrDist = u*(fA00*u+fA01*v+2.0f*fB0) + v*(fA01*u+fA11*v+2.0f*fB1)+fC;
				}
			}
		}
	}

	return fabsf(SqrDist) < mRadius2;
}

#ifndef OPC_SUPPORT_SIMD
// Do a partial AABB against triangle overlap test.
inline_	 BOOL SphereCollider::LooseSphereTriOverlap(const Point& vert0, const Point& vert1, const Point& vert2)
{
	const Point sphereMin(mCenter.x - mRadius, mCenter.y - mRadius, mCenter.z - mRadius);
	const Point sphereMax(mCenter.x + mRadius, mCenter.y + mRadius, mCenter.z + mRadius);

	// Test triangle AABB
	Point triMin = Point(MIN(vert0.x,MIN(vert1.x, vert2.x)), MIN(vert0.y,MIN(vert1.y, vert2.y)), MIN(vert0.z,MIN(vert1.z, vert2.z)));
	Point triMax = Point(MAX(vert0.x,MAX(vert1.x, vert2.x)), MAX(vert0.y,MAX(vert1.y, vert2.y)), MAX(vert0.z,MAX(vert1.z, vert2.z)));

	if((triMin.x > sphereMax.x) || (triMax.x < sphereMin.x)) return FALSE;
	if((triMin.y > sphereMax.y) || (triMax.y < sphereMin.y)) return FALSE;
	if((triMin.z > sphereMax.z) || (triMax.z < sphereMin.z)) return FALSE;

	// Test the triangle plane.
	Point normal;
	normal.Cross(vert1 - vert0,vert2 - vert0);
	float dist = normal.Dot(vert0);

	// find the min max on normal.
	Point vMin, vMax;

	if(normal.x > 0) 
	{
		vMin.x = sphereMin.x;
		vMax.x = sphereMax.x;
	}
	else
	{
		vMin.x = sphereMax.x;
		vMax.x = sphereMin.x;
	}

	if(normal.y > 0) 
	{
		vMin.y = sphereMin.y;
		vMax.y = sphereMax.y;
	}
	else
	{
		vMin.y = sphereMax.y;
		vMax.y = sphereMin.y;
	}


	if(normal.z > 0) 
	{
		vMin.z = sphereMin.z;
		vMax.z = sphereMax.z;
	}
	else
	{
		vMin.z = sphereMax.z;
		vMax.z = sphereMin.z;
	}

	// are they disjoint?

	float minDist = vMin.Dot(normal);
	float maxDist = vMax.Dot(normal);

	if((minDist > dist) || (maxDist < dist))
		return FALSE;

	// Test edge axes.

	Point axis;
	float p0, p1, p2, pMin, pMax, axisRadius;

	Point v0 = vert0 - mCenter;
	Point v1 = vert1 - mCenter;
	Point v2 = vert2 - mCenter;

	Point edge0 = v1 - v0;
	Point edge1 = v2 - v1;
	Point edge2 = v0 - v2;

	Point radius(mRadius, mRadius, mRadius);

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

	

	axis.Cross(Point(1.0f, 0.0f, 0.0f),edge0);
	p0 = axis.Dot(v0);
	p2 = axis.Dot(v2);
	pMin = MIN(p0, p2);
	pMax = MAX(p0, p2);
	axisRadius = radius.Dot(Point(fabsf(axis.x), fabsf(axis.y), fabsf(axis.z)));
	// we test against the sphere radius, not the box radius on the axis...(so this isnt a standard AABB test).
	if((pMin > axisRadius) || (pMax < -axisRadius))
		return FALSE;

	// axis == [1,0,0] x e1 == [0, -e1.z, e1.y]
	axis.Cross(Point(1.0f, 0.0f, 0.0f),edge1);
	p0 = axis.Dot(v0);
	p1 = axis.Dot(v1);
	pMin = MIN(p0, p1);
	pMax = MAX(p0, p1);
	axisRadius = radius.Dot(Point(fabsf(axis.x), fabsf(axis.y), fabsf(axis.z)));
	// we test against the sphere radius, not the box radius on the axis...(so this isnt a standard AABB test).
	if((pMin > axisRadius) || (pMax < -axisRadius))
		return FALSE;

	// axis == [1,0,0] x e2 == [0, -e2.z, e2.y]
	axis.Cross(Point(1.0f, 0.0f, 0.0f),edge2);
	p0 = axis.Dot(v0);
	p1 = axis.Dot(v1);
	pMin = MIN(p0, p1);
	pMax = MAX(p0, p1);
	axisRadius = radius.Dot(Point(fabsf(axis.x), fabsf(axis.y), fabsf(axis.z)));
	// we test against the sphere radius, not the box radius on the axis...(so this isnt a standard AABB test).
	if((pMin > axisRadius) || (pMax < -axisRadius))
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

	axis.Cross(Point(0.0f, 1.0f, 0.0f),edge0);
	p0 = axis.Dot(v0);
	p2 = axis.Dot(v2);
	pMin = MIN(p0, p2);
	pMax = MAX(p0, p2);
	axisRadius = radius.Dot(Point(fabsf(axis.x), fabsf(axis.y), fabsf(axis.z)));
	// we test against the sphere radius, not the box radius on the axis...(so this isnt a standard AABB test).
	if((pMin > axisRadius) || (pMax < -axisRadius))
		return FALSE;


	// axis == [0,1,0] x e1 == [e1.z, 0, -e1.x]
	axis.Cross(Point(0.0f, 1.0f, 0.0f),edge1);
	p0 = axis.Dot(v0);
	p1 = axis.Dot(v1);
	pMin = MIN(p0, p1);
	pMax = MAX(p0, p1);
	axisRadius = radius.Dot(Point(fabsf(axis.x), fabsf(axis.y), fabsf(axis.z)));
	// we test against the sphere radius, not the box radius on the axis...(so this isnt a standard AABB test).
	if((pMin > axisRadius) || (pMax < -axisRadius))
		return FALSE;

	// axis == [0, 1, 0] x e2 == [e2.z, 0, -e2.x]
	axis.Cross(Point(0.0f, 1.0f, 0.0f),edge2);
	p0 = axis.Dot(v0);
	p1 = axis.Dot(v1);
	pMin = MIN(p0, p1);
	pMax = MAX(p0, p1);
	axisRadius = radius.Dot(Point(fabsf(axis.x), fabsf(axis.y), fabsf(axis.z)));
	// we test against the sphere radius, not the box radius on the axis...(so this isnt a standard AABB test).
	if((pMin > axisRadius) || (pMax < -axisRadius))
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

	// axis == [0, 1, 0] x e2 == [e2.z, 0, -e2.x]
	axis.Cross(Point(0.0f, 0.0f, 1.0f),edge0);
	p0 = axis.Dot(v0);
	p2 = axis.Dot(v2);
	pMin = MIN(p0, p2);
	pMax = MAX(p0, p2);
	axisRadius = radius.Dot(Point(fabsf(axis.x), fabsf(axis.y), fabsf(axis.z)));
	// we test against the sphere radius, not the box radius on the axis...(so this isnt a standard AABB test).
	if((pMin > axisRadius) || (pMax < -axisRadius))
		return FALSE;


	// axis == [0, 0, 1] x e1 == [-e1.y, e1.x, 0]

	axis.Cross(Point(0.0f, 0.0f, 1.0f),edge1);
	p0 = axis.Dot(v0);
	p1 = axis.Dot(v1);
	pMin = MIN(p0, p1);
	pMax = MAX(p0, p1);
	axisRadius = radius.Dot(Point(fabsf(axis.x), fabsf(axis.y), fabsf(axis.z)));
	// we test against the sphere radius, not the box radius on the axis...(so this isnt a standard AABB test).
	if((pMin > axisRadius) || (pMax < -axisRadius))
		return FALSE;

	// axis == [0, 0, 1] x e2 == [-e2.y, e2.x, 0]

	axis.Cross(Point(0.0f, 0.0f, 1.0f),edge2);
	p0 = axis.Dot(v0);
	p1 = axis.Dot(v1);
	pMin = MIN(p0, p1);
	pMax = MAX(p0, p1);
	axisRadius = radius.Dot(Point(fabsf(axis.x), fabsf(axis.y), fabsf(axis.z)));
	// we test against the sphere radius, not the box radius on the axis...(so this isnt a standard AABB test).
	if((pMin > axisRadius) || (pMax < -axisRadius))
		return FALSE;

	return TRUE;
}
#else

inline_ BOOL SphereCollider::LooseSphereTriOverlap(const Point& vert0, const Point& vert1, const Point& vert2)
{
	OpcSimd::Vector4 center = OpcSimd::load(mCenter);
	OpcSimd::Vector4 radius = OpcSimd::splatX(OpcSimd::load(mRadius));
	OpcSimd::Vector4 zero = OpcSimd::zero();

	OpcSimd::Vector4 sphereMin = OpcSimd::subtract(center, radius);
	OpcSimd::Vector4 sphereMax = OpcSimd::add(center, radius);

	return LooseSphereTriOverlap(vert0, vert1, vert2,
								center, radius,
								sphereMin, sphereMax,
								zero);
}

inline_	BOOL SphereCollider::LooseSphereTriOverlap(
	const Point& vert0, const Point& vert1, const Point& vert2,
	const OpcSimd::Vector4 &center, const OpcSimd::Vector4 &radius,
	const OpcSimd::Vector4 &sphereMin, const OpcSimd::Vector4 &sphereMax,
	const OpcSimd::Vector4 &zero)
{

	//We could optimize further, but most of the time is spent in these first few instructions.

	//3 unaligned loads are painful:-(
	OpcSimd::Vector4 v0 = OpcSimd::load(vert0);
	OpcSimd::Vector4 v1 = OpcSimd::load(vert1);
	OpcSimd::Vector4 v2 = OpcSimd::load(vert2);

	// Test triangle AABB
	OpcSimd::Vector4 triMin = OpcSimd::min4(v0, OpcSimd::min4(v1, v2));
	OpcSimd::Vector4 triMax = OpcSimd::max4(v0, OpcSimd::max4(v1, v2));

	OpcSimd::Vector4 mask = OpcSimd::or4(OpcSimd::greater(triMin, sphereMax), OpcSimd::greater(sphereMin, triMax));
	mask = OpcSimd::permuteXYZX(mask); //slightly more efficient than two swizzles.

	if(OpcSimd::intNotEqual4Bool(mask, zero))
		return FALSE;


	// Test the triangle plane.

	OpcSimd::Vector4 normal = OpcSimd::cross(OpcSimd::subtract(v1, v0), OpcSimd::subtract(v2, v0));
	OpcSimd::Vector4 dist = OpcSimd::dot(normal, v0);

	
	OpcSimd::Vector4 vMask = OpcSimd::greater(normal, OpcSimd::zero());
	OpcSimd::Vector4 vMin = OpcSimd::select(sphereMax, sphereMin, vMask);
	OpcSimd::Vector4 vMax = OpcSimd::select(sphereMin, sphereMax, vMask);

	OpcSimd::Vector4 minDist = OpcSimd::dot(vMin, normal);
	OpcSimd::Vector4 maxDist = OpcSimd::dot(vMax, normal);

	mask = OpcSimd::or4(OpcSimd::greater(minDist, dist), OpcSimd::less(maxDist, dist));
	if(OpcSimd::intNotEqualBool(mask, OpcSimd::zero()))
		return FALSE;

	//////// test edge axis
	//transform the triangle.

	v0 = OpcSimd::subtract(v0, center);
	v1 = OpcSimd::subtract(v1, center);
	v2 = OpcSimd::subtract(v2, center);

	OpcSimd::Vector4 edge0 = OpcSimd::subtract(v1, v0);
	OpcSimd::Vector4 edge1 = OpcSimd::subtract(v2, v1);
	OpcSimd::Vector4 edge2 = OpcSimd::subtract(v0, v2);

	// zero out w
	edge0 = OpcSimd::and4(edge0, OpcSimd::xyzMask());
	edge1 = OpcSimd::and4(edge1, OpcSimd::xyzMask());
	edge2 = OpcSimd::and4(edge2, OpcSimd::xyzMask());
	
	OpcSimd::Vector4 minusEdge0 = OpcSimd::subtract(OpcSimd::zero(), edge0);
	OpcSimd::Vector4 minusEdge1 = OpcSimd::subtract(OpcSimd::zero(), edge1);
	OpcSimd::Vector4 minusEdge2 = OpcSimd::subtract(OpcSimd::zero(), edge2);

	OpcSimd::Vector4 minusRadius = OpcSimd::subtract(OpcSimd::zero(), radius);


	OpcSimd::Vector4 axis, p0, p1, p2, pMin, pMax, axisRadius, minusAxisRadius;

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

	axis = OpcSimd::permute(edge0, minusEdge0, PERMUTE_0W1Z0Y0W );
	p0 = OpcSimd::dot(v0, axis);
	p2 = OpcSimd::dot(v2, axis);
	pMin = OpcSimd::min4(p0, p2);
	pMax = OpcSimd::max4(p0, p2);
	axisRadius = OpcSimd::dot(radius, OpcSimd::abs(axis));
	minusAxisRadius = OpcSimd::subtract(OpcSimd::zero(), axisRadius);
	// we test against the sphere radius, not the box radius on the axis...(so this isnt a standard AABB test).
	if(OpcSimd::greaterXBool(pMin, axisRadius) || OpcSimd::lessXBool(pMax, minusAxisRadius))
		return FALSE;

	// axis == [1,0,0] x e1 == [0, -e1.z, e1.y]
	axis = OpcSimd::permute(edge1, minusEdge1, PERMUTE_0W1Z0Y0W );
	p0 = OpcSimd::dot(v0, axis);
	p1 = OpcSimd::dot(v1, axis);
	pMin = OpcSimd::min4(p0, p1);
	pMax = OpcSimd::max4(p0, p1);
	axisRadius = OpcSimd::dot(radius, OpcSimd::abs(axis));
	minusAxisRadius = OpcSimd::subtract(OpcSimd::zero(), axisRadius);
	if(OpcSimd::greaterXBool(pMin, axisRadius) || OpcSimd::lessXBool(pMax, minusAxisRadius))
		return FALSE;


	// axis == [1,0,0] x e2 == [0, -e2.z, e2.y]
	axis = OpcSimd::permute(edge2, minusEdge2, PERMUTE_0W1Z0Y0W);
	p0 = OpcSimd::dot(v0, axis);
	p1 = OpcSimd::dot(v1, axis);
	pMin = OpcSimd::min4(p0, p1);
	pMax = OpcSimd::max4(p0, p1);
	axisRadius = OpcSimd::dot(radius, OpcSimd::abs(axis));
	minusAxisRadius = OpcSimd::subtract(OpcSimd::zero(), axisRadius);
	if(OpcSimd::greaterXBool(pMin, axisRadius) || OpcSimd::lessXBool(pMax, minusAxisRadius))
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
	pMin = OpcSimd::min4(p0, p2);
	pMax = OpcSimd::max4(p0, p2);
	axisRadius = OpcSimd::dot(radius, OpcSimd::abs(axis));
	minusAxisRadius = OpcSimd::subtract(OpcSimd::zero(), axisRadius);
	if(OpcSimd::greaterXBool(pMin, axisRadius) || OpcSimd::lessXBool(pMax, minusAxisRadius))
		return FALSE;

	// axis == [0,1,0] x e1 == [e1.z, 0, -e1.x]
	axis = OpcSimd::permute(edge1, minusEdge1, PERMUTE_0Z0W1X0W);
	p0 = OpcSimd::dot(v0, axis);
	p1 = OpcSimd::dot(v1, axis);
	pMin = OpcSimd::min4(p0, p1);
	pMax = OpcSimd::max4(p0, p1);
	axisRadius = OpcSimd::dot(radius, OpcSimd::abs(axis));
	minusAxisRadius = OpcSimd::subtract(OpcSimd::zero(), axisRadius);
	if(OpcSimd::greaterXBool(pMin, axisRadius) || OpcSimd::lessXBool(pMax, minusAxisRadius))
		return FALSE;

	// axis == [0, 1, 0] x e2 == [e2.z, 0, -e2.x]
	axis = OpcSimd::permute(edge2, minusEdge2, PERMUTE_0Z0W1X0W);
	p0 = OpcSimd::dot(v0, axis);
	p1 = OpcSimd::dot(v1, axis);
	pMin = OpcSimd::min4(p0, p1);
	pMax = OpcSimd::max4(p0, p1);
	axisRadius = OpcSimd::dot(radius, OpcSimd::abs(axis));
	minusAxisRadius = OpcSimd::subtract(OpcSimd::zero(), axisRadius);
	if(OpcSimd::greaterXBool(pMin, axisRadius) || OpcSimd::lessXBool(pMax, minusAxisRadius))
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
	pMin = OpcSimd::min4(p0, p2);
	pMax = OpcSimd::max4(p0, p2);
	axisRadius = OpcSimd::dot(radius, OpcSimd::abs(axis));
	minusAxisRadius = OpcSimd::subtract(OpcSimd::zero(), axisRadius);
	if(OpcSimd::greaterXBool(pMin, axisRadius) || OpcSimd::lessXBool(pMax, minusAxisRadius))
		return FALSE;

	// axis == [0, 0, 1] x e1 == [-e1.y, e1.x, 0]
	axis = OpcSimd::permute(edge1, minusEdge1, PERMUTE_1Y0X0W0W);
	p0 = OpcSimd::dot(v0, axis);
	p1 = OpcSimd::dot(v1, axis);
	pMin = OpcSimd::min4(p0, p1);
	pMax = OpcSimd::max4(p0, p1);
	axisRadius = OpcSimd::dot(radius, OpcSimd::abs(axis));
	minusAxisRadius = OpcSimd::subtract(OpcSimd::zero(), axisRadius);
	if(OpcSimd::greaterXBool(pMin, axisRadius) || OpcSimd::lessXBool(pMax, minusAxisRadius))
		return FALSE;

	// axis == [0, 0, 1] x e2 == [-e2.y, e2.x, 0]
	axis = OpcSimd::permute(edge2, minusEdge2, PERMUTE_1Y0X0W0W);
	p0 = OpcSimd::dot(v0, axis);
	p1 = OpcSimd::dot(v1, axis);
	pMin = OpcSimd::min4(p0, p1);
	pMax = OpcSimd::max4(p0, p1);
	axisRadius = OpcSimd::dot(radius, OpcSimd::abs(axis));
	minusAxisRadius = OpcSimd::subtract(OpcSimd::zero(), axisRadius);
	if(OpcSimd::greaterXBool(pMin, axisRadius) || OpcSimd::lessXBool(pMax, minusAxisRadius))
		return FALSE;

	return TRUE;
}

#endif

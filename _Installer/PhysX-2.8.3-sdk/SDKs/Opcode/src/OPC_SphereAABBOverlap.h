
#include "OPC_Simd.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Sphere-AABB overlap test, based on Jim Arvo's code.
 *	\param		center		[in] box center
 *	\param		extents		[in] box extents
 *	\return		TRUE on overlap
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef OPC_SUPPORT_SIMD
inline_ BOOL SphereCollider::SphereAABBOverlap(const Point& center, const Point& extents)
{ 
	// Stats
	mNbVolumeBVTests++;

	float d = 0.0f;

	//find the square of the distance
	//from the sphere to the box
#ifdef OLDIES
	for(udword i=0;i<3;i++)
	{
		float tmp = mCenter[i] - center[i];
		float s = tmp + extents[i];

		if(s<0.0f)	d += s*s;
		else
		{
			s = tmp - extents[i];
			if(s>0.0f)	d += s*s;
		}
	}
#endif

//#ifdef NEW_TEST

//	float tmp = mCenter.x - center.x;
//	float s = tmp + extents.x;

	float tmp,s;

	tmp = mCenter.x - center.x;
	s = tmp + extents.x;

	if(s<0.0f)
	{
		d += s*s;
		if(d>mRadius2)	return FALSE;
	}
	else
	{
		s = tmp - extents.x;
		if(s>0.0f)
		{
			d += s*s;
			if(d>mRadius2)	return FALSE;
		}
	}

	tmp = mCenter.y - center.y;
	s = tmp + extents.y;

	if(s<0.0f)
	{
		d += s*s;
		if(d>mRadius2)	return FALSE;
	}
	else
	{
		s = tmp - extents.y;
		if(s>0.0f)
		{
			d += s*s;
			if(d>mRadius2)	return FALSE;
		}
	}

	tmp = mCenter.z - center.z;
	s = tmp + extents.z;

	if(s<0.0f)
	{
		d += s*s;
		if(d>mRadius2)	return FALSE;
	}
	else
	{
		s = tmp - extents.z;
		if(s>0.0f)
		{
			d += s*s;
			if(d>mRadius2)	return FALSE;
		}
	}
//#endif

#ifdef OLDIES
//	Point Min = center - extents;
//	Point Max = center + extents;

	float d = 0.0f;

	//find the square of the distance
	//from the sphere to the box
	for(udword i=0;i<3;i++)
	{
float Min = center[i] - extents[i];

//		if(mCenter[i]<Min[i])
		if(mCenter[i]<Min)
		{
//			float s = mCenter[i] - Min[i];
			float s = mCenter[i] - Min;
			d += s*s;
		}
		else
		{
float Max = center[i] + extents[i];

//			if(mCenter[i]>Max[i])
			if(mCenter[i]>Max)
			{
				float s = mCenter[i] - Max;
				d += s*s;
			}
		}
	}
#endif
	return d <= mRadius2;
}
#else

inline_ BOOL SphereCollider::SphereAABBOverlap(const Point& center, const Point& extents)
{
	OpcSimd::Vector4 center4 = OpcSimd::load(center);
	OpcSimd::Vector4 extents4 = OpcSimd::load(extents);

	return SphereAABBOverlap(center4, extents4);
}

inline_ BOOL SphereCollider::SphereAABBOverlap(const OpcSimd::Vector4& center4, const OpcSimd::Vector4& extents4)
{
	// calculate the squared distance between the point and the AABB

	OpcSimd::Vector4 sphereCen = OpcSimd::load(mCenter);
	OpcSimd::Vector4 sphereRadius2 = OpcSimd::splatX(OpcSimd::load(mRadius2));

	OpcSimd::Vector4 bbMin = OpcSimd::subtract(center4, extents4);
	OpcSimd::Vector4 bbMax = OpcSimd::add(center4, extents4);

	OpcSimd::Vector4 minMask = OpcSimd::less(sphereCen, bbMin);
	OpcSimd::Vector4 maxMask = OpcSimd::greater(sphereCen, bbMax);

	OpcSimd::Vector4 sqDist = OpcSimd::zero();

	OpcSimd::Vector4 tmp = OpcSimd::subtract(bbMin, sphereCen);
	OpcSimd::Vector4 sqDistA = OpcSimd::multiplyAdd(tmp, tmp, sqDist);
	sqDist = OpcSimd::select(sqDist, sqDistA, minMask);

	OpcSimd::Vector4 tmp2 = OpcSimd::subtract(sphereCen, bbMax);
	OpcSimd::Vector4 sqDistB = OpcSimd::multiplyAdd(tmp2, tmp2, sqDist);
	sqDist = OpcSimd::select(sqDist, sqDistB, maxMask);

	// horizontal add(3 component)
	sqDist = OpcSimd::dot(sqDist, OpcSimd::one());

	//compare to radius squared
	return OpcSimd::lessEqual4Bool(sqDist, sphereRadius2);
}

#endif

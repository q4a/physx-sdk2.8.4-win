///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains code for ray-AABB intersection
 *	\file		CTC_RayAABBOverlap.h
 *	\author		Pierre Terdiman
 *	\date		January, 13, 2000
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef CTCRAYAABBOVERLAP_H
#define CTCRAYAABBOVERLAP_H


	CONTACT_API bool	SegmentOBB(const Segment& segment, const Point& center, const Point& extents, const Matrix3x3& rot);
	CONTACT_API bool	RayOBB(const Ray& ray, const Point& center, const Point& extents, const Matrix3x3& rot);

//	CONTACT_API bool	SegmentAABB(const Segment& segment, const Point& min, const Point& max);
//	CONTACT_API bool	RayAABB(const Ray& ray, const Point& min, const Point& max);

	// Inlining those is really faster in some cases...

	inline_ bool SegmentAABB(const Segment& segment, const Point& min, const Point& max)
	{
		Point BoxExtents, Diff, Dir;
		float fAWdU[3];

		Dir.x = (segment.mP1.x - segment.mP0.x);
		BoxExtents.x = (max.x - min.x);
		Diff.x = ((segment.mP1.x + segment.mP0.x) - (max.x + min.x));
		fAWdU[0] = fabsf(Dir.x);
		if(fabsf(Diff.x)>BoxExtents.x + fAWdU[0])	return false;

		Dir.y = (segment.mP1.y - segment.mP0.y);
		BoxExtents.y = (max.y - min.y);
		Diff.y = ((segment.mP1.y + segment.mP0.y) - (max.y + min.y));
		fAWdU[1] = fabsf(Dir.y);
		if(fabsf(Diff.y)>BoxExtents.y + fAWdU[1])	return false;

		Dir.z = (segment.mP1.z - segment.mP0.z);
		BoxExtents.z = (max.z - min.z);
		Diff.z = ((segment.mP1.z + segment.mP0.z) - (max.z + min.z));
		fAWdU[2] = fabsf(Dir.z);
		if(fabsf(Diff.z)>BoxExtents.z + fAWdU[2])	return false;

		float f;
		f = Dir.y * Diff.z - Dir.z * Diff.y;	if(fabsf(f)>BoxExtents.y*fAWdU[2] + BoxExtents.z*fAWdU[1])	return false;
		f = Dir.z * Diff.x - Dir.x * Diff.z;	if(fabsf(f)>BoxExtents.x*fAWdU[2] + BoxExtents.z*fAWdU[0])	return false;
		f = Dir.x * Diff.y - Dir.y * Diff.x;	if(fabsf(f)>BoxExtents.x*fAWdU[1] + BoxExtents.y*fAWdU[0])	return false;

		return true;
	}

#ifndef OPC_SUPPORT_SIMD

	inline_ bool RayAABB(const Ray& ray, const Point& min, const Point& max)
	{
		Point BoxExtents, Diff;

		Diff.x = ray.mOrig.x - ((max.x + min.x)*0.5f);
		BoxExtents.x = (max.x - min.x)*0.5f;
#ifdef WIN32
		if(AIR(Diff.x)>IR(BoxExtents.x) && Diff.x*ray.mDir.x>=0.0f)	return false;
#else
		if(fabsf(Diff.x)>BoxExtents.x && Diff.x*ray.mDir.x>=0.0f)	return false;
#endif

		Diff.y = ray.mOrig.y - ((max.y + min.y)*0.5f);
		BoxExtents.y = (max.y - min.y)*0.5f;
#ifdef WIN32
		if(AIR(Diff.y)>IR(BoxExtents.y) && Diff.y*ray.mDir.y>=0.0f)	return false;
#else
		if(fabsf(Diff.y)>BoxExtents.y && Diff.y*ray.mDir.y>=0.0f)	return false;
#endif

		Diff.z = ray.mOrig.z - ((max.z + min.z)*0.5f);
		BoxExtents.z = (max.z - min.z)*0.5f;
#ifdef WIN32
		if(AIR(Diff.z)>IR(BoxExtents.z) && Diff.z*ray.mDir.z>=0.0f)	return false;
#else
		if(fabsf(Diff.z)>BoxExtents.z && Diff.z*ray.mDir.z>=0.0f)	return false;
#endif

		float fAWdU[3];
#ifdef WIN32
		IR(fAWdU[0]) = AIR(ray.mDir.x);
		IR(fAWdU[1]) = AIR(ray.mDir.y);
		IR(fAWdU[2]) = AIR(ray.mDir.z);
#else
		fAWdU[0] = fabsf(ray.mDir.x);
		fAWdU[1] = fabsf(ray.mDir.y);
		fAWdU[2] = fabsf(ray.mDir.z);
#endif

		float f;
		f = ray.mDir.y * Diff.z - ray.mDir.z * Diff.y;	if(fabsf(f)>BoxExtents.y*fAWdU[2] + BoxExtents.z*fAWdU[1])	return false;
		f = ray.mDir.z * Diff.x - ray.mDir.x * Diff.z;	if(fabsf(f)>BoxExtents.x*fAWdU[2] + BoxExtents.z*fAWdU[0])	return false;
		f = ray.mDir.x * Diff.y - ray.mDir.y * Diff.x;	if(fabsf(f)>BoxExtents.x*fAWdU[1] + BoxExtents.y*fAWdU[0])	return false;

		return true;
	}

#else

	inline_ bool RayAABB(const Ray& ray, const Point& min, const Point& max)
	{

		OpcSimd::Vector4 half = OpcSimd::half(); //opt: preload
		OpcSimd::Vector4 zero = OpcSimd::zero(); //opt: preload

		OpcSimd::Vector4 orig = OpcSimd::load(ray.mOrig); //opt: preload
		OpcSimd::Vector4 dir = OpcSimd::load(ray.mDir); //opt: preload

		OpcSimd::Vector4 bbMin = OpcSimd::load(min);
		OpcSimd::Vector4 bbMax = OpcSimd::load(max);

		OpcSimd::Vector4 boxCen = OpcSimd::multiply(OpcSimd::add(bbMin, bbMax), half);
		OpcSimd::Vector4 boxExtents = OpcSimd::multiply(OpcSimd::subtract(bbMax, bbMin), half);

		/*
		Diff.x = ray.mOrig.x - ((max.x + min.x)*0.5f);
		BoxExtents.x = (max.x - min.x)*0.5f;

		if(fabsf(Diff.x)>BoxExtents.x && Diff.x*ray.mDir.x>=0.0f)	return false;

		Diff.y = ray.mOrig.y - ((max.y + min.y)*0.5f);
		BoxExtents.y = (max.y - min.y)*0.5f;

		if(fabsf(Diff.y)>BoxExtents.y && Diff.y*ray.mDir.y>=0.0f)	return false;

		Diff.z = ray.mOrig.z - ((max.z + min.z)*0.5f);
		BoxExtents.z = (max.z - min.z)*0.5f;

		if(fabsf(Diff.z)>BoxExtents.z && Diff.z*ray.mDir.z>=0.0f)	return false;
		*/


		OpcSimd::Vector4 diff = OpcSimd::subtract(orig, boxCen);
		OpcSimd::Vector4 absDiff = OpcSimd::abs(diff);

		OpcSimd::Vector4 maskA = OpcSimd::greater(absDiff, boxExtents);
		OpcSimd::Vector4 maskB = OpcSimd::greaterEqual(OpcSimd::multiply(diff, dir), zero);
		
		/*float fAWdU[3];

		fAWdU[0] = fabsf(ray.mDir.x);
		fAWdU[1] = fabsf(ray.mDir.y);
		fAWdU[2] = fabsf(ray.mDir.z);
		*/

		OpcSimd::Vector4 absDir = OpcSimd::abs(dir); //opt precompute/load
		OpcSimd::Vector4 absDirYZZ = OpcSimd::permuteYZZ(absDir); //opt precompute/load
		OpcSimd::Vector4 absDirXYX = OpcSimd::permuteXYX(absDir); //opt precompute/load

		OpcSimd::Vector4 diffYZX = OpcSimd::permuteYZX(diff);
		OpcSimd::Vector4 dirYZX = OpcSimd::permuteYZX(dir); //opt precompute/load

		OpcSimd::Vector4 boxExtentsXYX = OpcSimd::permuteXYX(boxExtents);
		OpcSimd::Vector4 boxExtentsYZZ = OpcSimd::permuteYZZ(boxExtents);

		//opt multiply subtract
		OpcSimd::Vector4 f = OpcSimd::subtract(OpcSimd::multiply(dir, diffYZX), OpcSimd::multiply(dirYZX, diff));
		OpcSimd::Vector4 absF = OpcSimd::abs(f);

		OpcSimd::Vector4 b = OpcSimd::multiplyAdd(boxExtentsXYX, absDirYZZ, OpcSimd::multiply(boxExtentsYZZ, absDirXYX));

		OpcSimd::Vector4 maskC = OpcSimd::greater(absF, b);
		/*
		float f;
		f = ray.mDir.x * Diff.y - ray.mDir.y * Diff.x;	if(fabsf(f)>BoxExtents.x*fAWdU[1] + BoxExtents.y*fAWdU[0])	return false;
		f = ray.mDir.y * Diff.z - ray.mDir.z * Diff.y;	if(fabsf(f)>BoxExtents.y*fAWdU[2] + BoxExtents.z*fAWdU[1])	return false;
		f = ray.mDir.z * Diff.x - ray.mDir.x * Diff.z;	if(fabsf(f)>BoxExtents.x*fAWdU[2] + BoxExtents.z*fAWdU[0])	return false;
		*/

		if(OpcSimd::intNotEqualBool(OpcSimd::or4(maskC, OpcSimd::and4(maskA, maskB)), zero))
			return false;
		else
			return true;
	}
#endif

/*
	inline_ bool RayAABBCenterExtents(const Ray& ray, const float* fAWdU, const Point& center, const Point& extents)
	{
		Point Diff;
		Diff.x = ray.mOrig.x - center.x;
#ifdef WIN32
		if(AIR(Diff.x)>IR(extents.x) && Diff.x*ray.mDir.x>=0.0f)	return false;
#else
		if(fabsf(Diff.x)>extents.x && Diff.x*ray.mDir.x>=0.0f)	return false;
#endif

		Diff.y = ray.mOrig.y - center.y;
#ifdef WIN32
		if(AIR(Diff.y)>IR(extents.y) && Diff.y*ray.mDir.y>=0.0f)	return false;
#else
		if(fabsf(Diff.y)>extents.y && Diff.y*ray.mDir.y>=0.0f)	return false;
#endif

		Diff.z = ray.mOrig.z - center.z;
#ifdef WIN32
		if(AIR(Diff.z)>IR(extents.z) && Diff.z*ray.mDir.z>=0.0f)	return false;
#else
		if(fabsf(Diff.z)>extents.z && Diff.z*ray.mDir.z>=0.0f)	return false;
#endif

		float f;
		f = ray.mDir.y * Diff.z - ray.mDir.z * Diff.y;	if(fabsf(f)>extents.y*fAWdU[2] + extents.z*fAWdU[1])	return false;
		f = ray.mDir.z * Diff.x - ray.mDir.x * Diff.z;	if(fabsf(f)>extents.x*fAWdU[2] + extents.z*fAWdU[0])	return false;
		f = ray.mDir.x * Diff.y - ray.mDir.y * Diff.x;	if(fabsf(f)>extents.x*fAWdU[1] + extents.y*fAWdU[0])	return false;

		return true;
	}
*/
	inline_ bool SegmentOBB(const Segment& segment, const OBB& obb)
	{
		return Ctc::SegmentOBB(segment, obb.mCenter, obb.mExtents, obb.mRot);
	}
	inline_ bool RayOBB(const Ray& ray, const OBB& obb)
	{
		return Ctc::RayOBB(ray, obb.mCenter, obb.mExtents, obb.mRot);
	}
	inline_ bool SegmentAABB(const Segment& segment, const AABB& aabb)
	{
#ifdef USE_MINMAX
		const ShadowAABB* Box = (const ShadowAABB*)&aabb;
		return Ctc::SegmentAABB(segment, Box->mMin, Box->mMax);
#else
		Point Min, Max;
		aabb.GetMin(Min);
		aabb.GetMax(Max);
		return Ctc::SegmentAABB(segment, Min, Max);
#endif
	}
	inline_ bool RayAABB(const Ray& ray, const AABB& aabb)
	{
#ifdef USE_MINMAX
		const ShadowAABB* Box = (const ShadowAABB*)&aabb;
		return Ctc::RayAABB(ray, Box->mMin, Box->mMax);
#else
		Point Min, Max;
		aabb.GetMin(Min);
		aabb.GetMax(Max);
		return Ctc::RayAABB(ray, Min, Max);
#endif
	}

#endif // CTCRAYAABBOVERLAP_H


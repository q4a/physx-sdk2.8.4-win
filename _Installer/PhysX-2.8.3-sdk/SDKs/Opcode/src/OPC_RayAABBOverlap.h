// Opcode 1.1: ray-AABB overlap tests based on Woo's code
// Opcode 1.2: ray-AABB overlap tests based on the separating axis theorem
//
// The point of intersection is not computed anymore. The distance to impact is not needed anymore
// since we now have two different queries for segments or rays.

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Computes a segment-AABB overlap test using the separating axis theorem. Segment is cached within the class.
 *	\param		center	[in] AABB center
 *	\param		extents	[in] AABB extents
 *	\return		true on overlap
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline_ BOOL RayCollider::SegmentAABBOverlap(const Point& center, const Point& extents)
{
	// Stats
	mNbRayBVTests++;

	float Dx = mData2.x - center.x;		if(fabsf(Dx) > extents.x + mFDir.x)	return FALSE;
	float Dy = mData2.y - center.y;		if(fabsf(Dy) > extents.y + mFDir.y)	return FALSE;
	float Dz = mData2.z - center.z;		if(fabsf(Dz) > extents.z + mFDir.z)	return FALSE;

	float f;
	f = mData.y * Dz - mData.z * Dy;	if(fabsf(f) > extents.y*mFDir.z + extents.z*mFDir.y)	return FALSE;
	f = mData.z * Dx - mData.x * Dz;	if(fabsf(f) > extents.x*mFDir.z + extents.z*mFDir.x)	return FALSE;
	f = mData.x * Dy - mData.y * Dx;	if(fabsf(f) > extents.x*mFDir.y + extents.y*mFDir.x)	return FALSE;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Computes a ray-AABB overlap test using the separating axis theorem. Ray is cached within the class.
 *	\param		center	[in] AABB center
 *	\param		extents	[in] AABB extents
 *	\return		true on overlap
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef OPC_SUPPORT_SIMD

inline_ BOOL RayCollider::RayAABBOverlap(const Point& center, const Point& extents)
{
	// Stats
	mNbRayBVTests++;

//	float Dx = mOrigin.x - center.x;	if(fabsf(Dx) > extents.x && Dx*mDir.x>=0.0f)	return FALSE;
//	float Dy = mOrigin.y - center.y;	if(fabsf(Dy) > extents.y && Dy*mDir.y>=0.0f)	return FALSE;
//	float Dz = mOrigin.z - center.z;	if(fabsf(Dz) > extents.z && Dz*mDir.z>=0.0f)	return FALSE;

	float Dx = mOrigin.x - center.x;	if(GREATER(Dx, extents.x) && Dx*mDir.x>=0.0f)	return FALSE;
	float Dy = mOrigin.y - center.y;	if(GREATER(Dy, extents.y) && Dy*mDir.y>=0.0f)	return FALSE;
	float Dz = mOrigin.z - center.z;	if(GREATER(Dz, extents.z) && Dz*mDir.z>=0.0f)	return FALSE;

//	float Dx = mOrigin.x - center.x;	if(GREATER(Dx, extents.x) && ((SIR(Dx)-1)^SIR(mDir.x))>=0.0f)	return FALSE;
//	float Dy = mOrigin.y - center.y;	if(GREATER(Dy, extents.y) && ((SIR(Dy)-1)^SIR(mDir.y))>=0.0f)	return FALSE;
//	float Dz = mOrigin.z - center.z;	if(GREATER(Dz, extents.z) && ((SIR(Dz)-1)^SIR(mDir.z))>=0.0f)	return FALSE;

	float f;
	f = mDir.y * Dz - mDir.z * Dy;		if(fabsf(f) > extents.y*mFDir.z + extents.z*mFDir.y)	return FALSE;
	f = mDir.z * Dx - mDir.x * Dz;		if(fabsf(f) > extents.x*mFDir.z + extents.z*mFDir.x)	return FALSE;
	f = mDir.x * Dy - mDir.y * Dx;		if(fabsf(f) > extents.x*mFDir.y + extents.y*mFDir.x)	return FALSE;

	return TRUE;
}
#else

inline_ BOOL RayCollider::RayAABBOverlap(const Point& center, const Point& extents)
{
	OpcSimd::Vector4 c = OpcSimd::load(center);
	OpcSimd::Vector4 e = OpcSimd::load(extents);

	return RayAABBOverlap(c, e);
}

inline_ BOOL RayCollider::RayAABBOverlap(const OpcSimd::Vector4& center, const OpcSimd::Vector4& extents)
{
	OpcSimd::Vector4 origin = OpcSimd::load(mOrigin);
	OpcSimd::Vector4 dir = OpcSimd::load(mDir);

	OpcSimd::Vector4 dirYZX = OpcSimd::permuteYZX(dir);

	OpcSimd::Vector4 fDir = OpcSimd::load(mFDir);
	OpcSimd::Vector4 fDirYZZ = OpcSimd::permuteYZZ(fDir); 
	OpcSimd::Vector4 fDirXYX = OpcSimd::permuteXYX(fDir);

	OpcSimd::Vector4 zero = OpcSimd::zero();

	return RayAABBOverlap(center, extents,
		origin, dir,
		dirYZX, fDir,
		fDirYZZ, fDirXYX,
		zero);

}

inline_ BOOL RayCollider::RayAABBOverlap(const OpcSimd::Vector4& center, const OpcSimd::Vector4& extents,
										 const OpcSimd::Vector4& origin, const OpcSimd::Vector4& dir,
										 const OpcSimd::Vector4& dirYZX, const OpcSimd::Vector4& fDir,
										 const OpcSimd::Vector4& fDirYZZ, const OpcSimd::Vector4& fDirXYX,
										 const OpcSimd::Vector4& zero
										 )
{
	// Stats
	mNbRayBVTests++;

	OpcSimd::Vector4 D = OpcSimd::subtract(origin, center);

	OpcSimd::Vector4 maskA = OpcSimd::greater(OpcSimd::abs(D), extents);
	OpcSimd::Vector4 maskB = OpcSimd::greaterEqual(OpcSimd::multiply(D, dir), zero);

	OpcSimd::Vector4 DYZX = OpcSimd::permuteYZX(D);
	
	OpcSimd::Vector4 f = OpcSimd::subtract(OpcSimd::multiply(dir, DYZX), OpcSimd::multiply(dirYZX, D));
	OpcSimd::Vector4 absF = OpcSimd::abs(f);

	OpcSimd::Vector4 extentsXYX = OpcSimd::permuteXYX(extents);
	OpcSimd::Vector4 extentsYZZ = OpcSimd::permuteYZZ(extents);
	
	OpcSimd::Vector4 tmp = OpcSimd::add(OpcSimd::multiply(extentsXYX, fDirYZZ), OpcSimd::multiply(extentsYZZ, fDirXYX));
	OpcSimd::Vector4 maskC = OpcSimd::greater(absF, tmp);

	OpcSimd::Vector4 mask = OpcSimd::or4(maskC, OpcSimd::and4(maskA, maskB));

	if(OpcSimd::intNotEqualBool(mask, zero))
		return FALSE;
	else
		return TRUE;

}

inline_ BOOL RayCollider::SegmentAABBOverlap(const OpcSimd::Vector4& center, const OpcSimd::Vector4& extents,
											 const OpcSimd::Vector4& data,const OpcSimd::Vector4& data2,
											 const OpcSimd::Vector4& fDir,const OpcSimd::Vector4& fDirYXX,const OpcSimd::Vector4& fDirZZY)
{
	mNbRayBVTests++;
	const OpcSimd::Vector4 D = OpcSimd::subtract(data2,center);
	const OpcSimd::Vector4 Comp1 = OpcSimd::add(extents,fDir);
	if(OpcSimd::lessEqualBool(OpcSimd::abs(D),Comp1))
	{	
		const OpcSimd::Vector4 extentsYXX = OpcSimd::permuteYXX(extents);
		const OpcSimd::Vector4 extentsZZY = OpcSimd::permuteZZY(extents);
		const OpcSimd::Vector4 Cross = OpcSimd::cross(data,D);
		const OpcSimd::Vector4 Comp2 = OpcSimd::multiplyAdd(extentsYXX,fDirZZY,OpcSimd::multiply(extentsZZY,fDirYXX));
		return OpcSimd::lessEqualBool(OpcSimd::abs(Cross),Comp2);
	}
	return false;
}
#endif

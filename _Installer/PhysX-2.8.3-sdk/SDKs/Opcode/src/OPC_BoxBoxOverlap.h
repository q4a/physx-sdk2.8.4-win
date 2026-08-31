///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	OBB-OBB overlap test using the separating axis theorem.
 *	- original code by Gomez / Gamasutra (similar to Gottschalk's one in RAPID)
 *	- optimized for AABB trees by computing the rotation matrix once (SOLID-fashion)
 *	- the fabs matrix is precomputed as well and epsilon-tweaked (RAPID-style, we found this almost mandatory)
 *	- Class III axes can be disabled... (SOLID & Intel fashion)
 *	- ...or enabled to perform some profiling
 *	- CPU comparisons used when appropriate
 *	- lazy evaluation sometimes saves some work in case of early exits (unlike SOLID)
 *
 *	\param		ea	[in] extents from box A
 *	\param		ca	[in] center from box A
 *	\param		eb	[in] extents from box B
 *	\param		cb	[in] center from box B
 *	\return		true if boxes overlap
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*inline_ BOOL AABBTreeCollider::BoxBoxOverlap(const Point& ea, const Point& ca, const Point& eb, const Point& cb)
{
	// Stats
	mNbBVBVTests++;

	float t,t2;

	// Class I : A's basis vectors
	float Tx = (mR1to0.m[0][0]*cb.x + mR1to0.m[1][0]*cb.y + mR1to0.m[2][0]*cb.z) + mT1to0.x - ca.x;
	t = ea.x + eb.x*mAR.m[0][0] + eb.y*mAR.m[1][0] + eb.z*mAR.m[2][0];
	if(GREATER(Tx, t))	return FALSE;

	float Ty = (mR1to0.m[0][1]*cb.x + mR1to0.m[1][1]*cb.y + mR1to0.m[2][1]*cb.z) + mT1to0.y - ca.y;
	t = ea.y + eb.x*mAR.m[0][1] + eb.y*mAR.m[1][1] + eb.z*mAR.m[2][1];
	if(GREATER(Ty, t))	return FALSE;

	float Tz = (mR1to0.m[0][2]*cb.x + mR1to0.m[1][2]*cb.y + mR1to0.m[2][2]*cb.z) + mT1to0.z - ca.z;
	t = ea.z + eb.x*mAR.m[0][2] + eb.y*mAR.m[1][2] + eb.z*mAR.m[2][2];
	if(GREATER(Tz, t))	return FALSE;

	// Class II : B's basis vectors
	t = Tx*mR1to0.m[0][0] + Ty*mR1to0.m[0][1] + Tz*mR1to0.m[0][2];	t2 = ea.x*mAR.m[0][0] + ea.y*mAR.m[0][1] + ea.z*mAR.m[0][2] + eb.x;
	if(GREATER(t, t2))	return FALSE;

	t = Tx*mR1to0.m[1][0] + Ty*mR1to0.m[1][1] + Tz*mR1to0.m[1][2];	t2 = ea.x*mAR.m[1][0] + ea.y*mAR.m[1][1] + ea.z*mAR.m[1][2] + eb.y;
	if(GREATER(t, t2))	return FALSE;

	t = Tx*mR1to0.m[2][0] + Ty*mR1to0.m[2][1] + Tz*mR1to0.m[2][2];	t2 = ea.x*mAR.m[2][0] + ea.y*mAR.m[2][1] + ea.z*mAR.m[2][2] + eb.z;
	if(GREATER(t, t2))	return FALSE;

	// Class III : 9 cross products
	// Cool trick: always perform the full test for first level, regardless of settings.
	// That way pathological cases (such as the pencils scene) are quickly rejected anyway !
	if(mFullBoxBoxTest || mNbBVBVTests==1)
	{
		t = Tz*mR1to0.m[0][1] - Ty*mR1to0.m[0][2];	t2 = ea.y*mAR.m[0][2] + ea.z*mAR.m[0][1] + eb.y*mAR.m[2][0] + eb.z*mAR.m[1][0];	if(GREATER(t, t2))	return FALSE;	// L = A0 x B0
		t = Tz*mR1to0.m[1][1] - Ty*mR1to0.m[1][2];	t2 = ea.y*mAR.m[1][2] + ea.z*mAR.m[1][1] + eb.x*mAR.m[2][0] + eb.z*mAR.m[0][0];	if(GREATER(t, t2))	return FALSE;	// L = A0 x B1
		t = Tz*mR1to0.m[2][1] - Ty*mR1to0.m[2][2];	t2 = ea.y*mAR.m[2][2] + ea.z*mAR.m[2][1] + eb.x*mAR.m[1][0] + eb.y*mAR.m[0][0];	if(GREATER(t, t2))	return FALSE;	// L = A0 x B2
		t = Tx*mR1to0.m[0][2] - Tz*mR1to0.m[0][0];	t2 = ea.x*mAR.m[0][2] + ea.z*mAR.m[0][0] + eb.y*mAR.m[2][1] + eb.z*mAR.m[1][1];	if(GREATER(t, t2))	return FALSE;	// L = A1 x B0
		t = Tx*mR1to0.m[1][2] - Tz*mR1to0.m[1][0];	t2 = ea.x*mAR.m[1][2] + ea.z*mAR.m[1][0] + eb.x*mAR.m[2][1] + eb.z*mAR.m[0][1];	if(GREATER(t, t2))	return FALSE;	// L = A1 x B1
		t = Tx*mR1to0.m[2][2] - Tz*mR1to0.m[2][0];	t2 = ea.x*mAR.m[2][2] + ea.z*mAR.m[2][0] + eb.x*mAR.m[1][1] + eb.y*mAR.m[0][1];	if(GREATER(t, t2))	return FALSE;	// L = A1 x B2
		t = Ty*mR1to0.m[0][0] - Tx*mR1to0.m[0][1];	t2 = ea.x*mAR.m[0][1] + ea.y*mAR.m[0][0] + eb.y*mAR.m[2][2] + eb.z*mAR.m[1][2];	if(GREATER(t, t2))	return FALSE;	// L = A2 x B0
		t = Ty*mR1to0.m[1][0] - Tx*mR1to0.m[1][1];	t2 = ea.x*mAR.m[1][1] + ea.y*mAR.m[1][0] + eb.x*mAR.m[2][2] + eb.z*mAR.m[0][2];	if(GREATER(t, t2))	return FALSE;	// L = A2 x B1
		t = Ty*mR1to0.m[2][0] - Tx*mR1to0.m[2][1];	t2 = ea.x*mAR.m[2][1] + ea.y*mAR.m[2][0] + eb.x*mAR.m[1][2] + eb.y*mAR.m[0][2];	if(GREATER(t, t2))	return FALSE;	// L = A2 x B2
	}
	return TRUE;
}*/

#ifndef OPC_SUPPORT_SIMD
//! A dedicated version when one box is constant
inline_ BOOL OBBCollider::BoxBoxOverlap(const Point& extents, const Point& center)
{
	// Stats
	mNbVolumeBVTests++;

	float t,t2;

	// Class I : A's basis vectors
	float Tx = mTBoxToModel.x - center.x;	t = extents.x + mBBx1;	if(GREATER(Tx, t))	return FALSE;
	float Ty = mTBoxToModel.y - center.y;	t = extents.y + mBBy1;	if(GREATER(Ty, t))	return FALSE;
	float Tz = mTBoxToModel.z - center.z;	t = extents.z + mBBz1;	if(GREATER(Tz, t))	return FALSE;

	// Class II : B's basis vectors
	t = Tx*mRBoxToModel.m[0][0] + Ty*mRBoxToModel.m[0][1] + Tz*mRBoxToModel.m[0][2];
	t2 = extents.x*mAR.m[0][0] + extents.y*mAR.m[0][1] + extents.z*mAR.m[0][2] + mBoxExtents.x;
	if(GREATER(t, t2))	return FALSE;

	t = Tx*mRBoxToModel.m[1][0] + Ty*mRBoxToModel.m[1][1] + Tz*mRBoxToModel.m[1][2];
	t2 = extents.x*mAR.m[1][0] + extents.y*mAR.m[1][1] + extents.z*mAR.m[1][2] + mBoxExtents.y;
	if(GREATER(t, t2))	return FALSE;

	t = Tx*mRBoxToModel.m[2][0] + Ty*mRBoxToModel.m[2][1] + Tz*mRBoxToModel.m[2][2];
	t2 = extents.x*mAR.m[2][0] + extents.y*mAR.m[2][1] + extents.z*mAR.m[2][2] + mBoxExtents.z;
	if(GREATER(t, t2))	return FALSE;

	// Class III : 9 cross products
	// Cool trick: always perform the full test for first level, regardless of settings.
	// That way pathological cases (such as the pencils scene) are quickly rejected anyway !
	if(mFullBoxBoxTest || mNbVolumeBVTests==1)
	{
		t = Tz*mRBoxToModel.m[0][1] - Ty*mRBoxToModel.m[0][2];	t2 = extents.y*mAR.m[0][2] + extents.z*mAR.m[0][1] + mBB_1;	if(GREATER(t, t2))	return FALSE;	// L = A0 x B0
		t = Tz*mRBoxToModel.m[1][1] - Ty*mRBoxToModel.m[1][2];	t2 = extents.y*mAR.m[1][2] + extents.z*mAR.m[1][1] + mBB_2;	if(GREATER(t, t2))	return FALSE;	// L = A0 x B1
		t = Tz*mRBoxToModel.m[2][1] - Ty*mRBoxToModel.m[2][2];	t2 = extents.y*mAR.m[2][2] + extents.z*mAR.m[2][1] + mBB_3;	if(GREATER(t, t2))	return FALSE;	// L = A0 x B2
		t = Tx*mRBoxToModel.m[0][2] - Tz*mRBoxToModel.m[0][0];	t2 = extents.x*mAR.m[0][2] + extents.z*mAR.m[0][0] + mBB_4;	if(GREATER(t, t2))	return FALSE;	// L = A1 x B0
		t = Tx*mRBoxToModel.m[1][2] - Tz*mRBoxToModel.m[1][0];	t2 = extents.x*mAR.m[1][2] + extents.z*mAR.m[1][0] + mBB_5;	if(GREATER(t, t2))	return FALSE;	// L = A1 x B1
		t = Tx*mRBoxToModel.m[2][2] - Tz*mRBoxToModel.m[2][0];	t2 = extents.x*mAR.m[2][2] + extents.z*mAR.m[2][0] + mBB_6;	if(GREATER(t, t2))	return FALSE;	// L = A1 x B2
		t = Ty*mRBoxToModel.m[0][0] - Tx*mRBoxToModel.m[0][1];	t2 = extents.x*mAR.m[0][1] + extents.y*mAR.m[0][0] + mBB_7;	if(GREATER(t, t2))	return FALSE;	// L = A2 x B0
		t = Ty*mRBoxToModel.m[1][0] - Tx*mRBoxToModel.m[1][1];	t2 = extents.x*mAR.m[1][1] + extents.y*mAR.m[1][0] + mBB_8;	if(GREATER(t, t2))	return FALSE;	// L = A2 x B1
		t = Ty*mRBoxToModel.m[2][0] - Tx*mRBoxToModel.m[2][1];	t2 = extents.x*mAR.m[2][1] + extents.y*mAR.m[2][0] + mBB_9;	if(GREATER(t, t2))	return FALSE;	// L = A2 x B2
	}
	return TRUE;
}
#else

inline_ BOOL OBBCollider::BoxBoxOverlap(const Point& extents, const Point& center)
{
	OpcSimd::Vector4 extents4 = OpcSimd::load(extents);
	OpcSimd::Vector4 center4 = OpcSimd::load(center);

	return BoxBoxOverlap(extents4, center4);
}

inline_ BOOL OBBCollider::BoxBoxOverlap(const OpcSimd::Vector4& extents4, const OpcSimd::Vector4& center4)
{
	OpcSimd::Vector4 TBoxToModel = OpcSimd::load(mTBoxToModel);
	OpcSimd::Vector4 BB = OpcSimd::load(mBBxyz1);

	OpcSimd::Vector4 rBoxToModel_0 = OpcSimd::load(mRBoxToModel[0]);
	OpcSimd::Vector4 rBoxToModel_1 = OpcSimd::load(mRBoxToModel[1]);
	OpcSimd::Vector4 rBoxToModel_2 = OpcSimd::load(mRBoxToModel[2]);

	OpcSimd::Vector4 ar_0 = OpcSimd::load(mAR[0]);//would be nice just to use abs, but ar has epsilon added
	OpcSimd::Vector4 ar_1 = OpcSimd::load(mAR[1]);//being able to aligned load would be much better.
	OpcSimd::Vector4 ar_2 = OpcSimd::load(mAR[2]);

	OpcSimd::Vector4 thisExtents = OpcSimd::load(mBoxExtents);

	OpcSimd::Vector4 BB_123 = OpcSimd::load(mBB_123);
	OpcSimd::Vector4 BB_456 = OpcSimd::load(mBB_456);
	OpcSimd::Vector4 BB_789 = OpcSimd::load(mBB_789);

	return BoxBoxOverlap(extents4, center4,
		TBoxToModel, BB,
		rBoxToModel_0, rBoxToModel_1, rBoxToModel_2,
		ar_0, ar_1, ar_2,
		thisExtents,
		BB_123, BB_456, BB_789);
}


//! A dedicated version when one box is constant
// preload constants into registers(passed as params)
inline_ BOOL OBBCollider::BoxBoxOverlap(const OpcSimd::Vector4& extents, const OpcSimd::Vector4& center,
		const OpcSimd::Vector4 &TBoxToModel, const OpcSimd::Vector4 &BB,
		const OpcSimd::Vector4 &rBoxToModel_0, const OpcSimd::Vector4 &rBoxToModel_1, const OpcSimd::Vector4 &rBoxToModel_2,
		const OpcSimd::Vector4 &ar_0, const OpcSimd::Vector4 &ar_1, const OpcSimd::Vector4 &ar_2,
		const OpcSimd::Vector4 &thisExtents,
		const OpcSimd::Vector4 &BB_123,	const OpcSimd::Vector4 &BB_456, const OpcSimd::Vector4 &BB_789
		)
{
	// Stats
	mNbVolumeBVTests++;

	/*
		// Class I : A's basis vectors
	float Tx = mTBoxToModel.x - center.x;	t = extents.x + mBBx1;	if(GREATER(Tx, t))	return FALSE;
	float Ty = mTBoxToModel.y - center.y;	t = extents.y + mBBy1;	if(GREATER(Ty, t))	return FALSE;
	float Tz = mTBoxToModel.z - center.z;	t = extents.z + mBBz1;	if(GREATER(Tz, t))	return FALSE;
*/
	OpcSimd::Vector4 T = OpcSimd::subtract(TBoxToModel, center);
	OpcSimd::Vector4 t = OpcSimd::add(extents, BB);

	if(!OpcSimd::inBounds3Bool(T, t))
		return FALSE;

/*
	// Class II : B's basis vectors
	t = Tx*mRBoxToModel.m[0][0] + Ty*mRBoxToModel.m[0][1] + Tz*mRBoxToModel.m[0][2];
	t2 = extents.x*mAR.m[0][0] + extents.y*mAR.m[0][1] + extents.z*mAR.m[0][2] + mBoxExtents.x;
	if(GREATER(t, t2))	return FALSE;

	t = Tx*mRBoxToModel.m[1][0] + Ty*mRBoxToModel.m[1][1] + Tz*mRBoxToModel.m[1][2];
	t2 = extents.x*mAR.m[1][0] + extents.y*mAR.m[1][1] + extents.z*mAR.m[1][2] + mBoxExtents.y;
	if(GREATER(t, t2))	return FALSE;

	t = Tx*mRBoxToModel.m[2][0] + Ty*mRBoxToModel.m[2][1] + Tz*mRBoxToModel.m[2][2];
	t2 = extents.x*mAR.m[2][0] + extents.y*mAR.m[2][1] + extents.z*mAR.m[2][2] + mBoxExtents.z;
	if(GREATER(t, t2))	return FALSE;
	*/

	//OPT: thisExtents is constant and could be factored out... (but would increase register pressure)

	OpcSimd::Vector4 t2;

	OpcSimd::Vector4 t_0 = OpcSimd::dot(T, rBoxToModel_0);
	OpcSimd::Vector4 t_1 = OpcSimd::dot(T, rBoxToModel_1);
	OpcSimd::Vector4 t_2 = OpcSimd::dot(T, rBoxToModel_2);
	OpcSimd::Vector4 t2_0 = OpcSimd::dot(extents, ar_0);
	OpcSimd::Vector4 t2_1 = OpcSimd::dot(extents, ar_1);
	OpcSimd::Vector4 t2_2 = OpcSimd::dot(extents, ar_2);

	t = OpcSimd::mergeXYZ(t_0, t_1, t_2);
	t2 = OpcSimd::mergeXYZ(t2_0, t2_1, t2_2);

	t2 = OpcSimd::add(t2, thisExtents);

	if(!OpcSimd::inBounds3Bool(t, t2))
		return FALSE;

	// Class III : 9 cross products
	// Cool trick: always perform the full test for first level, regardless of settings.
	// That way pathological cases (such as the pencils scene) are quickly rejected anyway !
	if(mFullBoxBoxTest || mNbVolumeBVTests==1)
	{
		//OPT: lots of constants could be factored out of the loop, but again increases register pressure.

		OpcSimd::Vector4 extentsX = OpcSimd::splatX(extents);
		OpcSimd::Vector4 extentsY = OpcSimd::splatY(extents);
		OpcSimd::Vector4 extentsZ = OpcSimd::splatZ(extents);
		
		OpcSimd::Vector4 rBoxToModel_colX, rBoxToModel_colY, rBoxToModel_colZ;
		OpcSimd::Vector4 ar_colX, ar_colY, ar_colZ;

		OpcSimd::transpose3x3(rBoxToModel_0, rBoxToModel_1, rBoxToModel_2,
			rBoxToModel_colX, rBoxToModel_colY, rBoxToModel_colZ);

		OpcSimd::transpose3x3(ar_0, ar_1, ar_2,
			ar_colX, ar_colY, ar_colZ);

		OpcSimd::Vector4 Tx = OpcSimd::splatX(T);
		OpcSimd::Vector4 Ty = OpcSimd::splatY(T);
		OpcSimd::Vector4 Tz = OpcSimd::splatZ(T);

		/*
		t = Tz*mRBoxToModel.m[0][1] - Ty*mRBoxToModel.m[0][2];	t2 = extents.y*mAR.m[0][2] + extents.z*mAR.m[0][1] + mBB_1;	if(GREATER(t, t2))	return FALSE;	// L = A0 x B0
		t = Tz*mRBoxToModel.m[1][1] - Ty*mRBoxToModel.m[1][2];	t2 = extents.y*mAR.m[1][2] + extents.z*mAR.m[1][1] + mBB_2;	if(GREATER(t, t2))	return FALSE;	// L = A0 x B1
		t = Tz*mRBoxToModel.m[2][1] - Ty*mRBoxToModel.m[2][2];	t2 = extents.y*mAR.m[2][2] + extents.z*mAR.m[2][1] + mBB_3;	if(GREATER(t, t2))	return FALSE;	// L = A0 x B2*/

		t = OpcSimd::multiply(Tz, rBoxToModel_colY);
		t = OpcSimd::negMultiplySubtract(Ty, rBoxToModel_colZ, t);
		
		t2 = OpcSimd::multiplyAdd(extentsY, ar_colZ, BB_123);
		t2 = OpcSimd::multiplyAdd(extentsZ, ar_colY, t2);

		if(!OpcSimd::inBounds3Bool(t, t2))
			return FALSE;

		/*
		t = Tx*mRBoxToModel.m[0][2] - Tz*mRBoxToModel.m[0][0];	t2 = extents.x*mAR.m[0][2] + extents.z*mAR.m[0][0] + mBB_4;	if(GREATER(t, t2))	return FALSE;	// L = A1 x B0
		t = Tx*mRBoxToModel.m[1][2] - Tz*mRBoxToModel.m[1][0];	t2 = extents.x*mAR.m[1][2] + extents.z*mAR.m[1][0] + mBB_5;	if(GREATER(t, t2))	return FALSE;	// L = A1 x B1
		t = Tx*mRBoxToModel.m[2][2] - Tz*mRBoxToModel.m[2][0];	t2 = extents.x*mAR.m[2][2] + extents.z*mAR.m[2][0] + mBB_6;	if(GREATER(t, t2))	return FALSE;	// L = A1 x B2*/

		t = OpcSimd::multiply(Tx, rBoxToModel_colZ);
		t = OpcSimd::negMultiplySubtract(Tz, rBoxToModel_colX, t);
		
		t2 = OpcSimd::multiplyAdd(extentsX, ar_colZ, BB_456);
		t2 = OpcSimd::multiplyAdd(extentsZ, ar_colX, t2);

		if(!OpcSimd::inBounds3Bool(t, t2))
			return FALSE;

		/*
		t = Ty*mRBoxToModel.m[0][0] - Tx*mRBoxToModel.m[0][1];	t2 = extents.x*mAR.m[0][1] + extents.y*mAR.m[0][0] + mBB_7;	if(GREATER(t, t2))	return FALSE;	// L = A2 x B0
		t = Ty*mRBoxToModel.m[1][0] - Tx*mRBoxToModel.m[1][1];	t2 = extents.x*mAR.m[1][1] + extents.y*mAR.m[1][0] + mBB_8;	if(GREATER(t, t2))	return FALSE;	// L = A2 x B1
		t = Ty*mRBoxToModel.m[2][0] - Tx*mRBoxToModel.m[2][1];	t2 = extents.x*mAR.m[2][1] + extents.y*mAR.m[2][0] + mBB_9;	if(GREATER(t, t2))	return FALSE;	// L = A2 x B2*/
		
		t = OpcSimd::multiply(Ty, rBoxToModel_colX);
		t = OpcSimd::negMultiplySubtract(Tx, rBoxToModel_colY, t);
		
		t2 = OpcSimd::multiplyAdd(extentsX, ar_colY, BB_789);
		t2 = OpcSimd::multiplyAdd(extentsY, ar_colX, t2);

		if(!OpcSimd::inBounds3Bool(t, t2))
			return FALSE;
	}
	

	return TRUE;
}
#endif //OPC_SUPPORT_SIMD

#ifdef __PPCGEKKO__
#include "wii\OPC_BoxBoxOverlap_wii.h"
#else
//! A special version for 2 axis-aligned boxes
inline_ BOOL AABBCollider::AABBAABBOverlap(const Point& extents, const Point& center)
{
	// Stats
	mNbVolumeBVTests++;
	float tx = mBox.mCenter.x - center.x;	float ex = extents.x + mBox.mExtents.x;	if(GREATER(tx, ex))	return FALSE;
	float ty = mBox.mCenter.y - center.y;	float ey = extents.y + mBox.mExtents.y;	if(GREATER(ty, ey))	return FALSE;
	float tz = mBox.mCenter.z - center.z;	float ez = extents.z + mBox.mExtents.z;	if(GREATER(tz, ez))	return FALSE;

	return TRUE;
}
#endif
inline_ BOOL AABBCollider::AABBAABBOverlapMinMax(const Point& bbMin, const Point& bbMax)
{
	//Could probably do this faster.
	if((bbMax.x < mMin.x) || (bbMax.y < mMin.y) || (bbMax.z < mMin.z)) return FALSE;
	if((bbMin.x > mMax.x) || (bbMin.y > mMax.y) || (bbMin.z > mMax.z)) return FALSE;

	return TRUE;
}

#ifdef OPC_SUPPORT_SIMD

inline_ BOOL AABBCollider::AABBAABBOverlap(const OpcSimd::Vector4 &extents, const OpcSimd::Vector4 &center)
{
	OpcSimd::Vector4 t = OpcSimd::load(mBox.mCenter) - center;
	OpcSimd::Vector4 e = extents + OpcSimd::load(mBox.mExtents);

	return OpcSimd::inBounds3Bool(t, e);
}

inline_ BOOL AABBCollider::AABBAABBOverlapMinMax(const OpcSimd::Vector4 &bbMin, const OpcSimd::Vector4 &bbMax)
{
	OpcSimd::Vector4 thisMin = OpcSimd::load(mMin);
	OpcSimd::Vector4 thisMax = OpcSimd::load(mMax);

	OpcSimd::Vector4 mask  = OpcSimd::or4(OpcSimd::less(bbMax, thisMin), OpcSimd::greater(bbMin, thisMax));
	return !OpcSimd::any3TrueBool(mask);
}

#endif

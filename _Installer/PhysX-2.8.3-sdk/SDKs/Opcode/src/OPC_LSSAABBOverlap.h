
#include "OPC_Simd.h"

// Following code from Magic-Software (http://www.magic-software.com/)
// A bit modified for Opcode

//#define OPCODE13
#ifdef OPCODE13

inline_ float OPC_PointAABBSqrDist(const Point& point, const Point& center, const Point& extents)
{
	// Compute coordinates of point in box coordinate system
	Point Closest = point - center;

	float SqrDistance = 0.0f;

	if(Closest.x < -extents.x)
	{
		float Delta = Closest.x + extents.x;
		SqrDistance += Delta*Delta;
	}
	else if(Closest.x > extents.x)
	{
		float Delta = Closest.x - extents.x;
		SqrDistance += Delta*Delta;
	}

	if(Closest.y < -extents.y)
	{
		float Delta = Closest.y + extents.y;
		SqrDistance += Delta*Delta;
	}
	else if(Closest.y > extents.y)
	{
		float Delta = Closest.y - extents.y;
		SqrDistance += Delta*Delta;
	}

	if(Closest.z < -extents.z)
	{
		float Delta = Closest.z + extents.z;
		SqrDistance += Delta*Delta;
	}
	else if(Closest.z > extents.z)
	{
		float Delta = Closest.z - extents.z;
		SqrDistance += Delta*Delta;
	}
	return SqrDistance;
}

static void Face(int i0, int i1, int i2, Point& rkPnt, const Point& rkDir, const Point& extents, const Point& rkPmE, float* pfLParam, float& rfSqrDistance)
{
    Point kPpE;
    float fLSqr, fInv, fTmp, fParam, fT, fDelta;

    kPpE[i1] = rkPnt[i1] + extents[i1];
    kPpE[i2] = rkPnt[i2] + extents[i2];
    if(rkDir[i0]*kPpE[i1] >= rkDir[i1]*rkPmE[i0])
    {
        if(rkDir[i0]*kPpE[i2] >= rkDir[i2]*rkPmE[i0])
        {
            // v[i1] >= -e[i1], v[i2] >= -e[i2] (distance = 0)
            if(pfLParam)
            {
                rkPnt[i0] = extents[i0];
                fInv = 1.0f/rkDir[i0];
                rkPnt[i1] -= rkDir[i1]*rkPmE[i0]*fInv;
                rkPnt[i2] -= rkDir[i2]*rkPmE[i0]*fInv;
                *pfLParam = -rkPmE[i0]*fInv;
            }
        }
        else
        {
            // v[i1] >= -e[i1], v[i2] < -e[i2]
            fLSqr = rkDir[i0]*rkDir[i0] + rkDir[i2]*rkDir[i2];
            fTmp = fLSqr*kPpE[i1] - rkDir[i1]*(rkDir[i0]*rkPmE[i0] + rkDir[i2]*kPpE[i2]);
            if(fTmp <= 2.0f*fLSqr*extents[i1])
            {
                fT = fTmp/fLSqr;
                fLSqr += rkDir[i1]*rkDir[i1];
                fTmp = kPpE[i1] - fT;
                fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*fTmp + rkDir[i2]*kPpE[i2];
                fParam = -fDelta/fLSqr;
                rfSqrDistance += rkPmE[i0]*rkPmE[i0] + fTmp*fTmp + kPpE[i2]*kPpE[i2] + fDelta*fParam;

                if(pfLParam)
                {
                    *pfLParam = fParam;
                    rkPnt[i0] = extents[i0];
                    rkPnt[i1] = fT - extents[i1];
                    rkPnt[i2] = -extents[i2];
                }
            }
            else
            {
                fLSqr += rkDir[i1]*rkDir[i1];
                fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*rkPmE[i1] + rkDir[i2]*kPpE[i2];
                fParam = -fDelta/fLSqr;
                rfSqrDistance += rkPmE[i0]*rkPmE[i0] + rkPmE[i1]*rkPmE[i1] + kPpE[i2]*kPpE[i2] + fDelta*fParam;

                if(pfLParam)
                {
                    *pfLParam = fParam;
                    rkPnt[i0] = extents[i0];
                    rkPnt[i1] = extents[i1];
                    rkPnt[i2] = -extents[i2];
                }
            }
        }
    }
    else
    {
        if ( rkDir[i0]*kPpE[i2] >= rkDir[i2]*rkPmE[i0] )
        {
            // v[i1] < -e[i1], v[i2] >= -e[i2]
            fLSqr = rkDir[i0]*rkDir[i0] + rkDir[i1]*rkDir[i1];
            fTmp = fLSqr*kPpE[i2] - rkDir[i2]*(rkDir[i0]*rkPmE[i0] + rkDir[i1]*kPpE[i1]);
            if(fTmp <= 2.0f*fLSqr*extents[i2])
            {
                fT = fTmp/fLSqr;
                fLSqr += rkDir[i2]*rkDir[i2];
                fTmp = kPpE[i2] - fT;
                fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*kPpE[i1] + rkDir[i2]*fTmp;
                fParam = -fDelta/fLSqr;
                rfSqrDistance += rkPmE[i0]*rkPmE[i0] + kPpE[i1]*kPpE[i1] + fTmp*fTmp + fDelta*fParam;

                if(pfLParam)
                {
                    *pfLParam = fParam;
                    rkPnt[i0] = extents[i0];
                    rkPnt[i1] = -extents[i1];
                    rkPnt[i2] = fT - extents[i2];
                }
            }
            else
            {
                fLSqr += rkDir[i2]*rkDir[i2];
                fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*kPpE[i1] + rkDir[i2]*rkPmE[i2];
                fParam = -fDelta/fLSqr;
                rfSqrDistance += rkPmE[i0]*rkPmE[i0] + kPpE[i1]*kPpE[i1] + rkPmE[i2]*rkPmE[i2] + fDelta*fParam;

                if(pfLParam)
                {
                    *pfLParam = fParam;
                    rkPnt[i0] = extents[i0];
                    rkPnt[i1] = -extents[i1];
                    rkPnt[i2] = extents[i2];
                }
            }
        }
        else
        {
            // v[i1] < -e[i1], v[i2] < -e[i2]
            fLSqr = rkDir[i0]*rkDir[i0]+rkDir[i2]*rkDir[i2];
            fTmp = fLSqr*kPpE[i1] - rkDir[i1]*(rkDir[i0]*rkPmE[i0] + rkDir[i2]*kPpE[i2]);
            if(fTmp >= 0.0f)
            {
                // v[i1]-edge is closest
                if ( fTmp <= 2.0f*fLSqr*extents[i1] )
                {
                    fT = fTmp/fLSqr;
                    fLSqr += rkDir[i1]*rkDir[i1];
                    fTmp = kPpE[i1] - fT;
                    fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*fTmp + rkDir[i2]*kPpE[i2];
                    fParam = -fDelta/fLSqr;
                    rfSqrDistance += rkPmE[i0]*rkPmE[i0] + fTmp*fTmp + kPpE[i2]*kPpE[i2] + fDelta*fParam;

                    if(pfLParam)
                    {
                        *pfLParam = fParam;
                        rkPnt[i0] = extents[i0];
                        rkPnt[i1] = fT - extents[i1];
                        rkPnt[i2] = -extents[i2];
                    }
                }
                else
                {
                    fLSqr += rkDir[i1]*rkDir[i1];
                    fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*rkPmE[i1] + rkDir[i2]*kPpE[i2];
                    fParam = -fDelta/fLSqr;
                    rfSqrDistance += rkPmE[i0]*rkPmE[i0] + rkPmE[i1]*rkPmE[i1] + kPpE[i2]*kPpE[i2] + fDelta*fParam;

                    if(pfLParam)
                    {
                        *pfLParam = fParam;
                        rkPnt[i0] = extents[i0];
                        rkPnt[i1] = extents[i1];
                        rkPnt[i2] = -extents[i2];
                    }
                }
                return;
            }

            fLSqr = rkDir[i0]*rkDir[i0] + rkDir[i1]*rkDir[i1];
            fTmp = fLSqr*kPpE[i2] - rkDir[i2]*(rkDir[i0]*rkPmE[i0] + rkDir[i1]*kPpE[i1]);
            if(fTmp >= 0.0f)
            {
                // v[i2]-edge is closest
                if(fTmp <= 2.0f*fLSqr*extents[i2])
                {
                    fT = fTmp/fLSqr;
                    fLSqr += rkDir[i2]*rkDir[i2];
                    fTmp = kPpE[i2] - fT;
                    fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*kPpE[i1] + rkDir[i2]*fTmp;
                    fParam = -fDelta/fLSqr;
                    rfSqrDistance += rkPmE[i0]*rkPmE[i0] + kPpE[i1]*kPpE[i1] + fTmp*fTmp + fDelta*fParam;

                    if(pfLParam)
                    {
                        *pfLParam = fParam;
                        rkPnt[i0] = extents[i0];
                        rkPnt[i1] = -extents[i1];
                        rkPnt[i2] = fT - extents[i2];
                    }
                }
                else
                {
                    fLSqr += rkDir[i2]*rkDir[i2];
                    fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*kPpE[i1] + rkDir[i2]*rkPmE[i2];
                    fParam = -fDelta/fLSqr;
                    rfSqrDistance += rkPmE[i0]*rkPmE[i0] + kPpE[i1]*kPpE[i1] + rkPmE[i2]*rkPmE[i2] + fDelta*fParam;

                    if(pfLParam)
                    {
                        *pfLParam = fParam;
                        rkPnt[i0] = extents[i0];
                        rkPnt[i1] = -extents[i1];
                        rkPnt[i2] = extents[i2];
                    }
                }
                return;
            }

            // (v[i1],v[i2])-corner is closest
            fLSqr += rkDir[i2]*rkDir[i2];
            fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*kPpE[i1] + rkDir[i2]*kPpE[i2];
            fParam = -fDelta/fLSqr;
            rfSqrDistance += rkPmE[i0]*rkPmE[i0] + kPpE[i1]*kPpE[i1] + kPpE[i2]*kPpE[i2] + fDelta*fParam;

            if(pfLParam)
            {
                *pfLParam = fParam;
                rkPnt[i0] = extents[i0];
                rkPnt[i1] = -extents[i1];
                rkPnt[i2] = -extents[i2];
            }
        }
    }
}

static void CaseNoZeros(Point& rkPnt, const Point& rkDir, const Point& extents, float* pfLParam, float& rfSqrDistance)
{
    Point kPmE(rkPnt.x - extents.x, rkPnt.y - extents.y, rkPnt.z - extents.z);

    float fProdDxPy, fProdDyPx, fProdDzPx, fProdDxPz, fProdDzPy, fProdDyPz;

    fProdDxPy = rkDir.x*kPmE.y;
    fProdDyPx = rkDir.y*kPmE.x;
    if(fProdDyPx >= fProdDxPy)
    {
        fProdDzPx = rkDir.z*kPmE.x;
        fProdDxPz = rkDir.x*kPmE.z;
        if(fProdDzPx >= fProdDxPz)
        {
            // line intersects x = e0
            Face(0, 1, 2, rkPnt, rkDir, extents, kPmE, pfLParam, rfSqrDistance);
        }
        else
        {
            // line intersects z = e2
            Face(2, 0, 1, rkPnt, rkDir, extents, kPmE, pfLParam, rfSqrDistance);
        }
    }
    else
    {
        fProdDzPy = rkDir.z*kPmE.y;
        fProdDyPz = rkDir.y*kPmE.z;
        if(fProdDzPy >= fProdDyPz)
        {
            // line intersects y = e1
            Face(1, 2, 0, rkPnt, rkDir, extents, kPmE, pfLParam, rfSqrDistance);
        }
        else
        {
            // line intersects z = e2
            Face(2, 0, 1, rkPnt, rkDir, extents, kPmE, pfLParam, rfSqrDistance);
        }
    }
}

static void Case0(int i0, int i1, int i2, Point& rkPnt, const Point& rkDir, const Point& extents, float* pfLParam, float& rfSqrDistance)
{
    float fPmE0 = rkPnt[i0] - extents[i0];
    float fPmE1 = rkPnt[i1] - extents[i1];
    float fProd0 = rkDir[i1]*fPmE0;
    float fProd1 = rkDir[i0]*fPmE1;
    float fDelta, fInvLSqr, fInv;

    if(fProd0 >= fProd1)
    {
        // line intersects P[i0] = e[i0]
        rkPnt[i0] = extents[i0];

        float fPpE1 = rkPnt[i1] + extents[i1];
        fDelta = fProd0 - rkDir[i0]*fPpE1;
        if(fDelta >= 0.0f)
        {
            fInvLSqr = 1.0f/(rkDir[i0]*rkDir[i0] + rkDir[i1]*rkDir[i1]);
            rfSqrDistance += fDelta*fDelta*fInvLSqr;
            if(pfLParam)
            {
                rkPnt[i1] = -extents[i1];
                *pfLParam = -(rkDir[i0]*fPmE0+rkDir[i1]*fPpE1)*fInvLSqr;
            }
        }
        else
        {
            if(pfLParam)
            {
                fInv = 1.0f/rkDir[i0];
                rkPnt[i1] -= fProd0*fInv;
                *pfLParam = -fPmE0*fInv;
            }
        }
    }
    else
    {
        // line intersects P[i1] = e[i1]
        rkPnt[i1] = extents[i1];

        float fPpE0 = rkPnt[i0] + extents[i0];
        fDelta = fProd1 - rkDir[i1]*fPpE0;
        if(fDelta >= 0.0f)
        {
            fInvLSqr = 1.0f/(rkDir[i0]*rkDir[i0] + rkDir[i1]*rkDir[i1]);
            rfSqrDistance += fDelta*fDelta*fInvLSqr;
            if(pfLParam)
            {
                rkPnt[i0] = -extents[i0];
                *pfLParam = -(rkDir[i0]*fPpE0+rkDir[i1]*fPmE1)*fInvLSqr;
            }
        }
        else
        {
            if(pfLParam)
            {
                fInv = 1.0f/rkDir[i1];
                rkPnt[i0] -= fProd1*fInv;
                *pfLParam = -fPmE1*fInv;
            }
        }
    }

    if(rkPnt[i2] < -extents[i2])
    {
        fDelta = rkPnt[i2] + extents[i2];
        rfSqrDistance += fDelta*fDelta;
        rkPnt[i2] = -extents[i2];
    }
    else if ( rkPnt[i2] > extents[i2] )
    {
        fDelta = rkPnt[i2] - extents[i2];
        rfSqrDistance += fDelta*fDelta;
        rkPnt[i2] = extents[i2];
    }
}

static void Case00(int i0, int i1, int i2, Point& rkPnt, const Point& rkDir, const Point& extents, float* pfLParam, float& rfSqrDistance)
{
    float fDelta;

    if(pfLParam)
        *pfLParam = (extents[i0] - rkPnt[i0])/rkDir[i0];

    rkPnt[i0] = extents[i0];

    if(rkPnt[i1] < -extents[i1])
    {
        fDelta = rkPnt[i1] + extents[i1];
        rfSqrDistance += fDelta*fDelta;
        rkPnt[i1] = -extents[i1];
    }
    else if(rkPnt[i1] > extents[i1])
    {
        fDelta = rkPnt[i1] - extents[i1];
        rfSqrDistance += fDelta*fDelta;
        rkPnt[i1] = extents[i1];
    }

    if(rkPnt[i2] < -extents[i2])
    {
        fDelta = rkPnt[i2] + extents[i2];
        rfSqrDistance += fDelta*fDelta;
        rkPnt[i1] = -extents[i2];
    }
    else if(rkPnt[i2] > extents[i2])
    {
        fDelta = rkPnt[i2] - extents[i2];
        rfSqrDistance += fDelta*fDelta;
        rkPnt[i2] = extents[i2];
    }
}

static void Case000(Point& rkPnt, const Point& extents, float& rfSqrDistance)
{
    float fDelta;

    if(rkPnt.x < -extents.x)
    {
        fDelta = rkPnt.x + extents.x;
        rfSqrDistance += fDelta*fDelta;
        rkPnt.x = -extents.x;
    }
    else if(rkPnt.x > extents.x)
    {
        fDelta = rkPnt.x - extents.x;
        rfSqrDistance += fDelta*fDelta;
        rkPnt.x = extents.x;
    }

    if(rkPnt.y < -extents.y)
    {
        fDelta = rkPnt.y + extents.y;
        rfSqrDistance += fDelta*fDelta;
        rkPnt.y = -extents.y;
    }
    else if(rkPnt.y > extents.y)
    {
        fDelta = rkPnt.y - extents.y;
        rfSqrDistance += fDelta*fDelta;
        rkPnt.y = extents.y;
    }

    if(rkPnt.z < -extents.z)
    {
        fDelta = rkPnt.z + extents.z;
        rfSqrDistance += fDelta*fDelta;
        rkPnt.z = -extents.z;
    }
    else if(rkPnt.z > extents.z)
    {
        fDelta = rkPnt.z - extents.z;
        rfSqrDistance += fDelta*fDelta;
        rkPnt.z = extents.z;
    }
}

static float SqrDistance(const Ray& rkLine, const Point& center, const Point& extents, float* pfLParam)
{
	// Hook
//	const Point& Axis0 = rkBox.mRot[0];
//	const Point& Axis1 = rkBox.mRot[1];
//	const Point& Axis2 = rkBox.mRot[2];

    // compute coordinates of line in box coordinate system
    Point kDiff = rkLine.mOrig - center;
//    Point kPnt(kDiff|Axis0, kDiff|Axis1, kDiff|Axis2);
    Point kPnt = kDiff;
//    Point kDir(rkLine.mDir|Axis0, rkLine.mDir|Axis1, rkLine.mDir|Axis2);
    Point kDir = rkLine.mDir;

    // Apply reflections so that direction vector has nonnegative components.
    bool bReflect[3];
    for(int i=0;i<3;i++)
    {
        if(kDir[i]<0.0f)
        {
            kPnt[i] = -kPnt[i];
            kDir[i] = -kDir[i];
            bReflect[i] = true;
        }
        else
        {
            bReflect[i] = false;
        }
    }

    float fSqrDistance = 0.0f;

    if(kDir.x>0.0f)
    {
        if(kDir.y>0.0f)
        {
            if(kDir.z>0.0f)	CaseNoZeros(kPnt, kDir, extents, pfLParam, fSqrDistance);		// (+,+,+)
            else			Case0(0, 1, 2, kPnt, kDir, extents, pfLParam, fSqrDistance);	// (+,+,0)
        }
        else
        {
            if(kDir.z>0.0f)	Case0(0, 2, 1, kPnt, kDir, extents, pfLParam, fSqrDistance);	// (+,0,+)
            else			Case00(0, 1, 2, kPnt, kDir, extents, pfLParam, fSqrDistance);	// (+,0,0)
        }
    }
    else
    {
        if(kDir.y>0.0f)
        {
            if(kDir.z>0.0f)	Case0(1, 2, 0, kPnt, kDir, extents, pfLParam, fSqrDistance);	// (0,+,+)
            else			Case00(1, 0, 2, kPnt, kDir, extents, pfLParam, fSqrDistance);	// (0,+,0)
        }
        else
        {
            if(kDir.z>0.0f)	Case00(2, 0, 1, kPnt, kDir, extents, pfLParam, fSqrDistance);	// (0,0,+)
            else
            {
							Case000(kPnt, extents, fSqrDistance);							// (0,0,0)
							if(pfLParam)	*pfLParam = 0.0f;
            }
        }
    }
    return fSqrDistance;
}

inline_ float OPC_SegmentOBBSqrDist(const Segment& segment, const Point& c0, const Point& e0)
{
	float fLP;
	float fSqrDistance = SqrDistance(Ray(segment.GetOrigin(), segment.ComputeDirection()), c0, e0, &fLP);
	if(fLP>=0.0f)
	{
		if(fLP<=1.0f)	return fSqrDistance;
		else			return OPC_PointAABBSqrDist(segment.mP1, c0, e0);
	}
	else				return OPC_PointAABBSqrDist(segment.mP0, c0, e0);
}

inline_ BOOL LSSCollider::LSSAABBOverlap(const Point& center, const Point& extents)
{
	// Stats
	mNbVolumeBVTests++;

	float s2 = OPC_SegmentOBBSqrDist(mSeg, center, extents);
	if(s2<mRadius2)	return TRUE;

	return FALSE;
}

#else

// New version in Opcode 1.4

#ifndef OPC_SUPPORT_SIMD
inline_ BOOL LSSCollider::LSSAABBOverlap(const Point& center, const Point& extents)
{
	// Stats
	mNbVolumeBVTests++;

	float dcx = mSCen.x - center.x;
	float ex = extents.x + mRadius;
	if(fabsf(dcx)>ex + mFDir.x)	return FALSE;

	float dcy = mSCen.y - center.y;
	float ey = extents.y + mRadius;
	if(fabsf(dcy)>ey + mFDir.y)	return FALSE;

	float dcz = mSCen.z - center.z;
	float ez = extents.z + mRadius;
	if(fabsf(dcz)>ez + mFDir.z)	return FALSE;

	if(fabsf(mSDir.y * dcz - mSDir.z * dcy) > ey*mFDir.z + ez*mFDir.y)	return FALSE;
	if(fabsf(mSDir.z * dcx - mSDir.x * dcz) > ex*mFDir.z + ez*mFDir.x)	return FALSE;
	if(fabsf(mSDir.x * dcy - mSDir.y * dcx) > ex*mFDir.y + ey*mFDir.x)	return FALSE;

	return TRUE;
}
#else
inline_ BOOL LSSCollider::LSSAABBOverlap(const Point& center, const Point& extents)
{
	OpcSimd::Vector4 center4 = OpcSimd::load(center);
	OpcSimd::Vector4 extents4 = OpcSimd::load(extents);

	return LSSAABBOverlap(center4, extents4);
}

inline_ BOOL LSSCollider::LSSAABBOverlap(const OpcSimd::Vector4& center, const OpcSimd::Vector4& extents)
{
	OpcSimd::Vector4 sCen = OpcSimd::load(mSCen);
	OpcSimd::Vector4 radius = OpcSimd::splatX(OpcSimd::load(mRadius));
	OpcSimd::Vector4 fDir = OpcSimd::load(mFDir);
	OpcSimd::Vector4 sDir = OpcSimd::load(mSDir);//sDir appears to equal sCen can we remove.


	OpcSimd::Vector4 dc = OpcSimd::subtract(sCen, center);
	OpcSimd::Vector4 e = OpcSimd::add(extents, radius);
	OpcSimd::Vector4 absDc = OpcSimd::abs(dc);
	OpcSimd::Vector4 ePlusFDir = OpcSimd::add(e, fDir);

	OpcSimd::Vector4 mask = OpcSimd::greater(absDc, ePlusFDir);
	if(OpcSimd::intNotEqualBool(mask, OpcSimd::zero()))//could avoid this with any true variant of greater.
		return FALSE;

	/*
	if(fabsf(mSDir.x * dcy - mSDir.y * dcx) > ex*mFDir.y + ey*mFDir.x)	return FALSE;
	if(fabsf(mSDir.y * dcz - mSDir.z * dcy) > ey*mFDir.z + ez*mFDir.y)	return FALSE;
	if(fabsf(mSDir.z * dcx - mSDir.x * dcz) > ex*mFDir.z + ez*mFDir.x)	return FALSE;
	*/

	OpcSimd::Vector4 dcYZX = OpcSimd::permuteYZX(dc);
	OpcSimd::Vector4 sDirYZX = OpcSimd::permuteYZX(sDir);
	OpcSimd::Vector4 eXYX = OpcSimd::permuteXYX(e);
	OpcSimd::Vector4 fDirYZZ = OpcSimd::permuteYZZ(fDir);
	OpcSimd::Vector4 eYZZ = OpcSimd::permuteYZZ(e);
	OpcSimd::Vector4 fDirXYX = OpcSimd::permuteXYX(fDir);

	OpcSimd::Vector4 a = OpcSimd::subtract(OpcSimd::multiply(sDir, dcYZX), OpcSimd::multiply(sDirYZX, dc));
	a = OpcSimd::abs(a);

	OpcSimd::Vector4 b = OpcSimd::multiplyAdd(eXYX, fDirYZZ, OpcSimd::multiply(eYZZ, fDirXYX));

	mask = OpcSimd::greater(a, b);
	if(OpcSimd::intNotEqualBool(mask, OpcSimd::zero()))//could avoid this with any true variant of greater.
		return FALSE;

	return TRUE;
}
#endif //OPC_SUPPORT_SIMD
#endif

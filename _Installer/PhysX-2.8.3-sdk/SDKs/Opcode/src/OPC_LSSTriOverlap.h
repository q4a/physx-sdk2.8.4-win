// Following code from Magic-Software (http://www.magic-software.com/)
// A bit modified for Opcode

static float OPC_PointTriangleSqrDist(const Point& point, const Point& p0, const Point& p1, const Point& p2)
{
	// Hook
	Point TriEdge0 = p1 - p0;
	Point TriEdge1 = p2 - p0;

	Point kDiff	= p0 - point;
	float fA00	= TriEdge0.SquareMagnitude();
	float fA01	= TriEdge0 | TriEdge1;
	float fA11	= TriEdge1.SquareMagnitude();
	float fB0	= kDiff | TriEdge0;
	float fB1	= kDiff | TriEdge1;
	float fC	= kDiff.SquareMagnitude();
	float fDet	= fabsf(fA00*fA11 - fA01*fA01);
	float fS	= fA01*fB1-fA11*fB0;
	float fT	= fA01*fB0-fA00*fB1;
	float fSqrDist;

	if(fS + fT <= fDet)
	{
		if(fS < 0.0f)
		{
			if(fT < 0.0f)  // region 4
			{
				if(fB0 < 0.0f)
				{
					if(-fB0 >= fA00)		fSqrDist = fA00+2.0f*fB0+fC;
					else					fSqrDist = fB0*(-fB0/fA00)+fC;
				}
				else
				{
					if(fB1 >= 0.0f)			fSqrDist = fC;
					else if(-fB1 >= fA11)	fSqrDist = fA11+2.0f*fB1+fC;
					else					fSqrDist = fB1*(-fB1/fA11)+fC;
				}
			}
			else  // region 3
			{
				if(fB1 >= 0.0f)				fSqrDist = fC;
				else if(-fB1 >= fA11)		fSqrDist = fA11+2.0f*fB1+fC;
				else						fSqrDist = fB1*(-fB1/fA11)+fC;
			}
		}
		else if(fT < 0.0f)  // region 5
		{
			if(fB0 >= 0.0f)					fSqrDist = fC;
			else if(-fB0 >= fA00)			fSqrDist = fA00+2.0f*fB0+fC;
			else							fSqrDist = fB0*(-fB0/fA00)+fC;
		}
		else  // region 0
		{
			// minimum at interior point
			if(fDet==0.0f)
			{
				fSqrDist = MAX_FLOAT;
			}
			else
			{
				float fInvDet = 1.0f/fDet;
				fS *= fInvDet;
				fT *= fInvDet;
				fSqrDist = fS*(fA00*fS+fA01*fT+2.0f*fB0) + fT*(fA01*fS+fA11*fT+2.0f*fB1)+fC;
			}
		}
	}
	else
	{
		float fTmp0, fTmp1, fNumer, fDenom;

		if(fS < 0.0f)  // region 2
		{
			fTmp0 = fA01 + fB0;
			fTmp1 = fA11 + fB1;
			if(fTmp1 > fTmp0)
			{
				fNumer = fTmp1 - fTmp0;
				fDenom = fA00-2.0f*fA01+fA11;
				if(fNumer >= fDenom)
				{
					fSqrDist = fA00+2.0f*fB0+fC;
				}
				else
				{
					fS = fNumer/fDenom;
					fT = 1.0f - fS;
					fSqrDist = fS*(fA00*fS+fA01*fT+2.0f*fB0) + fT*(fA01*fS+fA11*fT+2.0f*fB1)+fC;
				}
			}
			else
			{
				if(fTmp1 <= 0.0f)		fSqrDist = fA11+2.0f*fB1+fC;
				else if(fB1 >= 0.0f)	fSqrDist = fC;
				else					fSqrDist = fB1*(-fB1/fA11)+fC;
			}
		}
		else if(fT < 0.0f)  // region 6
		{
			fTmp0 = fA01 + fB1;
			fTmp1 = fA00 + fB0;
			if(fTmp1 > fTmp0)
			{
				fNumer = fTmp1 - fTmp0;
				fDenom = fA00-2.0f*fA01+fA11;
				if(fNumer >= fDenom)
				{
					fSqrDist = fA11+2.0f*fB1+fC;
				}
				else
				{
					fT = fNumer/fDenom;
					fS = 1.0f - fT;
					fSqrDist = fS*(fA00*fS+fA01*fT+2.0f*fB0) + fT*(fA01*fS+fA11*fT+2.0f*fB1)+fC;
				}
			}
			else
			{
				if(fTmp1 <= 0.0f)		fSqrDist = fA00+2.0f*fB0+fC;
				else if(fB0 >= 0.0f)	fSqrDist = fC;
				else					fSqrDist = fB0*(-fB0/fA00)+fC;
			}
		}
		else  // region 1
		{
			fNumer = fA11 + fB1 - fA01 - fB0;
			if(fNumer <= 0.0f)
			{
				fSqrDist = fA11+2.0f*fB1+fC;
			}
			else
			{
				fDenom = fA00-2.0f*fA01+fA11;
				if(fNumer >= fDenom)
				{
					fSqrDist = fA00+2.0f*fB0+fC;
				}
				else
				{
					fS = fNumer/fDenom;
					fT = 1.0f - fS;
					fSqrDist = fS*(fA00*fS+fA01*fT+2.0f*fB0) + fT*(fA01*fS+fA11*fT+2.0f*fB1)+fC;
				}
			}
		}
	}
	return fabsf(fSqrDist);
}

static float OPC_SegmentSegmentSqrDist(const Segment& rkSeg0, const Segment& rkSeg1)
{
	// Hook
	Point rkSeg0Direction	= rkSeg0.ComputeDirection();
	Point rkSeg1Direction	= rkSeg1.ComputeDirection();

	Point kDiff	= rkSeg0.mP0 - rkSeg1.mP0;
	float fA00	= rkSeg0Direction.SquareMagnitude();
	float fA01	= -rkSeg0Direction.Dot(rkSeg1Direction);
	float fA11	= rkSeg1Direction.SquareMagnitude();
	float fB0	= kDiff.Dot(rkSeg0Direction);
	float fC	= kDiff.SquareMagnitude();
	float fDet	= fabsf(fA00*fA11-fA01*fA01);

	float fB1, fS, fT, fSqrDist, fTmp;

	if(fDet>=PARALLEL_TOLERANCE)
	{
		// line segments are not parallel
		fB1 = -kDiff.Dot(rkSeg1Direction);
		fS = fA01*fB1-fA11*fB0;
		fT = fA01*fB0-fA00*fB1;

		if(fS >= 0.0f)
		{
			if(fS <= fDet)
			{
				if(fT >= 0.0f)
				{
					if(fT <= fDet)  // region 0 (interior)
					{
						// minimum at two interior points of 3D lines
						float fInvDet = 1.0f/fDet;
						fS *= fInvDet;
						fT *= fInvDet;
						fSqrDist = fS*(fA00*fS+fA01*fT+2.0f*fB0) + fT*(fA01*fS+fA11*fT+2.0f*fB1)+fC;
					}
					else  // region 3 (side)
					{
						fTmp = fA01+fB0;
						if(fTmp>=0.0f)			fSqrDist = fA11+2.0f*fB1+fC;
						else if(-fTmp>=fA00)	fSqrDist = fA00+fA11+fC+2.0f*(fB1+fTmp);
						else					fSqrDist = fTmp*(-fTmp/fA00)+fA11+2.0f*fB1+fC;
					}
				}
				else  // region 7 (side)
				{
					if(fB0>=0.0f)				fSqrDist = fC;
					else if(-fB0>=fA00)			fSqrDist = fA00+2.0f*fB0+fC;
					else						fSqrDist = fB0*(-fB0/fA00)+fC;
				}
			}
			else
			{
				if ( fT >= 0.0 )
				{
					if ( fT <= fDet )  // region 1 (side)
					{
						fTmp = fA01+fB1;
						if(fTmp>=0.0f)			fSqrDist = fA00+2.0f*fB0+fC;
						else if(-fTmp>=fA11)	fSqrDist = fA00+fA11+fC+2.0f*(fB0+fTmp);
						else					fSqrDist = fTmp*(-fTmp/fA11)+fA00+2.0f*fB0+fC;
					}
					else  // region 2 (corner)
					{
						fTmp = fA01+fB0;
						if ( -fTmp <= fA00 )
						{
							if(fTmp>=0.0f)		fSqrDist = fA11+2.0f*fB1+fC;
							else				fSqrDist = fTmp*(-fTmp/fA00)+fA11+2.0f*fB1+fC;
						}
						else
						{
							fTmp = fA01+fB1;
							if(fTmp>=0.0f)			fSqrDist = fA00+2.0f*fB0+fC;
							else if(-fTmp>=fA11)	fSqrDist = fA00+fA11+fC+2.0f*(fB0+fTmp);
							else					fSqrDist = fTmp*(-fTmp/fA11)+fA00+2.0f*fB0+fC;
						}
					}
				}
				else  // region 8 (corner)
				{
					if ( -fB0 < fA00 )
					{
						if(fB0>=0.0f)	fSqrDist = fC;
						else			fSqrDist = fB0*(-fB0/fA00)+fC;
					}
					else
					{
						fTmp = fA01+fB1;
						if(fTmp>=0.0f)			fSqrDist = fA00+2.0f*fB0+fC;
						else if(-fTmp>=fA11)	fSqrDist = fA00+fA11+fC+2.0f*(fB0+fTmp);
						else					fSqrDist = fTmp*(-fTmp/fA11)+fA00+2.0f*fB0+fC;
					}
				}
			}
		}
		else 
		{
			if ( fT >= 0.0f )
			{
				if ( fT <= fDet )  // region 5 (side)
				{
					if(fB1>=0.0f)		fSqrDist = fC;
					else if(-fB1>=fA11)	fSqrDist = fA11+2.0f*fB1+fC;
					else				fSqrDist = fB1*(-fB1/fA11)+fC;
				}
				else  // region 4 (corner)
				{
					fTmp = fA01+fB0;
					if ( fTmp < 0.0f )
					{
						if(-fTmp>=fA00)	fSqrDist = fA00+fA11+fC+2.0f*(fB1+fTmp);
						else			fSqrDist = fTmp*(-fTmp/fA00)+fA11+2.0f*fB1+fC;
					}
					else
					{
						if(fB1>=0.0f)		fSqrDist = fC;
						else if(-fB1>=fA11)	fSqrDist = fA11+2.0f*fB1+fC;
						else				fSqrDist = fB1*(-fB1/fA11)+fC;
					}
				}
			}
			else   // region 6 (corner)
			{
				if ( fB0 < 0.0f )
				{
					if(-fB0>=fA00)	fSqrDist = fA00+2.0f*fB0+fC;
					else			fSqrDist = fB0*(-fB0/fA00)+fC;
				}
				else
				{
					if(fB1>=0.0f)		fSqrDist = fC;
					else if(-fB1>=fA11)	fSqrDist = fA11+2.0f*fB1+fC;
					else				fSqrDist = fB1*(-fB1/fA11)+fC;
				}
			}
		}
	}
	else
	{
		// line segments are parallel
		if ( fA01 > 0.0f )
		{
			// direction vectors form an obtuse angle
			if ( fB0 >= 0.0f )
			{
				fSqrDist = fC;
			}
			else if ( -fB0 <= fA00 )
			{
				fSqrDist = fB0*(-fB0/fA00)+fC;
			}
			else
			{
				fB1 = -kDiff.Dot(rkSeg1Direction);
				fTmp = fA00+fB0;
				if ( -fTmp >= fA01 )
				{
					fSqrDist = fA00+fA11+fC+2.0f*(fA01+fB0+fB1);
				}
				else
				{
					fT = -fTmp/fA01;
					fSqrDist = fA00+2.0f*fB0+fC+fT*(fA11*fT+2.0f*(fA01+fB1));
				}
			}
		}
		else
		{
			// direction vectors form an acute angle
			if ( -fB0 >= fA00 )
			{
				fSqrDist = fA00+2.0f*fB0+fC;
			}
			else if ( fB0 <= 0.0f )
			{
				fSqrDist = fB0*(-fB0/fA00)+fC;
			}
			else
			{
				fB1 = -kDiff.Dot(rkSeg1Direction);
				if ( fB0 >= -fA01 )
				{
					fSqrDist = fA11+2.0f*fB1+fC;
				}
				else
				{
					fT = -fB0/fA01;
					fSqrDist = fC+fT*(2.0f*fB1+fA11*fT);
				}
			}
		}
	}
	return fabsf(fSqrDist);
}

inline_ float OPC_SegmentRaySqrDist(const Segment& rkSeg0, const Ray& rkSeg1)
{
	return OPC_SegmentSegmentSqrDist(rkSeg0, Segment(rkSeg1.mOrig, rkSeg1.mOrig + rkSeg1.mDir));
}

static float OPC_SegmentTriangleSqrDist(const Segment& segment, const Point& p0, const Point& p1, const Point& p2)
{
	// Hook
	const Point TriEdge0 = p1 - p0;
	const Point TriEdge1 = p2 - p0;

	const Point& rkSegOrigin	= segment.GetOrigin();
	Point rkSegDirection		= segment.ComputeDirection();

	Point kDiff = p0 - rkSegOrigin;
	float fA00 = rkSegDirection.SquareMagnitude();
	float fA01 = -rkSegDirection.Dot(TriEdge0);
	float fA02 = -rkSegDirection.Dot(TriEdge1);
	float fA11 = TriEdge0.SquareMagnitude();
	float fA12 = TriEdge0.Dot(TriEdge1);
	float fA22 = TriEdge1.Dot(TriEdge1);
	float fB0  = -kDiff.Dot(rkSegDirection);
	float fB1  = kDiff.Dot(TriEdge0);
	float fB2  = kDiff.Dot(TriEdge1);
	float fCof00 = fA11*fA22-fA12*fA12;
	float fCof01 = fA02*fA12-fA01*fA22;
	float fCof02 = fA01*fA12-fA02*fA11;
	float fDet = fA00*fCof00+fA01*fCof01+fA02*fCof02;

	Ray kTriSeg;
	Point kPt;
	float fSqrDist, fSqrDist0;

	// PT: replaced with same fix as in TTP 4021. Seems to fix TTP 5514.
//	if(fabsf(fDet)>=PARALLEL_TOLERANCE)

	Point kNormal = TriEdge0^TriEdge1;
	float fDot = kNormal|rkSegDirection;
	if ( fDot*fDot >= 1e-6*rkSegDirection.SquareMagnitude()*kNormal.SquareMagnitude())

	{
		float fCof11 = fA00*fA22-fA02*fA02;
		float fCof12 = fA02*fA01-fA00*fA12;
		float fCof22 = fA00*fA11-fA01*fA01;
		float fInvDet = 1.0f/fDet;
		float fRhs0 = -fB0*fInvDet;
		float fRhs1 = -fB1*fInvDet;
		float fRhs2 = -fB2*fInvDet;

		float fR = fCof00*fRhs0+fCof01*fRhs1+fCof02*fRhs2;
		float fS = fCof01*fRhs0+fCof11*fRhs1+fCof12*fRhs2;
		float fT = fCof02*fRhs0+fCof12*fRhs1+fCof22*fRhs2;

		if ( fR < 0.0f )
		{
			if ( fS+fT <= 1.0f )
			{
				if ( fS < 0.0f )
				{
					if ( fT < 0.0f )  // region 4m
					{
						// min on face s=0 or t=0 or r=0
						kTriSeg.mOrig = p0;
						kTriSeg.mDir = TriEdge1;
						fSqrDist = OPC_SegmentRaySqrDist(segment, kTriSeg);
						kTriSeg.mOrig = p0;
						kTriSeg.mDir = TriEdge0;
						fSqrDist0 = OPC_SegmentRaySqrDist(segment, kTriSeg);
						if(fSqrDist0<fSqrDist)	fSqrDist = fSqrDist0;
						fSqrDist0 = OPC_PointTriangleSqrDist(rkSegOrigin, p0, p1, p2);
						if(fSqrDist0<fSqrDist)	fSqrDist = fSqrDist0;
                    }
                    else  // region 3m
                    {
                        // min on face s=0 or r=0
                        kTriSeg.mOrig = p0;
                        kTriSeg.mDir = TriEdge1;
                        fSqrDist = OPC_SegmentRaySqrDist(segment, kTriSeg);
                        fSqrDist0 = OPC_PointTriangleSqrDist(rkSegOrigin, p0, p1, p2);
                        if(fSqrDist0<fSqrDist)	fSqrDist = fSqrDist0;
                    }
                }
                else if ( fT < 0.0f )  // region 5m
                {
                    // min on face t=0 or r=0
                    kTriSeg.mOrig = p0;
                    kTriSeg.mDir = TriEdge0;
                    fSqrDist = OPC_SegmentRaySqrDist(segment, kTriSeg);
                    fSqrDist0 = OPC_PointTriangleSqrDist(rkSegOrigin, p0, p1, p2);
                    if(fSqrDist0<fSqrDist)	fSqrDist = fSqrDist0;
                }
                else  // region 0m
                {
                    // min on face r=0
                    fSqrDist = OPC_PointTriangleSqrDist(rkSegOrigin, p0, p1, p2);
                }
            }
            else
            {
                if ( fS < 0.0f )  // region 2m
                {
                    // min on face s=0 or s+t=1 or r=0
                    kTriSeg.mOrig = p0;
                    kTriSeg.mDir = TriEdge1;
                    fSqrDist = OPC_SegmentRaySqrDist(segment, kTriSeg);
                    kTriSeg.mOrig = p1;
                    kTriSeg.mDir = TriEdge1-TriEdge0;
                    fSqrDist0 = OPC_SegmentRaySqrDist(segment, kTriSeg);
                    if(fSqrDist0<fSqrDist)	fSqrDist = fSqrDist0;
                    fSqrDist0 = OPC_PointTriangleSqrDist(rkSegOrigin, p0, p1, p2);
                    if(fSqrDist0<fSqrDist)	fSqrDist = fSqrDist0;
                }
                else if ( fT < 0.0f )  // region 6m
                {
                    // min on face t=0 or s+t=1 or r=0
                    kTriSeg.mOrig = p0;
                    kTriSeg.mDir = TriEdge0;
                    fSqrDist = OPC_SegmentRaySqrDist(segment, kTriSeg);
                    kTriSeg.mOrig = p1;
                    kTriSeg.mDir = TriEdge1-TriEdge0;
                    fSqrDist0 = OPC_SegmentRaySqrDist(segment, kTriSeg);
                    if(fSqrDist0<fSqrDist)	fSqrDist = fSqrDist0;
                    fSqrDist0 = OPC_PointTriangleSqrDist(rkSegOrigin, p0, p1, p2);
                    if(fSqrDist0<fSqrDist)	fSqrDist = fSqrDist0;
                }
                else  // region 1m
                {
                    // min on face s+t=1 or r=0
                    kTriSeg.mOrig = p1;
                    kTriSeg.mDir = TriEdge1-TriEdge0;
                    fSqrDist = OPC_SegmentRaySqrDist(segment, kTriSeg);
                    fSqrDist0 = OPC_PointTriangleSqrDist(rkSegOrigin, p0, p1, p2);
                    if(fSqrDist0<fSqrDist)	fSqrDist = fSqrDist0;
                }
            }
        }
        else if ( fR <= 1.0f )
        {
            if ( fS+fT <= 1.0f )
            {
                if ( fS < 0.0f )
                {
                    if ( fT < 0.0f )  // region 4
                    {
                        // min on face s=0 or t=0
                        kTriSeg.mOrig = p0;
                        kTriSeg.mDir = TriEdge1;
                        fSqrDist = OPC_SegmentRaySqrDist(segment, kTriSeg);
                        kTriSeg.mOrig = p0;
                        kTriSeg.mDir = TriEdge0;
                        fSqrDist0 = OPC_SegmentRaySqrDist(segment, kTriSeg);
                        if(fSqrDist0<fSqrDist)	fSqrDist = fSqrDist0;
                    }
                    else  // region 3
                    {
                        // min on face s=0
                        kTriSeg.mOrig = p0;
                        kTriSeg.mDir = TriEdge1;
                        fSqrDist = OPC_SegmentRaySqrDist(segment, kTriSeg);
                    }
                }
                else if ( fT < 0.0f )  // region 5
                {
                    // min on face t=0
                    kTriSeg.mOrig = p0;
                    kTriSeg.mDir = TriEdge0;
                    fSqrDist = OPC_SegmentRaySqrDist(segment, kTriSeg);
                }
                else  // region 0
                {
                    // global minimum is interior, done
                    fSqrDist = fR*(fA00*fR+fA01*fS+fA02*fT+2.0f*fB0)
                          +fS*(fA01*fR+fA11*fS+fA12*fT+2.0f*fB1)
                          +fT*(fA02*fR+fA12*fS+fA22*fT+2.0f*fB2)
                          +kDiff.SquareMagnitude();
                }
            }
            else
            {
                if ( fS < 0.0f )  // region 2
                {
                    // min on face s=0 or s+t=1
                    kTriSeg.mOrig = p0;
                    kTriSeg.mDir = TriEdge1;
                    fSqrDist = OPC_SegmentRaySqrDist(segment, kTriSeg);
                    kTriSeg.mOrig = p1;
                    kTriSeg.mDir = TriEdge1-TriEdge0;
                    fSqrDist0 = OPC_SegmentRaySqrDist(segment, kTriSeg);
                    if(fSqrDist0<fSqrDist)	fSqrDist = fSqrDist0;
                }
                else if ( fT < 0.0f )  // region 6
                {
                    // min on face t=0 or s+t=1
                    kTriSeg.mOrig = p0;
                    kTriSeg.mDir = TriEdge0;
                    fSqrDist = OPC_SegmentRaySqrDist(segment, kTriSeg);
                    kTriSeg.mOrig = p1;
                    kTriSeg.mDir = TriEdge1-TriEdge0;
                    fSqrDist0 = OPC_SegmentRaySqrDist(segment, kTriSeg);
                    if(fSqrDist0<fSqrDist)	fSqrDist = fSqrDist0;
                }
                else  // region 1
                {
                    // min on face s+t=1
                    kTriSeg.mOrig = p1;
                    kTriSeg.mDir = TriEdge1-TriEdge0;
                    fSqrDist = OPC_SegmentRaySqrDist(segment, kTriSeg);
                }
            }
        }
        else  // fR > 1
        {
            if ( fS+fT <= 1.0f )
            {
                if ( fS < 0.0f )
                {
                    if ( fT < 0.0f )  // region 4p
                    {
                        // min on face s=0 or t=0 or r=1
                        kTriSeg.mOrig = p0;
                        kTriSeg.mDir = TriEdge1;
                        fSqrDist = OPC_SegmentRaySqrDist(segment, kTriSeg);
                        kTriSeg.mOrig = p0;
                        kTriSeg.mDir = TriEdge0;
                        fSqrDist0 = OPC_SegmentRaySqrDist(segment, kTriSeg);
                        if(fSqrDist0<fSqrDist)	fSqrDist = fSqrDist0;
                        kPt = rkSegOrigin+rkSegDirection;
                        fSqrDist0 = OPC_PointTriangleSqrDist(kPt, p0, p1, p2);
                        if(fSqrDist0<fSqrDist)	fSqrDist = fSqrDist0;
                    }
                    else  // region 3p
                    {
                        // min on face s=0 or r=1
                        kTriSeg.mOrig = p0;
                        kTriSeg.mDir = TriEdge1;
                        fSqrDist = OPC_SegmentRaySqrDist(segment, kTriSeg);
                        kPt = rkSegOrigin+rkSegDirection;
                        fSqrDist0 = OPC_PointTriangleSqrDist(kPt, p0, p1, p2);
                        if(fSqrDist0<fSqrDist)	fSqrDist = fSqrDist0;
                    }
                }
                else if ( fT < 0.0f )  // region 5p
                {
                    // min on face t=0 or r=1
                    kTriSeg.mOrig = p0;
                    kTriSeg.mDir = TriEdge0;
                    fSqrDist = OPC_SegmentRaySqrDist(segment, kTriSeg);
                    kPt = rkSegOrigin+rkSegDirection;
                    fSqrDist0 = OPC_PointTriangleSqrDist(kPt, p0, p1, p2);
                    if(fSqrDist0<fSqrDist)	fSqrDist = fSqrDist0;
                }
                else  // region 0p
                {
                    // min face on r=1
                    kPt = rkSegOrigin+rkSegDirection;
                    fSqrDist = OPC_PointTriangleSqrDist(kPt, p0, p1, p2);
                }
            }
            else
            {
                if ( fS < 0.0f )  // region 2p
                {
                    // min on face s=0 or s+t=1 or r=1
                    kTriSeg.mOrig = p0;
                    kTriSeg.mDir = TriEdge1;
                    fSqrDist = OPC_SegmentRaySqrDist(segment, kTriSeg);
                    kTriSeg.mOrig = p1;
                    kTriSeg.mDir = TriEdge1-TriEdge0;
                    fSqrDist0 = OPC_SegmentRaySqrDist(segment, kTriSeg);
                    if(fSqrDist0<fSqrDist)	fSqrDist = fSqrDist0;
                    kPt = rkSegOrigin+rkSegDirection;
                    fSqrDist0 = OPC_PointTriangleSqrDist(kPt, p0, p1, p2);
                    if(fSqrDist0<fSqrDist)	fSqrDist = fSqrDist0;
                }
                else if ( fT < 0.0f )  // region 6p
                {
                    // min on face t=0 or s+t=1 or r=1
                    kTriSeg.mOrig = p0;
                    kTriSeg.mDir = TriEdge0;
                    fSqrDist = OPC_SegmentRaySqrDist(segment, kTriSeg);
                    kTriSeg.mOrig = p1;
                    kTriSeg.mDir = TriEdge1-TriEdge0;
                    fSqrDist0 = OPC_SegmentRaySqrDist(segment, kTriSeg);
                    if(fSqrDist0<fSqrDist)	fSqrDist = fSqrDist0;
                    kPt = rkSegOrigin+rkSegDirection;
                    fSqrDist0 = OPC_PointTriangleSqrDist(kPt, p0, p1, p2);
                    if(fSqrDist0<fSqrDist)	fSqrDist = fSqrDist0;
                }
                else  // region 1p
                {
                    // min on face s+t=1 or r=1
                    kTriSeg.mOrig = p1;
                    kTriSeg.mDir = TriEdge1-TriEdge0;
                    fSqrDist = OPC_SegmentRaySqrDist(segment, kTriSeg);
                    kPt = rkSegOrigin+rkSegDirection;
                    fSqrDist0 = OPC_PointTriangleSqrDist(kPt, p0, p1, p2);
                    if(fSqrDist0<fSqrDist)	fSqrDist = fSqrDist0;
                }
            }
        }
    }
    else
    {
        // segment and triangle are parallel
        kTriSeg.mOrig = p0;
        kTriSeg.mDir = TriEdge0;
        fSqrDist = OPC_SegmentRaySqrDist(segment, kTriSeg);

        kTriSeg.mDir = TriEdge1;
        fSqrDist0 = OPC_SegmentRaySqrDist(segment, kTriSeg);
        if(fSqrDist0<fSqrDist)	fSqrDist = fSqrDist0;

        kTriSeg.mOrig = p1;
        kTriSeg.mDir = TriEdge1 - TriEdge0;
        fSqrDist0 = OPC_SegmentRaySqrDist(segment, kTriSeg);
        if(fSqrDist0<fSqrDist)	fSqrDist = fSqrDist0;

        fSqrDist0 = OPC_PointTriangleSqrDist(rkSegOrigin, p0, p1, p2);
        if(fSqrDist0<fSqrDist)	fSqrDist = fSqrDist0;

        kPt = rkSegOrigin+rkSegDirection;
        fSqrDist0 = OPC_PointTriangleSqrDist(kPt, p0, p1, p2);
        if(fSqrDist0<fSqrDist)	fSqrDist = fSqrDist0;
    }
    return fabsf(fSqrDist);
}

inline_ BOOL LSSCollider::LSSTriOverlap(const Point& vert0, const Point& vert1, const Point& vert2)
{
	// Stats
	mNbVolumePrimTests++;

//	return LSSTriangleOverlap(Triangle(vert0, vert1, vert2), mSeg.mP0, mSeg.ComputeDirection(), sqrtf(mRadius2));

	float s2 = OPC_SegmentTriangleSqrDist(mSeg, vert0, vert1, vert2);
	if(s2<mRadius2)	return TRUE;
	return FALSE;
}

#ifndef OPC_SUPPORT_SIMD

inline_ BOOL LSSCollider::LooseLSSTriOverlap(const Point& vert0, const Point& vert1, const Point& vert2)
{
	
	Point v0 = mOBB.mRot * (vert0 - mOBB.mCenter);//should be inverse rotate.
	Point v1 = mOBB.mRot * (vert1 - mOBB.mCenter);
	Point v2 = mOBB.mRot * (vert2 - mOBB.mCenter);

// now do a tri-AABB test, with AABB at origin.

	//test tri AABB

	Point triMin = Point(MIN(v0.x, MIN(v1.x, v2.x)), MIN(v0.y, MIN(v1.y, v2.y)), MIN(v0.z, MIN(v1.z, v2.z)));
	Point triMax = Point(MAX(v0.x, MAX(v1.x, v2.x)), MAX(v0.y, MAX(v1.y, v2.y)), MAX(v0.z, MAX(v1.z, v2.z)));

	if((triMin.x > mOBB.mExtents.x) || (triMin.y > mOBB.mExtents.y) || (triMin.z > mOBB.mExtents.z))
		return FALSE;

	if((triMax.x < -mOBB.mExtents.x) || (triMax.y < -mOBB.mExtents.y) || (triMax.z < -mOBB.mExtents.z))
		return FALSE;

	// test tri plane.

	Point normal;
	normal.Cross(v1 - v0,v2 - v0);
	float dist = normal.Dot(v0);

	// find the min max on normal.
	Point vMin, vMax;

	if(normal.x > 0) 
	{
		vMin.x = -mOBB.mExtents.x;
		vMax.x = mOBB.mExtents.x;
	}
	else
	{
		vMin.x = mOBB.mExtents.x;
		vMax.x = -mOBB.mExtents.x;
	}

	if(normal.y > 0) 
	{
		vMin.y = -mOBB.mExtents.y;
		vMax.y = mOBB.mExtents.y;
	}
	else
	{
		vMin.y = mOBB.mExtents.y;
		vMax.y = -mOBB.mExtents.y;
	}


	if(normal.z > 0) 
	{
		vMin.z = -mOBB.mExtents.z;
		vMax.z = mOBB.mExtents.z;
	}
	else
	{
		vMin.z = mOBB.mExtents.z;
		vMax.z = -mOBB.mExtents.z;
	}

	// are they disjoint?

	float minDist = vMin.Dot(normal);
	float maxDist = vMax.Dot(normal);

	if((minDist > dist) || (maxDist < dist))
		return FALSE;


	// Test edge axes.

	Point axis;
	float p0, p1, p2, pMin, pMax, axisRadius;

	Point edge0 = v1 - v0;
	Point edge1 = v2 - v1;
	Point edge2 = v0 - v2;

	Point radius = mOBB.mExtents;

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
	if((pMin > axisRadius) || (pMax < -axisRadius))
		return FALSE;

	// axis == [1,0,0] x e1 == [0, -e1.z, e1.y]
	axis.Cross(Point(1.0f, 0.0f, 0.0f),edge1);
	p0 = axis.Dot(v0);
	p1 = axis.Dot(v1);
	pMin = MIN(p0, p1);
	pMax = MAX(p0, p1);
	axisRadius = radius.Dot(Point(fabsf(axis.x), fabsf(axis.y), fabsf(axis.z)));
	if((pMin > axisRadius) || (pMax < -axisRadius))
		return FALSE;

	// axis == [1,0,0] x e2 == [0, -e2.z, e2.y]
	axis.Cross(Point(1.0f, 0.0f, 0.0f),edge2);
	p0 = axis.Dot(v0);
	p1 = axis.Dot(v1);
	pMin = MIN(p0, p1);
	pMax = MAX(p0, p1);
	axisRadius = radius.Dot(Point(fabsf(axis.x), fabsf(axis.y), fabsf(axis.z)));
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
	if((pMin > axisRadius) || (pMax < -axisRadius))
		return FALSE;


	// axis == [0,1,0] x e1 == [e1.z, 0, -e1.x]
	axis.Cross(Point(0.0f, 1.0f, 0.0f),edge1);
	p0 = axis.Dot(v0);
	p1 = axis.Dot(v1);
	pMin = MIN(p0, p1);
	pMax = MAX(p0, p1);
	axisRadius = radius.Dot(Point(fabsf(axis.x), fabsf(axis.y), fabsf(axis.z)));
	if((pMin > axisRadius) || (pMax < -axisRadius))
		return FALSE;

	// axis == [0, 1, 0] x e2 == [e2.z, 0, -e2.x]
	axis.Cross(Point(0.0f, 1.0f, 0.0f),edge2);
	p0 = axis.Dot(v0);
	p1 = axis.Dot(v1);
	pMin = MIN(p0, p1);
	pMax = MAX(p0, p1);
	axisRadius = radius.Dot(Point(fabsf(axis.x), fabsf(axis.y), fabsf(axis.z)));
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
	if((pMin > axisRadius) || (pMax < -axisRadius))
		return FALSE;


	// axis == [0, 0, 1] x e1 == [-e1.y, e1.x, 0]

	axis.Cross(Point(0.0f, 0.0f, 1.0f),edge1);
	p0 = axis.Dot(v0);
	p1 = axis.Dot(v1);
	pMin = MIN(p0, p1);
	pMax = MAX(p0, p1);
	axisRadius = radius.Dot(Point(fabsf(axis.x), fabsf(axis.y), fabsf(axis.z)));
	if((pMin > axisRadius) || (pMax < -axisRadius))
		return FALSE;

	// axis == [0, 0, 1] x e2 == [-e2.y, e2.x, 0]

	axis.Cross(Point(0.0f, 0.0f, 1.0f),edge2);
	p0 = axis.Dot(v0);
	p1 = axis.Dot(v1);
	pMin = MIN(p0, p1);
	pMax = MAX(p0, p1);
	axisRadius = radius.Dot(Point(fabsf(axis.x), fabsf(axis.y), fabsf(axis.z)));
	if((pMin > axisRadius) || (pMax < -axisRadius))
		return FALSE;


	return TRUE;
}

#else
inline_ BOOL LSSCollider::LooseLSSTriOverlap(const Point& vert0, const Point& vert1, const Point& vert2)
{
	// Stats
	mNbVolumePrimTests++;

	//transform obb to origin
	OpcSimd::Vector4 v0 = OpcSimd::load(vert0);
	OpcSimd::Vector4 v1 = OpcSimd::load(vert1);
	OpcSimd::Vector4 v2 = OpcSimd::load(vert2);

	OpcSimd::Vector4 rot_0 = OpcSimd::load(mOBB.mRot[0]);
	OpcSimd::Vector4 rot_1 = OpcSimd::load(mOBB.mRot[1]);
	OpcSimd::Vector4 rot_2 = OpcSimd::load(mOBB.mRot[2]);

	OpcSimd::Vector4 center = OpcSimd::load(mOBB.mCenter);

	v0 = OpcSimd::rotateInv(rot_0, rot_1, rot_2, OpcSimd::subtract(v0, center));
	v1 = OpcSimd::rotateInv(rot_0, rot_1, rot_2, OpcSimd::subtract(v1, center));
	v2 = OpcSimd::rotateInv(rot_0, rot_1, rot_2, OpcSimd::subtract(v2, center));

	OpcSimd::Vector4 signMask = OpcSimd::signMask();
	OpcSimd::Vector4 zero = OpcSimd::zero();

	OpcSimd::Vector4 extents = OpcSimd::load(mOBB.mExtents);
	OpcSimd::Vector4 minusExtents = OpcSimd::xor4(signMask, extents);

// now do a tri-AABB test, with AABB at origin.

// Test triangle AABB
	OpcSimd::Vector4 triMin = OpcSimd::min4(v0, OpcSimd::min4(v1, v2));
	OpcSimd::Vector4 triMax = OpcSimd::max4(v0, OpcSimd::max4(v1, v2));

	OpcSimd::Vector4 mask = OpcSimd::or4(OpcSimd::greater(triMin, extents), OpcSimd::greater(minusExtents, triMax));
	if(OpcSimd::intNotEqualBool(mask, zero))
		return FALSE;

	// test tri plane.
	OpcSimd::Vector4 normal = OpcSimd::cross(OpcSimd::subtract(v1, v0), OpcSimd::subtract(v2, v0));
	OpcSimd::Vector4 dist = OpcSimd::dot(normal, v0);

	
	OpcSimd::Vector4 vMask = OpcSimd::greater(normal, zero);
	OpcSimd::Vector4 vMin = OpcSimd::select(extents, minusExtents, vMask);
	OpcSimd::Vector4 vMax = OpcSimd::select(minusExtents, extents, vMask);

	OpcSimd::Vector4 minDist = OpcSimd::dot(vMin, normal);
	OpcSimd::Vector4 maxDist = OpcSimd::dot(vMax, normal);

	mask = OpcSimd::or4(OpcSimd::greater(minDist, dist), OpcSimd::less(maxDist, dist));
	if(OpcSimd::intNotEqualBool(mask, zero))
		return FALSE;

	// Test edge axes.
//////// test edge axis
	//transform the triangle.

	OpcSimd::Vector4 edge0 = OpcSimd::subtract(v1, v0);
	OpcSimd::Vector4 edge1 = OpcSimd::subtract(v2, v1);
	OpcSimd::Vector4 edge2 = OpcSimd::subtract(v0, v2);

	// zero out w
	edge0 = OpcSimd::and4(edge0, OpcSimd::xyzMask());
	edge1 = OpcSimd::and4(edge1, OpcSimd::xyzMask());
	edge2 = OpcSimd::and4(edge2, OpcSimd::xyzMask());
	
	OpcSimd::Vector4 minusEdge0 = OpcSimd::xor4(signMask, edge0);
	OpcSimd::Vector4 minusEdge1 = OpcSimd::xor4(signMask, edge1);
	OpcSimd::Vector4 minusEdge2 = OpcSimd::xor4(signMask, edge2);

	OpcSimd::Vector4 radius = extents;
	OpcSimd::Vector4 minusRadius = OpcSimd::xor4(signMask, radius);


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
	minusAxisRadius = OpcSimd::xor4(signMask, axisRadius);
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
	minusAxisRadius = OpcSimd::xor4(signMask, axisRadius);
	if(OpcSimd::greaterXBool(pMin, axisRadius) || OpcSimd::lessXBool(pMax, minusAxisRadius))
		return FALSE;


	// axis == [1,0,0] x e2 == [0, -e2.z, e2.y]
	axis = OpcSimd::permute(edge2, minusEdge2, PERMUTE_0W1Z0Y0W);
	p0 = OpcSimd::dot(v0, axis);
	p1 = OpcSimd::dot(v1, axis);
	pMin = OpcSimd::min4(p0, p1);
	pMax = OpcSimd::max4(p0, p1);
	axisRadius = OpcSimd::dot(radius, OpcSimd::abs(axis));
	minusAxisRadius = OpcSimd::xor4(signMask, axisRadius);
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
	minusAxisRadius = OpcSimd::xor4(signMask, axisRadius);
	if(OpcSimd::greaterXBool(pMin, axisRadius) || OpcSimd::lessXBool(pMax, minusAxisRadius))
		return FALSE;

	// axis == [0,1,0] x e1 == [e1.z, 0, -e1.x]
	axis = OpcSimd::permute(edge1, minusEdge1, PERMUTE_0Z0W1X0W);
	p0 = OpcSimd::dot(v0, axis);
	p1 = OpcSimd::dot(v1, axis);
	pMin = OpcSimd::min4(p0, p1);
	pMax = OpcSimd::max4(p0, p1);
	axisRadius = OpcSimd::dot(radius, OpcSimd::abs(axis));
	minusAxisRadius = OpcSimd::xor4(signMask, axisRadius);
	if(OpcSimd::greaterXBool(pMin, axisRadius) || OpcSimd::lessXBool(pMax, minusAxisRadius))
		return FALSE;

	// axis == [0, 1, 0] x e2 == [e2.z, 0, -e2.x]
	axis = OpcSimd::permute(edge2, minusEdge2, PERMUTE_0Z0W1X0W);
	p0 = OpcSimd::dot(v0, axis);
	p1 = OpcSimd::dot(v1, axis);
	pMin = OpcSimd::min4(p0, p1);
	pMax = OpcSimd::max4(p0, p1);
	axisRadius = OpcSimd::dot(radius, OpcSimd::abs(axis));
	minusAxisRadius = OpcSimd::xor4(signMask, axisRadius);
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
	minusAxisRadius = OpcSimd::xor4(signMask, axisRadius);
	if(OpcSimd::greaterXBool(pMin, axisRadius) || OpcSimd::lessXBool(pMax, minusAxisRadius))
		return FALSE;

	// axis == [0, 0, 1] x e1 == [-e1.y, e1.x, 0]
	axis = OpcSimd::permute(edge1, minusEdge1, PERMUTE_1Y0X0W0W);
	p0 = OpcSimd::dot(v0, axis);
	p1 = OpcSimd::dot(v1, axis);
	pMin = OpcSimd::min4(p0, p1);
	pMax = OpcSimd::max4(p0, p1);
	axisRadius = OpcSimd::dot(radius, OpcSimd::abs(axis));
	minusAxisRadius = OpcSimd::xor4(signMask, axisRadius);
	if(OpcSimd::greaterXBool(pMin, axisRadius) || OpcSimd::lessXBool(pMax, minusAxisRadius))
		return FALSE;

	// axis == [0, 0, 1] x e2 == [-e2.y, e2.x, 0]
	axis = OpcSimd::permute(edge2, minusEdge2, PERMUTE_1Y0X0W0W);
	p0 = OpcSimd::dot(v0, axis);
	p1 = OpcSimd::dot(v1, axis);
	pMin = OpcSimd::min4(p0, p1);
	pMax = OpcSimd::max4(p0, p1);
	axisRadius = OpcSimd::dot(radius, OpcSimd::abs(axis));
	minusAxisRadius = OpcSimd::xor4(signMask, axisRadius);
	if(OpcSimd::greaterXBool(pMin, axisRadius) || OpcSimd::lessXBool(pMax, minusAxisRadius))
		return FALSE;

	return TRUE;
}

#endif


#ifndef PXC_VECTORHELPER_H
#define PXC_VECTORHELPER_H

#include "PxcVector.h"
#include "PxcMatrix33.h"
#include "PxcMath.h"

/*!\file
Helper-functions related to vector and vector operations
*/

//! Compute point as combination of barycentric coordinates
PXD_INLINE PxcVector PxcCombineBarycentric(const PxcVector& p0, const PxcVector& p1, const PxcVector& p2,
										   PxFloat u, PxFloat v)
{
	return (1.0f - u - v)*p0 + u*p1 + v*p2;
}

//! Compute two arbitrary tangent vectors given a normal vector
PXD_INLINE void PxcNormalToTangents(const PxcVector& n, PxcVector& t1, PxcVector& t2)
{
#if defined(PXD_PLATFORM_WII) || defined(PXD_PLATFORM_WIIU)
	if(PxcAbs(n.z) > 0.70710678f) //1/sqrt(2)
	{
		t1.x = 0;		t1.y = -n.z;			t1.z = n.y;
		t2.x = n.y*n.y+n.z*n.z;		t2.y = -n.x*n.y;		t2.z = -n.x*n.z;
	}
	else
	{
		t1.x = -n.y;		t1.y = n.x;			t1.z = 0;
		t2.x = -n.x*n.z;	t2.y = -n.y*n.z;		t2.z = n.y*n.y+n.z*n.z;
	}
	t1.normalize();
	t2.normalize();

#else
	if(PxcAbs(n.z) > 0.70710678f) //1/sqrt(2)
	{
		PxFloat a = n.y*n.y + n.z*n.z;
		PxFloat k = 1.0f / PxcSqrt(a);
		t1.x = 0;		t1.y = -n.z*k;			t1.z = n.y*k;
		t2.x = a*k;		t2.y = -n.x*t1.z;		t2.z = n.x*t1.y;
	}
	else
	{
		PxFloat a = n.x*n.x + n.y*n.y;
		PxFloat k = 1.0f / PxcSqrt(a);
		t1.x = -n.y*k;		t1.y = n.x*k;			t1.z = 0;
		t2.x = -n.z*t1.y;	t2.y = n.z*t1.x;		t2.z = a*k;
	}
	t1 *= 1.0f/(t1.magnitude());
	t2 *= 1.0f/(t2.magnitude());
#endif	
}

//!Given a vector, compute a 3x3 star matrix. Equivalent to v'=S(x)*v <=> v' = x.cross(v)
PXD_INLINE PxcMatrix33 PxcComputeStarMatrix(const PxcVector& v)
{
	PxcMatrix33 result;

	result[0][0] = 0.0f;	result[0][1] =  v.z;	result[0][2] = -v.y; 
	result[1][0] = -v.z;	result[1][1] = 0.0f;	result[1][2] = v.x;  
	result[2][0] = v.y;		result[2][1] = -v.x;	result[2][2] = 0.0f;

	return result;
}

/*!
Compute a rotation matrix that rotates vector "from" into vector "to".

Written by Tomas Möller, John Hughes 1999,
Adapted to PhysX by Adam M, to Pxc by Marten S
*/
PXD_INLINE PxcMatrix33 PxcFindRotationMatrix(const PxcVector& from, const PxcVector& to)
{
	PxcMatrix33 result;

	PxcVector v = from.cross(to);
	PxFloat e = from.dot(to);
	PxFloat f = PxcAbs(e);

	if(f > 0.9999f)
	{
		//Vectors almost parallel
		PxcVector u, v;		//temporary
		PxcVector absFrom;		//vector mostly orthogonal to from
		PxFloat c1, c2, c3; //constants used later
		PxUInt i, j;

		absFrom.x = PxcAbs(from.x);
		absFrom.y = PxcAbs(from.y);
		absFrom.z = PxcAbs(from.z);

		if(absFrom.x < absFrom.y)
		{
			if(absFrom.x < absFrom.z)
			{
				absFrom.x = 1.0f;
				absFrom.y = 0.0f;
				absFrom.z = 0.0f;
			}
			else
			{
				absFrom.x = 0.0f;
				absFrom.y = 0.0f;
				absFrom.z = 1.0f;
			}
		}
		else
		{
			if(absFrom.y < absFrom.z)
			{
				absFrom.x = 0.0f;
				absFrom.y = 1.0f;
				absFrom.z = 0.0f;
			}
			else
			{
				absFrom.x = 0.0f;
				absFrom.y = 0.0f;
				absFrom.z = 1.0f;
			}
		}


		u.x = absFrom.x - from.x; u.y = absFrom.y - from.y; u.z = absFrom.z - from.z;
		v.x = absFrom.x - to.x; v.y = absFrom.y - to.y; v.z = absFrom.z - to.z;

		c1 = 2.0f / u.dot(u);
		c2 = 2.0f / v.dot(v);
		c3 = c1 * c2 * u.dot(v);

		for(i = 0; i < 3; i++)
		{
			for(j = 0; j < 3; j++)
			{
				result(i,j) = - c1*u[i]*u[j] - c2*v[i]*v[j] + c3*v[i]*u[j];
			}
			result(i,i) += 1.0f;
		}
	}
	else
	{
		// Normal case, to and from are not parallel or anti-parallel
		PxFloat h, hvx, hvz, hvxy, hvxz, hvyz;
		h = 1.0f/(1.0f + e); /* optimization by Gottfried Chen */
		hvx = h * v.x;
		hvz = h * v.z;
		hvxy = hvx * v.y;
		hvxz = hvx * v.z;
		hvyz = hvz * v.y;

		result(0,0) = e + hvx*v.x;
		result(0,1) = hvxy - v.z;
		result(0,2) = hvxz + v.y;

		result(1,0) = hvxy + v.z;
		result(1,1) = e + h*v.y*v.y;
		result(1,2) = hvyz - v.x;

		result(2,0) = hvxz - v.y;
		result(2,1) = hvyz + v.x;
		result(2,2) = e + hvz*v.z;
	}

	return result;
}

#endif

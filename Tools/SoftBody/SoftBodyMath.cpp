#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "SoftBodyMath.h"

namespace SOFTBODY
{

/* a = b - c */
#define vectorMacro(a,b,c) \
	(a)[0] = (b)[0] - (c)[0];	\
	(a)[1] = (b)[1] - (c)[1];	\
	(a)[2] = (b)[2] - (c)[2];



#define innerProduct(v,q) \
		((v)[0] * (q)[0] + \
		(v)[1] * (q)[1] + \
		(v)[2] * (q)[2])

#define crossProduct(a,b,c) \
	(a)[0] = (b)[1] * (c)[2] - (c)[1] * (b)[2]; \
	(a)[1] = (b)[2] * (c)[0] - (c)[2] * (b)[0]; \
	(a)[2] = (b)[0] * (c)[1] - (c)[0] * (b)[1];



bool SBM_rayIntersectsTriangle(const float *p,const float *d,const float *v0,const float *v1,const float *v2,float &t)
{

	float e1[3],e2[3],h[3],s[3],q[3];
	float a,f,u,v;

	vectorMacro(e1,v1,v0);
	vectorMacro(e2,v2,v0);
	crossProduct(h,d,e2);
	a = innerProduct(e1,h);

	if (a > -0.00001 && a < 0.00001)
		return(false);

	f = 1/a;
	vectorMacro(s,p,v0);
	u = f * (innerProduct(s,h));

	if (u < 0.0 || u > 1.0)
		return(false);

	crossProduct(q,s,e1);
	v = f * innerProduct(d,q);
	if (v < 0.0 || u + v > 1.0)
		return(false);
	// at this stage we can compute t to find out where
	// the intersection point is on the line
	t = f * innerProduct(e2,q);
	if (t > 0) // ray intersection
		return(true);
	else // this means that there is a line intersection
		 // but not a ray intersection
		 return (false);
}


/*
An Efficient Bounding Sphere
by Jack Ritter
from "Graphics Gems", Academic Press, 1990
*/

/* Routine to calculate tight bounding sphere over    */
/* a set of points in 3D */
/* This contains the routine find_bounding_sphere(), */
/* the struct definition, and the globals used for parameters. */
/* The abs() of all coordinates must be < BIGNUMBER */
/* Code written by Jack Ritter and Lyle Rains. */

#define BIGNUMBER 100000000.0  		/* hundred million */

static inline void Set(float *n,float x,float y,float z)
{
	n[0] = x;
	n[1] = y;
	n[2] = z;
};

static inline void Copy(float *dest,const float *source)
{
	dest[0] = source[0];
	dest[1] = source[1];
	dest[2] = source[2];
}

float SBM_computeBoundingSphere(unsigned int vcount,const float *points,float *center)
{

  float mRadius;
  float mRadius2;

	float xmin[3];
	float xmax[3];
	float ymin[3];
	float ymax[3];
	float zmin[3];
	float zmax[3];
	float dia1[3];
	float dia2[3];

  /* FIRST PASS: find 6 minima/maxima points */
  Set(xmin,BIGNUMBER,BIGNUMBER,BIGNUMBER);
  Set(xmax,-BIGNUMBER,-BIGNUMBER,-BIGNUMBER);
  Set(ymin,BIGNUMBER,BIGNUMBER,BIGNUMBER);
  Set(ymax,-BIGNUMBER,-BIGNUMBER,-BIGNUMBER);
  Set(zmin,BIGNUMBER,BIGNUMBER,BIGNUMBER);
  Set(zmax,-BIGNUMBER,-BIGNUMBER,-BIGNUMBER);

  for (unsigned i=0; i<vcount; i++)
	{
		const float *caller_p = &points[i*3];

   	if (caller_p[0]<xmin[0])
  	  Copy(xmin,caller_p); /* New xminimum point */
  	if (caller_p[0]>xmax[0])
  	  Copy(xmax,caller_p);
  	if (caller_p[1]<ymin[1])
  	  Copy(ymin,caller_p);
  	if (caller_p[1]>ymax[1])
  	  Copy(ymax,caller_p);
  	if (caller_p[2]<zmin[2])
  	  Copy(zmin,caller_p);
  	if (caller_p[2]>zmax[2])
  	  Copy(zmax,caller_p);
	}

  /* Set xspan = distance between the 2 points xmin & xmax (squared) */
  float dx = xmax[0] - xmin[0];
  float dy = xmax[1] - xmin[1];
  float dz = xmax[2] - xmin[2];
  float xspan = dx*dx + dy*dy + dz*dz;

  /* Same for y & z spans */
  dx = ymax[0] - ymin[0];
  dy = ymax[1] - ymin[1];
  dz = ymax[2] - ymin[2];
  float yspan = dx*dx + dy*dy + dz*dz;

  dx = zmax[0] - zmin[0];
  dy = zmax[1] - zmin[1];
  dz = zmax[2] - zmin[2];
  float zspan = dx*dx + dy*dy + dz*dz;

  /* Set points dia1 & dia2 to the maximally separated pair */
  Copy(dia1,xmin);
  Copy(dia2,xmax); /* assume xspan biggest */
  float maxspan = xspan;

  if (yspan>maxspan)
	{
	  maxspan = yspan;
  	Copy(dia1,ymin);
  	Copy(dia2,ymax);
	}

  if (zspan>maxspan)
	{
	  Copy(dia1,zmin);
	  Copy(dia2,zmax);
	}


  /* dia1,dia2 is a diameter of initial sphere */
  /* calc initial center */
  center[0] = (dia1[0]+dia2[0])*0.5f;
  center[1] = (dia1[1]+dia2[1])*0.5f;
  center[2] = (dia1[2]+dia2[2])*0.5f;

  /* calculate initial radius**2 and radius */

  dx = dia2[0]-center[0]; /* x component of radius vector */
  dy = dia2[1]-center[1]; /* y component of radius vector */
  dz = dia2[2]-center[2]; /* z component of radius vector */

  mRadius2 = dx*dx + dy*dy + dz*dz;
  mRadius = float(sqrt(mRadius2));

  /* SECOND PASS: increment current sphere */
  if ( 1 )
  {
	  for (unsigned i=0; i<vcount; i++)
		{
			const float *caller_p = &points[i*3];

  		dx = caller_p[0]-center[0];
		  dy = caller_p[1]-center[1];
  		dz = caller_p[2]-center[2];

		  float old_to_p_sq = dx*dx + dy*dy + dz*dz;

  		if (old_to_p_sq > mRadius2) 	/* do r**2 test first */
			{ 	/* this point is outside of current sphere */
	  		float old_to_p = float(sqrt(old_to_p_sq));
			  /* calc radius of new sphere */
  			mRadius = (mRadius + old_to_p) * 0.5f;
	  		mRadius2 = mRadius*mRadius; 	/* for next r**2 compare */
  			float old_to_new = old_to_p - mRadius;

	  		/* calc center of new sphere */

		  float recip = 1.0f /old_to_p;

  			float cx = (mRadius*center[0] + old_to_new*caller_p[0]) * recip;
	  		float cy = (mRadius*center[1] + old_to_new*caller_p[1]) * recip;
			  float cz = (mRadius*center[2] + old_to_new*caller_p[2]) * recip;

		  Set(center,cx,cy,cz);
			}
		}
  }

  return mRadius;
}



float SBM_computeSphereVolume(float r)
{
	return (4.0f*3.141592654f*r*r*r)/3.0f;  // 4/3 PI R cubed
}


// ray-sphere intersection test from Graphics Gems p.388
// **NOTE** There is a bug in this Graphics Gem.  If the origin
// of the ray is *inside* the sphere being tested, it reports the
// wrong intersection location.  This code has a fix for the bug.
bool  SBM_rayIntersectsSphere(const float *point,const float *direction,const float *_center,float radius,float &t)
{
  bool ret = false;

  NxVec3 rayOrigin(point);
  NxVec3 dir(direction);
  NxVec3 center(_center);

	// notation:
	// point E  = rayOrigin
	// point O  = sphere center

	NxVec3 EO = center - rayOrigin;
  NxVec3 V = dir;

  float dist2 = EO.x*EO.x + EO.y*EO.y + EO.z * EO.z;

  float r2 = radius*radius;

  // Bug Fix For Gem, if origin is *inside* the sphere, invert the
  // direction vector so that we get a valid intersection location.
  if ( dist2 < r2 )
    V*=-1;

	float v = EO.dot(V);

	float disc = r2 - (EO.magnitudeSquared() - v*v);

	if (disc > 0.0f)
	{
    t = sqrtf(disc);
		ret = true;
	}

	return ret;
}

}; // END OF SOFTBODY NAMESPACE

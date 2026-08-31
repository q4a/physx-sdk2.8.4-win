#ifndef NX_PHYSICS_PLUECKER
#define NX_PHYSICS_PLUECKER
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Nxp.h"
#include "NxPlane.h"
#include "FPU.h"

const NxReal gPlueckerEpsilon = 0.0001f;


class PlueckerLine
	{
	public:
		PlueckerLine(){} // Doesn't initialize anything.
	PlueckerLine(NxReal p0, NxReal p1, NxReal p2, NxReal p3, NxReal p4, NxReal p5);
	PlueckerLine(const NxVec3 & p0, const NxVec3 & p1); // Init with given two points.
	~PlueckerLine(){};

	NX_INLINE NxReal & operator[](int index);
	NX_INLINE NxReal operator[](int index) const;

	// Init the pluecker line with a line given by two points. The direction of the line is b-a.
	NX_INLINE void fromLine(const NxVec3 & a, const NxVec3 & b);

	// Init the pluecker line with a line given by l = s + t * dir.
	NX_INLINE void fromRay(const NxVec3 & s, const NxVec3 & dir);

	// Intersects the plane with a second plane and saves the resulting line in this.
	// The return value is NX_TRUE iff the result is valid (planes not parallel).
	NX_BOOL intersectPlanes(const NxPlane & plane1, const NxPlane & plane2);

	// Intersects the plane with a line and gives the resulting point.
	// The return value is NX_TRUE iff the result is valid (line and plane not parallel)
	NX_BOOL intersectWithPlane(const NxPlane & plane, NxVec3 & intersectionPoint) const;

	// Convert the pluecker line to a line given by two points.
	NX_BOOL toLine(NxVec3 & a, NxVec3 & b) const;

	// Convert the pluecker line to a line given by l = s + t * dir.
	NX_BOOL toRay(NxVec3 & s, NxVec3 & dir) const;

	NX_INLINE void getDirection(NxVec3 & dest) const;

	// Tests if all pluecker coordinates are zero.
	NX_INLINE NX_BOOL isZero() const;

	// Tests if all pluecker coordinates are closer to zero than epsilon.
	NX_INLINE NX_BOOL isZero(NxReal epsilon) const;

	// Sets coordinates close to zero to exactly zero.
	NX_INLINE void clean();

	// Tests if the pluecker coordinates represent a valid line.
	NX_INLINE NX_BOOL isValid() const;

	// Returns a value that describes the relative position of the line to a second line.
	// >0 : The first line turns positively (counter-clockwise) around the second line
	// =0 : The two lines intersect in a point
	// <0 : The first line turns negatively around the second line
	NX_INLINE NxReal getRelation(const PlueckerLine & line) const;

	// Checks if the line (which is directed) hits the triangle given by its three edges from the positive side.
	NX_INLINE NX_BOOL hits(const PlueckerLine & e0, const PlueckerLine & e1, const PlueckerLine & e2) const;
	NX_INLINE NX_BOOL hits(const PlueckerLine & e0, const PlueckerLine & e1, const PlueckerLine & e2, const NxU32 * signBits) const;

	private:
		NxReal p[6]; // The actual pluecker coordinates of the line in natural order.
	};
/*
class PlueckerPlane
{
public:
PlueckerPlane(){} // Doesn't initialize anything.
PlueckerPlane(NxReal p0, NxReal p1, NxReal p2, NxReal p3);
PlueckerPlane(const NxVec3 & p0, const NxVec3 & p1, const NxVec3 & p2); // Init with three points.
PlueckerPlane(const PlueckerPlane & plane, NxReal t); // Init as a parallel plane with distance t to the given plane.
~PlueckerPlane(){};

NX_INLINE NxReal & operator[](int index);
NX_INLINE NxReal operator[](int index) const;

// Multiply all coordinates with a given factor. This doesn't change the plane that is represented by the pluecker coordinates.
NX_INLINE PlueckerPlane & multiply(NxReal factor);

// Get the pluecker plane from a triangle given by three points.
void fromTriangle(const NxVec3 & a, const NxVec3 & b, const NxVec3 & c);

// Sets this plane parallel to the given plane and with distance t.
void fromParallelPlane(const PlueckerPlane & plane, NxReal t);

// Tests if all pluecker coordinates are zero.
NX_INLINE NX_BOOL isZero() const;

// Zero test with epsilon.
NX_INLINE NX_BOOL isZero(NxReal epsilon) const;

// Sets coordinates close to zero to exactly zero.
NX_INLINE void clean();

// Intersects the plane with a second plane and gives the resulting line.
// The return value is NX_TRUE iff the result is valid (planes not parallel).
NX_BOOL intersect(const PlueckerPlane & plane, PlueckerLine & result) const;

// Intersects the plane with a line and gives the resulting point.
// The return value is NX_TRUE iff the result is valid (line and plane not parallel)
NX_BOOL intersect(const PlueckerLine & line, NxVec3 & result) const;

// Returns a value that describes the relative position of the point to the plane.
// >0 : positive side
// =0 : on plane
// <0 : negative side
NX_INLINE NxReal getRelation(const NxVec3 & point) const;

// The length of the vector n = (b-a).cross(c-a), so the length of the unnormalized normal vector.
NX_INLINE NxReal magnitude() const;
NX_INLINE NxReal magnitudeSquared() const;

// Returns the distance between the point and the plane.
// If the point is on the positive side of the plane, the distance is positive,
// if the point is on the neagative side of the plane, the distance is NEGATIVE.
NX_INLINE NxReal getDistance(const NxVec3 & point) const;

// If the plane doesn't change, and you call getDistance often, you can use this funcion
// to compute the distance faster. You must have called prepareDistanceFast() once.
// Another possibility to rapidly get the distance is to normalize the PlueckerPlane as
// preprocessing and then call getRelation, which then returns the distance actually.
NX_INLINE NxReal getDistanceFast(const NxVec3 & point) const;
NX_INLINE void prepareDistanceFast();

// Returns NX_TRUE iff the two points are on the same side of the plane (plane inclusive).
NX_INLINE NX_BOOL onSameSide(const NxVec3 & a, const NxVec3 & b) const;

// Normalizes the plane, i.e. scales all coordinates such that getRelation returns the same
// as getDistance and getNormalDirection returns the same as getNormal. The plane doesn't
// change its pose.
NX_INLINE PlueckerPlane & normalize();

// Returns the normal of the plane
NX_INLINE void getNormal(NxVec3 & normal);

// Returns a vector that points into the same direction as the normal, but is not normalized.
NX_INLINE void getNormalDirection(NxVec3 & normal);

private:
NxReal p[4]; // The actual pluecker coordinates of the plane in natural order.
NxReal oneOverMagnitude; // Cache to accelerate the call of getDistanceFast().
// It is written by calling prepareDistanceFast().
};

*/
// Returns the way the two lines lie with respect to each other
enum LineRelation { IDENTICAL, INTERSECTING, PARALLEL, SKEW };
LineRelation lineRelation(const NxVec3 & p0, const NxVec3 & dir0, const NxVec3 & p1, const NxVec3 & dir1);

// Returns NX_TRUE iff the point lies on the line segment ab.
NX_BOOL pointOnSegment(const NxVec3 & point, const NxVec3 & a, const NxVec3 & b);

// Calculates the intersection point between two intersecting lines.
void intersectLineLine(const NxVec3 & p0, const NxVec3 & dir0, const NxVec3 & p1, const NxVec3 & dir1, NxVec3 & ip);

// Calculates the intersection points of a line and a sphere. The return value is the number of solutions.
NxU32 intersectLineSphere(const NxVec3 & a, const NxVec3 & dir, const NxVec3 & center, NxReal radius, NxVec3 & ip0, NxVec3 & ip1);

// Solves the quadratic equation a * t^2 + b * t + c = 0 and saves the result(s) in t0 and t1.
// The return value is the number of solutions.
NX_INLINE NxU32 solveQuadrEq(NxReal a, NxReal b, NxReal c, NxReal & t0, NxReal & t1);

// Computes the angle between two points on a circle with a given radius and having a distance of dist between them.
// It returns NX_FALSE, if the distance is greater than the circle diameter.
NX_INLINE NX_BOOL getAngleFromDistance(NxReal radius, NxReal dist, NxReal & angle)
	{
	radius *= NxReal(2.0);
	if (dist > radius) // The two points can't lie on the same circle, perhaps there was a rounding error.
		{
		angle = NxPiF32;
		return NX_FALSE;
		}
	else
		{
		angle = NxReal(2.0) * NxMath::asin(dist / radius);
		return NX_TRUE;
		}
	}

NX_INLINE PlueckerLine::PlueckerLine(NxReal p0, NxReal p1, NxReal p2, NxReal p3, NxReal p4, NxReal p5)
	{
	p[0] = p0;
	p[1] = p1;
	p[2] = p2;
	p[3] = p3;
	p[4] = p4;
	p[5] = p5;
	}

NX_INLINE PlueckerLine::PlueckerLine(const NxVec3 & p0, const NxVec3 & p1)
	{
	fromLine(p0, p1);
	}

NX_INLINE NxReal & PlueckerLine::operator[](int index)
	{
	return p[index]; 
	}

NX_INLINE NxReal PlueckerLine::operator[](int index) const
	{
	return p[index];
	}

NX_INLINE void PlueckerLine::fromLine(const NxVec3 & a, const NxVec3 & b)
	{
	p[0] = b.x - a.x;
	p[1] = b.y - a.y;
	p[2] = a.x * b.y - b.x * a.y;
	p[3] = b.z - a.z;
	p[4] = a.x * b.z - b.x * a.z;
	p[5] = a.y * b.z - b.y * a.z;
	}

NX_INLINE void PlueckerLine::fromRay(const NxVec3 & s, const NxVec3 & dir)
	{
	p[0] = dir.x;
	p[1] = dir.y;
	p[2] = s.x * dir.y - dir.x * s.y;
	p[3] = dir.z;
	p[4] = s.x * dir.z - dir.x * s.z;
	p[5] = s.y * dir.z - dir.y * s.z;
	}

NX_INLINE NX_BOOL PlueckerLine::isZero() const
	{
	return (p[0] == NxReal(0.0)) && (p[1] == NxReal(0.0)) && (p[2] == NxReal(0.0)) &&
		(p[3] == NxReal(0.0)) && (p[4] == NxReal(0.0)) && (p[5] == NxReal(0.0));
	}

NX_INLINE NX_BOOL PlueckerLine::isZero(NxReal epsilon) const
	{
	return (NxMath::abs(p[0]) < epsilon) &&
		(NxMath::abs(p[1]) < epsilon) &&
		(NxMath::abs(p[2]) < epsilon) &&
		(NxMath::abs(p[3]) < epsilon) &&
		(NxMath::abs(p[4]) < epsilon) &&
		(NxMath::abs(p[5]) < epsilon);
	}

NX_INLINE void PlueckerLine::clean()
	{
	if (NxMath::equals(p[0], NxReal(0.0), gPlueckerEpsilon)) p[0] = NxReal(0.0);
	if (NxMath::equals(p[1], NxReal(0.0), gPlueckerEpsilon)) p[1] = NxReal(0.0);
	if (NxMath::equals(p[2], NxReal(0.0), gPlueckerEpsilon)) p[2] = NxReal(0.0);
	if (NxMath::equals(p[3], NxReal(0.0), gPlueckerEpsilon)) p[3] = NxReal(0.0);
	if (NxMath::equals(p[4], NxReal(0.0), gPlueckerEpsilon)) p[4] = NxReal(0.0);
	if (NxMath::equals(p[5], NxReal(0.0), gPlueckerEpsilon)) p[5] = NxReal(0.0);
	}

NX_INLINE NX_BOOL PlueckerLine::isValid() const
	{
	return p[0] * p[5] - p[1] * p[4] + p[2] * p[3] == NxReal(0.0);
	}

NX_INLINE NxReal PlueckerLine::getRelation(const PlueckerLine & line) const
	{
	return p[0] * line[5] - p[1] * line[4] + p[2] * line[3] +
		p[3] * line[2] - p[4] * line[1] + p[5] * line[0];
	}

NX_INLINE NX_BOOL PlueckerLine::hits(const PlueckerLine & e0, const PlueckerLine & e1, const PlueckerLine & e2) const
	{
	return (getRelation(e0) <= NxReal(0.0)) &&
		(getRelation(e1) <= NxReal(0.0)) &&
		(getRelation(e2) <= NxReal(0.0));
	}

NX_INLINE NX_BOOL PlueckerLine::hits(const PlueckerLine & e0, const PlueckerLine & e1, const PlueckerLine & e2, const NxU32 * signBits) const
	{
	NxReal u = getRelation(e0);
	NxReal v = getRelation(e1);
	NxReal w = getRelation(e2);

/*
	NxU32 a = (NX_IR(u) >> 31);
	NxU32 b = (NX_IR(v) >> 31);
	NxU32 c = (NX_IR(w) >> 31);

	NX_BOOL y = (a ^ (signBits[0] >> 31)) &&	(b ^ (signBits[1] >> 31)) &&	(c ^ (signBits[2] >> 31));
*/
	//AM: Shiftless version -- assumes that float and int sign bit are in the same place:
	NxU32 a = (NX_IR(u));
	NxU32 b = (NX_IR(v));
	NxU32 c = (NX_IR(w));

	NX_BOOL y = ((a ^ (signBits[0])) &	(b ^ (signBits[1])) &	(c ^ (signBits[2]))) >> 31;
	return y;
	
	}

NX_INLINE NxU32 solveQuadrEq(NxReal a, NxReal b, NxReal c, NxReal & t0, NxReal & t1)
	{
	if (a != NxReal(0.0)) // nulldivision (the quadratic equation is actually a linear equation)
		{
		//Try to get a bit better precision by utilizing the fact that
		//x = -b +- sqrt(b^2 -4ac) / 2a = 2c / (-b -+ sqrt(b^2 -4ac)
		//and also that x2 = c/a*x1

		NxReal d = b*b - NxReal(4.0) * a * c;	//potential catastrophic cancelation here on subtract of near equal values
		if (d < NxReal(0.0)) 
			{
			return 0; // no real solution	
			}
		else if (d != NxReal(0.0)) // two real solutions
			{
			d = NxMath::sqrt(d);
			if (b < NxReal(0.0))
				{
				t1 = (2*c) / -(b + d);				
				}
			else
				{
				t1 = (-b + d) / (2*a);				
				}
			t0 = c / (a*t1);
			return 2;
			}
		else // one solution
			{
			t0 = NxReal(0.5) * -b / a;
			return 1;
			}
		}
	else
		{
		if (b == NxReal(0.0)) 
			{
			return 0; // t is not defined
			}
		else // linear equation
			{
			t0 = -c / b;
			return 1;
			}
		}
	}

NX_INLINE void PlueckerLine::getDirection(NxVec3 & dest) const
	{
	dest.x = p[0];
	dest.y = p[1];
	dest.z = p[3];
	}



#define IDPLUCK_0(x) ((x)[2])
#define IDPLUCK_1(x) ((x)[4])
#define IDPLUCK_2(x) (-(x)[0])
#define IDPLUCK_3(x) ((x)[5])
#define IDPLUCK_4(x) (-(x)[3])
#define IDPLUCK_5(x) ((x)[1])

#define WIDPLUCK_0(x,v) ((x)[2] = (v))
#define WIDPLUCK_1(x,v) ((x)[4] = (v))
#define WIDPLUCK_2(x,v) ((x)[0] = (-v))
#define WIDPLUCK_3(x,v) ((x)[5] = (v))
#define WIDPLUCK_4(x,v) ((x)[3] = (-v))
#define WIDPLUCK_5(x,v) ((x)[1] = (v))

#endif // NX_PHYSICS_PLUECKER


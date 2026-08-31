#ifndef NX_PHYSICS_INTERVAL
#define NX_PHYSICS_INTERVAL
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Physics.h"

//#define INTERVAL_USE_FCOMI

// From Redon's thesis:
// [a,b] + [c,d] = [a+c, b+d]
// [a,b] - [c,d] = [a-d, b-c]
// [a,b] * [c,d] = [min(ac, ad, bc, bd), max(ac, ad, bc, bd)]
// 1 / [a,b] = [1/b, 1/a] if a>0 or b<0
// [a,b] / [c,d] = [a,b] * (1, [c,d]) if c>0 or d<0
// [a,b] <= [c,d] if b<=c

class Interval
	{
	public:
	NX_INLINE					Interval()										{}
	NX_INLINE					Interval(const Interval& it) : a(it.a), b(it.b)	{}
	NX_INLINE					Interval(NxReal f) : a(f), b(f)					{}
	NX_INLINE					Interval(NxReal _a, NxReal _b) : a(_a), b(_b)	{}
	NX_INLINE					~Interval()										{}

	NX_INLINE	NxReal			Width()								const		{ return b - a;									}
	NX_INLINE	NxReal			MidPoint()							const		{ return (a+b)*0.5f;							}

	// Arithmetic operators

	//! Operator for Interval Plus = Interval + Interval.
	NX_INLINE	Interval		operator+(const Interval& it)		const		{ return Interval(a + it.a, b + it.b);			}

	//! Operator for Interval Plus = Interval + NxReal.
	NX_INLINE	Interval		operator+(NxReal f)					const		{ return Interval(a + f, b + f);				}

	//! Operator for Interval Minus = Interval - Interval.
	NX_INLINE	Interval		operator-(const Interval& it)		const		{ return Interval(a - it.b, b - it.a);			}

	//! Operator for Interval Minus = Interval - NxReal.
	NX_INLINE	Interval		operator-(NxReal f)					const		{ return Interval(a - f, b - f);				}

	//! Operator for Interval Mul = Interval * Interval.
	NX_INLINE	Interval		operator*(const Interval& it)		const
							{
								NxReal ac = a*it.a;
								NxReal ad = a*it.b;
								NxReal bc = b*it.a;
								NxReal bd = b*it.b;
#ifdef INTERVAL_USE_FCOMI
								NxReal Min = FCMin4(ac, ad, bc, bd);
								NxReal Max = FCMax4(ac, ad, bc, bd);
#else
								NxReal Min = ac;
								if(ad<Min)	Min = ad;
								if(bc<Min)	Min = bc;
								if(bd<Min)	Min = bd;

								NxReal Max = ac;
								if(ad>Max)	Max = ad;
								if(bc>Max)	Max = bc;
								if(bd>Max)	Max = bd;
#endif
								return Interval(Min, Max);
							}

	//! Operator for Interval Scale = Interval * NxReal.
	NX_INLINE	Interval		operator*(NxReal s)					const
							{
								NxReal Min = a*s;
								NxReal Max = b*s;
								if(Min>Max)	NX_Swap(Min,Max);

								return Interval(Min, Max);
							}

	//! Operator for Interval Scale = NxReal * Interval.
	NX_INLINE friend	Interval operator*(NxReal s, const Interval& it)
							{
								NxReal Min = it.a*s;
								NxReal Max = it.b*s;
								if(Min>Max)	NX_Swap(Min,Max);

								return Interval(Min, Max);
							}

	//! Operator for Interval Scale = NxReal / Interval.
	NX_INLINE	friend	Interval operator/(NxReal s, const Interval& it)			{ return Interval(s/it.b, s/it.a);					}

	//! Operator for NxVec3 Scale = NxVec3 / NxReal.
//		NX_INLINE	NxVec3			operator/(NxReal s)					const		{ s = 1.0f / s; return NxVec3(x * s, y * s, z * s);	}

	//! Operator for Interval Div = Interval / Interval.
	NX_INLINE	Interval		operator/(const Interval& it)		const		{ return (*this) * (1.0f / it);						}



	//! Operator for Interval += Interval.
	NX_INLINE	Interval&		operator+=(const Interval& it)					{ a += it.a; b += it.b;			return *this;		}

	//! Operator for Interval += NxReal.
	NX_INLINE	Interval&		operator+=(NxReal f)								{ a += f; b += f;				return *this;		}

	//! Operator for Interval -= Interval.
	NX_INLINE	Interval&		operator-=(const Interval& it)					{ a -= it.b; b -= it.a;			return *this;		}

	//! Operator for Interval -= NxReal.
	NX_INLINE	Interval&		operator-=(NxReal f)								{ a -= f; b -= f;				return *this;		}

	//! Operator for Interval *= Interval.
	NX_INLINE	Interval&		operator*=(const Interval& it)
							{
								NxReal ac = a*it.a;
								NxReal ad = a*it.b;
								NxReal bc = b*it.a;
								NxReal bd = b*it.b;
#ifdef INTERVAL_USE_FCOMI
								a = FCMin4(ac, ad, bc, bd);
								b = FCMax4(ac, ad, bc, bd);
#else
								a = ac;
								if(ad<a)	a = ad;
								if(bc<a)	a = bc;
								if(bd<a)	a = bd;

								b = ac;
								if(ad>b)	b = ad;
								if(bc>b)	b = bc;
								if(bd>b)	b = bd;
#endif
								return *this;
							}

	//! Operator for Interval /= Interval.
	NX_INLINE	Interval&		operator/=(const Interval& it)
							{
								*this *= 1.0f / it;
								return *this;
							}

	//! Operator for "Interval A = Interval B"
	NX_INLINE	void			operator = (const Interval& interval)
							{
								a = interval.a;
								b = interval.b;
							}

	// Logical operators

	//! Operator for "if(Interval<=Interval)"
	NX_INLINE	bool			operator<=(const Interval& it)		const		{ return b <= it.a;	}
	//! Operator for "if(Interval<Interval)"
	NX_INLINE	bool			operator<(const Interval& it)		const		{ return b < it.a;	}
	//! Operator for "if(Interval>=Interval)"
	NX_INLINE	bool			operator>=(const Interval& it)		const		{ return a >= it.b;	}
	//! Operator for "if(Interval>Interval)"
	NX_INLINE	bool			operator>(const Interval& it)		const		{ return a > it.b;	}

	NxReal	a, b;
	};

NX_INLINE	Interval		operator-(NxReal f, const Interval & i)	{ return Interval(f - i.b, f - i.a);				}


class IntervalPoint
	{
	public:

	NX_INLINE					IntervalPoint()		{}
	NX_INLINE					IntervalPoint(const IntervalPoint& p) : x(p.x), y(p.y), z(p.z)	{}
	NX_INLINE					IntervalPoint(const Interval& _x, const Interval& _y, const Interval& _z) : x(_x), y(_y), z(_z)	{}
	NX_INLINE					IntervalPoint(const NxVec3& p0, const NxVec3& p1)
							{
								x = Interval(p0.x, p1.x);	if(x.a>x.b)	NX_Swap(x.a, x.b);
								y = Interval(p0.y, p1.y);	if(y.a>y.b)	NX_Swap(y.a, y.b);
								z = Interval(p0.z, p1.z);	if(z.a>z.b)	NX_Swap(z.a, z.b);
							}
	NX_INLINE					~IntervalPoint()	{}

	//! Operator for IntervalPoint Plus = IntervalPoint + IntervalPoint.
	NX_INLINE	IntervalPoint	operator+(const IntervalPoint& p)	const	{ return IntervalPoint(x + p.x, y + p.y, z + p.z);	}

	//! Operator for IntervalPoint Minus = IntervalPoint - IntervalPoint.
	NX_INLINE	IntervalPoint	operator-(const IntervalPoint& p)	const	{ return IntervalPoint(x - p.x, y - p.y, z - p.z);	}

	//! Operator for IntervalPoint Minus = IntervalPoint - IntervalPoint.
	NX_INLINE	IntervalPoint	operator-(const NxVec3& p)	const	{ return IntervalPoint(x - p.x, y - p.y, z - p.z);	}

	//! Operator for Interval DotProd = IntervalPoint | IntervalPoint.
	NX_INLINE	Interval		operator|(const IntervalPoint& p)	const	{ return x*p.x + y*p.y + z*p.z;						}
	//! Operator for IntervalPoint VecProd = IntervalPoint ^ IntervalPoint.
	NX_INLINE	IntervalPoint	operator^(const IntervalPoint& p)	const
							{
								return IntervalPoint(
									y * p.z - z * p.y,
									z * p.x - x * p.z,
									x * p.y - y * p.x);
							}
	NX_INLINE	IntervalPoint	operator^(const NxVec3& p)	const
							{
								return IntervalPoint(
									y * p.z - z * p.y,
									z * p.x - x * p.z,
									x * p.y - y * p.x);
							}
	//! Computes square magnitude
	NX_INLINE	Interval		squareMagnitude()	const		{ return x*x + y*y + z*z;										}

	//! Operator for IntervalPoint *= Matrix4x4.
	NX_INLINE	IntervalPoint&	operator*=(const NxMat34& mat)
							{
							NxVec3 a = mat * NxVec3(x.a, y.a, z.a);
							NxVec3 b = mat * NxVec3(x.b, y.b, z.b);

							x.a = a.x;
							x.b = b.x;
							y.a = a.y;
							y.b = b.y;
							z.a = a.z;
							z.b = b.z;

/*
								Interval xp = x * mat.m[0][0] + y * mat.m[1][0] + z * mat.m[2][0] + mat.m[3][0];
								Interval yp = x * mat.m[0][1] + y * mat.m[1][1] + z * mat.m[2][1] + mat.m[3][1];
								Interval zp = x * mat.m[0][2] + y * mat.m[1][2] + z * mat.m[2][2] + mat.m[3][2];

								x = xp;
								y = yp;
								z = zp;
*/
								return *this;
							}

	//! Operator for "IntervalPoint A = IntervalPoint B"
	NX_INLINE	void			operator = (const IntervalPoint& interval)
							{
								x = interval.x;
								y = interval.y;
								z = interval.z;
							}

	NX_INLINE	void			set(const Interval& _x, const Interval& _y, const Interval& _z)
							{
								x = _x;
								y = _y;
								z = _z;
							}

	NX_INLINE	void			getMinPoint(NxVec3& min)
							{
								min.x = x.a;
								min.y = y.a;
								min.z = z.a;
							}

	NX_INLINE	void			getMaxPoint(NxVec3& max)
							{
								max.x = x.b;
								max.y = y.b;
								max.z = z.b;
							}

	NX_INLINE					operator	const	Interval*() const	{ return &x; }
	NX_INLINE					operator			Interval*()			{ return &x; }

			Interval		x,y,z;
	};

class IntervalMatrix3x3
	{
	public:
	//! Empty constructor
	NX_INLINE					IntervalMatrix3x3()									{}
	//! Constructor from 9 values
	NX_INLINE					IntervalMatrix3x3(Interval m00, Interval m01, Interval m02, Interval m10, Interval m11, Interval m12, Interval m20, Interval m21, Interval m22)
							{
								m[0][0] = m00;	m[0][1] = m01;	m[0][2] = m02;
								m[1][0] = m10;	m[1][1] = m11;	m[1][2] = m12;
								m[2][0] = m20;	m[2][1] = m21;	m[2][2] = m22;
							}
	//! Destructor
	NX_INLINE					~IntervalMatrix3x3()								{}

	NX_INLINE	const IntervalPoint&	operator[](int row)		const	{ return *(const IntervalPoint*)&m[row][0];	}
	NX_INLINE	IntervalPoint&			operator[](int row)				{ return *(IntervalPoint*)&m[row][0];		}

	Interval	m[3][3];
	};


#endif 

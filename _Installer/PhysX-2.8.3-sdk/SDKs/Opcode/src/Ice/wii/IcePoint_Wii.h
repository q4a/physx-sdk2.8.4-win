///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Optimized point3d for wii
 *	\file		IcePoint.h
 *	\date		December, 09, 2008
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef __ICEPOINT_WII_H__
#define __ICEPOINT_WII_H__
#if defined(__PPCGEKKO__)

#include <revolution\mtx.h>
	// Forward declarations
	class HPoint;
	class Plane;
	class Matrix3x3;
	class Matrix4x4;

	#define CROSS2D(a, b)	(a.x*b.y - b.x*a.y)

	const float EPSILON2 = 1.0e-20f;

	class ICEMATHS_API Point
	{
		public:

		//! Empty constructor
		inline_	Point(){}
		//! Constructor from a single float
// Removed since it introduced the nasty "Point T = *Matrix4x4.GetTrans();" bug.......
		//! Constructor from floats
		inline_	Point(float _x, float _y, float _z) : x(_x), y(_y), z(_z){}
		//! Constructor from array
		inline_	Point(const float f[3]) : x(f[X_]), y(f[Y_]), z(f[Z_]){}
		//! Copy constructor
		inline_	Point(const Point& p) : x(p.x), y(p.y), z(p.z){}
		//! Destructor
		inline_	~Point(){}

		//! Clears the vector
		inline_	Point&	Zero()	{ x = y = z = 0.0f; return *this; }

		//! + infinity
		inline_	Point&	SetPlusInfinity() { x =	y = z = MAX_FLOAT; return *this; }
		//! - infinity
		inline_	Point&	SetMinusInfinity() { x =	y = z = MIN_FLOAT; return *this; }

		//! Sets positive unit random vector
		Point&	PositiveUnitRandomVector();
		//! Sets unit random vector
		Point&	UnitRandomVector();

		//! Assignment from values
		inline_	Point&	Set(float _x, float _y, float _z) { x = _x; y  = _y; z  = _z; return *this; }
		//! Assignment from array
		inline_	Point&	Set(const float f[3]) { x = f[X_]; y = f[Y_]; z = f[Z_]; return *this; }
		//! Assignment from another point
		inline_	Point&	Set(const Point& src) { x = src.x; y = src.y; z = src.z; return *this; }
		//! Adds a vector
		inline_	Point&	Add(const Point& p) { VECAdd(&v, &p.v, &v); return *this; }
		//! Adds a vector
		inline_	Point&	Add(float _x, float _y, float _z)		
		{
			Point p(_x, _y, _z);
			return Add(p);
		}
		//! Adds a vector
		inline_	Point&	Add(const float f[3])		
		{
	#if X_==0 && Y_==1 && Z_==2  		
			register const Point* p = this;
			register const Point* p1 = &f[0];
			register __vec2x32float__ a, b, c;
			asm volatile
			{
				psq_l	a, 0(p), 0, 0
				psq_l	b, 0(p1), 0, 0
				ps_add c, a, b
				psq_l	a, 8(p), 1, 0
				psq_l	b, 8(p1), 1, 0
				psq_st	c, 0(p), 0, 0
				ps_add c, a, b
				psq_st	c, 8(p), 1, 0
			}
			return *this;
	#else
			return Add(f[X_], f[Y_], f[Z_]); 
	#endif		
		}
		//! Adds vectors
		inline_	Point&	Add(const Point& p, const Point& q) { VECAdd(&p.v, &q.v, &v);	return *this; }
		
		//! Subtracts a vector
		inline_	Point&	Sub(const Point& p) { VECSubtract(&v, &p.v, &v); return *this; }
		//! Subtracts a vector
		inline_	Point&	Sub(float _x, float _y, float _z)		
		{
			Point p(_x, _y, _z);
			return Sub(p);
		}
		//! Subtracts a vector
		inline_	Point&	Sub(const float f[3])
		{
	#if X_==0 && Y_==1 && Z_==2  		
			register const Point* p = this;
			register const Point* p1 = &f[0];
			register __vec2x32float__ a, b, c;
			asm volatile
			{
				psq_l	a, 0(p), 0, 0
				psq_l	b, 0(p1), 0, 0
				ps_sub c, a, b
				psq_l	a, 8(p), 1, 0
				psq_l	b, 8(p1), 1, 0
				psq_st	c, 0(p), 0, 0
				ps_sub c, a, b
				psq_st	c, 8(p), 1, 0
			}
			return *this;
	#else
			return Add(f[X_], f[Y_], f[Z_]); 
	#endif		
		}
		//! Subtracts vectors
		inline_	Point&	Sub(const Point& p, const Point& q)		{ VECSubtract(&p.v, &q.v, &v);		return *this;	}
		//! this = -this
		inline_	Point&	Neg()
		{
			register const Point*p = this;
			register __vec2x32float__ a, b;
			asm volatile
			{
				psq_l	a, 0(p), 0, 0
				psq_l	b, 8(p), 1, 0
				ps_neg	a, a
				ps_neg	b, b
				psq_st	a, 0(p), 0, 0
				psq_st	b, 8(p), 1, 0
			}
			return *this;
		}
		//! this = -a
		inline_	Point&	Neg(const Point& a)	
		{
			register const Point*p = &a;
			register const Point*q = this;
			register __vec2x32float__ va, vb;
			asm volatile
			{
				psq_l	va, 0(p), 0, 0
				psq_l	vb, 8(p), 1, 0
				ps_neg	va, va
				ps_neg	vb, vb
				psq_st	va, 0(q), 0, 0
				psq_st	vb, 8(q), 1, 0
			}
			return *this;
		}
		//! Multiplies by a scalar
		inline_	Point&	Mult(float s) { VECScale(&v, &v, s); return *this; }

		//! this = a * scalar
		inline_	Point&	Mult(const Point& a, float scalar) { VECScale(&a.v, &v, scalar); return *this; }
		
		//! this = a + b * scalar
		inline_	Point&	Mac(const Point& a, const Point& b, float scalar)
		{
			register const Point* p = &a;
			register const Point* q = &b;
			register const Point* pOut = this;
			register const float* ps = &scalar;
			register __vec2x32float__ va, vb, vc, vd;
			
			asm volatile
			{
				psq_l va, 0(p), 0, 0
				psq_l vb, 0(q), 0, 0
				psq_l vc, 0(ps), 1, 0
				ps_madds0 vd, vb, vc, va
				psq_st vd, 0(pOut), 0, 0
				
				psq_l va, 8(p), 0, 0
				psq_l vb, 8(q), 0, 0
				ps_madds0 vd, vb, vc, va
				psq_st vd, 8(pOut), 1, 0
				
			}
			return *this;
		}
		
		//! this = this + a * scalar
		inline_	Point&	Mac(const Point& a, float scalar)
		{
			register const Point* p = this;
			register const Point* q = &a;
			register const Point* pOut = this;
			register const float* ps = &scalar;
			register __vec2x32float__ va, vb, vc, vd;
			
			asm volatile
			{
				psq_l va, 0(p), 0, 0
				psq_l vb, 0(q), 0, 0
				psq_l vc, 0(ps), 1, 0
				ps_madds0 vd, vb, vc, va
				psq_st vd, 0(pOut), 0, 0
				
				psq_l va, 8(p), 0, 0
				psq_l vb, 8(q), 0, 0
				ps_madds0 vd, vb, vc, va
				psq_st vd, 8(pOut), 1, 0
				
			}
			return *this;
		}
		
		//! this = a - b * scalar
		inline_	Point&	Msc(const Point& a, const Point& b, float scalar)
		{
			register const Point* p = &a;
			register const Point* q = &b;
			register const Point* pOut = this;
			register const float* ps = &scalar;
			register __vec2x32float__ va, vb, vc, vd, ve;
			
			asm volatile
			{
				psq_l va, 0(p), 0, 0
				psq_l vb, 0(q), 0, 0
				psq_l vc, 0(ps), 1, 0
				ps_merge00 ve, vc, vc
				ps_nmsub vd, vb, ve, va
				psq_st vd, 0(pOut), 0, 0
				
				psq_l va, 8(p), 0, 0
				psq_l vb, 8(q), 0, 0
				ps_nmsub vd, vb, ve, va
				psq_st vd, 8(pOut), 1, 0
				
			}
			return *this;
		}

		//! this = this - a * scalar
		inline_	Point&	Msc(const Point& a, float scalar)
		{
			register const Point* p = this;
			register const Point* q = &a;
			register const Point* pOut = this;
			register const float* ps = &scalar;
			register __vec2x32float__ va, vb, vc, vd, ve;
			
			asm volatile
			{
				psq_l va, 0(p), 0, 0
				psq_l vb, 0(q), 0, 0
				psq_l vc, 0(ps), 1, 0
				ps_merge00 ve, vc, vc
				ps_nmsub vd, vb, ve, va
				psq_st vd, 0(pOut), 0, 0
				
				psq_l va, 8(p), 0, 0
				psq_l vb, 8(q), 0, 0
				ps_nmsub vd, vb, ve, va
				psq_st vd, 8(pOut), 1, 0
				
			}
			return *this;
		}
		
		//! this = a + b * scalarb + c * scalarc
		inline_	Point&	Mac2(const Point& a, const Point& b, float scalarb, const Point& c, float scalarc)
		{
			register const Point* pa = &a;
			register const Point* pb = &b;
			register const Point* pc = &c;
			register const Point* pOut = this;
			register const float* psb = &scalarb;
			register const float* psc = &scalarc;
			register __vec2x32float__ va, vb, vc, vd, ve;
			
			asm volatile
			{
				psq_l va, 0(pa), 0, 0
				psq_l vb, 0(pb), 0, 0
				psq_l vc, 0(psb), 1, 0
				ps_madds0 vd, vb, vc, va
				psq_l vb, 0(pc), 0, 0
				psq_l ve, 0(psc), 1, 0				
				ps_madds0 vd, vb, ve, vd
				psq_st vd, 0(pOut), 0, 0
				
				psq_l va, 8(pa), 0, 0
				psq_l vb, 8(pb), 0, 0
				ps_madds0 vd, vb, vc, va
				psq_l vb, 8(pc), 0, 0
				ps_madds0 vd, vb, ve, vd
				psq_st vd, 8(pOut), 1, 0
				
			}
			return *this;								
		}

		//! this = a - b * scalarb - c * scalarc
		inline_	Point&	Msc2(const Point& a, const Point& b, float scalarb, const Point& c, float scalarc)
		{
			register const Point* pa = &a;
			register const Point* pb = &b;
			register const Point* pc = &c;
			register const Point* pOut = this;
			register const float* psb = &scalarb;
			register const float* psc = &scalarc;
			register __vec2x32float__ va, vb, vc, vd, ve, vf;
			
			asm volatile
			{
				psq_l va, 0(pa), 0, 0
				psq_l vb, 0(pb), 0, 0
				psq_l vc, 0(psb), 1, 0
				ps_merge00 ve, vc, vc
				ps_nmsub vd, vb, ve, va
				psq_l vb, 0(pc), 0, 0
				psq_l vc, 0(psc), 1, 0
				ps_merge00 vf, vc, vc				
				ps_madds0 vd, vb, vf, vd
				psq_st vd, 0(pOut), 0, 0
				
				psq_l va, 8(pa), 0, 0
				psq_l vb, 8(pb), 0, 0
				psq_l vc, 0(psb), 1, 0
				ps_nmsub vd, vb, ve, va
				psq_l vb, 8(pc), 0, 0
				psq_l vc, 0(psc), 1, 0				
				ps_nmsub vd, vb, vf, vd
				psq_st vd, 8(pOut), 1, 0
				
			}
			return *this;								
		}
		
		//! Linear interpolate between two vectors: this = a + t * (b - a)
		inline_	Point&	Lerp(const Point& a, const Point& b, float t)
		{
			register const Point* pa = &a;
			register const Point* pb = &b;
			register const float* pt = &t;
			register const Point* pOut = this;
			register __vec2x32float__ va, vb, vc, vt;
			
			asm volatile
			{
				psq_l	va, 0(pa), 0, 0
				psq_l	vb, 0(pb), 0, 0
				psq_l	vt, 0(pt), 1, 0
				ps_sub	vb, vb, va
				ps_nmadd vc, va, vt, vb
				psq_st	vc, 0(pOut), 0, 0
				
				psq_l	va, 8(pa), 1, 0
				psq_l	vb, 8(pb), 1, 0
				ps_sub	vb, vb, va
				ps_nmadd vc, va, vt, vb
				psq_st	vc, 8(pOut), 1, 0				
			}
			return *this;
		}

		//! this = mat * a
		inline_	Point&			Mult(const Matrix3x3& mat, const Point& a);

		//! this = mat1 * a1 + mat2 * a2
		inline_	Point&			Mult2(const Matrix3x3& mat1, const Point& a1, const Matrix3x3& mat2, const Point& a2);

		//! this = this + mat * a
		inline_	Point&			Mac(const Matrix3x3& mat, const Point& a);

		//! this = transpose(mat) * a
		inline_	Point&			TransMult(const Matrix3x3& mat, const Point& a);



		//! Hermite interpolate between p1 and p2. p0 and p3 are used for finding gradient at p1 and p2.
		//! this =	p0 * (2t^2 - t^3 - t)/2
		//!			+ p1 * (3t^3 - 5t^2 + 2)/2
		//!			+ p2 * (4t^2 - 3t^3 + t)/2
		//!			+ p3 * (t^3 - t^2)/2
		inline_	Point&			Herp(const Point& p0, const Point& p1, const Point& p2, const Point& p3, float t)
								{
									float t2 = t * t;
									float t3 = t2 * t;
									float kp0 = (2.0f * t2 - t3 - t) * 0.5f;
									float kp1 = (3.0f * t3 - 5.0f * t2 + 2.0f) * 0.5f;
									float kp2 = (4.0f * t2 - 3.0f * t3 + t) * 0.5f;
									float kp3 = (t3 - t2) * 0.5f;
									x = p0.x * kp0 + p1.x * kp1 + p2.x * kp2 + p3.x * kp3;
									y = p0.y * kp0 + p1.y * kp1 + p2.y * kp2 + p3.y * kp3;
									z = p0.z * kp0 + p1.z * kp1 + p2.z * kp2 + p3.z * kp3;
									return *this;
								}

		//! this = rotpos * r + linpos
		inline_	Point&			Transform(const Point& r, const Matrix3x3& rotpos, const Point& linpos);

		//! this = trans(rotpos) * (r - linpos)
		inline_	Point&			InvTransform(const Point& r, const Matrix3x3& rotpos, const Point& linpos);

		//! Returns MIN(x, y, z);
		inline_	float			Min()				const		{ return MIN(x, MIN(y, z));												}
		//! Returns MAX(x, y, z);
		inline_	float			Max()				const		{ return MAX(x, MAX(y, z));												}
		//! Sets each element to be componentwise minimum
		inline_	Point&			Min(const Point& p)				{ x = MIN(x, p.x); y = MIN(y, p.y); z = MIN(z, p.z);	return *this;	}
		//! Sets each element to be componentwise maximum
		inline_	Point&			Max(const Point& p)				{ x = MAX(x, p.x); y = MAX(y, p.y); z = MAX(z, p.z);	return *this;	}

		//! Clamps each element
		inline_	Point&			Clamp(float min, float max)
								{
									if(x<min)	x=min;	if(x>max)	x=max;
									if(y<min)	y=min;	if(y>max)	y=max;
									if(z<min)	z=min;	if(z>max)	z=max;
									return *this;
								}
		//! Computes square magnitude
		inline_	float			SquareMagnitude()	const		{ return VECSquareMag(&v);												}
		//! Computes magnitude
		inline_	float			Magnitude()			const		{ return VECMag(&v);													}

		//! Computes volume
		inline_	float			Volume()			const		{ return x * y * z;														}

		//! Checks the point is near zero
		inline_	bool			ApproxZero()		const		{ return SquareMagnitude() < EPSILON2;									}

		//! Tests for exact zero vector - avoids slow FPU compares and handles -0.0 correctly
		inline_	BOOL			IsZero()			const
								{
									// Early exit version
									if((IR(x)& ~SIGN_BITMASK) || (IR(y)& ~SIGN_BITMASK) || (IR(z)& ~SIGN_BITMASK))	return FALSE;
									return TRUE;
								}

		//! Tests for non-zero vector - avoids slow FPU compares and handles -0.0 correctly
		inline_	BOOL			IsNonZero()			const
								{
									// Branchless version
									return (IR(x)|IR(y)|IR(z)) & ~SIGN_BITMASK;
								}

		//! Checks point validity
		inline_	BOOL			IsValid()			const
								{
									if(!IsValidFloat(x))	return FALSE;
									if(!IsValidFloat(y))	return FALSE;
									if(!IsValidFloat(z))	return FALSE;
									return TRUE;
								}

		//! Slighty moves the point
				void			Tweak(udword coord_mask, udword tweak_mask)
								{
									if(coord_mask&1)	{ udword Dummy = IR(x);	Dummy^=tweak_mask;	x = FR(Dummy); }
									if(coord_mask&2)	{ udword Dummy = IR(y);	Dummy^=tweak_mask;	y = FR(Dummy); }
									if(coord_mask&4)	{ udword Dummy = IR(z);	Dummy^=tweak_mask;	z = FR(Dummy); }
								}

		#define TWEAKMASK		0x3fffff
		#define TWEAKNOTMASK	~TWEAKMASK
		//! Slighty moves the point out
		inline_	void			TweakBigger()
								{
									udword	Dummy = (IR(x)&TWEAKNOTMASK);	if(!IS_NEGATIVE_FLOAT(x))	Dummy+=TWEAKMASK+1;	x = FR(Dummy);
											Dummy = (IR(y)&TWEAKNOTMASK);	if(!IS_NEGATIVE_FLOAT(y))	Dummy+=TWEAKMASK+1;	y = FR(Dummy);
											Dummy = (IR(z)&TWEAKNOTMASK);	if(!IS_NEGATIVE_FLOAT(z))	Dummy+=TWEAKMASK+1;	z = FR(Dummy);
								}

		//! Slighty moves the point in
		inline_	void			TweakSmaller()
								{
									udword	Dummy = (IR(x)&TWEAKNOTMASK);	if(IS_NEGATIVE_FLOAT(x))	Dummy+=TWEAKMASK+1;	x = FR(Dummy);
											Dummy = (IR(y)&TWEAKNOTMASK);	if(IS_NEGATIVE_FLOAT(y))	Dummy+=TWEAKMASK+1;	y = FR(Dummy);
											Dummy = (IR(z)&TWEAKNOTMASK);	if(IS_NEGATIVE_FLOAT(z))	Dummy+=TWEAKMASK+1;	z = FR(Dummy);
								}

		//! Normalizes the vector
		inline_	Point&			Normalize()
								{
									float M = Magnitude();
									if(M)
									{
										M = 1.0f / M;
										x *= M;
										y *= M;
										z *= M;
									}					
									return *this;
								}

		//! Sets vector length
		inline_	Point&			SetLength(float length)
								{
									float NewLength = length / Magnitude();
									x *= NewLength;
									y *= NewLength;
									z *= NewLength;
									return *this;
								}

		//! Clamps vector length
		inline_	Point&			ClampLength(float limit_length)
								{
									if(limit_length>=0.0f)	// Magnitude must be positive
									{
										float CurrentSquareLength = SquareMagnitude();

										if(CurrentSquareLength > limit_length * limit_length)
										{
											float Coeff = limit_length / sqrtf(CurrentSquareLength);
											x *= Coeff;
											y *= Coeff;
											z *= Coeff;
										}
									}
									return *this;
								}

		//! Computes distance to another point
		inline_	float			Distance(const Point& b)			const
								{
									return VECDistance(&v, &b.v);
								}

		//! Computes square distance to another point
		inline_	float			SquareDistance(const Point& b)		const
								{
									return VECSquareDistance(&v, &b.v);
								}

		//! Dot product dp = this|a
		inline_	float			Dot(const Point& p)					const
		{	
			return VECDotProduct(&v, &p.v);
		}

		//! Compares orientations (more readable, user-friendly function)
		inline_ bool			SameDirection(const Point& p)		const		{	return p.x*x + p.y*y + p.z*z >= 0.0f;		}

		//! Cross product this = a x b
		inline_	Point&			Cross(const Point& a, const Point& b)
								{
									Point temp;
									VECCrossProduct( &a.v, &b.v, &temp.v );
									Set(temp);
						
									return *this;
								}

		//! Vector code ( bitmask = sign(z) | sign(y) | sign(x) )
		inline_	udword			VectorCode()						const
								{
									return (IR(x)>>31) | ((IR(y)&SIGN_BITMASK)>>30) | ((IR(z)&SIGN_BITMASK)>>29);
								}

		//! Returns largest axis
		inline_	PointComponent	LargestAxis()						const
								{
									const float* Vals = &x;
									PointComponent m = X_;
									if(Vals[Y_] > Vals[m]) m = Y_;
									if(Vals[Z_] > Vals[m]) m = Z_;
									return m;
								}

		//! Returns closest axis
		inline_	PointComponent	ClosestAxis()						const
								{
									const float* Vals = &x;
									PointComponent m = X_;
									if(AIR(Vals[Y_]) > AIR(Vals[m])) m = Y_;
									if(AIR(Vals[Z_]) > AIR(Vals[m])) m = Z_;
									return m;
								}

		//! Returns smallest axis
		inline_	PointComponent	SmallestAxis()						const
								{
									const float* Vals = &x;
									PointComponent m = X_;
									if(Vals[Y_] < Vals[m]) m = Y_;
									if(Vals[Z_] < Vals[m]) m = Z_;
									return m;
								}

		//! Refracts the point
				Point&			Refract(const Point& eye, const Point& n, float refractindex, Point& refracted);

		//! Projects the point onto a plane
				Point&			ProjectToPlane(const Plane& p);

		//! Projects the point onto the screen
				void			ProjectToScreen(float halfrenderwidth, float halfrenderheight, const Matrix4x4& mat, HPoint& projected) const;

		//! Unfolds the point onto a plane according to edge(a,b)
				Point&			Unfold(Plane& p, Point& a, Point& b);

		//! Hash function from Ville Miettinen
		inline_	udword			GetHashValue()						const
								{
									const udword* h = (const udword*)(this);
									udword f = (h[0]+h[1]*11-(h[2]*17)) & 0x7fffffff;	// avoid problems with +-0
									return (f>>22)^(f>>12)^(f);
								}

		//! Stuff magic values in the point, marking it as explicitely not used.
				void			SetNotUsed();
		//! Checks the point is marked as not used
				BOOL			IsNotUsed()							const;

		// Arithmetic operators

		//! Unary operator for Point Negate = - Point
		inline_	Point			operator-()							const		{ return Point(-x, -y, -z);							}

		//! Operator for Point Plus = Point + Point.
		inline_	Point			operator+(const Point& p)			const		{ return Point(x + p.x, y + p.y, z + p.z);			}
		//! Operator for Point Minus = Point - Point.
		inline_	Point			operator-(const Point& p)			const		{ return Point(x - p.x, y - p.y, z - p.z);			}

		//! Operator for Point Mul   = Point * Point.
		inline_	Point			operator*(const Point& p)			const
		{
			Point ret;
			register const Point* pl = this;
			register const Point* pr = &p;
			register Point* pOut = &ret;
			register __vec2x32float__ a, b, c;
			
			asm volatile
			{
				psq_l	a, 0(pl), 0, 0
				psq_l	b, 0(pr), 0, 0
				psq_l	c, 8(pl), 1, 0
				ps_mul	b, a, b
				psq_l	a, 8(pr), 1, 0
				psq_st	b, 0(pOut), 0, 0
				ps_mul	b, a, c
				psq_st	b, 8(pOut), 1, 0						
			}
		 
		 	return ret;							
		 }
		//! Operator for Point Scale = Point * float.
		inline_	Point			operator*(float s)					const		
		{
			Point ret;
			VECScale(&v, &ret.v, s);
			return ret;
		}
		//! Operator for Point Scale = float * Point.
		inline_ friend	Point	operator*(float s, const Point& p)		
		{
			Point ret;
			VECScale(&p.v, &ret.v, s);
			return ret;
		}
		//! Operator for Point Div   = Point / Point.
		inline_	Point			operator/(const Point& p)			const		
		{
			Point ret;
			register const Point* pl = this;
			register const Point* pr = &p;
			register Point* pOut = &ret;
			register __vec2x32float__ a, b, c;
			
			asm volatile
			{
				psq_l	a, 0(pl), 0, 0
				psq_l	b, 0(pr), 0, 0
				psq_l	c, 8(pl), 1, 0
				ps_div	b, a, b
				psq_l	a, 8(pr), 1, 0
				psq_st	b, 0(pOut), 0, 0
				ps_div	b, c, a
				psq_st	b, 8(pOut), 1, 0						
			}
		 
		 	return ret;							
		 }
		//! Operator for Point Scale = Point / float.
		inline_	Point			operator/(float s)					const		
		{
			s = 1.0f/s;
			Point ret;
			VECScale(&v, &ret.v, s);			
			return ret;
		}
		
		//! Operator for Point Scale = float / Point.
		inline_	friend	Point	operator/(float s, const Point& p)						
		{
			Point ret;
			register const float* pf = &s;
			register const Point* pr = &p;
			register Point* pOut = &ret;
			register __vec2x32float__ a, b, c;
			
			asm volatile
			{
				psq_l	a, 0(pf), 1, 0
				ps_merge00 a, a, a
				psq_l	b, 0(pr), 0, 0
				ps_div	b, a, b
				psq_l	c, 8(pr), 1, 0
				psq_st	b, 0(pOut), 0, 0
				ps_div	b, a, c
				psq_st	b, 8(pOut), 1, 0						
			}
		 
		 	return ret;							
		 }

		//! Operator for float DotProd = Point | Point.
		inline_	float			operator|(const Point& p)			const
		{
			return VECDotProduct(&v, &p.v);
		}	
		//! Operator for Point VecProd = Point ^ Point.
		inline_	Point			operator^(const Point& p)			const
		{
			Point ret;
			VECCrossProduct( &v, &p.v, &ret.v);
			return ret;
		}

		//! Operator for Point += Point.
		inline_	Point&			operator+=(const Point& p)
		{
			VECAdd(&v, &p.v, &v);
			return *this;
		}
		
		//! Operator for Point -= Point.
		inline_	Point&			operator-=(const Point& p)
		{
			VECSubtract(&v, &p.v, &v);
			return *this;		
		}
		
		//! Operator for Point *= Point.
		inline_	Point&			operator*=(const Point& p)		
		{
			register const Point* pl = this;
			register const Point* pr = &p;
			register Point* pOut = this;
			register __vec2x32float__ a, b, c;
			
			asm volatile
			{
				psq_l	a, 0(pl), 0, 0
				psq_l	b, 0(pr), 0, 0
				psq_l	c, 8(pl), 1, 0
				ps_mul	b, a, b
				psq_l	a, 8(pr), 1, 0
				psq_st	b, 0(pOut), 0, 0
				ps_mul	b, a, c
				psq_st	b, 8(pOut), 1, 0						
			}		 
		 	return *this;							
		 }						
		 
		//! Operator for Point *= float.
		inline_	Point&			operator*=(float s)
		{
			VECScale(&v, &v, s);
			return *this;
		}

		//! Operator for Point /= Point.
		inline_	Point&			operator/=(const Point& p)
		{
			register const Point* pl = this;
			register const Point* pr = &p;
			register Point* pOut = this;
			register __vec2x32float__ a, b, c;
			
			asm volatile
			{
				psq_l	a, 0(pl), 0, 0
				psq_l	b, 0(pr), 0, 0
				psq_l	c, 8(pl), 1, 0
				ps_div	b, a, b
				psq_l	a, 8(pr), 1, 0
				psq_st	b, 0(pOut), 0, 0
				ps_div	b, c, a
				psq_st	b, 8(pOut), 1, 0						
			}
		 
		 	return *this;							
		 }
		 					
		//! Operator for Point /= float.
		inline_	Point&			operator/=(float s)
		{
			s = 1.0f/s;
			VECScale(&v, &v, s);			
			return *this;
		}								

		// Logical operators

		//! Operator for "if(Point==Point)"
		inline_	bool			operator==(const Point& p)			const		{ return ( (IR(x)==IR(p.x))&&(IR(y)==IR(p.y))&&(IR(z)==IR(p.z)));	}
		//! Operator for "if(Point!=Point)"
		inline_	bool			operator!=(const Point& p)			const		{ return ( (IR(x)!=IR(p.x))||(IR(y)!=IR(p.y))||(IR(z)!=IR(p.z)));	}

		// Arithmetic operators

		//! Operator for Point Mul = Point * Matrix3x3.
		inline_	Point			operator*(const Matrix3x3& mat)		const
								{
									class ShadowMatrix3x3{ public: float m[3][3]; };	// To allow inlining
									const ShadowMatrix3x3* Mat = (const ShadowMatrix3x3*)&mat;

									return Point(
									x * Mat->m[0][0] + y * Mat->m[1][0] + z * Mat->m[2][0],
									x * Mat->m[0][1] + y * Mat->m[1][1] + z * Mat->m[2][1],
									x * Mat->m[0][2] + y * Mat->m[1][2] + z * Mat->m[2][2] );
								}

		//! Operator for Point Mul = Point * Matrix4x4.
		inline_	Point			operator*(const Matrix4x4& mat)		const
								{
									class ShadowMatrix4x4{ public: float m[4][4]; };	// To allow inlining
									const ShadowMatrix4x4* Mat = (const ShadowMatrix4x4*)&mat;

									return Point(
									x * Mat->m[0][0] + y * Mat->m[1][0] + z * Mat->m[2][0] + Mat->m[3][0],
									x * Mat->m[0][1] + y * Mat->m[1][1] + z * Mat->m[2][1] + Mat->m[3][1],
									x * Mat->m[0][2] + y * Mat->m[1][2] + z * Mat->m[2][2] + Mat->m[3][2]);
								}

		//! Operator for Point *= Matrix3x3.
		inline_	Point&			operator*=(const Matrix3x3& mat)
								{
									class ShadowMatrix3x3{ public: float m[3][3]; };	// To allow inlining
									const ShadowMatrix3x3* Mat = (const ShadowMatrix3x3*)&mat;

									float xp = x * Mat->m[0][0] + y * Mat->m[1][0] + z * Mat->m[2][0];
									float yp = x * Mat->m[0][1] + y * Mat->m[1][1] + z * Mat->m[2][1];
									float zp = x * Mat->m[0][2] + y * Mat->m[1][2] + z * Mat->m[2][2];

									x = xp;	y = yp;	z = zp;

									return *this;
								}

		//! Operator for Point *= Matrix4x4.
		inline_	Point&			operator*=(const Matrix4x4& mat)
								{
									class ShadowMatrix4x4{ public: float m[4][4]; };	// To allow inlining
									const ShadowMatrix4x4* Mat = (const ShadowMatrix4x4*)&mat;

									float xp = x * Mat->m[0][0] + y * Mat->m[1][0] + z * Mat->m[2][0] + Mat->m[3][0];
									float yp = x * Mat->m[0][1] + y * Mat->m[1][1] + z * Mat->m[2][1] + Mat->m[3][1];
									float zp = x * Mat->m[0][2] + y * Mat->m[1][2] + z * Mat->m[2][2] + Mat->m[3][2];

									x = xp;	y = yp;	z = zp;

									return *this;
								}
		//Assignment operator

		inline Point&		operator=(const Point &p)
		{
			x = p.x; y = p.y; z = p.z;
			return *this;
		}

		// Cast operators

		//! Cast a Point to a HPoint. w is set to zero.
								operator	HPoint()				const;

		inline_					operator	const	float*() const	{ return &x; }
		inline_					operator			float*()		{ return &x; }

		public:

		union
		{
				Vec v;
				struct
				{
				float			x, y, z;
				};
		};				
	};

	FUNCTION ICEMATHS_API void Normalize1(Point& a);
	FUNCTION ICEMATHS_API void Normalize2(Point& a);

	// For CCT Big Worlds
	class ICEMATHS_API BigPoint
	{
		public:

		inline_					BigPoint()															{}
		inline_					BigPoint(BigType _x, BigType _y, BigType _z) : x(_x), y(_y), z(_z)	{}
		inline_					BigPoint(const BigType f[3]) : x(f[X_]), y(f[Y_]), z(f[Z_])			{}
		inline_					BigPoint(const BigPoint& p) : x(p.x), y(p.y), z(p.z)				{}
		inline_					~BigPoint()															{}

		inline_	BigPoint&		Zero()									{ x =			y =			z = 0.0;			return *this;	}

		inline_	BigPoint&		Min(const BigPoint& p)			{ x = MIN(x, p.x); y = MIN(y, p.y); z = MIN(z, p.z);	return *this;	}
		inline_	BigPoint&		Max(const BigPoint& p)			{ x = MAX(x, p.x); y = MAX(y, p.y); z = MAX(z, p.z);	return *this;	}
		inline_	BigType			Min()				const		{ return MIN(x, MIN(y, z));												}
		inline_	BigType			Max()				const		{ return MAX(x, MAX(y, z));												}

		inline_	BigPoint&		Neg()									{ x = -x;		y = -y;			z = -z;			return *this;	}
		inline_	BigPoint&		Neg(const BigPoint& a)					{ x = -a.x;		y = -a.y;		z = -a.z;		return *this;	}

		inline_	PointComponent	LargestAxis()						const
								{
									const BigType* Vals = &x;
									PointComponent m = X_;
									if(Vals[Y_] > Vals[m]) m = Y_;
									if(Vals[Z_] > Vals[m]) m = Z_;
									return m;
								}

		inline_	BigType			Distance(const BigPoint& b)			const
								{
									return sqrt((x - b.x)*(x - b.x) + (y - b.y)*(y - b.y) + (z - b.z)*(z - b.z));
								}

		inline_	BigType			SquareDistance(const BigPoint& b)	const
								{
									return ((x - b.x)*(x - b.x) + (y - b.y)*(y - b.y) + (z - b.z)*(z - b.z));
								}

		inline_	BigPoint		operator-()							const		{ return BigPoint(-x, -y, -z);						}
		inline_	BigPoint&		operator/=(float s)								{ s = 1.0f/s; x *= s; y *= s; z *= s; return *this; }
		inline_	BigPoint&		operator-=(const Point& p)						{ x -= p.x; y -= p.y; z -= p.z;	return *this;		}
		inline_	BigPoint&		operator-=(const BigPoint& p)					{ x -= p.x; y -= p.y; z -= p.z;	return *this;		}
		inline_	BigPoint&		operator+=(const Point& p)						{ x += p.x; y += p.y; z += p.z;	return *this;		}
		inline_	BigPoint&		operator+=(const BigPoint& p)					{ x += p.x; y += p.y; z += p.z;	return *this;		}
		inline_	BigPoint&		operator*=(float s)								{ x *= s; y *= s; z *= s;		return *this;		}
		inline_	BigPoint		operator*(const BigPoint& p)		const		{ return BigPoint(x * p.x, y * p.y, z * p.z);		}
		inline_	BigPoint		operator*(float s)					const		{ return BigPoint(x * s,   y * s,   z * s );		}
		inline_ friend BigPoint	operator*(float s, const BigPoint& p)			{ return BigPoint(s * p.x, s * p.y, s * p.z);		}
		inline_	BigPoint		operator+(const BigPoint& p)		const		{ return BigPoint(x + p.x, y + p.y, z + p.z);		}
		inline_	Point			operator-(const BigPoint& p)		const		{ return Point(float(x - p.x), float(y - p.y), float(z - p.z));		}

		inline_					operator	const	BigType*() const	{ return &x; }
		inline_					operator			BigType*()			{ return &x; }

		public:
				BigType			x, y, z;
	};

#endif //__PPCGEKKO__
#endif //__ICEPOINT_WII_H__


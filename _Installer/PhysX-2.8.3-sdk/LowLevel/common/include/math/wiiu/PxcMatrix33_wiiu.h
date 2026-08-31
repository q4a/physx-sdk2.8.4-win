#ifndef PXC_MATRIX33_WIIU_H
#define PXC_MATRIX33_WIIU_H

#ifdef __PPCGEKKO__
#include "px_config.h"
#include "PxcQuaternion.h"

/*!
Basic mathematical 3x3 matrix

Some clarifications, as there have been much confusion about matrix formats etc in the past.

Short:
- Matrix have base vectors in columns (vectors are column matrices, 3x1 matrices).
- Matrix is physically stored in column major format
- Matrices are concaternated from left

Long:
Given three base vectors a, b and c the matrix is stored as
         
/a.x b.x c.x\
|a.y b.y c.y|
\a.z b.z c.z/

Vectors are treated as columns, so the vector v is 

/x\
|y|
\z/

And matrices are applied _before_ the vector (pre-multiplication)
v' = M*v

/x'\   /a.x b.x c.x\   /x\   /a.x*x + b.x*y + c.x*z\
|y'| = |a.y b.y c.y| * |y| = |a.y*x + b.y*y + c.y*z|
\z'/   \a.z b.z c.z/   \z/   \a.z*x + b.z*y + c.z*z/


Physical storage and indexing:
To be compatible with popular 3d rendering APIs (read D3d and OpenGL)
the physical indexing is

/0 3 6\
|1 4 7|
\2 5 8/

index = column*3 + row

which in C++ translates to M[column][row]

The mathematical indexing is M_row,column and this is what is used for _-notation 
so _12 is 1st row, second column and operator(row, column)!

*/
class PxcMatrix33
{
public:
	//! Default constructor
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcMatrix33()
	{}

	//! Construct from three base vectors
	CUDA_CALLABLE_MEMBER PxcMatrix33(const PxcVector& base0, const PxcVector& base1, const PxcVector& base2)
		: base0(base0), base1(base1), base2(base2)
	{}

	//! Construct from float[9]
	explicit CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcMatrix33(PxFloat values[])
//		: base0(values[0]), base1(values[3]), base2(values[6])
	{
		base0.x=values[0];
		base0.y=values[1];
		base0.z=values[2];
		base1.x=values[3];
		base1.y=values[4];
		base1.z=values[5];
		base2.x=values[6];
		base2.y=values[7];
		base2.z=values[8];
	}

	//! Construct from a quaternion
	explicit CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcMatrix33(const PxcQuaternion& q)
	{
		set(q);
	}

	//! Construct from a diagonal vector
	explicit CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcMatrix33(const PxcVector& diagonal)
//		: base0(PxcVector(diagonal.x,0,0)), base1(PxcVector(0,diagonal.y,0)), base2(PxcVector(0,0,diagonal.z))
	{
		base0.setZero(); base0.x = diagonal.x;
		base1.setZero(); base1.y = diagonal.y;
		base2.setZero(); base2.z = diagonal.z;
	}

	//! Copy constructor
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcMatrix33(const PxcMatrix33& other)
		: base0(other.base0), base1(other.base1), base2(other.base2)
	{}

	//! Assignment operator
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE const PxcMatrix33& operator=(const PxcMatrix33& other)
	{
		base0 = other.base0;
		base1 = other.base1;
		base2 = other.base2;
		return *this;
	}

	//! Set to identity matrix
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE void setIdentity()
	{
		base0[0] = base1[1] = base2[2] = 1.0f;

		base0[1] = base0[2] = 0.0f;
		base1[0] = base1[2] = 0.0f;
		base2[0] = base2[1] = 0.0f;
	}

	//! Check for identity matrix
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE bool isIdentity() const
	{
		if(base0[0] != 1.0f || base1[0] != 0.0f || base2[0] != 0.0f) return false;
		if(base0[1] != 0.0f || base1[1] != 1.0f || base2[1] != 0.0f) return false;
		if(base0[2] != 0.0f || base1[2] != 0.0f || base2[2] != 1.0f) return false;

		return true;
	}

	//! Set to zero matrix
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE void setZero()
	{
		base0[0] = base0[1] = base0[2] = 0.0f;
		base1[0] = base1[1] = base1[2] = 0.0f;
		base2[0] = base2[1] = base2[2] = 0.0f;
		
	}

	//! Check for zero matrix
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE bool isZero() const
	{
		if(base0[0] != 0.0f || base0[1] != 0.0f || base0[2] != 0.0f) return false;
		if(base1[0] != 0.0f || base1[1] != 0.0f || base1[2] != 0.0f) return false;
		if(base2[0] != 0.0f || base2[1] != 0.0f || base2[2] != 0.0f) return false;

		return true;
	}

	//! Get transposed matrix
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcMatrix33 getTranspose() const
	{
		PxcVector v0;	v0.x = base0.x;	v0.y = base1.x;	v0.z = base2.x;
		PxcVector v1;	v1.x = base0.y;	v1.y = base1.y;	v1.z = base2.y;
		PxcVector v2;	v2.x = base0.z;	v2.y = base1.z;	v2.z = base2.z;

		return PxcMatrix33(v0,v1,v2);   
		//return PxcMatrix33( PxcVector(base0.x, base1.x, base2.x),
		//					PxcVector(base0.y, base1.y, base2.y),
		//					PxcVector(base0.z, base1.z, base2.z));
	}

	//! Get the real inverse
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcMatrix33 getInverse() const
	{
		const PxFloat det = getDeterminant();
		PxcMatrix33 inverse;

		if(det != 0)
		{
			const PxFloat invDet = 1.0f/det;

			inverse.base0[0] = invDet * (base1[1]*base2[2] - base2[1]*base1[2]);							
			inverse.base0[1] = invDet *-(base0[1]*base2[2] - base2[1]*base0[2]);
			inverse.base0[2] = invDet * (base0[1]*base1[2] - base0[2]*base1[1]);

			inverse.base1[0] = invDet *-(base1[0]*base2[2] - base1[2]*base2[0]);
			inverse.base1[1] = invDet * (base0[0]*base2[2] - base0[2]*base2[0]);
			inverse.base1[2] = invDet *-(base0[0]*base1[2] - base0[2]*base1[0]);

			inverse.base2[0] = invDet * (base1[0]*base2[1] - base1[1]*base2[0]);
			inverse.base2[1] = invDet *-(base0[0]*base2[1] - base0[1]*base2[0]);
			inverse.base2[2] = invDet * (base0[0]*base1[1] - base1[0]*base0[1]);
		}
		else
		{
			inverse.setIdentity();
		}

		return inverse;
	}

	//! Get determinant
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxFloat getDeterminant() const
	{
		return base0[0] * (base1[1]*base2[2] - base1[2]*base2[1]) +
			  -base1[0] * (base0[1]*base2[2] - base0[2]*base2[1]) + 
			   base2[0] * (base0[1]*base1[2] - base0[2]*base1[1]);
	}

	// Simpler operators
	//! Equality operator
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE bool operator==(const PxcMatrix33& other) const
	{
		return (base0 == other.base0 &&
				base1 == other.base1 &&
				base2 == other.base2);
	}

	//! Inequality operator
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE bool operator!=(const PxcMatrix33& other) const
	{
		return (base0 != other.base0 ||
				base1 != other.base1 ||
				base2 != other.base2);
	}

	//! Unary minus
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcMatrix33 operator-() const
	{
		return PxcMatrix33(-base0, -base1, -base2);
	}

	//! Add
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcMatrix33 operator+(const PxcMatrix33& other) const
	{
		return PxcMatrix33( base0+other.base0,
							base1+other.base1,
							base2+other.base2);
	}

	//! Subtract
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcMatrix33 operator-(const PxcMatrix33& other) const
	{
		return PxcMatrix33( base0-other.base0,
							base1-other.base1,
							base2-other.base2);
	}

	//! Scalar multiplication
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcMatrix33 operator*(PxFloat scalar) const
	{
		return PxcMatrix33(base0*scalar, base1*scalar, base2*scalar);
	}

	friend PxcMatrix33 operator*(PxFloat, const PxcMatrix33&);

	//! Matrix multiplication
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcMatrix33 operator*(const PxcMatrix33& other)
	{
		//Rows from this <dot> columns from other
		//base0 = transform(other.base0) etc
		return PxcMatrix33(transform(other.base0), transform(other.base1), transform(other.base2));
	}

	// a <op>= b operators

	//! Equals-add
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcMatrix33& operator+=(const PxcMatrix33& other)
	{
		base0 += other.base0;
		base1 += other.base1;
		base2 += other.base2;
		return *this;
	}

	//! Equals-sub
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcMatrix33& operator-=(const PxcMatrix33& other)
	{
		base0 -= other.base0;
		base1 -= other.base1;
		base2 -= other.base2;
		return *this;
	}

	//! Equals scalar multiplication
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcMatrix33& operator*=(PxFloat scalar)
	{
		base0 *= scalar;
		base1 *= scalar;
		base2 *= scalar;
		return *this;
	}

	//! Element access, mathematical way!
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxFloat operator()(PxUInt row, PxUInt col) const
	{
		return (*this)[col][row];
	}

	//! Element access, mathematical way!
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxFloat& operator()(PxUInt row, PxUInt col)
	{
		return (*this)[col][row];
	}

	//! Get a row from the matrix
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector getRow(PxUInt row)
	{
		PxcVector vec;
		vec.x = base0[row];
		vec.y = base1[row];
		vec.z = base2[row];
		return vec;
		//return PxcVector(m[0][row], m[1][row], m[2][row]);
	}

	// Transform etc
	
	//! Transform vector by matrix, equal to v' = M*v
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector transform(const PxcVector& other) const
	{
		Mtx m;
		PxcVector r;
		m[0][0] = (*this)[0][0];	m[0][1] = (*this)[1][0];	m[0][2] = (*this)[2][0];
		m[1][0] = (*this)[0][1];	m[1][1] = (*this)[1][1];	m[1][2] = (*this)[2][1];
		m[2][0] = (*this)[0][2];	m[2][1] = (*this)[1][2];	m[2][2] = (*this)[2][2];
		m[0][3] = 0;				m[1][3] = 0;				m[2][3] = 0;
		
		MTXMultVec( m, (Vec*)&other.x, (Vec*)&r.x);
		return r;		
	}

	//! Transform vector by matrix transpose, v' = M^t*v
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE PxcVector transformTranspose(const PxcVector& other) const
	{
		PxcVector vec;
		vec.x = base0.dot(other);
		vec.y = base1.dot(other);
		vec.z = base2.dot(other);
		return vec;
		//return PxcVector(base0.dot(other),
		//				 base1.dot(other),
		//				 base2.dot(other));
	}


	// Conversion
	//! Set matrix from quaternion
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE void set(const PxcQuaternion& q) 
	{
		// Convert from quaternion
		q.getBasisVectors(base0, base1, base2);
	}

	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE		PxcVector& operator[](int num)			{return (&base0)[num];}
	CUDA_CALLABLE_MEMBER PXD_FORCE_INLINE const	PxcVector& operator[](int num) const	{return (&base0)[num];}

	//Data, see above for format!

	PxcVector base0, base1, base2; //the three base vectors

};

#endif
#endif

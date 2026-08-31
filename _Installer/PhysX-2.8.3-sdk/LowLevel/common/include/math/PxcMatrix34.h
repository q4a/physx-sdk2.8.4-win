#ifndef PXC_MATRIX34_H
#define PXC_MATRIX34_H

#include "px_config.h"

#include "PxcMatrix33.h"
#include "PxcTransform.h"

/*!
Basic mathematical 3x4 matrix

Some clarifications, as there have been much confusion about matrix formats etc in the past.

Short:
- Matrix have base vectors in columns (vectors are column matrices, 3x1 matrices).
- Matrix is physically stored in column major format
- Matrices are concaternated from left

Long:
Given four base vectors a, b, c and d the matrix is stored as
         
/a.x b.x c.x d.x\
|a.y b.y c.y d.y|
\a.z b.z c.z d.z/

Vectors are treated as columns, so the vector v is 

/x\
|y|
\z/

And matrices are applied _before_ the vector (pre-multiplication)
v' = M*v

Depending on if it is treated as a vector or point the result will be a
bit different.

Vector:

/x'\   /a.x b.x c.x d.x\   /x\   /a.x*x + b.x*y + c.x*z + 0\
|y'| = |a.y b.y c.y d.y| * |y| = |a.y*x + b.y*y + c.y*z + 0|
\z'/   \a.z b.z c.z d.z/   |z|   \a.z*x + b.z*y + c.z*z + 0/
                           \0/

Point:

/x'\   /a.x b.x c.x d.x\   /x\   /a.x*x + b.x*y + c.x*z + d.x\
|y'| = |a.y b.y c.y d.y| * |y| = |a.y*x + b.y*y + c.y*z + d.y|
\z'/   \a.z b.z c.z d.z/   |z|   \a.z*x + b.z*y + c.z*z + d.z/
                           \1/


Physical storage and indexing:
To be compatible with popular 3d rendering APIs (read D3d and OpenGL)
the physical indexing is

/0 3 6  9\
|1 4 7 10|
\2 5 8 11/

index = column*3 + row

which in C++ translates to M[column][row]

The mathematical indexing is M_row,column and this is what is used for _-notation 
so _12 is 1st row, second column and operator(row, column)!

*/
class PxcMatrix34
{
public:
	//! Default constructor
	PXD_FORCE_INLINE PxcMatrix34()
	{}

	//! Construct from four base vectors
	PxcMatrix34(const PxcVector& base0, const PxcVector& base1, const PxcVector& base2, const PxcVector& base3)
		: base0(base0), base1(base1), base2(base2), base3(base3)
	{}

	//! Construct from float[12]
	explicit PXD_FORCE_INLINE PxcMatrix34(PxFloat values[])
		//: base0(&values[0]), base1(&values[3]), base2(&values[6]), base3(&values[9])
	{		
		base0.set(&values[0]);
		base1.set(&values[3]);
		base2.set(&values[6]);
		base3.set(&values[9]);
	}

	//! Construct from a 3x3 matrix
	explicit PXD_FORCE_INLINE PxcMatrix34(const PxcMatrix33& other)
		: base0(other.base0), base1(other.base1), base2(other.base2)
	{
		base3.setZero();
	}

	//! Construct from a 3x3 matrix and a translation vector
	PXD_FORCE_INLINE PxcMatrix34(const PxcMatrix33& other, const PxcVector& t)
		: base0(other.base0), base1(other.base1), base2(other.base2), base3(t)
	{}

	//! Construct from a PxcTransform
	explicit PXD_FORCE_INLINE PxcMatrix34(const PxcTransform& other)
	{
//		set(other.rotation);
		other.rotation.getBasisVectors(base0, base1, base2);
		base3 = other.translation;
	}

	//! Construct from a quaternion
	explicit PXD_FORCE_INLINE PxcMatrix34(const PxcQuaternion& q)
	{
		set(q);
	}

	//! Copy constructor
	PXD_FORCE_INLINE PxcMatrix34(const PxcMatrix34& other)
	{
		memcpy(this, &other, sizeof(PxcMatrix34));
	}

	//! Assignment operator
	PXD_FORCE_INLINE const PxcMatrix34& operator=(const PxcMatrix34& other)
	{
		base0 = other.base0;
		base1 = other.base1;
		base2 = other.base2;
		base3 = other.base3;

		return *this;
	}

	//! Set to identity matrix
	PXD_FORCE_INLINE void setIdentity()
	{
		base0[0] = base1[1] = base2[2] = 1.0f;

		base0[1] = base0[2] = 0.0f;
		base1[0] = base1[2] = 0.0f;
		base2[0] = base2[1] = 0.0f;
		base3[0] = base3[1] = base3[2] = 0.0f;
	}

	//! Check for identity matrix
	PXD_FORCE_INLINE bool isIdentity() const
	{
		if(base0[0] != 1.0f || base1[0] != 0.0f || base2[0] != 0.0f || base3[0] != 0.0f) return false;
		if(base0[1] != 0.0f || base1[1] != 1.0f || base2[1] != 0.0f || base3[1] != 0.0f) return false;
		if(base0[2] != 0.0f || base1[2] != 0.0f || base2[2] != 1.0f || base3[2] != 0.0f) return false;

		return true;
	}

	//! Set to zero matrix
	PXD_FORCE_INLINE void setZero()
	{
		base0[0] = base0[1] = base0[2] = 0.0f;
		base1[0] = base1[1] = base1[2] = 0.0f;
		base2[0] = base2[1] = base2[2] = 0.0f;
		base3[0] = base3[1] = base3[2] = 0.0f;
	}

	//! Check for zero matrix
	PXD_FORCE_INLINE bool isZero() const
	{
		if(base0[0] != 0.0f || base0[1] != 0.0f || base0[2] != 0.0f || base3[0] != 0.0f) return false;
		if(base1[0] != 0.0f || base1[1] != 0.0f || base1[2] != 0.0f || base3[1] != 0.0f) return false;
		if(base2[0] != 0.0f || base2[1] != 0.0f || base2[2] != 0.0f || base3[2] != 0.0f) return false;

		return true;
	}

	
	// Simpler operators
	//! Equality operator
	PXD_FORCE_INLINE bool operator==(const PxcMatrix34& other) const
	{
		return (base0 == other.base0 &&
				base1 == other.base1 &&
				base2 == other.base2 &&
				base3 == other.base3);
	}

	//! Inequality operator
	PXD_FORCE_INLINE bool operator!=(const PxcMatrix34& other) const
	{
		return (base0 != other.base0 ||
				base1 != other.base1 ||
				base2 != other.base2 ||
				base3 != other.base3);
	}

	//! Unary minus
	PXD_FORCE_INLINE PxcMatrix34 operator-() const
	{
		return PxcMatrix34(-base0, -base1, -base2, -base3);
	}

	//! Add
	PXD_FORCE_INLINE PxcMatrix34 operator+(const PxcMatrix34& other) const
	{
		return PxcMatrix34( base0+other.base0,
							base1+other.base1,
							base2+other.base2,
							base3+other.base3);
	}

	//! Subtract
	PXD_FORCE_INLINE PxcMatrix34 operator-(const PxcMatrix34& other) const
	{
		return PxcMatrix34( base0-other.base0,
							base1-other.base1,
							base2-other.base2,
							base3-other.base3);
	}

	//! Scalar multiplication
	PXD_FORCE_INLINE PxcMatrix34 operator*(PxFloat scalar) const
	{
		return PxcMatrix34(base0*scalar, base1*scalar, base2*scalar, base3*scalar);
	}

	friend PxcMatrix34 operator*(PxFloat, const PxcMatrix34&);

	//! Matrix multiplication
	PXD_FORCE_INLINE PxcMatrix34 operator*(const PxcMatrix34& other) const
	{
		//Rows from this <dot> columns from other
		//base0 = rotate(other.base0) etc
		return PxcMatrix34(rotate(other.base0), rotate(other.base1), rotate(other.base2),
			rotate(other.base3)+base3);
	}

	//! Matrix multiplication, extend the second matrix
	PXD_FORCE_INLINE PxcMatrix34 operator*(const PxcMatrix33& other) const
	{
		//Rows from this <dot> columns from other
		//base0 = transform(other.base0) etc
		return PxcMatrix34(rotate(other.base0), rotate(other.base1), rotate(other.base2), base3);
	}

	friend PxcMatrix34 operator*(const PxcMatrix33& a, const PxcMatrix34& b);
	
	// a <op>= b operators

	//! Equals-add
	PXD_FORCE_INLINE PxcMatrix34& operator+=(const PxcMatrix34& other)
	{
		base0 += other.base0;
		base1 += other.base1;
		base2 += other.base2;
		base3 += other.base3;
		return *this;
	}

	//! Equals-sub
	PXD_FORCE_INLINE PxcMatrix34& operator-=(const PxcMatrix34& other)
	{
		base0 -= other.base0;
		base1 -= other.base1;
		base2 -= other.base2;
		base3 -= other.base3;
		return *this;
	}

	//! Equals scalar multiplication
	PXD_FORCE_INLINE PxcMatrix34& operator*=(PxFloat scalar)
	{
		base0 *= scalar;
		base1 *= scalar;
		base2 *= scalar;
		base3 *= scalar;
		return *this;
	}

	//! Element access, mathematical way!
	PXD_FORCE_INLINE PxFloat operator()(PxUInt row, PxUInt col) const
	{
		return (*this)[col][row];
	}

	//! Element access, mathematical way!
	PXD_FORCE_INLINE PxFloat& operator()(PxUInt row, PxUInt col)
	{
		return (*this)[col][row];
	}

	// Transform etc
	
	//! Transform vector by matrix, equal to v' = M*v
	PXD_FORCE_INLINE PxcVector rotate(const PxcVector& other) const
	{
		return base0*other.x + base1*other.y + base2*other.z;
	}

	//! Transform vector by transpose of matrix, equal to v' = M^t*v
	PXD_FORCE_INLINE PxcVector rotateTranspose(const PxcVector& other) const
	{
		//return PxcVector(base0.dot(other),
		//				 base1.dot(other),
		//				 base2.dot(other));
		PxcVector temp;
		temp.set(base0.dot(other), base1.dot(other), base2.dot(other));
		return temp;
	}

	//! Transform point by matrix
	PXD_FORCE_INLINE PxcVector transform(const PxcVector& other) const
	{
		return base0*other.x + base1*other.y + base2*other.z + base3;

		//return PxcVector(
		//	base0.x*other.x + base1.x*other.y + base2.x*other.z + base3.x,
		//	base0.y*other.x + base1.y*other.y + base2.y*other.z + base3.y,
		//	base0.z*other.x + base1.z*other.y + base2.z*other.z + base3.z);
	}

	//! Transform point by transposed matrix
	PXD_FORCE_INLINE PxcVector transformTranspose(const PxcVector& other) const
	{
		return rotateTranspose(other-base3);
	}

	//! Invert matrix treating it as a rotation+translation matrix only
	PXD_FORCE_INLINE PxcMatrix34 getInverseRT() const
	{
		//M  = T  * R
		//M' = R' * T'
		PxcMatrix33 rot = PxcMatrix33(base0, base1, base2);
		return PxcMatrix34(rot.getTranspose(), rot.transformTranspose(-base3));
	}


	// Conversion
	//! Set matrix from quaternion
	PXD_FORCE_INLINE void set(const PxcQuaternion& q) 
	{
		// Convert from quaternion
		q.getBasisVectors(base0, base1, base2);
		
		// Col3
		base3[0] = 0.0f;
		base3[1] = 0.0f;
		base3[2] = 0.0f;
	}

	PXD_FORCE_INLINE		PxcVector& operator[](int num)			{return (&base0)[num];}
	PXD_FORCE_INLINE const	PxcVector& operator[](int num) const	{return (&base0)[num];}

	//Data, see above for format!

	PxcVector base0, base1, base2, base3; //the four base vectors

};

//! Multiply a*b, a is extended
PXD_INLINE PxcMatrix34 operator*(const PxcMatrix33& a, const PxcMatrix34& b)
{
	return PxcMatrix34(a.transform(b.base0), a.transform(b.base1), a.transform(b.base2), a.transform(b.base3));
}

#endif

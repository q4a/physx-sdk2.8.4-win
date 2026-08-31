///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 *	OPCODE - Optimized Collision Detection
 *	Copyright (C) 2001 Pierre Terdiman
 *	Homepage: http://www.codercorner.com/Opcode.htm
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains common classes & defs used in OPCODE.
 *	\file		OPC_Common.h
 *	\author		Pierre Terdiman
 *	\date		March, 20, 2001
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef OPC_COMMON_H
#define OPC_COMMON_H

// [GOTTFRIED]: Just a small change for readability.
#ifdef OPC_CPU_COMPARE
	#define GREATER(x, y)	AIR(x) > IR(y)
#else
	#define GREATER(x, y)	fabsf(x) > (y)
#endif

	class OPCODE_API CollisionAABB
	{
		public:
		//! Constructor
		inline_				CollisionAABB()						{}
		//! Constructor
		inline_				CollisionAABB(const AABB& b)		{ b.GetCenter(mCenter);	b.GetExtents(mExtents);	}
		//! Destructor
		inline_				~CollisionAABB()					{}

		//! Get min point of the box
		inline_	void		GetMin(Point& min)		const		{ min = mCenter - mExtents;					}
		//! Get max point of the box
		inline_	void		GetMax(Point& max)		const		{ max = mCenter + mExtents;					}

		//! Get component of the box's min point along a given axis
		inline_	float		GetMin(udword axis)		const		{ return mCenter[axis] - mExtents[axis];	}
		//! Get component of the box's max point along a given axis
		inline_	float		GetMax(udword axis)		const		{ return mCenter[axis] + mExtents[axis];	}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Setups an AABB from min & max vectors.
		 *	\param		min			[in] the min point
		 *	\param		max			[in] the max point
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		inline_	void		SetMinMax(const Point& min, const Point& max)		{ mCenter = (max + min)*0.5f; mExtents = (max - min)*0.5f;		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Checks a box is inside another box.
		 *	\param		box		[in] the other box
		 *	\return		true if current box is inside input box
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		inline_	BOOL		IsInside(const CollisionAABB& box) const
							{
								if(box.GetMin(0)>GetMin(0))	return FALSE;
								if(box.GetMin(1)>GetMin(1))	return FALSE;
								if(box.GetMin(2)>GetMin(2))	return FALSE;
								if(box.GetMax(0)<GetMax(0))	return FALSE;
								if(box.GetMax(1)<GetMax(1))	return FALSE;
								if(box.GetMax(2)<GetMax(2))	return FALSE;
								return TRUE;
							}
/*
		//! Operator for "CollisionAABB A = AABB B"
		inline_	void		operator=(const AABB& b)	{ b.GetCenter(mCenter);	b.GetExtents(mExtents);	}

		inline_	void		Add(const Point& min, const Point& max)
							{
								// Compute new min & max values
								Point Min;
								GetMin(Min);
#ifdef OPC_USE_FCOMI
								Min.x = FCMin2(Min.x, min.x);
								Min.y = FCMin2(Min.y, min.y);
								Min.z = FCMin2(Min.z, min.z);
#else
								Min.Min(min);
#endif

								Point Max;
								GetMax(Max);
#ifdef OPC_USE_FCOMI
								Max.x = FCMax2(Max.x, max.x);
								Max.y = FCMax2(Max.y, max.y);
								Max.z = FCMax2(Max.z, max.z);
#else
								Max.Max(max);
#endif
								SetMinMax(Min, Max);
							}

		inline_	void		Add(const CollisionAABB& aabb)
							{
								// Compute new min & max values
								Point Min;	GetMin(Min);
								Point Tmp;	aabb.GetMin(Tmp);
#ifdef OPC_USE_FCOMI
								Min.x = FCMin2(Min.x, Tmp.x);
								Min.y = FCMin2(Min.y, Tmp.y);
								Min.z = FCMin2(Min.z, Tmp.z);
#else
								Min.Min(Tmp);
#endif

								Point Max;	GetMax(Max);
								aabb.GetMax(Tmp);
#ifdef OPC_USE_FCOMI
								Max.x = FCMax2(Max.x, Tmp.x);
								Max.y = FCMax2(Max.y, Tmp.y);
								Max.z = FCMax2(Max.z, Tmp.z);
#else
								Max.Max(Tmp);
#endif
								SetMinMax(Min, Max);
							}
*/
				Point		mCenter;				//!< Box center
				Point		mExtents;				//!< Box extents
	};

	class OPCODE_API QuantizedAABB
	{
		public:
		//! Constructor
		inline_				QuantizedAABB()			{}
		//! Destructor
		inline_				~QuantizedAABB()		{}

				sword		mCenter[3];				//!< Quantized center
				uword		mExtents[3];			//!< Quantized extents
	};

	//! Quickly rotates & translates a vector
	inline_ void TransformPoint(Point& dest, const Point& source, const Matrix3x3& rot, const Point& trans)
	{
		dest.x = trans.x + source.x * rot.m[0][0] + source.y * rot.m[1][0] + source.z * rot.m[2][0];
		dest.y = trans.y + source.x * rot.m[0][1] + source.y * rot.m[1][1] + source.z * rot.m[2][1];
		dest.z = trans.z + source.x * rot.m[0][2] + source.y * rot.m[1][2] + source.z * rot.m[2][2];
	}

#endif // OPC_COMMON_H

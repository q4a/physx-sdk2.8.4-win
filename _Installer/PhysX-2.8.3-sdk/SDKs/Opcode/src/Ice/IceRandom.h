///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains code for random generators.
 *	\file		IceRandom.h
 *	\author		Pierre Terdiman
 *	\date		August, 9, 2001
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef ICERANDOM_H
#define ICERANDOM_H

	FUNCTION ICECORE_API	void	SRand(udword seed);
	FUNCTION ICECORE_API	udword	Rand();

	//! Returns a random floating-point value f such as 0.0f <= f <= 1.0f
	inline_ float UnitRandomFloat()	{ return float(Rand()) * ONE_OVER_RAND_MAX;	}

	//! Returns a random number i so that 0 <= i < max_value
	ICECORE_API	udword GetRandomNumber(udword max_value);

	class ICECORE_API BasicRandom
	{
		public:

		//! Constructor
		inline_				BasicRandom(udword seed=0)	: mRnd(seed)	{}
		//! Destructor
		inline_				~BasicRandom()								{}

		inline_	void		SetSeed(udword seed)		{ mRnd = seed;											}
		inline_	udword		GetCurrentValue()	const	{ return mRnd;											}
		inline_	udword		Randomize()					{ mRnd = mRnd * 2147001325 + 715136305; return mRnd;	}

		private:
				udword		mRnd;
	};

#endif // ICERANDOM_H


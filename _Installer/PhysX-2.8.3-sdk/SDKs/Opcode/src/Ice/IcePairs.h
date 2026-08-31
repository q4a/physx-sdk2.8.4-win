///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains a simple pair class.
 *	\file		IcePairs.h
 *	\author		Pierre Terdiman
 *	\date		January, 13, 2003
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef ICEPAIRS_H
#define ICEPAIRS_H

	//! A generic couple structure
#pragma pack(1)
	class ICECORE_API Pair : public Allocateable
	{
		public:
		inline_	Pair()											{}
		inline_	Pair(udword i0, udword i1) : id0(i0), id1(i1)	{}
		inline_	~Pair()											{}

		//! Operator for "if(Pair==Pair)"
		inline_	bool			operator==(const Pair& p)	const	{ return (id0==p.id0) && (id1==p.id1);	}
		//! Operator for "if(Pair!=Pair)"
		inline_	bool			operator!=(const Pair& p)	const	{ return (id0!=p.id0) || (id1!=p.id1);	}

		udword	id0;	//!< First index of the pair
		udword	id1;	//!< Second index of the pair
	};
#pragma pack()
	
	ICE_COMPILE_TIME_ASSERT(sizeof(Pair)==8);

	class ICECORE_API Pairs : private Container
	{
		public:
		// Constructor / Destructor
								Pairs()							{}
								~Pairs()						{}

		inline_	udword			GetNbPairs()		const		{ return GetNbEntries()>>1;					}
		inline_	const Pair*		GetPairs()			const		{ return (const Pair*)GetEntries();			}
		inline_	const Pair*		GetPair(udword i)	const		{ return (const Pair*)&GetEntries()[i+i];	}

		inline_	BOOL			HasPairs()			const		{ return IsNotEmpty();						}

		inline_	void			ResetPairs()					{ Reset();									}
		inline_	void			DeleteLastPair()				{ DeleteLastEntry();	DeleteLastEntry();	}

		inline_	void			AddPair(const Pair& p)			{ Add(p.id0).Add(p.id1);					}
		inline_	void			AddPair(udword id0, udword id1)	{ Add(id0).Add(id1);						}

		// HANDLE WITH CARE - I hope you know what you're doing
		inline_	void			ForceNbPairs(udword nb_pairs)	{ ForceSize(nb_pairs<<1);					}
	};

#endif // ICEPAIRS_H

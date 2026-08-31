// SUPPORT FILE FROM ICE, NEEDED FOR CONVEX-CONVEX - DO NOT TOUCH !

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains code to create vertex valencies.
 *	\file		IceValency.h
 *	\author		Pierre Terdiman
 *	\date		February, 29, 2000
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef ICEVALENCY_H
#define ICEVALENCY_H

#include "px_valency.h"

	class MESHMERIZER_API Valencies: public Allocateable
	{
		public:
		//! Constructor
								Valencies();
		//! Destructor
								~Valencies();

				bool			Load(const Stream& stream);
		// Data access
		inline_	udword			GetNbVerts()				const	{ return mData.mNbVerts;										}
		inline_	const PxValency*GetValencies()				const	{ return mData.mValencies;										}
		inline_	uword			GetValency(udword i)		const	{ return mData.mValencies[i].mCount;							}
		inline_	uword			GetOffset(udword i)			const	{ return mData.mValencies[i].mOffset;							}
		inline_	const ubyte*	GetAdjacentVerts()			const	{ return mData.mAdjacentVerts;									}

		inline_	uword			GetNbNeighbors(udword i)	const	{ return mData.mValencies[i].mCount;							}
		inline_	const ubyte*	GetNeighbors(udword i)		const	{ return &mData.mAdjacentVerts[mData.mValencies[i].mOffset];	}

		// Stats
		inline_	PxValenciesData* GetPXValenciesData()				{ return &mData; }
		protected:
				void*			mBuffer;
				PxValenciesData mData;
		// Internal methods
				void			CreateOffsets();
	};

#endif // ICEVALENCY_H


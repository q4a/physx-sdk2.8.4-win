///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains a new optimized pair manager.
 *	\file		IcePairManager.h
 *	\author		Pierre Terdiman
 *	\date		December, 16, 2003
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef ICEPAIRMANAGER_H
#define ICEPAIRMANAGER_H

#ifdef REMOVED	// PT: GRRRRR. DON'T TOUCH THIS, STUPID!
	ICE_COMPILE_TIME_ASSERT(sizeof(Pair)==8);

	class ICECORE_API PairManager_
	{
		public:
		// Constructor/Destructor
								PairManager_();
								~PairManager_();

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Adds a pair.
		 *	\param		id0		[in] first pair element
		 *	\param		id1		[in] second pair element
		 *	\return		corresponding pair
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				const Pair*		AddPair(udword id0, udword id1);
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Removes a pair.
		 *	\param		id0		[in] first pair element
		 *	\param		id1		[in] second pair element
		 *	\return		true if success (false if the pair didn't exist)
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				bool			RemovePair(udword id0, udword id1);
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Finds a pair.
		 *	\param		id0		[in] first pair element
		 *	\param		id1		[in] second pair element
		 *	\return		corresponding pair, or null if the pair doesn't exist
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				const Pair*		FindPair(udword id0, udword id1)	const;

#ifdef ICE32
		inline_	udword			GetPairIndex(const Pair* pair)		const { return (udword(pair) - udword(mActivePairs))>>3;	}
#else
		inline_	udword			GetPairIndex(const Pair* pair)		const
		{
			return udword((uqword(pair) - uqword(mActivePairs))/sizeof(Pair));
		}
#endif
				void			DumpPairs(Pairs& pairs)				const;

		private:
				udword			mHashSize;
				udword			mMask;
				udword			mNbActivePairs;
				udword			mFirstFree;
				udword*			mHashTable;
				udword*			mNext;
				Pair*			mActivePairs;
		inline_	Pair*			FindPair(udword id0, udword id1, udword hash_value) const;
	};

#pragma pack(1)
	struct UserPair
	{
		uword	mID0;
		uword	mID1;
		void*	mUserData;
	};
#pragma pack()

#if defined(__CELLOS_LV2__)
	ICE_COMPILE_TIME_ASSERT(sizeof(UserPair)==12);
#else
	ICE_COMPILE_TIME_ASSERT(sizeof(UserPair)==8);
#endif

	class ICECORE_API UserPairManager
	{
		public:
		// Constructor/Destructor
								UserPairManager();
								~UserPairManager();

				const UserPair*	AddPair(uword id0, uword id1, void* user_data);
				bool			RemovePair(uword id0, uword id1);
				const UserPair*	FindPair(uword id0, uword id1)		const;

#ifdef ICE32
		inline_	udword			GetPairIndex(const UserPair* pair)	const { return (udword(pair) - udword(mActivePairs))>>3;	}
#else
		inline_	udword			GetPairIndex(const UserPair* pair)	const
		{
			return udword((uqword(pair) - uqword(mActivePairs))/sizeof(Pair));
		}
#endif

		private:
				udword			mHashSize;
				udword			mMask;
				udword			mNbActivePairs;
				udword			mFirstFree;
				udword*			mHashTable;
				udword*			mNext;
				UserPair*		mActivePairs;
		inline_	UserPair*		FindPair(uword id0, uword id1, udword hash_value) const;
	};
#endif

#endif // ICEPAIRMANAGER_H

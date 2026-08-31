#ifndef NX_PHYSICS_PAIRMANAGER
#define NX_PHYSICS_PAIRMANAGER
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Nxf.h"

#define INVALID_USER_ID	0xffff

#pragma pack(1)
struct userPair
	{
		NxU16	id0;
		NxU16	id1;
		void*	userData;
	};
#pragma pack()

#if defined(NX64)
NX_COMPILE_TIME_ASSERT(sizeof(userPair)==12);
#else
NX_COMPILE_TIME_ASSERT(sizeof(userPair)==8);
#endif

class PairManager
	{
	public:
								PairManager();
								~PairManager();

				void			purge();

				const userPair*	addPair		(NxU16 id0, NxU16 id1, void* userData, bool overwrite_data=true);
				bool			removePair	(NxU16 id0, NxU16 id1, const void** userData=NULL);
				const userPair*	findPair	(NxU16 id0, NxU16 id1)	const;

//		private:
#if defined(NX64)
			NX_INLINE	NxU32			getPairIndex(const userPair* pair)	const
			{
				return ((NxU32)((NxU64(pair) - NxU64(activePairs)))/sizeof(userPair));
			}
#elif defined(NX32)
			NX_INLINE	NxU32			getPairIndex(const userPair* pair)	const { return NxU32((size_t(pair) - size_t(activePairs))>>3);	}
#endif
			NX_INLINE	userPair*		findPair(NxU16 id0, NxU16 id1, NxU32 hashValue) const;

				NxU32			hashSize;
				NxU32			mask;
				NxU32			nbActivePairs;		//number of pairs
//				NxU32			firstFree;
				NxU32*			hashTable;
				NxU32*			next;
				userPair*		activePairs;		//dense arrat of pairs (if fill holes is defined)
	};

#endif

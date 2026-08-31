#ifndef NX_COLLISION_GENERICCACHE
#define NX_COLLISION_GENERICCACHE

#include "Nxf.h"

/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

	typedef NxU32	CacheIndex;

	class GenericCache
		{
		public:
								GenericCache();
								~GenericCache();

					bool		init(NxU32 cacheSize);

					CacheIndex	cacheData(NxU32 nbBytes, const void* data=NULL, void** cache=NULL);
					bool		shrinkCache(CacheIndex cookie, NxU32 usedSize);
					void*		getData(CacheIndex cookie)	const;

		NX_INLINE	void		cacheBegin()	{ marker = writePtr;					}
		NX_INLINE	NxU32		cacheEnd()		{ return ::abs(int(writePtr - marker));	}
		NX_INLINE	NxU32		getSize()		{ return cacheSize; }
		private:
					NxU32		cacheSize;
					CacheIndex	writePtr;	//!< Virtual running index
					CacheIndex	ptrOwner;	//!< Owner of write pointer
					NxU8*		pool;		//!< Cyclic-array of cacheSize bytes
					CacheIndex	marker;		//!< 

		// Internal methods
					void		release();
					void*		allocFromCache(NxU32 nbBytes, CacheIndex& cookie);
		NX_INLINE	NxU32		computeRealIndex(NxU32 virtualIndex)	const	{ return cacheSize ? virtualIndex % cacheSize : 0;	}
		};

#endif

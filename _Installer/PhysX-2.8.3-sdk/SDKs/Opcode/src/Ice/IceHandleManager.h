///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains a handle manager.
 *	\file		IceHandleManager.h
 *	\author		Pierre Terdiman
 *	\date		December, 12, 1999
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef ICEHANDLEMANAGER_H
#define ICEHANDLEMANAGER_H

    typedef udword  Handle;

	class ICECORE_API HandleManager : public Allocateable
	{
		public:
		// Constructor / Destructor
							HandleManager();
							~HandleManager();
		// Basic usage
				Handle		Add(void* object);
				void		Remove(Handle handle);

		// Advanced usage
				bool		Remap(const udword* ranks);

		// Physical data access
		inline_	udword		GetMaxNbObjects()			const	{ return mMaxNbObjects;		}	//!< Returns max number of objects
		inline_	udword		GetNbObjects()				const	{ return mCurrentNbObjects;	}	//!< Returns current number of objects
		inline_	void**		GetObjects()				const	{ return mObjects;			}	//!< Gets the complete list of objects

				void*		GetObject(Handle handle)	const;	// Returns object according to handle.

		//! High-speed access - same as GetObject without any checkings - handle with care.
		inline_	void*		PickObject(Handle handle)	const	{ return mObjects[mOutToIn[uword(handle)]]; }

		// Stats
				udword		GetUsedRam()				const;

							PREVENT_COPY(HandleManager)
		private:
		// Physical list
				void**		mObjects;			//!< Physical list, with no holes but unsorted.
				udword		mCurrentNbObjects;	//!< Current number of objects in the physical list.
				udword		mMaxNbObjects;		//!< Maximum possible number of objects in the physical list.

		// Cross-references
				uword*		mOutToIn;			//!< Maps virtual indices (handles) to real ones.
				uword*		mInToOut;			//!< Maps real indices to virtual ones (handles).
                uword*      mStamps;

		// Recycled locations
				udword		mNbFreeIndices;		//!< Current number of free indices

		// Internal methods
				bool		SetupLists(void** objects=null, uword* oti=null, uword* ito=null, uword* stamps=null);
	};

#endif //ICEHANDLEMANAGER_H

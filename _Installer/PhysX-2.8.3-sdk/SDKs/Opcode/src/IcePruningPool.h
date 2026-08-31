///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains a pruning pool.
 *	\file		IcePruningPool.h
 *	\author		Pierre Terdiman
 *	\date		November, 15, 2001
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef ICEPRUNINGPOOL_H
#define ICEPRUNINGPOOL_H

	typedef void (*SwapCallback)(udword old_index, udword new_index, void* user_data);

	class OPCODE_API PruningPool : public Allocateable
	{
		public:
		// Constructor / destructor
									PruningPool();
//									PruningPool(const PruningPool& source, bool transfer_ownership=false);
									~PruningPool();

//				bool				ReplaceWith(const PruningPool& source, const PrunedObjects& hack);

		// Management
				bool				Init(udword nb_objects);
				bool				AddObject(Prunable& object, SwapCallback callback=null, void* user_data=null);
				void				RemoveObject(Prunable& object, SwapCallback callback=null, void* user_data=null);

		// Data access
		inline_	uword				GetNbObjects()			const	{ return mNbObjects;						}
		inline_	uword				GetMaxNbObjects()		const	{ return mMaxNbObjects;						}
		inline_	Prunable**			GetObjects()			const	{ return mObjects;							}
#ifdef OPC_PRUNING_SECTIONS
		inline_	udword				GetNbHighObjects()		const	{ return mNbHigh;							}
		inline_	udword				GetNbMediumObjects()	const	{ return mNbMedium;							}
		inline_	udword				GetNbLowObjects()		const	{ return mNbLow;							}
		inline_	udword				GetNbHeadObjects()		const	{ return mNbHigh + mNbMedium;				}
		inline_	udword				GetNbTailObjects()		const	{ return mNbMedium + mNbLow;				}
		inline_	Prunable**			GetHighObjects()		const	{ return mObjects;							}
		inline_	Prunable**			GetMediumObjects()		const	{ return mObjects + mNbHigh;				}
		inline_	Prunable**			GetLowObjects()			const	{ return mObjects + mNbHigh + mNbMedium;	}
		inline_	Prunable**			GetHeadObjects()		const	{ return mObjects;							}
		inline_	Prunable**			GetTailObjects()		const	{ return mObjects + mNbHigh;				}
#endif
		inline_	udword				GetNbActiveObjects()	const
									{
#ifdef OPC_PRUNING_SECTIONS
										// "Low" objects aren't added to the database, so subsequent queries won't ever return them.
										return GetNbHeadObjects();
#else
										return GetNbObjects();
#endif
									}

		//! WARNING: Handle that one with care: boxes are lazy-evaluated so some of them may not
		//! be up-to-date !
		inline_	const AABB*			GetCurrentWorldBoxes()	const	{ return mWorldBoxes;						}
#ifdef OPC_PRUNING_SECTIONS
		inline_	const AABB*			GetHighBoxes()			const	{ return mWorldBoxes;						}
		inline_	const AABB*			GetMediumBoxes()		const	{ return mWorldBoxes + mNbHigh;				}
		inline_	const AABB*			GetLowBoxes()			const	{ return mWorldBoxes + mNbHigh + mNbMedium;	}
		inline_	const AABB*			GetHeadBoxes()			const	{ return mWorldBoxes;						}
		inline_	const AABB*			GetTailBoxes()			const	{ return mWorldBoxes + mNbHigh;				}
#endif

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Gets the world AABB of a given object.
		 *	- that world AABB is cached within the pruning engine, so you don't need to recompute it on your own.
		 *
		 *	\param		object	[in] the object to update
		 *	\return		the object's world AABB
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		inline_	const AABB*			GetWorldAABB(const Prunable& object)
									{
										// Checkings
										if(object.mHandle==INVALID_PRUNING_HANDLE)	return null;	// the object has not been added to the engine

										ASSERT(object.mEngine==this);

										// Lazy-rebuild the world box
										if(!object.IsSet(PRN_VALIDAABB))
										{
											// Direct validation bypasses callbacks
											const_cast<Prunable*>(&object)->mPRNFlags|=PRN_VALIDAABB;

											object.GetWorldAABB(mWorldBoxes[object.mHandle]);
										}

										// return cached box
										return &mWorldBoxes[object.mHandle];
									}

		inline_	const AABB*			GetCurrentWorldAABB(const Prunable& object)
									{
										// Checkings
										if(object.mHandle==INVALID_PRUNING_HANDLE)	return null;	// the object has not been added to the engine

										// return cached box
										return &mWorldBoxes[object.mHandle];
									}
		private:
#ifdef OPC_PRUNING_SECTIONS
				udword				mNbHigh, mNbMedium, mNbLow;
#endif
				uword				mNbObjects;		//!< Current number of objects
				uword				mMaxNbObjects;	//!< Max. number of objects
		// Note: using 2 different dynamic arrays instead of a collection of (AABB, Prunable*)
		// allows one to access a linear list of AABBs. At some point it was handy.
				AABB*				mWorldBoxes;	//!< List of world boxes
				Prunable**			mObjects;		//!< List of objects
		// Internal methods
				bool				Resize();
	};

#endif // ICEPRUNINGPOOL_H

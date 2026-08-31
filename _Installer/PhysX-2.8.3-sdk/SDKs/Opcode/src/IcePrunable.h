///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains .
 *	\file		IcePrunable.h
 *	\author		Pierre Terdiman
 *	\date		November, 15, 2001
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef ICEPRUNABLE_H
#define ICEPRUNABLE_H

	#define INVALID_PRUNING_HANDLE	0xffff	// WARNING: don't use -1 as if fails on some comparisons

	// Forward declarations
#ifdef OPC_PRUNING_POOL_OWNER
	class PruningPool;
#else
	class Pruner;
#endif

	enum PruningFlag
	{
		// Hidden flag added since the pruner needs to know the hide status to handle occluders correctly
		PRN_HIDDEN			= (1<<0),		//!< Object is hidden and never displayed

		PRN_VALIDAABB		= (1<<1),		//!< Object's cached AABB is valid

		// Dynamic flag removed since it's now captured by the PruningType.
		//PRN_DYNAMIC		= (1<<X),

		// Clip flag removed since it's not an object's property, it's all relative to current camera.
		// And hence, it breaks with multiple cams.
		//PRN_CLIP			= (1<<X),

		PRN_READONLY		= PRN_VALIDAABB,

		PRN_FORCE_DWORD		= 0x7fffffff
	};

	enum PruningType
	{
		PRT_STATIC_AABB_TREE,	//!< Static type	=> kept in a static AABB-tree
		PRT_DYNAMIC_AABB_TREE,	//!< Dynamic type	=> kept in a dynamic AABB-tree
		PRT_DYNAMIC_OCTREE,		//!< Dynamic type	=> kept in a loose octree
		PRT_DYNAMIC_QUADTREE,	//!< Dynamic type	=> kept in a loose quadtree
		PRT_FREE,				//!< Free type		=> kept in a linear list

		PRT_LAST,

		PRT_FORCE_DWORD		= 0x7fffffff
	};

	ICE_COMPILE_TIME_ASSERT(PRT_LAST<256);

	enum PruningQueryFlag
	{
		PQF_STATIC			= (1<<0),	//!< WARNING: must be 1<<0
		PQF_DYNAMIC			= (1<<1),	//!< WARNING: must be 1<<1
		PQF_FIRST_CONTACT	= (1<<2),
	};

	enum PruningSection
	{
		PRP_HIGH,			//!< High part of pruning pool
		PRP_MEDIUM,			//!< Medium part of pruning pool
		PRP_LOW,			//!< Low part of pruning pool

		PRP_LAST,

		PRP_FORCE_DWORD		= 0x7fffffff
	};

	ICE_COMPILE_TIME_ASSERT(PRP_LAST<256);

	typedef void				(*SphereCallback)	(void* user_data, Sphere& sphere);
	typedef void				(*AABBCallback)		(void* user_data, AABB& box);
#ifdef OPC_PRUNING_HULL
	typedef const CullingHull*	(*HullCallback)		(void* user_data);
#endif

	class OPCODE_API PruningInterface
	{
		public:
		// Constructor / Destructor
		inline_						PruningInterface() :
										mUserData(null)						{}
		inline_						~PruningInterface()						{}
		// Callback settings
		inline_	void				SetCallback(SphereCallback callback)	{ mSphereCallback = callback;	}
		inline_	void				SetCallback(AABBCallback callback)		{ mAABBCallback = callback;		}
#ifdef OPC_PRUNING_HULL
		inline_	void				SetCallback(HullCallback callback)		{ mHullCallback = callback;		}
#endif
		inline_	void				SetUserData(void* user_data)			{ mUserData = user_data;		}
		inline_	void*				GetUserData()					const	{ return mUserData;				}

		inline_	void				GetWorldSphere(Sphere& sphere)	const	{ if(mSphereCallback)	(mSphereCallback)(mUserData, sphere);	}
		inline_	void				GetWorldAABB(AABB& box)			const	{ if(mAABBCallback)		(mAABBCallback)(mUserData, box);		}
#ifdef OPC_PRUNING_HULL
		inline_	const CullingHull*	GetCullingHull()				const	{ return mHullCallback ? (mHullCallback)(mUserData) : null;		}
#endif
		private:
		// User callbacks
				void*				mUserData;			//!< User-defined data sent to callback
		static	SphereCallback		mSphereCallback;	//!< Object callback
		static	AABBCallback		mAABBCallback;		//!< Object callback
#ifdef OPC_PRUNING_HULL
		static	HullCallback		mHullCallback;		//!< Object callback
#endif
	};

	class OPCODE_API Prunable : public Allocateable, public PruningInterface
	{
		public:
									Prunable();
		virtual						~Prunable();

									DECLARE_FLAGS(PruningFlag, mPRNFlags, PRN_READONLY)

		inline_	bool				IsValid()	const	{ return mHandle!=INVALID_PRUNING_HANDLE;	}
#ifdef OPC_PRUNING_POOL_OWNER
		inline_	PruningPool*		GetPruner()			{ return mEngine;							}
#else
		inline_	Pruner*				GetPruner()			{ return mEngine;							}
#endif
		inline_	uword				GetHandle()	const	{ return mHandle;							}

		inline_	void				SetDynamic(bool flag)				{ mDynamic = flag;					}
		inline_	bool				IsDynamic()					const	{ return mDynamic;					}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Sets the pruning section.
		 *	\param		section	[in] pruning section
		 *	\return		true if success
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				bool				SetPruningSection(PruningSection section);
		inline_	PruningSection		GetPruningSection()			const	{ return PruningSection(mSection);	}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Gets the object's world AABB.
		 *	- that world AABB is cached within the pruning engine, so you don't need to recompute it on your own.
		 *
		 *	\return		the object's world AABB
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// WE CAN'T INLINE THAT BECAUSE OF INCLUDE ORDER...
				const AABB*			GetWorldBox();
				const AABB*			GetCurrentWorldBox();

		// Object is visible if (mCullingGroups & active_groups)!=0
		inline_	udword				GetCullingGroups()				const	{ return mCullingGroups;					}
		inline_	void				SetCullingGroups(udword mask)			{ mCullingGroups = mask;					}
		inline_	BOOL				IsActive(udword active_groups)	const	{ return mCullingGroups & active_groups;	}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Updates the object, i.e. updates the pruning engine's spatial database.
		 *	You must call this each time the object's bounding volume has been modified, which can happen:
		 *	- for dynamic meshes
		 *	- for moving static meshes
		 *
		 *	If the object has been declared dynamic (PRN_DYNAMIC) the update should be reasonably efficient - O(1)
		 *	Else it can be arbitrary slow as it hasn't been designed that way - so don't update static meshes!
		 *
		 *	\return		true if success
		 *	\warning	the app is responsible for calling this method!
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*		inline_	bool				UpdateObject()
									{
										// Checkings
										if(mHandle==INVALID_PRUNING_HANDLE)	return false;	// the object has not been added to the engine
//										if(mType>=PRT_LAST)					return false;	// invalid type
										if(!mEngine)						return false;	// no pruner available

										// Invalidate world box
										mPRNFlags&=~PRN_VALIDAABB;

										// Update appropriate pool
										return mEngine->UpdateObject(*this);
									}
*/
// Octree/quadtree stuff
		inline_	Prunable*			GetPreviousObject()		const	{ return mPrevious;	}
		inline_	Prunable*			GetNextObject()			const	{ return mNext;		}
		inline_	void*				GetOwnerCell()			const	{ return mOwner;	}

		inline_	void				Clear()
									{
										mPrevious	= null;
										mNext		= null;
										mOwner		= null;
									}

//				void*				mUserData;			//!< User-defined data sent to callback
			// Linked list
				Prunable*			mPrevious;	//!< Previous object, or null for top of the list
				Prunable*			mNext;		//!< Next object, or null for bottom of the list

		// Owner cell
				void*				mOwner;		//!< Octree or quadtree cell containing those listed objects
//~Octree/quadtree stuff

		private:
#ifdef OPC_PRUNING_POOL_OWNER
				PruningPool*		mEngine;		//!< Shortcut to owner [could be removed to save some ram]
#else
				Pruner*				mEngine;		//!< Shortcut to owner [could be removed to save some ram]
#endif
				udword				mCullingGroups;	//!< Culling groups flags

				uword				mHandle;		//!< Index in the pruning engine's arrays
				ubyte				mSection;		//!< Actually a PruningSection, on a byte to save ram
				bool				mDynamic;

		// Internal methods
#ifndef OPC_PRUNING_POOL_OWNER
				bool				RemoveFromPruner();
#endif

		friend	class				PruningEngine;
		friend	class				PruningPool;
	};

	enum StabResult
	{
		STAB_CONTINUE			= 0,
		STAB_STOP				= (1<<0),
		STAB_UPDATE_MAX_DIST	= (1<<1),
	};

	typedef udword		(*StabCallback)		(const Prunable* prunable, float& max_dist, void* user_data);

	class OPCODE_API PrunedObjects : public ContainerSizeT
	{
		public:
									PrunedObjects()						{}
									~PrunedObjects()					{}

		inline_	udword				GetNbPrunables()			const	{ return GetNbEntries();				}
		inline_	Prunable*			GetPrunable(udword i)				{ return (Prunable*)GetEntry(i);		}
		inline_	Prunable**			GetPrunables()						{ return (Prunable**)GetEntries();		}
		inline_	const Prunable*		GetPrunable(udword i)		const	{ return (const Prunable*)GetEntry(i);	}
		inline_	const Prunable**	GetPrunables()				const	{ return (const Prunable**)GetEntries();}

		inline_	void				ResetObjects()						{ Reset();								}

		inline_	void				AddPrunable(const Prunable* object)	{ Add((size_t)object);					};
	};

	class OPCODE_API CulledObjects : public ContainerSizeT
	{
		public:
									CulledObjects()						{}
									~CulledObjects()					{}

		inline_	udword				GetNbPrunables()			const	{ return GetNbEntries();				}
		// BEWARE of clip flags inside this pointers at bit position 0
		inline_	const size_t*		GetData()					const	{ return GetEntries();					}

		inline_	Prunable*			GetCulled(udword i)			const	{ return (Prunable*)(GetEntry(i)&~size_t(1));	}
		inline_	BOOL				IsClipped(udword i)			const	{ return (BOOL)(GetEntry(i)&1);					}
		inline_	void				DeleteCulled(udword index)			{ DeleteIndex(index);					}

		inline_	void				ResetObjects()						{ Reset();								}

		inline_	void				AddPrunable(const Prunable* object)
									{
										Add((size_t)object);
									}
		inline_	void				AddPrunable(const Prunable* object, bool clipped)
									{
										ASSERT(!(size_t(object)&1));
										Add(size_t(object)|size_t(clipped));
									}
	};

#endif // ICEPRUNABLE_H

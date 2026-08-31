///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains the pruning engine.
 *	\file		IcePruningEngine.h
 *	\author		Pierre Terdiman
 *	\date		November, 15, 2001
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef ICEPRUNINGENGINE_H
#define ICEPRUNINGENGINE_H

	enum PruningEngineFlag
	{
		PE_HULL_CULLING			= (1<<0),
		PE_FULL_BOX_BOX_TEST	= (1<<1),
		PE_FULL_PRIM_BOX_TEST	= (1<<2),
		PE_FIRST_CONTACT		= (1<<3),
		PE_NO_LEAF				= (1<<4),
		PE_QUANTIZED_TREE		= (1<<5),
		PE_TEMPORAL_COHERENCE	= (1<<6),
		PE_COLLISION_RESPONSE	= (1<<7),

		PE_FORCE_DWORD			= 0x7fffffff
	};

	/*
	This struct is passed to opcode queries, it contains tempory buffers used by the pruners. This is to allow
	different buffers to be used per thread. In particular for the static pruner.
	*/
	struct PruningTemps
		{
		PruningTemps()
			{
			mSCache.TouchedPrimitives	= &mBoxIndices;
			mACache.TouchedPrimitives	= &mBoxIndices;
			mLCache.TouchedPrimitives	= &mBoxIndices;
			mOCache.TouchedPrimitives	= &mBoxIndices;
			}
		Container			mBoxIndices;

		SphereCache			mSCache;
		AABBCache			mACache;
		OBBCache			mOCache;
		LSSCache			mLCache;

		// These are used by DynamicPruner::Cull, previously static.
		// We do not share memory and allow them to be dynamically sized as before.
		ContainerSizeT		VisibleBoxIndicesClip;
		ContainerSizeT		VisibleBoxIndicesNoClip;
		};

	struct PRUNERCREATE
	{
						PRUNERCREATE();

		AABB			mExpectedWorldBox;
		udword			mUpAxis;
		udword			mSubdivisionLevel;

		udword			mNbStaticObjects;		//!< expected number of static objects
		udword			mNbDynamicObjects;		//!< expected number of dynamic objects
		PruningType		mStaticType;			//!< expected type of static objects
		PruningType		mDynamicType;			//!< expected type of dynamic objects
	};

	class OPCODE_API PruningEngine : public Allocateable
	{
		public:
		// Constructor / Destructor
									PruningEngine();
									~PruningEngine();

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Pre-allocates pruners & pruning pools.
		 *	\param		create	[in] creation structure
		 *	\return		true if success
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				bool				Init(const PRUNERCREATE& create);

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Adds an object to the pruning engine.
		 *
		 *	\param		object	[in] the object to register
		 *	\return		true if success
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				bool				AddObject(Prunable& object);

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Removes an object from the pruning engine.
		 *
		 *	\param		object	[in] the object to remove
		 *	\return		true if success
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				bool				RemoveObject(Prunable& object);

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Updates an object, i.e. updates the pruning engine's spatial database.
		 *	You must call this each time the object's bounding volume has been modified, which can happen:
		 *	- for dynamic meshes
		 *	- for moving static meshes
		 *
		 *	If the object has been declared dynamic (PRN_DYNAMIC) the update should be reasonably efficient - O(1)
		 *	Else it can be arbitrary slow as it hasn't been designed that way - so don't update static meshes!
		 *
		 *	\param		object	[in] the object to update
		 *	\return		true if success
		 *	\warning	the app is responsible for calling this method!
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		inline_	bool				UpdateObject(Prunable& object)
									{
										// Checkings
										if(object.mHandle==INVALID_PRUNING_HANDLE)	return false;	// the object has not been added to the engine

										// Invalidate world box
										object.mPRNFlags &= ~PRN_VALIDAABB;
										
										// Update appropriate pool
										return mPruners[object.mDynamic]->UpdateObject(object);
									}

		// Data access
		inline_	Pruner*				GetStaticPruner()					{ return mPruners[0];	}
		inline_	const Pruner*		GetStaticPruner()			const	{ return mPruners[0];	}
		inline_	Pruner*				GetDynamicPruner()					{ return mPruners[1];	}
		inline_	const Pruner*		GetDynamicPruner()			const	{ return mPruners[1];	}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Gets the world AABB of a given object.
		 *	- that world AABB is cached within the pruning engine, so you don't need to recompute it on your own.
		 *
		 *	\param		object	[in] the object to update
		 *	\return		the object's world AABB
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		inline_	const AABB*			GetWorldBox(Prunable& object)		{ return mPruners[object.mDynamic]->GetWorldAABB(object);	}

				/* entries will always be NULL now, see Scene::prepareCaches() for details */
				void				InitSharedArrays(udword max_entries, udword* entries);
				
		/**
         * Force a rebuild of the aabb/loose octree etc to allow raycasting on multiple threads.
		*/
				void				EagerUpdatePruningTrees();

		// Queries dispatcher
				bool				Cull(PruningTemps& temps, CulledObjects& objects, const Plane* planes, udword nb_planes, BOOL show_hidden, bool setup_clip, udword flags, udword active_groups);
				bool				Stab(PruningTemps& temps, StabCallback callback, void* user_data, const Ray& ray, float max_dist,	udword flags, udword active_groups);
				bool				Overlap(PruningTemps& temps, PrunedObjects& objects, const Sphere& sphere,							udword flags, udword active_groups);
				bool				Overlap(PruningTemps& temps, PrunedObjects& objects, const AABB& box,								udword flags, udword active_groups);
				bool				Overlap(PruningTemps& temps, PrunedObjects& objects, const OBB& box,								udword flags, udword active_groups);
				bool				Overlap(PruningTemps& temps, PrunedObjects& objects, const LSS& capsule,							udword flags, udword active_groups);

		// Settings
				BOOL				mHullCulling;
		private:
				Pruner*				mPruners[2];	// 0 = static, 1 = dynamic
	};

#endif // ICEPRUNINGENGINE_H

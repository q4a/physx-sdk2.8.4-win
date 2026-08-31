///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains a base class for pruners.
 *	\file		IcePruner.h
 *	\author		Pierre Terdiman
 *	\date		November, 15, 2001
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef ICEPRUNER_H
#define ICEPRUNER_H

	enum CullingFlag
	{
		CUF_SHOW_HIDDEN		= (1<<0),
		CUF_SETUP_CLIP		= (1<<1),
		CUF_HULL_CULLING	= (1<<2),

		CUF_FORCE_DWORD		= 0x7fffffff
	};

	struct PruningTemps;
	struct PRUNERCREATE;

	class OPCODE_API Pruner : public PruningPool
	{
		public:
		// Constructor / destructor
									Pruner();
		virtual						~Pruner();

		virtual	bool				Setup(const PRUNERCREATE& create);

		// Data access
		inline_	const Signature&	GetSignature()					const	{ return mSignature;			}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Adds an object to the pruner.
		 *	\param		object	[in] the object to register
		 *	\return		true if success
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual	inline_ bool		AddObject(Prunable& object)
									{
										// Invalidate acceleration structure
										mSignature.Invalidate();
										// Add the object to the pool
										return PruningPool::AddObject(object);
									}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Removes an object from the pruner.
		 *	\param		object	[in] the object to remove
		 *	\return		true if success
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual	inline_ bool		RemoveObject(Prunable& object)
									{
										// Invalidate acceleration structure
										mSignature.Invalidate();
										// Remove the object from the pool
										PruningPool::RemoveObject(object);
										return true;
									}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Updates an object, i.e. updates the pruner's spatial database.
		 *	\param		object	[in] the object to update
		 *	\return		true if success
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual	inline_ bool		UpdateObject(Prunable& object)
									{
										// Invalidate acceleration structure
										mSignature.Invalidate();
										// ### to do lazy => mais pb avec GetWorldBoxes()
										// Compute and cache the new AABB
										return true;
									}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Initializes shared buffers. If not used, buffers are dynamic and not shared.
		 *	\param		max_entries	[in] expected max number of objects
		 *	\param		entries		[in] address of shared buffer
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/* entries will always be NULL now, see Scene::prepareCaches() for details */
		virtual	void				InitSharedArrays(udword max_entries, udword* entries)								{				}
		virtual void				EagerUpdatePruningTrees()															{				}

		// Queries
		virtual	bool				Cull(PruningTemps& temps, CulledObjects& objects, const Plane* planes, udword nb_planes, udword culling_flags, udword active_groups)	{ return false;	}
//		virtual	udword				Stab(PruningTemps& temps, PrunedObjects& objects, const Ray& ray, float& max_dist, bool first_contact, udword active_groups)			{ return STAB_CONTINUE;	}
		virtual	udword				Stab(PruningTemps& temps, StabCallback callback, void* user_data, const Ray& ray, float& max_dist, udword active_groups)				{ return STAB_CONTINUE;	}
		virtual	bool				Overlap(PruningTemps& temps, PrunedObjects& objects, const Sphere& sphere, bool first_contact, udword active_groups)					{ return false;	}
		virtual	bool				Overlap(PruningTemps& temps, PrunedObjects& objects, const AABB& box, bool first_contact, udword active_groups)							{ return false;	}
		virtual	bool				Overlap(PruningTemps& temps, PrunedObjects& objects, const OBB& box, bool first_contact, udword active_groups)							{ return false;	}
		virtual	bool				Overlap(PruningTemps& temps, PrunedObjects& objects, const LSS& capsule, bool first_contact, udword active_groups)						{ return false;	}

		protected:
				Signature			mSignature;		//!< Pruner's signature

		friend	class				PruningEngine;
//		friend	class				Prunable;
	};

#endif // ICEPRUNER_H

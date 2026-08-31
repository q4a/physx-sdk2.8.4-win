///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains code for a pruner of static objects.
 *	\file		IceStaticPruner.h
 *	\author		Pierre Terdiman
 *	\date		November, 15, 2001
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef ICESTATICPRUNER_H
#define ICESTATICPRUNER_H

//#define USE_STATIC_OPTIMIZED	// Probably painful because of triangle leaves?

	class OPCODE_API StaticPruner : public Pruner
	{
		public:
		// Constructor / destructor
												StaticPruner();
		virtual									~StaticPruner();

		override(Pruner)	bool				AddObject(Prunable& object);
		override(Pruner)	bool				RemoveObject(Prunable& object);
		override(Pruner)	bool				UpdateObject(Prunable& object);

		override(Pruner)	void				EagerUpdatePruningTrees();

		/* entries will always be NULL now, see Scene::prepareCaches() for details */
		override(Pruner)	void				InitSharedArrays(udword max_entries, udword* entries);

		// Queries
		override(Pruner)	bool				Cull(PruningTemps& temps, CulledObjects& objects, const Plane* planes, udword nb_planes, udword culling_flags, udword active_groups);
//		override(Pruner)	udword				Stab(PruningTemps& temps, PrunedObjects& objects, const Ray& ray, float& max_dist, bool first_contact, udword active_groups);
		override(Pruner)	udword				Stab(PruningTemps& temps, StabCallback callback, void* user_data, const Ray& ray, float& max_dist, udword active_groups);
		override(Pruner)	bool				Overlap(PruningTemps& temps, PrunedObjects& objects, const Sphere& sphere, bool first_contact, udword active_groups);
		override(Pruner)	bool				Overlap(PruningTemps& temps, PrunedObjects& objects, const AABB& box, bool first_contact, udword active_groups);
		override(Pruner)	bool				Overlap(PruningTemps& temps, PrunedObjects& objects, const OBB& box, bool first_contact, udword active_groups);
		override(Pruner)	bool				Overlap(PruningTemps& temps, PrunedObjects& objects, const LSS& capsule, bool first_contact, udword active_groups);

#ifdef USE_STATIC_OPTIMIZED
		inline_		const AABBNoLeafTree*		GetAABBTree()	const
#else
		inline_				const AABBTree*		GetAABBTree()	const
#endif
												{
													// Lazy build
													if(!mAABBTree)	const_cast<StaticPruner* const>(this)->BuildAABBTree();	// "mutable method"
													return mAABBTree;
												}

		inline_				const AABBTree*		HasAABBTree()	const	{ return mAABBTree;	}

		protected:
		// Acceleration structure
#ifdef USE_STATIC_OPTIMIZED
							AABBNoLeafTree*		mAABBTree;		//!< AABB tree for static objects (one we can refit easily)
#else
							AABBTree*			mAABBTree;		//!< AABB tree for static objects
#endif
		// Internal methods
							bool				BuildAABBTree();
							void				DumpTouchedEntities(const Container& array, PrunedObjects& objects, udword active_groups);
		virtual				void				PostBuildCallback() {}
	};

#endif // ICESTATICPRUNER_H

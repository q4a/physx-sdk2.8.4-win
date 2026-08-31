///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains code for a pruner of dynamic objects.
 *	\file		IceDynamicPruner.h
 *	\author		Pierre Terdiman
 *	\date		November, 15, 2001
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef ICEDYNAMICPRUNER_H
#define ICEDYNAMICPRUNER_H

	class OPCODE_API PRUNER_NAME : public Opcode::Pruner
	{
		public:
		// Constructor / destructor
												PRUNER_NAME();
		virtual									~PRUNER_NAME();

		override(Pruner)	bool				Setup(const Opcode::PRUNERCREATE& create);

		override(Pruner)	bool				AddObject(Opcode::Prunable& object);
		override(Pruner)	bool				RemoveObject(Opcode::Prunable& object);
		override(Pruner)	bool				UpdateObject(Opcode::Prunable& object);
		
		override(Pruner)	void				EagerUpdatePruningTrees();

		// Queries
		override(Pruner)	bool				Cull(Opcode::PruningTemps& temps, Opcode::CulledObjects& objects, const Plane* planes, udword nb_planes, udword culling_flags, udword active_groups);
//		override(Pruner)	udword				Stab(Opcode::PruningTemps& temps, PrunedObjects& objects, const Ray& ray, float& max_dist, bool first_contact, udword active_groups);
		override(Pruner)	udword				Stab(Opcode::PruningTemps& temps, Opcode::StabCallback callback, void* user_data, const Ray& ray, float& max_dist, udword active_groups);
		override(Pruner)	bool				Overlap(Opcode::PruningTemps& temps, Opcode::PrunedObjects& objects, const Sphere& sphere, bool first_contact, udword active_groups);
		override(Pruner)	bool				Overlap(Opcode::PruningTemps& temps, Opcode::PrunedObjects& objects, const AABB& box, bool first_contact, udword active_groups);
		override(Pruner)	bool				Overlap(Opcode::PruningTemps& temps, Opcode::PrunedObjects& objects, const OBB& box, bool first_contact, udword active_groups);
		override(Pruner)	bool				Overlap(Opcode::PruningTemps& temps, Opcode::PrunedObjects& objects, const LSS& capsule, bool first_contact, udword active_groups);

		inline_		const	TREE_CLASS_NAME*	GetOctree()		const
												{
													// Lazy build
													if(!mOctree)	const_cast<PRUNER_NAME* const>(this)->BuildLooseOctree();	// "mutable method"
													return mOctree;
												}
		protected:
							AABB				mExpectedWorldBox;
							udword				mUpAxis;
							udword				mSubdivisionLevel;
		// Acceleration structure
							TREE_CLASS_NAME*	mOctree;		//!< Loose octree for dynamic objects
		// Internal methods
							bool				BuildLooseOctree();
	};

#endif // ICEDYNAMICPRUNER_H

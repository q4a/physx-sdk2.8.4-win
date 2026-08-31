///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains code for a pruner of dynamic objects.
 *	\file		IceDynamicPruner2.h
 *	\author		Pierre Terdiman
 *	\date		January, 13, 2003
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef ICEDYNAMICPRUNER2_H
#define ICEDYNAMICPRUNER2_H

	enum BuildStatus
	{
		BUILD_NOT_STARTED,
		BUILD_INIT,
		BUILD_IN_PROGRESS,
		BUILD_FINISHED,

		BUILD_FORCE_DWORD	= 0xffffffff
	};

	class OPCODE_API DynamicPruner2 : public StaticPruner
	{
		public:
		// Constructor / destructor
												DynamicPruner2();
		virtual									~DynamicPruner2();

		override(Pruner)	bool				AddObject(Prunable& object);
		override(Pruner)	bool				RemoveObject(Prunable& object);
		override(Pruner)	bool				UpdateObject(Prunable& object);

		override(Pruner)	void				EagerUpdatePruningTrees();

		// Queries
		override(Pruner)	bool				Cull(PruningTemps& temps, CulledObjects& objects, const Plane* planes, udword nb_planes, udword culling_flags, udword active_groups);
//		override(Pruner)	udword				Stab(PruningTemps& temps, PrunedObjects& objects, const Ray& ray, float& max_dist, bool first_contact, udword active_groups);
		override(Pruner)	udword				Stab(PruningTemps& temps, StabCallback callback, void* user_data, const Ray& ray, float& max_dist, udword active_groups);
		override(Pruner)	bool				Overlap(PruningTemps& temps, PrunedObjects& objects, const Sphere& sphere, bool first_contact, udword active_groups);
		override(Pruner)	bool				Overlap(PruningTemps& temps, PrunedObjects& objects, const AABB& box, bool first_contact, udword active_groups);
		override(Pruner)	bool				Overlap(PruningTemps& temps, PrunedObjects& objects, const OBB& box, bool first_contact, udword active_groups);
		override(Pruner)	bool				Overlap(PruningTemps& temps, PrunedObjects& objects, const LSS& capsule, bool first_contact, udword active_groups);

		inline_				void				SetRebuildRateHint(udword nbStepsForRebuild) { ASSERT(nbStepsForRebuild > 3); mRebuildRateHint = (nbStepsForRebuild-3); mAdaptiveRebuildTerm = 0; }
																								// Besides the actual rebuild steps, 3 additional steps are needed.

		inline_				udword				GetNbMappingEntries()	const	{ return mNbMappingEntries;	}
		inline_				const udword*		GetMapping()			const	{ return mMapping;			}
		inline_				udword*				GetMapping()					{ return mMapping;			}
		inline_				bool				AllowRecording()		const	{ return mAllowRecord;		}
		inline_				const Container&	GetRecords()			const	{ return mRecorded;			}
		inline_				Container&			GetRecords()					{ return mRecorded;			}

							udword*				ResizeMapping(udword new_index);

							bool				CheckTree()		const;
							bool				CheckMapping()	const;

		//CA: SPU raycasts
		inline_				Prunable**			GetAddedPrunables()		const	{ return (Prunable**)mAdded.GetPrunables(); }
		inline_				udword				GetNbAddedPrunables()	const	{ return mAdded.GetNbPrunables(); }

		protected:
						AABBTreeOfAABBsBuilder	mBuilder;
							AABBTree*			mNewTree;
							AABB*				mCachedBoxes;
							udword				mNbCachedBoxes;
							BuildStatus			mProgress;
							udword				mNbMappingEntries;
							udword*				mMapping;
							udword				mNbCalls;
#ifndef SUPPORT_REFIT_BITMASK
							Container			mModifiedIndices;
#endif
							Container			mRecorded;		// Records position changes of pruning objects which result from adding/removing objects.
																// These changes have to be recorded and applied to the new tree when it is ready.
							PrunedObjects		mAdded;			// New objects are not directly added to the tree. They are kept in a list until a new tree is built.
							udword				mAddedSize;		// When the build of a new tree is initiated, this member records the current size of the added objects list.
																// All the objects in the list will be part of the new tree, objects which are added later will have to wait for the next tree build.
							bool				mAllowRecord;
							bool				mAllowBuild;	// A new AABB tree is built only if an object was added, removed or updated

							udword				mRebuildRateHint;		// Fraction of the total number of primitives that should be updated per step.
							udword				mTotalWorkUnits;		// Estimate for how much work has to be done to rebuild the tree.
							sdword				mAdaptiveRebuildTerm;	// Term to correct the work unit estimate if the rebuild rate is not matched.
		// Internal methods
							void				Release();
							void				Refit();
							void				ComputeMapping();
		virtual				void				PostBuildCallback() { ComputeMapping();	}
	};

#endif // ICEDYNAMICPRUNER2_H

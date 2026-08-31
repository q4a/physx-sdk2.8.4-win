///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains code for a pruner of free objects.
 *	\file		IceFreePruner.h
 *	\author		Pierre Terdiman
 *	\date		November, 15, 2001
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef ICEFREEPRUNER_H
#define ICEFREEPRUNER_H

	bool CullObjects(Prunable** objects, udword nb, CulledObjects& dest, const Plane* planes, udword nb_planes, udword culling_flags, udword active_groups);
	udword StabObjects(Prunable** objects, udword nb, StabCallback callback, void* user_data, const Ray& ray, float& max_dist, udword active_groups);
	bool OverlapObjects(Prunable** Objects, udword Nb, PruningTemps& temps, PrunedObjects& objects, const Sphere& sphere, bool first_contact, udword active_groups);
	bool OverlapObjects(Prunable** Objects, udword Nb, PruningTemps& temps, PrunedObjects& objects, const AABB& box, bool first_contact, udword active_groups);
	bool OverlapObjects(Prunable** Objects, udword Nb, PruningTemps& temps, PrunedObjects& objects, const OBB& box, bool first_contact, udword active_groups);
	bool OverlapObjects(Prunable** Objects, udword Nb, PruningTemps& temps, PrunedObjects& objects, const LSS& capsule, bool first_contact, udword active_groups);

	bool OBBOBBOverlap(const Point& e0, const Point& c0, const Matrix3x3& r0, const Point& e1, const Point& c1, const Matrix3x3& r1, bool full_test);
	inline_ bool Intersect(const OBB& obb, const AABB& aabb)
	{
		Point Center;
		Point Extents;
		aabb.GetCenter(Center);
		aabb.GetExtents(Extents);
		Matrix3x3 Idt;	Idt.Identity();
		return OBBOBBOverlap(obb.mExtents, obb.mCenter, obb.mRot, Extents, Center, Idt, true);
	}

	class OPCODE_API FreePruner : public Pruner
	{
		public:
		// Constructor / destructor
												FreePruner();
		virtual									~FreePruner();

		// Queries
		override(Pruner)	bool				Cull(PruningTemps& temps, CulledObjects& objects, const Plane* planes, udword nb_planes, udword culling_flags, udword active_groups);
//		override(Pruner)	udword				Stab(PruningTemps& temps, PrunedObjects& objects, const Ray& ray, float& max_dist, bool first_contact, udword active_groups);
		override(Pruner)	udword				Stab(PruningTemps& temps, StabCallback callback, void* user_data, const Ray& ray, float& max_dist, udword active_groups);
		override(Pruner)	bool				Overlap(PruningTemps& temps, PrunedObjects& objects, const Sphere& sphere, bool first_contact, udword active_groups);
		override(Pruner)	bool				Overlap(PruningTemps& temps, PrunedObjects& objects, const AABB& box, bool first_contact, udword active_groups);
		override(Pruner)	bool				Overlap(PruningTemps& temps, PrunedObjects& objects, const OBB& box, bool first_contact, udword active_groups);
		override(Pruner)	bool				Overlap(PruningTemps& temps, PrunedObjects& objects, const LSS& capsule, bool first_contact, udword active_groups);

		// Note: free objects don't have a corresponding acceleration structure
	};

#endif // ICEFREEPRUNER_H

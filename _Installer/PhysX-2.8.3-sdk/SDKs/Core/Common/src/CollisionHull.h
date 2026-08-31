// SUPPORT FILE FROM ICE, NEEDED FOR CONVEX-CONVEX - DO NOT TOUCH !'
//msvanfeldt: touched for hl/ll split

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains code for collision hulls (i.e. convex hulls used for collision detection)
 *	\file		IceCollisionHull.h
 *	\author		Pierre Terdiman
 *	\date		January, 29, 2000
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef ICECOLLISIONHULL_H
#define ICECOLLISIONHULL_H

#include "ConvexHull.h"
#include "Valency.h"

#include "px_collisionhull.h"

	class MESHMERIZER_API CollisionHull : public ConvexHull
	{
		public:
		// Constructor/Destructor
											CollisionHull();
		virtual								~CollisionHull();

		override(ConvexHull)	bool		Load(const Stream& stream);

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Computes hull's supporting vertex.
		 *	\param		dir		[in] separating vector
		 *	\param		id		[in] starting index
		 *	\return		supporting vertex's ID
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		inline_					udword		ComputeSupportingVertex(const Point& dir, udword id)	const
											{
												// "Hill climbing"
												if(LocalSearch(id, dir, GetVerts(), &mValencies))	return id;
												else												return INVALID_ID;
/*
												float Max = MIN_FLOAT;
												udword sv=INVALID_ID;
												udword NbVerts = GetNbVerts();
												const Point* v= GetVerts();
												for(udword i=0;i<NbVerts;i++)
												{
													float dp = dir|v[i];
													if(dp>Max)
													{
														Max=dp;
														sv = i;
													}
												}
												return sv;*/
/*
												// Test to fix "Bruno's bug"
												udword* Stamps = (udword*)StackAlloc(GetNbVerts()*sizeof(udword));
												memset(Stamps, 0, GetNbVerts()*sizeof(udword));
												if(LocalSearchTimestamps(id, dir, GetVerts(), mValencies, 1, Stamps))	return id;
												else												return INVALID_ID;*/
											}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Computes hull's tight AABB.
		 *	\param		box		[out] tight AABB around the hull
		 *	\param		world	[in] current world matrix (or null for identity)
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
								void		ComputeAABB(AABB& box, const Matrix4x4* world)			const;

		// Data access
						const	Valencies*	GetValencies()	const	{ return &mValencies;	}
//		protected:
						PxCollisionHullData	mCollisionHullData;
								Valencies	mValencies;
						
		mutable					udword		mSupport[6];

		friend class CollisionHullBuilder;
	};

#endif	// ICECOLLISIONHULL_H


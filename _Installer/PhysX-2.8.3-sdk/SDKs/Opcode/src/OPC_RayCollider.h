///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 *	OPCODE - Optimized Collision Detection
 *	Copyright (C) 2001 Pierre Terdiman
 *	Homepage: http://www.codercorner.com/Opcode.htm
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains code for a ray collider.
 *	\file		OPC_RayCollider.h
 *	\author		Pierre Terdiman
 *	\date		June, 2, 2001
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef OPC_RAYCOLLIDER_H
#define OPC_RAYCOLLIDER_H

#include "OPC_Simd.h"

	class OPCODE_API CollisionFace
	{
		public:
		//! Constructor
		inline_				CollisionFace()			{}
		//! Destructor
		inline_				~CollisionFace()		{}

				udword		mFaceID;				//!< Index of touched face
				float		mDistance;				//!< Distance from collider to hitpoint
				float		mU, mV;					//!< Impact barycentric coordinates
	};

	class OPCODE_API CollisionFaces : public Container
	{
		public:
		//! Constructor
										CollisionFaces()						{}
		//! Destructor
										~CollisionFaces()						{}

		inline_	udword					GetNbFaces()					const	{ return GetNbEntries()>>2;											}
		inline_	const CollisionFace*	GetFaces()						const	{ return (const CollisionFace*)GetEntries();						}

		inline_	void					Reset()									{ Container::Reset();												}

		inline_	void					AddFace(const CollisionFace& face)		{ Add((const udword*)&face, sizeof(CollisionFace)/sizeof(udword));	}
	};

#ifdef OPC_RAYHIT_CALLBACK
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/**
	 *	User-callback, called by OPCODE to record a hit. You can use this to implement custom collision filters.
	 *	\param		hit			[in] current hit
	 *	\param		user_data	[in] user-defined data from SetCallback()
	 */
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	typedef void	(*HitCallback)	(const CollisionFace& hit, void* user_data);
#endif
	typedef udword	(*BoxHitCallback)	(const udword* indices, udword nb_indices, float& max_dist, void* user_data);

	class OPCODE_API RayCollider : public Collider
	{
		public:
		// Constructor / Destructor
											RayCollider();
		virtual								~RayCollider();

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Generic stabbing query for generic OPCODE models. After the call, access the results:
		 *	- with GetContactStatus()
		 *	- in the user-provided destination array
		 *
		 *	\param		world_ray		[in] stabbing ray in world space
		 *	\param		model			[in] Opcode model to collide with
		 *	\param		world			[in] model's world matrix, or null
		 *	\param		cache			[in] a possibly cached face index, or null
		 *	\return		true if success
		 *	\warning	SCALE NOT SUPPORTED. The matrices must contain rotation & translation parts only.
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
							bool			Collide(const Ray& world_ray, const Model& model, const Matrix4x4* world=null, udword* cache=null);
		//
							bool			Collide(const Ray& world_ray, const AABBTree* tree, Container& box_indices);
							bool			ClosestHit(const Ray& world_ray, const AABBTree* tree, BoxHitCallback callback, void* user_data);
		// Settings

#ifndef OPC_RAYHIT_CALLBACK
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Settings: enable or disable "closest hit" mode.
		 *	\param		flag		[in] true to report closest hit only
		 *	\see		SetCulling(bool flag)
		 *	\see		SetMaxDist(float max_dist)
		 *	\see		SetDestination(StabbedFaces* sf)
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		inline_				void			SetClosestHit(bool flag)				{ mClosestHit	= flag;		}
#endif
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Settings: enable or disable backface culling.
		 *	\param		flag		[in] true to enable backface culling
		 *	\see		SetClosestHit(bool flag)
		 *	\see		SetMaxDist(float max_dist)
		 *	\see		SetDestination(StabbedFaces* sf)
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		inline_				void			SetCulling(bool flag)					{ mCulling		= flag;		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Settings: sets the higher distance bound.
		 *	\param		max_dist	[in] higher distance bound. Default = maximal value, for ray queries (else segment)
		 *	\see		SetClosestHit(bool flag)
		 *	\see		SetCulling(bool flag)
		 *	\see		SetDestination(StabbedFaces* sf)
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		inline_				void			SetMaxDist(float max_dist=MAX_FLOAT)	{ mMaxDist		= max_dist;	}

#ifdef OPC_GEOM_EPSILON
		inline_				void			SetGeomEpsilon(float epsilon=0.0f)		{ mGeomEpsilon	= epsilon;	}
#endif

#ifdef OPC_RAYHIT_CALLBACK
		inline_				void			SetHitCallback(HitCallback cb)			{ mHitCallback	= cb;			}
		inline_				void			SetUserData(void* user_data)			{ mUserData		= user_data;	}
#else
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Settings: sets the destination array for stabbed faces.
		 *	\param		cf			[in] destination array, filled during queries
		 *	\see		SetClosestHit(bool flag)
		 *	\see		SetCulling(bool flag)
		 *	\see		SetMaxDist(float max_dist)
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		inline_				void			SetDestination(CollisionFaces* cf)		{ mStabbedFaces	= cf;		}
#endif
		// Stats
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Stats: gets the number of Ray-BV overlap tests after a collision query.
		 *	\see		GetNbRayPrimTests()
		 *	\see		GetNbIntersections()
		 *	\return		the number of Ray-BV tests performed during last query
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		inline_				udword			GetNbRayBVTests()				const	{ return mNbRayBVTests;		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Stats: gets the number of Ray-Triangle overlap tests after a collision query.
		 *	\see		GetNbRayBVTests()
		 *	\see		GetNbIntersections()
		 *	\return		the number of Ray-Triangle tests performed during last query
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		inline_				udword			GetNbRayPrimTests()				const	{ return mNbRayPrimTests;	}

		// In-out test
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Stats: gets the number of intersection found after a collision query. Can be used for in/out tests.
		 *	\see		GetNbRayBVTests()
		 *	\see		GetNbRayPrimTests()
		 *	\return		the number of valid intersections during last query
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		inline_				udword			GetNbIntersections()			const	{ return mNbIntersections;	}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Validates current settings. You should call this method after all the settings and callbacks have been defined for a collider.
		 *	\return		null if everything is ok, else a string describing the problem
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		override(Collider)	const char*		ValidateSettings();

		protected:
		// Ray in local space
							Point			mOrigin;			//!< Ray origin
							Point			mDir;				//!< Ray direction (normalized)
							Point			mFDir;				//!< fabsf(mDir)
							Point			mData, mData2;
		// Stabbed faces
							CollisionFace	mStabbedFace;		//!< Current stabbed face
#ifdef OPC_RAYHIT_CALLBACK
							HitCallback		mHitCallback;		//!< Callback used to record a hit
							void*			mUserData;			//!< User-defined data
#else
							CollisionFaces*	mStabbedFaces;		//!< List of stabbed faces
#endif
		// Stats
							udword			mNbRayBVTests;		//!< Number of Ray-BV tests
							udword			mNbRayPrimTests;	//!< Number of Ray-Primitive tests
		// In-out test
							udword			mNbIntersections;	//!< Number of valid intersections
		// Dequantization coeffs
							Point			mCenterCoeff;
							Point			mExtentsCoeff;
		// Settings
							float			mMaxDist;			//!< Valid segment on the ray
#ifdef OPC_GEOM_EPSILON
							float			mGeomEpsilon;		//!< Geometry-dependent epsilon
#endif
#ifndef OPC_RAYHIT_CALLBACK
							bool			mClosestHit;		//!< Report closest hit only
#endif
							bool			mCulling;			//!< Stab culled faces or not
		// Internal methods
							void			TestAndShrink(udword prim_index);

	#ifdef OPC_SUPPORT_STACKLESS_TREE_NORMAL
							void			_SegmentClosestStab(const AABBStacklessCollisionNode* node, const AABBStacklessCollisionNode* const nodes);
                            void			_SegmentStab(const AABBStacklessCollisionNode* node, const AABBStacklessCollisionNode* const nodes);
							void			_RayStab(const AABBStacklessCollisionNode* node, const AABBStacklessCollisionNode* const nodes);
    #endif
    #ifdef OPC_SUPPORT_STACKLESS_TREE_QUANTIZED
							void            _SegmentClosestStab(const AABBStacklessQuantizedNode* node, const AABBStacklessQuantizedNode* const nodes);
                            void            _SegmentStab(const AABBStacklessQuantizedNode* node, const AABBStacklessQuantizedNode* const nodes);
							void			_RayStab(const AABBStacklessQuantizedNode* node, const AABBStacklessQuantizedNode* const nodes);
    #endif
    #ifdef OPC_SUPPORT_STACKLESS_TREE_NO_LEAF
							void			_SegmentClosestStab(const AABBStacklessNoLeafNode* node, const AABBStacklessNoLeafNode* const nodes);
                            void			_SegmentStab(const AABBStacklessNoLeafNode* node, const AABBStacklessNoLeafNode* const nodes);
							void			_RayStab(const AABBStacklessNoLeafNode* node, const AABBStacklessNoLeafNode* const nodes);
    #endif
    #ifdef OPC_SUPPORT_STACKLESS_TREE_NO_LEAF_QUANTIZED
							void			_SegmentClosestStab(const AABBStacklessQuantizedNoLeafNode* node, const AABBStacklessQuantizedNoLeafNode* const nodes);
                            void			_SegmentStab(const AABBStacklessQuantizedNoLeafNode* node, const AABBStacklessQuantizedNoLeafNode* const nodes);
							void			_RayStab(const AABBStacklessQuantizedNoLeafNode* node, const AABBStacklessQuantizedNoLeafNode* const nodes);
    #endif

							void			_SegmentStab(const AABBTreeNode* node, Container& box_indices);
							void			_SegmentClosestStab(const AABBTreeNode* node, BoxHitCallback callback, void* user_data);
							void			_RayStab(const AABBTreeNode* node, Container& box_indices);
			// Overlap tests
		inline_				BOOL			RayAABBOverlap(const Point& center, const Point& extents);

		inline_				BOOL			SegmentAABBOverlap(const Point& center, const Point& extents);
		inline_				BOOL			RayTriOverlap(const Point& vert0, const Point& vert1, const Point& vert2);

		inline_				BOOL			RayTriOverlap(const Point& vert0, const Point& vert1, const Point& vert2, CollisionFace &stabbedFace);

#ifdef OPC_SUPPORT_SIMD
		inline_				BOOL			RayAABBOverlap(const OpcSimd::Vector4& center, const OpcSimd::Vector4& extents);
		inline_				BOOL			RayAABBOverlap(const OpcSimd::Vector4& center, const OpcSimd::Vector4& extents,
															const OpcSimd::Vector4& origin, const OpcSimd::Vector4& dir,
															const OpcSimd::Vector4& dirYZX, const OpcSimd::Vector4& fDir,
															const OpcSimd::Vector4& fDirYZZ, const OpcSimd::Vector4& fDirXYX,
															const OpcSimd::Vector4& zero);
		inline_				BOOL			SegmentAABBOverlap(const OpcSimd::Vector4& center, const OpcSimd::Vector4& extents,
																const OpcSimd::Vector4& data,const OpcSimd::Vector4& data2,
																const OpcSimd::Vector4& fDir,const OpcSimd::Vector4& fDirYXX,const OpcSimd::Vector4& fDirZZY);
#endif
			// Init methods
							BOOL			InitQuery(const Ray& world_ray, const Matrix4x4* world=null, udword* face_id=null);

		inline_				void			SetupSegment()
											{
												// For Segment-AABB overlap
												mData = 0.5f * mDir * mMaxDist;
												mData2 = mOrigin + mData;

												// Precompute mFDir;
												mFDir.x = fabsf(mData.x);
												mFDir.y = fabsf(mData.y);
												mFDir.z = fabsf(mData.z);
											}
	};

#ifdef OPC_SUPPORT_HYBRID_MODELS

#define USE_SHRINKING_CALLBACK

	class OPCODE_API HybridRayCollider : public RayCollider
	{
		public:
		// Constructor / Destructor
											HybridRayCollider();
		virtual								~HybridRayCollider();

							bool			Collide(const Ray& world_ray, const HybridModel& model, const Matrix4x4* world=null, udword* cache=null);
		protected:

#ifdef USE_SHRINKING_CALLBACK
							void			TestLeaf(udword touched_box);
							void			TestLeafAndShrink(udword touched_box);
#else
							Container		mTouchedBoxes;
#endif

		// Internal methods
	#ifdef OPC_SUPPORT_STACKLESS_TREE_NORMAL
                            void			_SegmentStabNPT(const AABBStacklessCollisionNode* node, const AABBStacklessCollisionNode* const nodes);
							void			_RayStabNPT(const AABBStacklessCollisionNode* node, const AABBStacklessCollisionNode* const nodes);
    #endif
    #ifdef OPC_SUPPORT_STACKLESS_TREE_QUANTIZED
                            void            _SegmentStabNPT(const AABBStacklessQuantizedNode* node, const AABBStacklessQuantizedNode* const nodes);
							void			_RayStabNPT(const AABBStacklessQuantizedNode* node, const AABBStacklessQuantizedNode* const nodes);
    #endif
    #ifdef OPC_SUPPORT_STACKLESS_TREE_NO_LEAF
                            void			_SegmentStabNPT(const AABBStacklessNoLeafNode* node, const AABBStacklessNoLeafNode* const nodes);
							void			_RayStabNPT(const AABBStacklessNoLeafNode* node, const AABBStacklessNoLeafNode* const nodes);
    #endif
    #ifdef OPC_SUPPORT_STACKLESS_TREE_NO_LEAF_QUANTIZED
                            void			_SegmentStabNPT(const AABBStacklessQuantizedNoLeafNode* node, const AABBStacklessQuantizedNoLeafNode* const nodes);
							void			_RayStabNPT(const AABBStacklessQuantizedNoLeafNode* node, const AABBStacklessQuantizedNoLeafNode* const nodes);
    #endif

	};
#endif

#endif // OPC_RAYCOLLIDER_H

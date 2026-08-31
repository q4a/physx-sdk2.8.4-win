///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 *	OPCODE - Optimized Collision Detection
 *	Copyright (C) 2001 Pierre Terdiman
 *	Homepage: http://www.codercorner.com/Opcode.htm
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains code for an AABB collider.
 *	\file		OPC_AABBCollider.h
 *	\author		Pierre Terdiman
 *	\date		January, 1st, 2002
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef OPC_AABBCOLLIDER_H
#define OPC_AABBCOLLIDER_H

	struct OPCODE_API AABBCache : VolumeCache
	{
						AABBCache() : FatCoeff(1.1f)
						{
							FatBox.mCenter.Zero();
							FatBox.mExtents.Zero();
						}

		// Cached faces signature
		CollisionAABB	FatBox;		//!< Box used when performing the query resulting in cached faces
		// User settings
		float			FatCoeff;	//!< mRadius2 multiplier used to create a fat sphere
	};

	class OPCODE_API AABBCollider : public VolumeCollider
	{
		public:
		// Constructor / Destructor
											AABBCollider();
		virtual								~AABBCollider();

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Generic collision query for generic OPCODE models. After the call, access the results:
		 *	- with GetContactStatus()
		 *	- with GetNbTouchedPrimitives()
		 *	- with GetTouchedPrimitives()
		 *
		 *	\param		cache			[in/out] a box cache
		 *	\param		box				[in] collision AABB in world space
		 *	\param		model			[in] Opcode model to collide with
		 *	\return		true if success
		 *	\warning	SCALE NOT SUPPORTED. The matrices must contain rotation & translation parts only.
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
							bool			Collide(AABBCache& cache, const CollisionAABB& box, const Model& model);
		//
							bool			Collide(AABBCache& cache, const CollisionAABB& box, const AABBTree* tree);
		protected:
							CollisionAABB	mBox;			//!< Query box in (center, extents) form
							Point			mMin;			//!< Query box min point
							Point			mMax;			//!< Query box max point
		// Leaf description
							Point			mLeafVerts[3];	//!< Triangle vertices
		// Internal methods
	#ifdef OPC_SUPPORT_STACKLESS_TREE_NORMAL
							void            _Collide(const AABBStacklessCollisionNode* node, const AABBStacklessCollisionNode* const nodes);
                            void            _CollideNoPrimitiveTest(const AABBStacklessCollisionNode* node, const AABBStacklessCollisionNode* const nodes);
    #endif
    #ifdef OPC_SUPPORT_STACKLESS_TREE_QUANTIZED
							void            _Collide(const AABBStacklessQuantizedNode* node, const AABBStacklessQuantizedNode* const nodes);
                            void            _CollideNoPrimitiveTest(const AABBStacklessQuantizedNode* node, const AABBStacklessQuantizedNode* const nodes);
    #endif
    #ifdef OPC_SUPPORT_STACKLESS_TREE_NO_LEAF
							void            _Collide(const AABBStacklessNoLeafNode* node, const AABBStacklessNoLeafNode* const nodes);
                            void            _CollideNoPrimitiveTest(const AABBStacklessNoLeafNode* node, const AABBStacklessNoLeafNode* const nodes);
    #endif
    #ifdef OPC_SUPPORT_STACKLESS_TREE_NO_LEAF_QUANTIZED
							void            _Collide(const AABBStacklessQuantizedNoLeafNode* node, const AABBStacklessQuantizedNoLeafNode* const nodes);
                            void            _CollideNoPrimitiveTest(const AABBStacklessQuantizedNoLeafNode* node, const AABBStacklessQuantizedNoLeafNode* const nodes);
    #endif

							void			_Collide(const AABBTreeNode* node);
			// Overlap tests
#ifdef OPC_PERFORM_CONTAINMENT_TESTS
		inline_				BOOL			AABBContainsBox(const Point& bc, const Point& be);
		inline_				BOOL			AABBContainsBoxMinMax(const Point& bbMin, const Point& bbMax);
	#ifdef OPC_SUPPORT_SIMD
		inline_				BOOL			AABBContainsBoxMinMax(const OpcSimd::Vector4 &bbMin, const OpcSimd::Vector4 &bbMax);
	#endif
#endif
		inline_				BOOL			AABBAABBOverlap(const Point& b, const Point& Pb);
		inline_				BOOL			AABBAABBOverlapMinMax(const Point& bbMin, const Point& bbMax);
#ifdef OPC_SUPPORT_SIMD
		inline_				BOOL			AABBAABBOverlap(const OpcSimd::Vector4 &extents, const OpcSimd::Vector4 &center);
		inline_				BOOL			AABBAABBOverlapMinMax(const OpcSimd::Vector4& bbMin, const OpcSimd::Vector4& bbMax);
		inline_				BOOL			TriBoxOverlap(const OpcSimd::Vector4 &leafVerts0,const OpcSimd::Vector4 &leafVerts1,const OpcSimd::Vector4 &leafVerts2,
														const OpcSimd::Vector4 &center,const OpcSimd::Vector4 &extents);

#endif
		inline_				BOOL			TriBoxOverlap();
			// Init methods
							BOOL			InitQuery(AABBCache& cache, const CollisionAABB& box);
	};

#ifdef OPC_SUPPORT_HYBRID_MODELS
	class OPCODE_API HybridAABBCollider : public AABBCollider
	{
		public:
		// Constructor / Destructor
											HybridAABBCollider();
		virtual								~HybridAABBCollider();

							bool			Collide(AABBCache& cache, const CollisionAABB& box, const HybridModel& model);
		protected:
							Container		mTouchedBoxes;
	};
#endif

#endif // OPC_AABBCOLLIDER_H

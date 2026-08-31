///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 *	OPCODE - Optimized Collision Detection
 *	Copyright (C) 2001 Pierre Terdiman
 *	Homepage: http://www.codercorner.com/Opcode.htm
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains code for a sphere collider.
 *	\file		OPC_SphereCollider.h
 *	\author		Pierre Terdiman
 *	\date		June, 2, 2001
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef OPC_SPHERECOLLIDER_H
#define OPC_SPHERECOLLIDER_H

#include "OPC_Simd.h"

	struct OPCODE_API SphereCache : VolumeCache
	{
					SphereCache() : Center(0.0f,0.0f,0.0f), FatRadius2(0.0f), FatCoeff(1.1f)	{}
					~SphereCache()																{}

		// Cached faces signature
		Point		Center;		//!< Sphere used when performing the query resulting in cached faces
		float		FatRadius2;	//!< Sphere used when performing the query resulting in cached faces
		// User settings
		float		FatCoeff;	//!< mRadius2 multiplier used to create a fat sphere
	};

	class OPCODE_API SphereCollider : public VolumeCollider
	{
		public:
		// Constructor / Destructor
											SphereCollider();
		virtual								~SphereCollider();

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Generic collision query for generic OPCODE models. After the call, access the results:
		 *	- with GetContactStatus()
		 *	- with GetNbTouchedPrimitives()
		 *	- with GetTouchedPrimitives()
		 *
		 *	\param		cache			[in/out] a sphere cache
		 *	\param		sphere			[in] collision sphere in local space
		 *	\param		model			[in] Opcode model to collide with
		 *	\param		worlds			[in] sphere's world matrix, or null
		 *	\param		worldm			[in] model's world matrix, or null
		 *	\return		true if success
		 *	\warning	SCALE NOT SUPPORTED. The matrices must contain rotation & translation parts only.
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
							bool			Collide(SphereCache& cache, const Sphere& sphere, const Model& model, const Matrix4x4* worlds=null, const Matrix4x4* worldm=null);

		// 
							bool			Collide(SphereCache& cache, const Sphere& sphere, const AABBTree* tree);
		protected:
		// Sphere in model space
							Point			mCenter;			//!< Sphere center
							float			mRadius2;			//!< Sphere radius squared
							float			mRadius;			//!< Sphere radius
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
		inline_				BOOL			SphereContainsBox(const Point& bc, const Point& be);
	#ifdef OPC_SUPPORT_SIMD
		inline_				BOOL			SphereContainsBox(const OpcSimd::Vector4& bc, const OpcSimd::Vector4& be);
	#endif
#endif

#ifdef OPC_SUPPORT_SIMD
		inline_				BOOL			SphereAABBOverlap(const OpcSimd::Vector4& center, const OpcSimd::Vector4& extents);

		inline_				BOOL			LooseSphereTriOverlap(const Point& vert0, const Point& vert1, const Point& vert2,
																	const OpcSimd::Vector4 &center, const OpcSimd::Vector4 &radius,
																	const OpcSimd::Vector4 &sphereMin, const OpcSimd::Vector4 &sphereMax,
																	const OpcSimd::Vector4 &zero);
#endif
		inline_				BOOL			SphereAABBOverlap(const Point& center, const Point& extents);
							BOOL			SphereTriOverlap(const Point& vert0, const Point& vert1, const Point& vert2);
		inline_				BOOL			LooseSphereTriOverlap(const Point& vert0, const Point& vert1, const Point& vert2);

			// Init methods
							BOOL			InitQuery(SphereCache& cache, const Sphere& sphere, const Matrix4x4* worlds=null, const Matrix4x4* worldm=null);
	};

#ifdef OPC_SUPPORT_HYBRID_MODELS
	class OPCODE_API HybridSphereCollider : public SphereCollider
	{
		public:
		// Constructor / Destructor
											HybridSphereCollider();
		virtual								~HybridSphereCollider();

							bool			Collide(SphereCache& cache, const Sphere& sphere, const HybridModel& model, const Matrix4x4* worlds=null, const Matrix4x4* worldm=null);
		protected:
							Container		mTouchedBoxes;
	};
#endif

#endif // OPC_SPHERECOLLIDER_H

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 *	OPCODE - Optimized Collision Detection
 *	Copyright (C) 2001 Pierre Terdiman
 *	Homepage: http://www.codercorner.com/Opcode.htm
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains code for an OBB collider.
 *	\file		OPC_OBBCollider.h
 *	\author		Pierre Terdiman
 *	\date		January, 1st, 2002
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef OPC_OBBCOLLIDER_H
#define OPC_OBBCOLLIDER_H

	struct OPCODE_API OBBCache : VolumeCache
	{
					OBBCache() : FatCoeff(1.1f)
					{
						FatBox.mCenter.Zero();
						FatBox.mExtents.Zero();
						FatBox.mRot.Identity();
					}

		// Cached faces signature
		OBB				FatBox;		//!< Box used when performing the query resulting in cached faces
		// User settings
		float			FatCoeff;	//!< extents multiplier used to create a fat box
	};

	class OPCODE_API OBBCollider : public VolumeCollider
	{
		public:
		// Constructor / Destructor
											OBBCollider();
		virtual								~OBBCollider();

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Generic collision query for generic OPCODE models. After the call, access the results:
		 *	- with GetContactStatus()
		 *	- with GetNbTouchedPrimitives()
		 *	- with GetTouchedPrimitives()
		 *
		 *	\param		cache			[in/out] a box cache
		 *	\param		box				[in] collision OBB in local space
		 *	\param		model			[in] Opcode model to collide with
		 *	\param		worldb			[in] OBB's world matrix, or null
		 *	\param		worldm			[in] model's world matrix, or null
		 *	\return		true if success
		 *	\warning	SCALE NOT SUPPORTED. The matrices must contain rotation & translation parts only.
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
							bool			Collide(OBBCache& cache, const OBB& box, const Model& model, const Matrix4x4* worldb=null, const Matrix4x4* worldm=null);

							bool			Collide(OBBCache& cache, const OBB& box, const AABBTree* tree);

#ifdef OPC_SUPPORT_SWEEP_TESTS
							bool			SweepTest(const OBB& box, const Point& dir, float d, const Model& model, const Matrix4x4* worldm=null);
#endif

		// Settings

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Settings: select between full box-box tests or "SAT-lite" tests (where Class III axes are discarded)
		 *	\param		flag		[in] true for full tests, false for coarse tests
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		inline_				void			SetFullBoxBoxTest(bool flag)	{ mFullBoxBoxTest = flag;	}

		// Settings

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Validates current settings. You should call this method after all the settings and callbacks have been defined for a collider.
		 *	\return		null if everything is ok, else a string describing the problem
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		override(Collider)	const char*		ValidateSettings();

		protected:
		// Precomputed data
							Matrix3x3		mAR;				//!< Absolute rotation matrix
							Matrix3x3		mRModelToBox;		//!< Rotation from model space to obb space
							Matrix3x3		mRBoxToModel;		//!< Rotation from obb space to model space
							Point			mTModelToBox;		//!< Translation from model space to obb space
							Point			mTBoxToModel;		//!< Translation from obb space to model space

							Point			mBoxExtents;
							Point			mB0;				//!< - mTModelToBox + mBoxExtents
							Point			mB1;				//!< - mTModelToBox - mBoxExtents

							float			mBBx1;
							float			mBBy1;
							float			mBBz1;

							float			mBB_1;
							float			mBB_2;
							float			mBB_3;
							float			mBB_4;
							float			mBB_5;
							float			mBB_6;
							float			mBB_7;
							float			mBB_8;
							float			mBB_9;

#ifdef OPC_SUPPORT_SIMD
							OpcSimd::Vector4 mBBxyz1;

							OpcSimd::Vector4 mBB_123;
							OpcSimd::Vector4 mBB_456;
							OpcSimd::Vector4 mBB_789;
#endif

		// Leaf description
							Point			mLeafVerts[3];		//!< Triangle vertices
		// Settings
							bool			mFullBoxBoxTest;	//!< Perform full BV-BV tests (true) or SAT-lite tests (false)
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
                            void            _CollideNoPrimitiveTest(const AABBStacklessQuantizedNoLeafNode* node, const AABBStacklessQuantizedNoLeafNode* const nodes, bool quickTest = false);							
    #endif

							void			_Collide(const AABBTreeNode* node);

			// Overlap tests
#ifdef OPC_PERFORM_CONTAINMENT_TESTS
		inline_				BOOL			OBBContainsBox(const Point& bc, const Point& be);

	#ifdef OPC_SUPPORT_SIMD
		inline_				BOOL			OBBContainsBox(const OpcSimd::Vector4& bc, const OpcSimd::Vector4& be);
	#endif
#endif

#ifdef OPC_SUPPORT_SIMD
		inline_				BOOL			BoxBoxOverlap(const OpcSimd::Vector4& extents, const OpcSimd::Vector4& center);

		inline_				BOOL			BoxBoxOverlap(const OpcSimd::Vector4& extents, const OpcSimd::Vector4& center,
														  const OpcSimd::Vector4 &TBoxToModel, const OpcSimd::Vector4 &BB,
														  const OpcSimd::Vector4 &rBoxToModel_0, const OpcSimd::Vector4 &rBoxToModel_1, const OpcSimd::Vector4 &rBoxToModel_2,
														  const OpcSimd::Vector4 &ar_0, const OpcSimd::Vector4 &ar_1, const OpcSimd::Vector4 &ar_2,
														  const OpcSimd::Vector4 &thisExtents,
														  const OpcSimd::Vector4 &BB_123,	const OpcSimd::Vector4 &BB_456, const OpcSimd::Vector4 &BB_789);

		inline_				BOOL			TriBoxOverlap(const Point& vert0, const Point& vert1, const Point& vert2,
														  const OpcSimd::Vector4 &rot_0, const OpcSimd::Vector4 &rot_1, const OpcSimd::Vector4 &rot2, 
														  const OpcSimd::Vector4 &tran, const OpcSimd::Vector4 &extents);
		inline_				BOOL			OBBCollider::TriBoxOverlap(const Point& vert0, const Point& vert1, const Point& vert2,
														  const OpcSimd::Vector4 &tran, const OpcSimd::Vector4 &extents);
#endif
		inline_				BOOL			BoxBoxOverlap(const Point& extents, const Point& center);
		inline_				BOOL			TriBoxOverlap(const Point& vert0, const Point& vert1, const Point& vert2);


			// Init methods
							BOOL			InitQuery(OBBCache& cache, const OBB& box, const Matrix4x4* worldb=null, const Matrix4x4* worldm=null);
							//Perform additional setup for obb-obb tests
							void			InitTraversal();

#ifdef OPC_SUPPORT_SWEEP_TESTS
		public:
							Point			mDir;
							udword			mTouchedID;
							float			mMinDist;
							float			mOffset;
							OBB				mOriginalBox;
							Point			mProj;
							Point			mDP;
							BOOL			InitSweepQuery(const OBB& box, const Point& dir, float d);
		inline_				void			CreateOBB(float d);
							bool			TriBoxSweep();
	#ifdef OPC_SUPPORT_NORMAL_TREES
							void			_SweepTest(const AABBCollisionNode* node);
	#endif
	#ifdef OPC_SUPPORT_NOLEAF_TREES
							void			_SweepTest(const AABBNoLeafNode* node);
	#endif
	#ifdef OPC_SUPPORT_QUANTIZED_TREES
							void			_SweepTest(const AABBQuantizedNode* node);
	#endif
	#ifdef OPC_SUPPORT_QUANTIZED_NOLEAF_TREES
							void			_SweepTest(const AABBQuantizedNoLeafNode* node);
	#endif
#endif
	};

#ifdef OPC_SUPPORT_HYBRID_MODELS
	class OPCODE_API HybridOBBCollider : public OBBCollider
	{
		public:
		// Constructor / Destructor
											HybridOBBCollider();
		virtual								~HybridOBBCollider();

							bool			Collide(OBBCache& cache, const OBB& box, const HybridModel& model, const Matrix4x4* worldb=null, const Matrix4x4* worldm=null);
#ifdef OPC_SUPPORT_SIMD
							bool			Collide(OBBCache& cache, const OBB& box, const HybridModel& model, bool quickTest);
#endif
		protected:
							Container		mTouchedBoxes;
	};
#endif

#endif // OPC_OBBCOLLIDER_H

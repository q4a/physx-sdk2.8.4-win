///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 *	OPCODE - Optimized Collision Detection
 *	Copyright (C) 2001 Pierre Terdiman
 *	Homepage: http://www.codercorner.com/Opcode.htm
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains code for point distance queries.
 *	\file		OPC_PointDistance.h
 *	\author		Pierre Terdiman
 *	\date		December, 12, 2002
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef OPC_POINTDISTANCE_H
#define OPC_POINTDISTANCE_H

		class OPCODE_API PointDistanceQuery
		{
			public:
			// Constructor / Destructor
											PointDistanceQuery();
			virtual							~PointDistanceQuery();

							bool			ComputeDistance(const Point& world_pt, const Model* model,					const Matrix4x4* world=null, udword* cache=null);

#ifdef OPC_SUPPORT_STACKLESS_TREE_NORMAL
							bool			ComputeDistance(const Point& world_pt, const AABBStacklessCollisionTree* tree,		const Matrix4x4* world=null, udword* cache=null);
#endif
#ifdef OPC_SUPPORT_STACKLESS_TREE_NO_LEAF
							bool			ComputeDistance(const Point& world_pt, const AABBStacklessNoLeafTree* tree,			const Matrix4x4* world=null, udword* cache=null);
#endif
#ifdef OPC_SUPPORT_STACKLESS_TREE_QUANTIZED
							bool			ComputeDistance(const Point& world_pt, const AABBStacklessQuantizedTree* tree,		const Matrix4x4* world=null, udword* cache=null);
#endif
#ifdef OPC_SUPPORT_STACKLESS_TREE_NO_LEAF_QUANTIZED
							bool			ComputeDistance(const Point& world_pt, const AABBStacklessQuantizedNoLeafTree* tree,	const Matrix4x4* world=null, udword* cache=null);
#endif
			inline_			float			GetDistance()			const	{ return mMinDist;	}
			inline_			udword			GetClosestTriangle()	const	{ return mPrim;		}
			inline_			float			GetU()					const	{ return mU;		}
			inline_			float			GetV()					const	{ return mV;		}

			private:
		// User mesh interface
					const	MeshInterface*	mIMesh;				//!< User-defined mesh interface
			// Dequantization coeffs
							Point			mCenterCoeff;
							Point			mExtentsCoeff;
			// Distance data
							Point			mLocalPt;
							float			mMinDist;
							float			mU, mV;
							udword			mPrim;
			// Internal methods
		inline_				void			DIST_PRIM(udword id);
#ifdef OPC_SUPPORT_STACKLESS_TREE_NORMAL
							void			_Distance(const AABBStacklessCollisionNode* node, const AABBStacklessCollisionNode* nodes);
#endif
#ifdef OPC_SUPPORT_STACKLESS_TREE_NO_LEAF
							void			_Distance(const AABBStacklessNoLeafNode* node, const AABBStacklessNoLeafNode* nodes);
#endif
#ifdef OPC_SUPPORT_STACKLESS_TREE_QUANTIZED
							void			_Distance(const AABBStacklessQuantizedNode* node, const AABBStacklessQuantizedNode* nodes);
#endif
#ifdef OPC_SUPPORT_STACKLESS_TREE_NO_LEAF_QUANTIZED
							void			_Distance(const AABBStacklessQuantizedNoLeafNode* node, const AABBStacklessQuantizedNoLeafNode* nodes);
#endif
							void			InitQuery(const Point& world_pt, const Matrix4x4* world=null, udword* face_id=null);

		inline_				BOOL			Setup(const MeshInterface* mi)
											{
												mIMesh = mi;
												return mIMesh!=null;
											}
		};

#endif // OPC_POINTDISTANCE_H

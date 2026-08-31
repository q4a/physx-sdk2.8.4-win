///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 *	OPCODE - Optimized Collision Detection
 *	Copyright (C) 2001 Pierre Terdiman
 *	Homepage: http://www.codercorner.com/Opcode.htm
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains code for a versatile AABB tree.
 *	\file		OPC_AABBTree.h
 *	\author		Pierre Terdiman
 *	\date		March, 20, 2001
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef OPC_AABBTREE_H
#define OPC_AABBTREE_H

#define KEEP_PARENT_PTR
#define SUPPORT_PROGRESSIVE_BUILDING
#define SUPPORT_REFIT_BITMASK
#define DWORD_REFIT_BITMASK

#ifdef DWORD_REFIT_BITMASK

	class ICECORE_API BitArray
	{
		public:
		//! Constructor
							BitArray();
							BitArray(udword nb_bits);
		//! Destructor
							~BitArray();

				bool		Init(udword nb_bits);

		// Data management
		inline_	void		SetBit(udword bit_number)
							{
//								mBits[bit_number>>5] |= BitMasks[bit_number&31];
								mBits[bit_number>>5] |= 1<<(bit_number&31);
							}
		inline_	void		ClearBit(udword bit_number)
							{
//								mBits[bit_number>>5] &= NegBitMasks[bit_number&31];
								mBits[bit_number>>5] &= ~(1<<(bit_number&31));
							}
		inline_	void		ToggleBit(udword bit_number)
							{
//								mBits[bit_number>>5] ^= BitMasks[bit_number&31];
								mBits[bit_number>>5] ^= 1<<(bit_number&31);
							}

		inline_	void		ClearAll()			{ ZeroMemory(mBits, mSize*4);			}
		inline_	void		SetAll()			{ FillMemory(mBits, mSize*4, 0xff);		}

		// Data access
		inline_	BOOL		IsSet(udword bit_number)	const
							{
//								return mBits[bit_number>>5] & BitMasks[bit_number&31];
								return mBits[bit_number>>5] & (1<<(bit_number&31));
							}

		inline_	const udword*		GetBits()	const	{ return mBits;	}
		inline_	udword				GetSize()	const	{ return mSize;	}

		protected:
				udword*		mBits;		//!< Array of bits
				udword		mSize;		//!< Size of the array in dwords
	};
#endif

#ifdef SUPPORT_PROGRESSIVE_BUILDING
	class FIFOStack2;
#endif

#ifdef OPC_NO_NEG_VANILLA_TREE
	//! TO BE DOCUMENTED
	#define IMPLEMENT_TREE(base_class, volume)																			\
		public:																											\
		/* Constructor / Destructor */																					\
									base_class();																		\
									~base_class();																		\
		/* Data access */																								\
		inline_	const volume&		Get##volume()	const	{ return mBV;							}					\
		inline_	volume&				Get##volume()			{ return mBV;							}					\
		/* Clear the last bit */																						\
		inline_	const base_class*	GetPos()		const	{ return (const base_class*)(mPos&~1);	}					\
		inline_	const base_class*	GetNeg()		const	{ const base_class* P = GetPos(); return P ? P+1 : null;}	\
																														\
		/* We don't need to test both nodes since we can't have one without the other	*/								\
		inline_	bool				IsLeaf()		const	{ return !GetPos();						}					\
																														\
		/* Stats */																										\
		inline_	udword				GetNodeSize()	const	{ return SIZEOFOBJECT;					}					\
		protected:																										\
		/* Tree-independent data */																						\
		/* Following data always belong to the BV-tree, regardless of what the tree actually contains.*/				\
		/* Whatever happens we need the two children and the enclosing volume.*/										\
				volume				mBV;		/* Global bounding-volume enclosing all the node-related primitives */	\
				size_t				mPos;		/* "Positive" & "Negative" children */
#else
	//! TO BE DOCUMENTED
	#define IMPLEMENT_TREE(base_class, volume)																			\
		public:																											\
		/* Constructor / Destructor */																					\
									base_class();																		\
									~base_class();																		\
		/* Data access */																								\
		inline_	const volume*		Get##volume()	const	{ return &mBV;							}					\
		/* Clear the last bit */																						\
		inline_	const base_class*	GetPos()		const	{ return (const base_class*)(mPos&~1);	}					\
		inline_	const base_class*	GetNeg()		const	{ return (const base_class*)(mNeg&~1);	}					\
																														\
/*		inline_	bool				IsLeaf()		const	{ return (!GetPos() && !GetNeg());	}	*/					\
		/* We don't need to test both nodes since we can't have one without the other	*/								\
		inline_	bool				IsLeaf()		const	{ return !GetPos();						}					\
																														\
		/* Stats */																										\
		inline_	udword				GetNodeSize()	const	{ return SIZEOFOBJECT;					}					\
		protected:																										\
		/* Tree-independent data */																						\
		/* Following data always belong to the BV-tree, regardless of what the tree actually contains.*/				\
		/* Whatever happens we need the two children and the enclosing volume.*/										\
				volume				mBV;		/* Global bounding-volume enclosing all the node-related primitives */	\
				size_t				mPos;		/* "Positive" child */													\
				size_t				mNeg;		/* "Negative" child */
#endif

//#ifdef OPCODE_USING_ICE
	typedef		void				(*CullingCallback)		(udword nb_primitives, udword* node_primitives, BOOL need_clipping, void* user_data);
//#endif

	class OPCODE_API AABBTreeNode : public Allocateable
	{
									IMPLEMENT_TREE(AABBTreeNode, AABB)
		public:
		// Data access
		inline_	const udword*		GetPrimitives()		const	{ return mNodePrimitives;	}
		inline_	udword				GetNbPrimitives()	const	{ return mNbPrimitives;		}

//#ifdef OPCODE_USING_ICE
				void				_TestAgainstPlanes(const Plane* planes, udword clip_mask, CullingCallback cb, void* user_data)	const;
				void				_TestAgainstPlanes(const Plane* planes, udword clip_mask, Container& box_indices_clip, Container& box_indices_noclip)	const;
#ifdef REMOVED
				void				_TestAgainstSegment(const Segment& segment, Container& box_indices)	const;
				void				_TestAgainstRay(const Ray& ray, Container& box_indices)	const;
				void				_TestAgainstSphere(const Sphere& sphere, Container& box_indices)	const;
#endif
//#endif

#ifdef OPC_USE_TREE_COHERENCE
				udword				mBitmask;
#endif
#ifdef KEEP_PARENT_PTR
				AABBTreeNode*		mParent;
#endif
//		protected:
		// Tree-dependent data
				udword*				mNodePrimitives;	//!< Node-related primitives (shortcut to a position in mIndices below)
				udword				mNbPrimitives;		//!< Number of primitives for this node
		// Internal methods
				udword				Split(udword axis, AABBTreeBuilder* builder);
				bool				Subdivide(AABBTreeBuilder* builder);
				void				_BuildHierarchy(AABBTreeBuilder* builder);
				void				_Refit(AABBTreeBuilder* builder);
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/**
	 *	User-callback, called for each node by the walking code.
	 *	\param		current		[in] current node
	 *	\param		depth		[in] current node's depth
	 *	\param		user_data	[in] user-defined data
	 *	\return		true to recurse through children, else false to bypass them
	 */
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	typedef		bool				(*WalkingCallback)	(const AABBTreeNode* current, udword depth, void* user_data);

	class OPCODE_API AABBTree : public Allocateable
	{
		public:
		// Constructor / Destructor
									AABBTree();
									~AABBTree();
		// Build
				bool				Build(AABBTreeBuilder* builder);
#ifdef SUPPORT_PROGRESSIVE_BUILDING
				udword				Build(AABBTreeBuilder* builder, udword progress, udword limit);
#endif
				void				Release();

		// Data access
		inline_	const udword*		GetIndices()		const	{ return mIndices;		}	//!< Catch the indices
		inline_	udword				GetNbNodes()		const	{ return mTotalNbNodes;	}	//!< Catch the number of nodes
		inline_	udword				GetTotalPrims()		const	{ return mTotalPrims;	}
		inline_	const AABBTreeNode*	GetNodes()			const	{ return mPool;			}

		// Infos
				bool				IsComplete()		const;
		// Stats
				udword				ComputeDepth()		const;
				udword				GetUsedBytes()		const;
				udword				Walk(WalkingCallback callback, void* user_data) const;
                void				Walk2(WalkingCallback callback, void* user_data) const;

				bool				Refit(AABBTreeBuilder* builder);
				bool				Refit2(AABBTreeBuilder* builder);
				bool				Refit3(udword nb_objects, const AABB* boxes, const Container& indices);
#ifdef SUPPORT_REFIT_BITMASK
				void				MarkForRefit(udword index);
				void				RefitMarked(udword nb_objects, const AABB* boxes);
#endif

void Test();
void Test2();

		inline_	void				TestAgainstPlanes(const Plane* planes, udword clip_mask, CullingCallback cb, void* user_data)							const
									{
										mPool->_TestAgainstPlanes(planes, clip_mask, cb, user_data);
									}
		inline_	void				TestAgainstPlanes(const Plane* planes, udword clip_mask, Container& box_indices_clip, Container& box_indices_noclip)	const
									{
										mPool->_TestAgainstPlanes(planes, clip_mask, box_indices_clip, box_indices_noclip);
									}

		private:
				udword*				mIndices;			//!< Indices in the app list. Indices are reorganized during build (permutation).
				AABBTreeNode*		mPool;				//!< Linear pool of nodes for complete trees. Null otherwise. [Opcode 1.3]
#ifdef SUPPORT_REFIT_BITMASK
	#ifdef DWORD_REFIT_BITMASK
				BitArray			mRefitBitmask;
	#else
				bool*				mRefitBitmask;
	#endif
#endif
		// Stats
				udword				mTotalNbNodes;		//!< Number of nodes in the tree.
				udword				mTotalPrims;
#ifdef SUPPORT_PROGRESSIVE_BUILDING
				FIFOStack2*			mStack;
#endif

#ifdef OPC_USE_TREE_COHERENCE
				void				ComputeBitmasks();
#endif
	};

#endif // OPC_AABBTREE_H

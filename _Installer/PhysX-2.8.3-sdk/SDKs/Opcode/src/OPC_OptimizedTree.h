///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 *	OPCODE - Optimized Collision Detection
 *	Copyright (C) 2001 Pierre Terdiman
 *	Homepage: http://www.codercorner.com/Opcode.htm
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains code for optimized trees.
 *	\file		OPC_OptimizedTree.h
 *	\author		Pierre Terdiman
 *	\date		March, 20, 2001
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef OPC_OPTIMIZEDTREE_H
#define OPC_OPTIMIZEDTREE_H

#if 0
	// Let's keep this here since it is a potential 64bit fix (it used to be for the CELL)
# error this should be turned off now

	//! Common interface for a node of an implicit tree
	#define IMPLEMENT_IMPLICIT_NODE(base_class, volume)														\
		public:																								\
		/* Constructor / Destructor */																		\
		inline_								base_class()/* : mData(0)*/	{}									\
		inline_								~base_class()			{}										\
		/* Leaf test */																						\
		inline_			BOOL				IsLeaf()		const	{ return BOOL(mData&1);				}	\
		/* Data access */																					\
		inline_			const base_class*	GetPos()		const	{ return (base_class*)((uqword)mData)&0xFFFFFFFF);		}	\
		inline_			const base_class*	GetNeg()		const	{ return ((base_class*)((uqword)mData)&0xFFFFFFFF))+1;	}	\
		inline_			udword				GetPrimitive()	const	{ return udword(mData>>1);			}	\
		/* Stats */																							\
		inline_			udword				GetNodeSize()	const	{ return SIZEOFOBJECT;				}	\
																											\
						volume				mAABB;															\
						size_t				mData;

	//! Common interface for a node of a no-leaf tree
	#define IMPLEMENT_NOLEAF_NODE(base_class, volume)														\
		public:																								\
		/* Constructor / Destructor */																		\
		inline_								base_class()/* : mPosData(0), mNegData(0)*/	{}					\
		inline_								~base_class()							{}						\
		/* Leaf tests */																					\
		inline_			BOOL				HasPosLeaf()		const	{ return BOOL(mPosData&1);		}	\
		inline_			BOOL				HasNegLeaf()		const	{ return BOOL(mNegData&1);		}	\
		/* Data access */																					\
		inline_			const base_class*	GetPos()			const	{ return (base_class*)(((uqword)mPosData)&0xFFFFFFFF);	}	\
		inline_			const base_class*	GetNeg()			const	{ return (base_class*)(((uqword)mNegData)&0xFFFFFFFF);	}	\
		inline_			udword				GetPosPrimitive()	const	{ return udword(mPosData>>1);	}	\
		inline_			udword				GetNegPrimitive()	const	{ return udword(mNegData>>1);	}	\
		/* Stats */																							\
		inline_			udword				GetNodeSize()		const	{ return SIZEOFOBJECT;			}	\
																											\
						volume				mAABB;															\
						size_t				mPosData;														\
						size_t				mNegData;

//#else
#endif


//! Common interface for a collision tree
	#define IMPLEMENT_COLLISION_TREE(base_class, node)																\
		public:																										\
		/* Constructor / Destructor */																				\
													base_class();													\
		virtual										~base_class();													\
		/* Builds from a standard tree */																			\
		override(AABBOptimizedTree)	bool			Build(AABBTree* tree);											\
		/* Refits the tree */																						\
		override(AABBOptimizedTree)	bool			Refit(const MeshInterface* mesh_interface);						\
		/* Walks the tree */																						\
		override(AABBOptimizedTree)	bool			Walk(GenericWalkingCallback callback, void* user_data) const;	\
		/* Computes size */																							\
		override(AABBOptimizedTree)	udword			ComputeSize()				const;								\
		/* Saves the tree */																						\
		override(AABBOptimizedTree)	bool			Save(bool mismatch, CustomArray& array)	const;					\
		/* Loads the tree */																						\
		override(AABBOptimizedTree)	bool			Load(bool mismatch, const Stream& stream);						\
		/* Data access */																							\
		inline_						const node*		GetNodes()		const	{ return mNodes;					}	\
		/* Stats */																									\
		override(AABBOptimizedTree)	udword			GetUsedBytes()	const	{ return mNbNodes*sizeof(node);		}	\
		private:																									\
									node*			mNodes;

	typedef		bool				(*GenericWalkingCallback)	(const void* current, void* user_data);

	class OPCODE_API AABBOptimizedTree : public Allocateable
	{
		public:
		// Constructor / Destructor
											AABBOptimizedTree() :
												mNbNodes	(0)
																							{}
		virtual								~AABBOptimizedTree()							{}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Builds the collision tree from a generic AABB tree.
		 *	\param		tree			[in] generic AABB tree
		 *	\return		true if success
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual			bool				Build(AABBTree* tree)											= 0;

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Refits the collision tree after vertices have been modified.
		 *	\param		mesh_interface	[in] mesh interface for current model
		 *	\return		true if success
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual			bool				Refit(const MeshInterface* mesh_interface)						= 0;

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Walks the tree and call the user back for each node.
		 *	\param		callback	[in] walking callback
		 *	\param		user_data	[in] callback's user data
		 *	\return		true if success
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual			bool				Walk(GenericWalkingCallback callback, void* user_data) const	= 0;

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Computes expected size needed for serialization.
		 *	\return		expected size
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual			udword				ComputeSize()					const							= 0;

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Serializes the collision tree.
		 *	\param		mismatch	[in] true if big-little endian conversion has to be done
		 *	\param		array		[out] model data
		 *	\return		true if success
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual			bool				Save(bool mismatch, CustomArray& array)		const				= 0;

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Loads a precomputed collision model.
		 *	\param		mismatch	[in] true if big-little endian conversion has to be done
		 *	\param		array		[in] model data
		 *	\return		true if success
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		virtual			bool				Load(bool mismatch, const Stream& array)						= 0;

		// Data access
		virtual			udword				GetUsedBytes()		const										= 0;
		inline_			udword				GetNbNodes()		const						{ return mNbNodes;	}

		protected:
						udword				mNbNodes;
	};


	inline_	udword	StoreAsLeaf(udword data)		{ return data | SIGN_BITMASK;	}
	inline_	udword	MarkAsLeafN(udword data)		{ return data | 0x40000000;		}
	inline_	udword	StoreAsNonLeaf(udword data)		{ return data;					}
	inline_	BOOL	_IsLeaf(udword data)			{ return data & SIGN_BITMASK;	}
	inline_	BOOL	_IsLeafP(udword data)			{ return data & SIGN_BITMASK;	}
	inline_	BOOL	_IsLeafN(udword data)			{ return data & 0x40000000;		}
	//inline_	BOOL	_IsLeafPAndNonLeafN(udword data){ return (data & (SIGN_BITMASK|0x40000000)) == SIGN_BITMASK;	}
	//inline_	bool	_IsLeafPAndLeafN(udword data)	{ return (data & (SIGN_BITMASK|0x40000000)) == (SIGN_BITMASK|0x40000000);	}
	inline_	udword	GetLeafData(udword data)		{ return data & ~(SIGN_BITMASK|0x40000000);	}
	inline_	udword	GetNonLeafData(udword data)		{ return data;					}


//! Common interface for a node of an implicit tree
#define IMPLEMENT_STACKLESS_NODE(base_class, volume)											            \
    public:                                                                                                 \
	inline_								base_class()			{}                                          \
	inline_								~base_class()			{}                                          \
	/* Leaf test */                                                                                         \
	inline_			BOOL				IsLeaf()		const	{ return _IsLeaf(mPosIndexData);	}       \
	/* Data access */                                                                                       \
	inline_			udword				GetPosIndex()	const	{ return GetNonLeafData(mPosIndexData);	}   \
	inline_			udword				GetNegIndex()	const	{ return GetNonLeafData(mNegIndexData);	}   \
	inline_			udword				GetPrimitive()	const	{ return GetLeafData(mPosIndexData);	}   \
	/* Stats */                                                                                             \
	inline_			udword				GetNodeSize()	const	{ return SIZEOFOBJECT;				}       \
                                                                                                            \
					volume              mAABB;                                                              \
					udword				mPosIndexData;                                                      \
					udword				mNegIndexData;                                                      \
					udword				mEscapeIndex;                                                       \


//! Common interface for a node of a no-leaf tree
#define IMPLEMENT_STACKLESS_NOLEAF_NODE(base_class, volume)													\
    public:                                                                                                 \
	inline_								base_class()					{}                                  \
	inline_								~base_class()					{}                                  \
	/* Leaf test */                                                                                         \
	inline_			BOOL				HasPosLeaf()			const	{ return _IsLeafP(mPosIndexData);				}   \
	inline_			BOOL				HasNegLeaf()			const	{ return _IsLeafN(mPosIndexData);				}   \
/*	inline_			BOOL				HasPosAndNotNegLeaf()	const	{ return _IsLeafPAndNonLeafN(mPosIndexData);	}*/ \
/*	inline_			BOOL				HasPosAndNegLeaf()		const	{ return _IsLeafPAndLeafN(mPosIndexData);		}*/ \
	/* Data access */                                                                                                       \
	inline_			udword				GetPosPrimitive()		const	{ return GetLeafData(mPosIndexData);			}   \
	inline_			udword				GetNegPrimitive()		const	{ return 1+GetLeafData(mPosIndexData);			}	/* REMAP_XP*/ \
	/* Stats */                                                                                                             \
	inline_			udword				GetNodeSize()			const	{ return SIZEOFOBJECT;							}   \
                                                                                                                            \
					volume              mAABB;                                                                              \
					udword				mPosIndexData;                                                                      \
/*					udword				mNegIndexData;	// REMAP_XP*/                                                       \
					udword				mEscapeIndex;	/* SEPT25BUG */                                                     \


#ifdef OPC_SUPPORT_STACKLESS_TREE_NORMAL
    class OPCODE_API AABBStacklessCollisionNode : public Allocateable
	{
        IMPLEMENT_STACKLESS_NODE(AABBStacklessCollisionNode, CollisionAABB)
	};

    class OPCODE_API AABBStacklessCollisionTree : public AABBOptimizedTree
	{
        IMPLEMENT_COLLISION_TREE(AABBStacklessCollisionTree, AABBStacklessCollisionNode)
    };
#endif


#ifdef OPC_SUPPORT_STACKLESS_TREE_QUANTIZED
    class OPCODE_API AABBStacklessQuantizedNode : public Allocateable
	{
        IMPLEMENT_STACKLESS_NODE(AABBStacklessQuantizedNode, QuantizedAABB)
	};

    class OPCODE_API AABBStacklessQuantizedTree : public AABBOptimizedTree
	{
        IMPLEMENT_COLLISION_TREE(AABBStacklessQuantizedTree, AABBStacklessQuantizedNode)
        public:
					Point				mCenterCoeff;
					Point				mExtentsCoeff;
    };
#endif


#ifdef OPC_SUPPORT_STACKLESS_TREE_NO_LEAF
	class OPCODE_API AABBStacklessNoLeafNode : public Allocateable
	{
        IMPLEMENT_STACKLESS_NOLEAF_NODE(AABBStacklessNoLeafNode, CollisionAABB)
	};

    class OPCODE_API AABBStacklessNoLeafTree : public AABBOptimizedTree
	{
        IMPLEMENT_COLLISION_TREE(AABBStacklessNoLeafTree, AABBStacklessNoLeafNode)
    };
#endif


#ifdef OPC_SUPPORT_STACKLESS_TREE_NO_LEAF_QUANTIZED
	class OPCODE_API AABBStacklessQuantizedNoLeafNode : public Allocateable
	{
        IMPLEMENT_STACKLESS_NOLEAF_NODE(AABBStacklessQuantizedNoLeafNode, QuantizedAABB)
	};

    class OPCODE_API AABBStacklessQuantizedNoLeafTree : public AABBOptimizedTree
	{
        IMPLEMENT_COLLISION_TREE(AABBStacklessQuantizedNoLeafTree, AABBStacklessQuantizedNoLeafNode)
        public:
					Point				mCenterCoeff;
					Point				mExtentsCoeff;
    };
#endif

#endif // OPC_OPTIMIZEDTREE_H

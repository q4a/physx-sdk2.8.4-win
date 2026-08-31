///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains an allocator base class.
 *	\file		IceAllocator.h
 *	\author		Pierre Terdiman
 *	\date		December, 19, 2003
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef ICEALLOCATOR_H
#define ICEALLOCATOR_H

	enum MemoryType
	{
		MEMORY_PERSISTENT, // default memory pool.
		MEMORY_TEMP,			 // out of temp pool *must* delete right away.
		// Ice Class Type enumeation
		MEMORY_SweepAndPrune,
		MEMORY_Valencies,
		MEMORY_ValenciesBuilder,
		MEMORY_IndexedTriangle16,
		MEMORY_HullPolygon,
		MEMORY_Edge,
		MEMORY_EdgeDesc,
		MEMORY_CollisionHull,
		MEMORY_SupportVertexMap,
		MEMORY_RaycastMap,
		MEMORY_MeshModel,
		MEMORY_HybridModel,
		MEMORY_EdgeTriangle,
		MEMORY_AdjTriangle,
		MEMORY_AdjacenciesBuilder,
		MEMORY_EdgeListBuilder,
		MEMORY_LinearLooseOctree,
		MEMORY_OctreeCell,
		MEMORY_FreePruner,
		MEMORY_DynamicPruner,
		MEMORY_StaticPruner,
		MEMORY_DynamicPruner2,
		MEMORY_AABBTree,
		MEMORY_AABBNoLeafTree,
		MEMORY_AABBTreeNode,
		MEMORY_AABBQuantizedNoLeafTree,
		MEMORY_AABBCollisionTree,
		MEMORY_PRUNING_SORTER,
		MEMORY_AABBCollisionNode,
		MEMORY_AABBNoLeafNode,
		MEMORY_AABBQuantizedNode,
		MEMORY_SAP_Element,
		MEMORY_SAP_Box,
		MEMORY_SAP_EndPoint,
		MEMORY_CustomCell,
		MEMORY_HandleManager,
		MEMORY_AABBQuantizedNoLeafNode,
		MEMORY_LeafTriangles,
		MEMORY_Pair,
		// Ice discrete allocations.
		MEMORY_ConvexDecomposer_FlatTags,
		MEMORY_ConvexDecomposer_ConvexTags,
		MEMORY_CookingUtils_XRef,
		MEMORY_CookingUtils_RVerts,
		MEMORY_CookingUtils_FaceNormals,
		MEMORY_CookingUtils_VertexNormals,
		MEMORY_ConvexHull_HullVertices,
		MEMORY_ConvexHull_HullNormals,
		MEMORY_ConvexHull_VertexData,
		MEMORY_ConvexHull_EdgeData,
		MEMORY_ConvexHull_EdgeNormals,
		MEMORY_ConvexHull_FacesByEdges,
		MEMORY_EdgeList_FacesByEdges,
		MEMORY_EdgeList_ActiveEdges,
		MEMORY_IceHullGaussMaps_Samples,
		MEMORY_Valency_Valencies,
		MEMORY_Valency_Offsets,
		MEMORY_Valency_AdjecentVerts,
		MEMORY_IcePruningPool_WorldBoxes,
		MEMORY_IcePruningPool_Objects,
		MEMORY_OPC_AABBTree_Indices,
		MEMORY_OPC_HybridModel_Indices,
		MEMORY_OPC_SweepAndPrune_Array,
		MEMORY_IceContainer_NewEntries,
		MEMORY_IceCustomArray_CellAddy,
		MEMORY_IceHandleManager_Objects,
		MEMORY_IceHandleManager_OutToIn,
		MEMORY_IceHandleManager_InToOut,
		MEMORY_IceHandleManager_Stamps,
		MEMORY_IceRevisedRadix_Histogram,
		MEMORY_IceRevistedRadix_Offset,
		MEMORY_IceRevistedRadix_Ranks,
		MEMORY_IceRevistedRadix_Ranks2,
	};

	class ICECORE_API Allocator
	{
		public:
		virtual void*	malloc(size_t size, MemoryType type)																	= 0;
		virtual void*	mallocDebug(size_t size, const char* filename, udword line, const char* class_name, MemoryType type)	= 0;
		virtual void*	realloc(void* memory, size_t size)																		= 0;
		virtual void	free(void* memory)																						= 0;
	};

	FUNCTION ICECORE_API Allocator* GetAllocator();
	FUNCTION ICECORE_API bool		SetAllocator(Allocator& allocator);

	class ICECORE_API Allocateable
	{
		public:
		inline_	void*	operator new		(size_t size, MemoryType type)															{ return GetAllocator()->malloc(size, type);									}
		inline_	void*	operator new[]		(size_t size, MemoryType type)															{ return GetAllocator()->malloc(size, type);									}
		inline_	void	operator delete		(void* p)																				{ GetAllocator()->free(p);	}
		inline_	void	operator delete		(void* p, MemoryType)																	{ GetAllocator()->free(p);	}
		inline_	void	operator delete[]	(void* p)																				{ GetAllocator()->free(p);	}
		inline_	void	operator delete[]	(void* p, MemoryType)																	{ GetAllocator()->free(p);	}
#ifdef _DEBUG
		inline_	void*	operator new		(size_t size, const char * filename, int line, const char* class_name, MemoryType type)	{ return GetAllocator()->mallocDebug(size, filename, line, class_name, type);	}
		inline_	void*	operator new[]		(size_t size, const char * filename, int line, const char* class_name, MemoryType type)	{ return GetAllocator()->mallocDebug(size, filename, line, class_name, type);	}
		inline_	void	operator delete		(void* p, const char*, int, const char*, MemoryType)									{ GetAllocator()->free(p);	}
		inline_	void	operator delete[]	(void* p, const char*, int, const char*, MemoryType)									{ GetAllocator()->free(p);	}
#endif
	};

#endif // ICEALLOCATOR_H

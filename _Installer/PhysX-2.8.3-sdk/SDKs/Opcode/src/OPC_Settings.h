///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 *	OPCODE - Optimized Collision Detection
 *	Copyright (C) 2001 Pierre Terdiman
 *	Homepage: http://www.codercorner.com/Opcode.htm
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains compilation flags.
 *	\file		OPC_Settings.h
 *	\author		Pierre Terdiman
 *	\date		May, 12, 2001
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef OPC_SETTINGS_H
#define OPC_SETTINGS_H

	//! Use CPU comparisons (comment that line to use standard FPU compares)
#ifdef WIN32
	#define OPC_CPU_COMPARE
#endif

	//! Use FCOMI / FCMOV on Pentium-Pro based processors (comment that line to use plain C++)
#ifdef WIN32
//	#define OPC_USE_FCOMI
#endif

//moved OPC_SUPPORT_SIMD to OPC_IceHook.h as some ice functions need SIMD

	//! Use epsilon value in tri-tri overlap test
	#define OPC_TRITRI_EPSILON_TEST

	//! Use tree-coherence or not [not implemented yet]
//	#define OPC_USE_TREE_COHERENCE

	//! Use callbacks or direct pointers. Using callbacks might be a bit slower (but probably not much)
//	#define OPC_USE_CALLBACKS

	//! Support triangle and vertex strides or not. Using strides might be a bit slower (but probably not much)
//	#define OPC_USE_STRIDE

	//! Discard negative pointer in vanilla trees
	#define OPC_NO_NEG_VANILLA_TREE

	//! Use a callback in the ray collider
//	#define OPC_RAYHIT_CALLBACK

	// NB: no compilation flag to enable/disable stats since they're actually needed in the box/box overlap test

//	#define OPC_USE_PAIR_MANAGER

	#define OPC_GEOM_EPSILON

#if defined(__AMD64__)
	//#define OPC_64
#endif

	#define OPC_STATIC_ARRAYS		// Use static arrays to store touched triangles

	#define OPC_PRUNING_POOL_OWNER
	#define OPC_PRUNING_SECTIONS
//	#define OPC_PRUNING_HULL

//	#define NOVODEX_VERSION
	
	#define OPC_DEBUG_MALLOC 0

	#define OPC_SUPPORT_HYBRID_MODELS
    
//	#define OPC_MIN_MAX_BOX

//	#define OPC_SUPPORT_SWEEP_TESTS

	// PT: the dynamic AABB-tree is not compatible with containment tests!!! Currently disabled.
//	#define OPC_PERFORM_CONTAINMENT_TESTS

    #define OPC_SUPPORT_STACKLESS_TREE_NORMAL                   // Experiment
    #ifdef OPC_SUPPORT_STACKLESS_TREE_NORMAL
        #define OPC_SUPPORT_STACKLESS_TREE_QUANTIZED            // Experiment
    #endif
    #define OPC_SUPPORT_STACKLESS_TREE_NO_LEAF                  // Experiment
    #ifdef OPC_SUPPORT_STACKLESS_TREE_NO_LEAF
        #define OPC_SUPPORT_STACKLESS_TREE_NO_LEAF_QUANTIZED    // Experiment
    #endif

#endif // OPC_SETTINGS_H

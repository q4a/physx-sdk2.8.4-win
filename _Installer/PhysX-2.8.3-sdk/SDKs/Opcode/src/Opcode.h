///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 *	OPCODE - Optimized Collision Detection
 *	Copyright (C) 2001 Pierre Terdiman
 *	Homepage: http://www.codercorner.com/Opcode.htm
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Main file for Opcode.dll.
 *	\file		Opcode.h
 *	\author		Pierre Terdiman
 *	\date		March, 20, 2001
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef __OPCODE_H__
#define __OPCODE_H__


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Preprocessor
#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif

#define ICE_NO_DLL	//Adam

#ifndef ICE_NO_DLL
	#ifdef OPCODE_EXPORTS
		#define OPCODE_API __declspec(dllexport)
#error
	#else
		#define OPCODE_API __declspec(dllimport)
#error
	#endif
#else
		#define OPCODE_API
#endif

	#include "OPC_IceHook.h"

	namespace Opcode
	{
		// Bulk-of-the-work
		#include "OPC_Settings.h"
		#include "OPC_Common.h"
		#include "OPC_MeshInterface.h"
		// Builders
		#include "OPC_TreeBuilders.h"
		// Trees
		#include "OPC_AABBTree.h"
		#include "OPC_OptimizedTree.h"
		// Models
		#include "OPC_BaseModel.h"
		#include "OPC_Model.h"
		#include "OPC_HybridModel.h"
		// Colliders
		#include "OPC_Collider.h"
		#include "OPC_VolumeCollider.h"
		#include "OPC_TreeCollider.h"
		#include "OPC_RayCollider.h"
		#include "OPC_SphereCollider.h"
		#include "OPC_OBBCollider.h"
		#include "OPC_AABBCollider.h"
		#include "OPC_LSSCollider.h"
		#include "OPC_PlanesCollider.h"
		// Usages
//		#include "OPC_Picking.h"
		// Sweep-and-prune
		#include "OPC_BoxPruning.h"
//		#include "OPC_SweepAndPrune.h"

		// Distance
		#include "OPC_PointDistance.h"

		// Character controllers
//		#include "OPC_SphereCamera.h"
//		#include "OPC_BoxCamera.h"

		// Pruning
//		#include "IceOctree.h"
//		#include "IcePrunable.h"

		#include "IcePrunable.h"
//		#include "IceOctree.h"

		#include "IcePruningPool.h"
		#include "IcePruner.h"
		#include "IceFreePruner.h"
		#include "IceStaticPruner.h"
//		#include "IceDynamicPruner.h"
		#include "IceDynamicPruner2.h"
		#include "IcePruningEngine.h"

		FUNCTION OPCODE_API bool InitOpcode();
		FUNCTION OPCODE_API bool CloseOpcode();
	}

#endif // __OPCODE_H__

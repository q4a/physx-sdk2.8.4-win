#ifndef PXD_CONTEXT_H
#define PXD_CONTEXT_H

#include "px_config.h"
#include "px_task.h"

#ifdef __cplusplus

extern "C"
{
#endif

/*!
\file
Context handling
*/

/************************************************************************/
/* Context handling, types                                              */
/************************************************************************/

typedef PxdHandle PxdContext;
typedef PxdHandle PxdAtom;

/*!
Context type. Used when creating a new context.
*/
typedef enum PxdContextType_
{
	/*!
	This context should run on CPU only
	*/
	PXD_CPU,
	
#ifdef PXD_SUPPORT_PPU	
	/*!
	This context should utilize the PPU where possible
	*/
	PXD_PPU
#endif
}PxdContextType;

typedef enum PxdBroadPhaseImplementation_
{
	PXD_BP_SINGLE_SAP,
	PXD_BP_MULTI_SAP,
#ifdef __CELLOS_LV2__
	PXD_BP_SPU_MULTI_SAP,
#endif
}PxdBroadPhaseImplementation;


/*!
Cell specific control properties.
*/
typedef enum PxdCellProperty_
{
	/*!
	Enables SPU processing of broadphase
	*/
	PXD_SPU_BROADPHASE,
	/*!
	Enables SPU processing of narrowphase 1
	*/
	PXD_SPU_NARROWPHASE_1,
	/*!
	Enables SPU processing of narrowphase 2
	*/
	PXD_SPU_NARROWPHASE_2,
	/*!
	Enables SPU processing of narrowphase 3
	*/
	PXD_SPU_NARROWPHASE_3,
	/*!
	Enables SPU processing of narrowphase 4
	*/
	PXD_SPU_NARROWPHASE_4,
	/*!
	Enables island generation on the SPU
	*/
	PXD_SPU_ISLAND_GEN,
	/*! 
	Enables SPU processing of dynamics (constraint prep and solve)
	*/
	PXD_SPU_DYNAMICS,
	/*!
	Enables SPU processing of height fields
	*/
	PXD_SPU_HEIGHT_FIELD,
	/*!
	Enables SPU processing of fluid collisions
	*/
	PXD_SPU_FLUID_COLLISION,
	/*!
	Enables SPU processing of fluid dynamics (no-SPH only)
	*/
	PXD_SPU_FLUID_DYNAMICS,
	/*!
	Enables SPU processing of fluid packets generation and particles reorder
	*/
	PXD_SPU_FLUID_PACKETS,
	/*!
	Enables SPU processing of fluid particles update
	*/
	PXD_SPU_FLUID_PARTICLES_UPDATE,
} PxdCellProperty;


/*!
Get the context type
\param context
The context to get the type of.
\return
Context type. 
*/
PXD_EXPORT_FUNCTION PxdContextType PxdContextGetType(PxdContext context);

/*!
Create a new context

\param type
Context type. This cannot be changed afterwards.
\param agPPUConnection
Pointer to an AgPPUConnection. Backdoor for sharing one PPU connection between
different types of scenes (fluid, cloth and RB)
\param bp
Selects broad-phase implementation
\return
Context identifier. Zero on failure.
*/
PXD_EXPORT_FUNCTION PxdContext PxdContextCreate(PxdContextType type, 
												void* agPPUConnection, 
												PxdBroadPhaseImplementation bp, 
												bool useFastMassiveBPVolumeDeletion, 
												bool useImprovedSpringSolver,
												bool useLegacyJointDrive);

/*!
Destroy context

\param context
Context identifier
*/
PXD_EXPORT_FUNCTION void PxdContextDestroy(PxdContext context);

/*!
Associate a sub task callback with a context.

\param contexts
Context identifier
\param userSubTaskCallback
User callback function for subtask queuing and execution
\param userParam
User parameter that is passed along on every call to the userSubTaskCallback
function
*/
PXD_EXPORT_FUNCTION void PxdContextSetSubTaskCallback(PxdContext context, PxdUserSubTaskCallback userSubTaskCallback, void* userParam);

/*!
Set Cell specific context properties

\param context
Contex identifier
\param property
Property identifier
\param value
Value to set the property to 
*/
PXD_EXPORT_FUNCTION void PxdContextCellSetInt(PxdContext context, PxdCellProperty property, PxInt value);


/*!
Get Cell specific context properties

\param context
Contex identifier
\param property
Property identifier
\return 
Property value 
*/
PXD_EXPORT_FUNCTION PxInt PxdContextCellGetInt(PxdContext context, PxdCellProperty property);


#ifdef PXD_SUPPORT_DEBUG_RENDERER

	enum PxDebugColor
	{
		PX_ARGB_BLACK	= 0xff000000,
		PX_ARGB_RED		= 0xffff0000,
		PX_ARGB_GREEN	= 0xff00ff00,
		PX_ARGB_BLUE	= 0xff0000ff,
		PX_ARGB_YELLOW	= 0xffffff00,
		PX_ARGB_MAGENTA	= 0xffff00ff,
		PX_ARGB_CYAN	= 0xff00ffff,
		PX_ARGB_WHITE	= 0xffffffff,
	};

	struct PxDebugPoint
	{
		PxdVector	p;
		PxU32		color;
	};

	struct PxDebugLine
	{
		PxdVector	p0;
		PxdVector	p1;
		PxU32		color;
	};

	struct PxDebugTriangle
	{
		PxdVector	p0;
		PxdVector	p1;
		PxdVector	p2;
		PxU32		color;
	};

	struct PxDebugAABB
	{
		PxdVector	min;
		PxdVector	max;
		PxU32		color;
	};

PXD_EXPORT_FUNCTION bool PxdContextClearDebugData(PxdContext context);

PXD_EXPORT_FUNCTION bool PxdContextGetDebugData(PxdContext context,
	PxU32& nbPoints, const PxDebugPoint*& points,
	PxU32& nbLines, const PxDebugLine*& lines,
	PxU32& nbTriangles, const PxDebugTriangle*& triangles,
	PxU32& nbAABBs, const PxDebugAABB*& aabbs
	);
#endif


#ifdef __cplusplus
}
#endif


#endif

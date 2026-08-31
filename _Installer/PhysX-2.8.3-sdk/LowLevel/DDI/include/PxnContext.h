#ifndef PXN_CONTEXT_H
#define PXN_CONTEXT_H

#include "px_context.h"
#include "px_config.h"
#include "px_fluid.h"
#include "px_geometry.h"
#include "px_manager.h"
#include "PxnError.h"

class PxnBodyAtom;
class PxnFluid;
class PxnBodyShape;
class PxnFluidShape;
class PxnBroadPhase;
class PxnContactConstraint;
class PxnContactManager;
class PxnFluidBodyContactManager;
class PxnCustomConstraint;
class PxnD6Joint;
class PxnShape;
class PxnTask;
class PxnVolume;
class PxcDebugData;
class AgPPUConnection;

typedef struct PxdAtomDesc_ PxdAtomDesc;
typedef struct PxdFluidDesc_ PxdFluidDesc;
typedef struct PxdConstraintDesc_ PxdConstraintDesc;
typedef struct PxdContactConstraintDesc_ PxdContactConstraintDesc;
typedef struct PxdD6JointDesc_ PxdD6JointDesc;
typedef struct PxdManagerDesc_ PxdManagerDesc;
typedef struct PxdFluidBodyManagerDesc_ PxdFluidBodyManagerDesc;
typedef struct PxdShapeDesc_ PxdShapeDesc;
typedef struct PxdVolumeDesc_ PxdVolumeDesc;

enum
{
	PXD_MAX_CONTEXTS = 64
};

enum
{
	PXN_HANDLE_TYPE_INVALID						= 0,
	PXN_HANDLE_TYPE_CONTEXT						= 1,
	PXN_HANDLE_TYPE_SHAPE						= 2,
	PXN_HANDLE_TYPE_BODYATOM					= 3,
	PXN_HANDLE_TYPE_FLUID						= 4,
	PXN_HANDLE_TYPE_BODYSHAPE					= 5,
	PXN_HANDLE_TYPE_FLUIDSHAPE					= 6,
	PXN_HANDLE_TYPE_CONTACTMANAGER				= 7,
	PXN_HANDLE_TYPE_FLUIDBODY_CONTACTMANAGER	= 8,
	PXN_HANDLE_TYPE_PPUCONTACTMANAGER			= 9,
	PXN_HANDLE_TYPE_MESHMANAGER					= 10,
	PXN_HANDLE_TYPE_VOLUME						= 11,
	PXN_HANDLE_TYPE_CONSTRAINT					= 12,
	PXN_HANDLE_TYPE_D6JOINT						= 13,
	PXN_HANDLE_TYPE_CONTACT						= 14,
	PXN_HANDLE_TYPE_INTERNAL_CONTACT			= 15, // Internal contact generated on host
	PXN_HANDLE_TYPE_PPU_CONTACT					= 16, // Internal contact generated on PPU
	PXN_HANDLE_TYPE_TASK						= 17,
	PXN_HANDLE_TYPE_COUNT						= 18
};

PXD_FORCE_INLINE PxU32 makeHandle(PxU32 contextID, PxU32 typeID, PxU32 index)
{
	PXN_ASSERT(contextID<PXD_MAX_CONTEXTS && typeID<PXN_HANDLE_TYPE_COUNT && index<(1<<20));
	return (contextID<<26)+(typeID<<20)+index;
}

PXD_FORCE_INLINE PxU32 getHandleContext(PxdHandle handle)
{
	return handle>>26;
}

PXD_FORCE_INLINE PxU32 getHandleType(PxdHandle handle)
{
	return (handle>>20)&63;
}

PXD_FORCE_INLINE PxU32 getHandleIndex(PxdHandle handle)
{
	return handle & ((1<<20)-1);
}

class PxnContext
{
public:
	static	PxnContext*				create(PxdContextType type, 
										   AgPPUConnection* connection, 
										   PxdBroadPhaseImplementation bp, 
										   bool useFastMassiveBPVolumeDeletion, 
										   bool useImprovedSpringSolver,
										   bool useLegacyJointDrive);
	static	PxnContext*				findContext(PxdContext context);
	static	PxnContext*				findHandleContext(PxdHandle handle);
	
			void					destroy();
	PXD_INLINE						PxnContext()		{ mFlags = 0; }
	PXD_INLINE	PxU32				getFlags()	const	{ return mFlags; }
	PXD_INLINE	void				setFlags(PxU32 f)	{ mFlags = f; }


	virtual	PxdContextType			getContextType()												= 0;

	virtual	PxnShape*				createShapeV(const PxdShapeDesc& desc)							= 0;
	virtual	PxnBodyShape*			createBodyShapeV(const PxdShapeDesc& desc)						= 0;
	virtual	PxnBodyAtom*			createBodyAtomV(const PxdAtomDesc& desc)						= 0;
	virtual PxnFluid*				createFluidV(const PxdFluidDesc& desc)							= 0;
	virtual	PxnVolume*				createVolumeV(const PxdVolumeDesc& desc)						= 0;
	virtual	PxnContactManager*		createContactManagerV(const PxdManagerDesc& desc)				= 0;
	virtual	PxnFluidBodyContactManager*		createContactManagerV(const PxdManagerDescFluidBody& desc)	= 0;
	virtual PxnCustomConstraint*	createCustomConstraintV(const PxdConstraintDesc& desc)			= 0;
	virtual PxnContactConstraint*	createContactConstraintV(const PxdContactConstraintDesc& desc)	= 0;
	virtual PxnD6Joint*				createD6JointV(const PxdD6JointDesc& desc)						= 0;

	virtual	PxnShape*				findShapeV(PxdHandle handle)				const				= 0;
	virtual	PxnBodyShape*			findBodyShapeV(PxdHandle handle)			const				= 0;
	virtual	PxnFluidShape*			findFluidShapeV(PxdHandle handle)			const				= 0;
	virtual	PxnBodyAtom*			findBodyAtomV(PxdHandle handle)				const				= 0;
	virtual PxnFluid*				findFluidV(PxdHandle handle)				const				= 0;
	virtual	PxnVolume*				findVolumeV(PxdHandle handle)				const				= 0;
	virtual	PxnContactManager*		findContactManagerV(PxdHandle handle)		const				= 0;
	virtual	PxnFluidBodyContactManager*		findFluidBodyContactManagerV(PxdHandle handle)	const	= 0;
	virtual PxnCustomConstraint*	findCustomConstraintV(PxdHandle handle)		const				= 0;
	virtual PxnContactConstraint*	findContactConstraintV(PxdHandle handle)	const				= 0;
	virtual PxnD6Joint*				findD6JointV(PxdHandle handle)				const				= 0;

	virtual PxdHandle				getHandleV()								const				= 0;
	virtual void					destroyV()														= 0;

	// Broad phase related
	virtual PxnBroadPhase*			getBroadPhaseV()												= 0;

	// Dynamics properties
	virtual PxFloat					getBounceThresholdV()						const				= 0;
	virtual void					setBounceThresholdV(PxFloat f)									= 0;
	virtual PxInt					getSolverBatchSizeV()						const				= 0;
	virtual void					setSolverBatchSizeV(PxInt i)									= 0;

	// Task-related
	virtual PxnTask*				findTaskV(PxdHandle handle)					const				= 0;
	virtual PxdTask					updateBroadPhaseV()												= 0;
	virtual PxdTask					updateContactManagerV(PxFloat dt)								= 0;
	virtual PxdTask					updateDynamicsV(PxFloat dt)										= 0;

	virtual	PxUInt					waitTaskAnyV(PxdHandle* tasks, PxUInt taskCount)	const		= 0;

	virtual void					setSubTaskCallbackV(PxdUserSubTaskCallback userSubTask, void* userParam)	= 0;

	// Manager status change
	virtual bool					getManagerStatusChangeCountV(PxInt* newTouch, PxInt* lostTouch)		const	= 0;
	virtual bool					fillManagerStatusChangeV(PxdContactManagerStatusChange* newTouch, PxdContactManagerStatusChange* lostTouch) = 0;

	// body pair reporting
	virtual PxUInt					getAtomReportPairCountV()					const				= 0;
	virtual bool					fillAtomReportPairBufferV(PxdAtomPair* )	const				= 0;

	// Profiling related
	virtual PxInt					getProfileZoneCountV()						const				= 0;
	virtual void					getProfileZoneDataV(PxdProfileZone* data)						= 0;

#ifdef PXD_SUPPORT_DEBUG_RENDERER
	virtual	PxcDebugData*			getDebugDataV()													{ return NULL;	}
#endif

protected:
	virtual							~PxnContext()													{}

	PxU32							mFlags;
	static PxnContext*				sArray[PXD_MAX_CONTEXTS];
};


#endif

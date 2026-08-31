/* NVIDIA/SCE CONFIDENTIAL
   NVIDIA PhysX(TM) SDK for PlayStation(R)3 Version PS3_PhysX_283_B2_forSDK330_hlsrc
*/
// SCE CONFIDENTIAL
// Copyright (C) Sony Computer Entertainment Inc.
// All Rights Reserved.

#ifndef __CELL_SPURS_SUPPORT_H
#define __CELL_SPURS_SUPPORT_H

#ifdef WIN32
#ifndef __attribute__
#define __attribute__(x) 
#endif

#define NOMINMAX
#include <windows.h>
#include <assert.h>

typedef HANDLE CellSpursQueue;
#ifndef uint32_t
typedef unsigned int uint32_t;
#endif

#ifndef CELL_PPU_POINTER
#define CELL_PPU_POINTER(x) x
#endif // __attribute__

#else // WIN32

#include "CellUtilityMacros.h"
#include <cell/spurs/queue.h>

#ifdef __PPU__
#include <cell/spurs/task.h>
#endif //__PPU__

#endif // WIN32

#define CELL_SPURS_RESPONSE_QUEUE_SIZE 128

#define SPU_FLUID_JOB_TYPE CellSpursJob128
#define SPU_FLUID_MAX_JOBS 128

/**
 * Note:
 * The order of elements in this enum are important, that's why each one is explicitly
 * given a value.  They will correspond to the .elf names/addresses that will be
 * loaded into SPURS.
 * Mixing up these values will cause the wrong code to execute, for instance, the
 * solver may be asked to do a collision detection job.
 */
typedef enum {
//	SPU_ELF_TEST_TASK=0,
	SPU_ELF_ISLANDGEN=0,
	SPU_ELF_NARROW_PHASE_1=1,
	SPU_ELF_NARROW_PHASE_2=2,
	SPU_ELF_NARROW_PHASE_3=3,
	SPU_ELF_NARROW_PHASE_4=4,
	SPU_ELF_CLOTH=5,
	SPU_ELF_HEIGHT_FIELD=6,
	SPU_ELF_RAYCAST=7,
	SPU_ELF_FLUIDSPATIALHASH=8,
	SPU_ELF_FLUIDCOLLISION=9,
	SPU_ELF_CONSTRAINT_PREP=10,
	SPU_ELF_SOLVER=11,
	SPU_ELF_ATOM_LIST_SORTER=12,
	SPU_ELF_SOLVER_FINALIZATION=13,
	SPU_ELF_ATOM_GEN=14,
	SPU_ELF_BP_CREATE_ENDPOINTS=15,
	SPU_ELF_BP_SORT_ENDPOINTS=16,
	SPU_ELF_BP_CREATE_PAIRS=17,
	SPU_ELF_BP_UPDATE_VOLUMES=18,
	SPU_ELF_CLOTH_UPDATE=19,
	SPU_ELF_CLOTH_COLLISION=20,
	SPU_ELF_FLUIDDYNAMIC = 21,
	SPU_ELF_LAST,
} CellSpursElfId_t;

typedef union CellSPURSArgument {
	struct {
		CELL_PPU_POINTER(CellSpursQueue) ppuResponseQueue;
		uint32_t uiCommand;
		uint32_t uiArgument0;
		uint32_t uiArgument1;
	};

#if defined(__PPU__)
	CellSpursTaskArgument spursArgument;
#elif defined(__SPU__)
	vec_uint4 uiQWord;
#endif
} CellSPURSArgument __attribute__((aligned(16)));

#ifdef WIN32
typedef void (*CellSPURSWin32Function)(CellSPURSArgument args);
#endif

#if defined(__PPU__) || defined(WIN32)
// Number of SPUs we can use.
extern int g_iPhysXSPUCount;

#if defined(__PPU__)
#include <NxCellConfiguration.h>
#elif defined(WIN32)
#include <windows.h>
#endif

class CellSPURSSupport {
public:
	CellSPURSSupport(CellSpursElfId_t elfId);
	~CellSPURSSupport();
	void sendRequest(uint32_t uiCommand, uint32_t uiArgument0, uint32_t uiArgument1=0);
	void waitForResponse(unsigned int *puiArgument0, unsigned int *puiArgument1);
	bool checkForResponse(unsigned int *puiArgument0, unsigned int *puiArgument1);
	void startSPU();
	void stopSPU();

	// Use these functions at your own risk.
	// They are exposed so that you can spawn new tasks
	// from inside SPU tasks.  Please don't do much more than
	// that with them.
	const void *getElfImage();

#ifdef __CELLOS_LV2__
	CellSpursTaskset *getSpursTaskSet();
	static CellSpurs *getSpurs();
	static void resetJobList();
	static bool isJobListFull();	
	static bool queueJob(SPU_FLUID_JOB_TYPE*job);
	static void kickAndWaitJobs();
#endif // __CELLOS_LV2__

	// This is here to support tasks with overlays.
	const void *getOverlayTable();

protected:
	CellSpursElfId_t m_elfId;
#if defined(__PPU__)
	uint32_t m_uiReferenceCounter;
	CellSpursQueue m_responseQueue __attribute__((aligned(128)));
	CellSPURSArgument m_aResponseBuffer[CELL_SPURS_RESPONSE_QUEUE_SIZE] __attribute__((aligned(16)));
	bool m_bQueueInitialized;
#elif defined(WIN32)
	HANDLE m_hInput, m_hOutput;
#endif // __PPU__ / WIN32
};
#endif // defined(__PPU__) || defined(WIN32)


// Slave (SPU/Windows) functions

#if defined(WIN32)
static inline void sendResponseToPPUAndExit(uint32_t ppuQueueEA, uint32_t uiArgument0,
											uint32_t uiArgument1, int iTag=1) {
	CellSPURSArgument response; 

	response.uiArgument0=uiArgument0;
	response.uiArgument1=uiArgument1;

	DWORD dwBytesWritten;
	BOOL bSucceeded;
	bSucceeded=WriteFile((HANDLE) ppuQueueEA, &response, sizeof(CellSPURSArgument),
		&dwBytesWritten, NULL);
	assert(bSucceeded && (dwBytesWritten==sizeof(CellSPURSArgument)));
	return;
}
#elif defined(__SPU__)

#include <simd>

// Sauce
// This is to allow 0.8.1+ code to work with 0.8.0 SDK
#include <sdk_version.h>

#if CELL_SDK_VERSION < 0x081000
#define CELL_SPURS_TASK_ERROR_AGAIN CELL_SPURS_EAGAIN
#define CELL_SPURS_TASK_ERROR_BUSY CELL_SPURS_EBUSY
#endif // CELL_SDK_VERSION < 0x081000

#include "SPUAssert.h"
#include <cell/spurs/task.h>

static inline void sendResponseToPPUAndExit(uint32_t ppuQueueEA, uint32_t uiArgument0,
											uint32_t uiArgument1, int iTag=1) {
	CellSPURSArgument response 
		__attribute__ ((aligned(16)));

	response.uiArgument0=uiArgument0;
	response.uiArgument1=uiArgument1;

	int iReturn;
	do {
		iReturn=cellSpursQueueTryPushBegin(ppuQueueEA, &response, iTag);
	} while (iReturn == CELL_SPURS_TASK_ERROR_AGAIN ||
			 iReturn == CELL_SPURS_TASK_ERROR_BUSY);

	SPU_ASSERT((iReturn == CELL_OK) && "PhysX: Error writing to SPURS queue.");

	cellSpursQueuePushEnd(ppuQueueEA, iTag);
	cellSpursExit();
}

static inline void noResponseExit() {
	cellSpursExit();
}

#endif // WIN32 / __SPU__


#ifdef __PPU__
// 
// Extern definitions of all SPURS tasks
//											
extern CellSPURSSupport g_SPURSTestTask;
extern CellSPURSSupport g_SPURSIslandGen;
extern CellSPURSSupport g_SPURSConstraintPrep;
extern CellSPURSSupport g_SPURSSolver;
extern CellSPURSSupport g_SPURSSolverFinalization;
extern CellSPURSSupport g_SPURSAtomGen;
extern CellSPURSSupport g_SPURSAtomListSorter;
extern CellSPURSSupport g_SPURSBpCreateEndpoints;
extern CellSPURSSupport g_SPURSBpSortEndpoints;
extern CellSPURSSupport g_SPURSBpCreatePairs;
extern CellSPURSSupport g_SPURSBpUpdateVolumes;
extern CellSPURSSupport g_SPURSNarrowPhase1;
extern CellSPURSSupport g_SPURSNarrowPhase2;
extern CellSPURSSupport g_SPURSNarrowPhase3;
extern CellSPURSSupport g_SPURSNarrowPhase4;
//extern CellSPURSSupport g_SPURSCloth;
extern CellSPURSSupport g_SPURSClothUpdate;
extern CellSPURSSupport g_SPURSClothCollision;
extern CellSPURSSupport g_SPURSHeightField;
extern CellSPURSSupport g_SPURSRaycast;
extern CellSPURSSupport g_SPURSFluidSpatialHash;
extern CellSPURSSupport g_SPURSFluidCollision;
//extern CellSPURSSupport g_SPURSFluidHeightFieldCollision;

#endif // __PPU__


#endif // CELL_SPURS_SUPPORT_H



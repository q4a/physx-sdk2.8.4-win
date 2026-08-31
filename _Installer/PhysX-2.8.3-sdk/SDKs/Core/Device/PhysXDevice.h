#ifndef ___PHYS_X_DEVICE_
#define ___PHYS_X_DEVICE_

#include <Windows.h>

/** typedefs */
typedef int PHYSX_DEV_STATUS;
typedef unsigned int PhysXDevHandle;

#define PHYSX_DEV_DLL_API      extern "C" __declspec(dllexport)

#ifdef _DEBUG 
#   define PHYSX_DEV_DLL_PRIVATE_API extern "C" __declspec(dllexport)
#else
#   define PHYSX_DEV_DLL_PRIVATE_API 
#endif
/** PHYSX_DEV_STATUS values */
enum 
{
    PHYSX_DEV_OK = 0,
    PHYSX_DEV_UNKNOWN_ERROR,
    PHYSX_DEV_INVALID_HANDLE,
    PHYSX_DEV_UNINITIALIZED,
    PHYSX_DEV_NV_API_UNAVAILABLE,
    PHYSX_DEV_CUDA_UNAVAILABLE,
    PHYSX_DEV_CUDA_MEMORY_ALLOC_FAILURE,
    PHYSX_DEV_LEGACY_MODE_GPU_HANDLE,
	PHYSX_DEV_INCORRECT_GRAPHICS_DRIVER_VERSION,
    PHYSX_DEV_PHYSX_DEV_UNAVAILABLE,
};


/** definitions */
#define USE_MONSTER_DEMO_REG_BASE               0

#ifdef UNICODE
#if USE_MONSTER_DEMO_REG_BASE
#   define AG_PHYSX_CORE_PATH_REG_KEY   L"Software\\NVIDIA Corporation\\NVIDIA Demos\\PhysX Creature"
#else
#   define AG_PHYSX_CORE_PATH_REG_KEY   L"Software\\AGEIA Technologies"
#endif
#define CUDA_DEV_SUBKEY                 L"CudaDeviceOrdinal"
#define HW_SELECTION_SUBKEY             L"HwSelection"

#else
#if USE_MONSTER_DEMO_REG_BASE
#   define AG_PHYSX_CORE_PATH_REG_KEY   "Software\\NVIDIA Corporation\\NVIDIA Demos\\PhysX Creature"
#else
#   define AG_PHYSX_CORE_PATH_REG_KEY   "Software\\AGEIA Technologies"
#endif
#define CUDA_DEV_SUBKEY                 "CudaDeviceOrdinal"
#define HW_SELECTION_SUBKEY             "HwSelection"
#endif

/** 
 *  physxDevInit
 *  Initialize the PhysX Device information functions.
 *  Must be called before using any other API functions.
 */
PHYSX_DEV_DLL_API PHYSX_DEV_STATUS physxDevInit();

/**
 *  physxDevClose
 *  Call this when finished with the PhysX Device API, it
 *  frees memory that is allocated in physxDevInit
 */
PHYSX_DEV_DLL_API PHYSX_DEV_STATUS physxDevClose();

/**
 *  physxDevGetCount
 *  Returns the number of PhysX GPU devices available
 *  
 */
PHYSX_DEV_DLL_PRIVATE_API PHYSX_DEV_STATUS physxDevGetCount(int *devCount);

/**
 *  physxDevGetHandleList
 *  Returns an array of PhysX GPU device handles and the 
 *  number of handles returned
 */
PHYSX_DEV_DLL_PRIVATE_API PHYSX_DEV_STATUS physxDevGetHandleList(PhysXDevHandle *devHandleList, int *devHandleCount);

/**
 *  physxDevGetName
 *  Returns the name of the device
 *  'len' is the size of the string, 'devName'.  The function
 *  will fail if the string provided is not large enough.
 */
PHYSX_DEV_DLL_PRIVATE_API PHYSX_DEV_STATUS physxDevGetName(char *devName, int len, PhysXDevHandle devHandle);

/**
 *  physxDevDrivingPrimaryDisplay
 *  Returns whether or not the specified PhysX GPU device is
 *  driving the primary display.  Sorry for passing a bool
 *  as a parameter, but I was trying to keep a common return status...
 */
PHYSX_DEV_DLL_PRIVATE_API PHYSX_DEV_STATUS physxDevDrivingPrimaryDisplay(PhysXDevHandle devHandle, bool *drivingPrimary);

/**
 *  physxDevGetCudaOrdinal
 *  Returns the CUDA device ordinal for the given PhysX GPU device
 */
PHYSX_DEV_DLL_API PHYSX_DEV_STATUS physxDevGetCudaOrdinal(int *cudaDevOrdinal, PhysXDevHandle devHandle);
PHYSX_DEV_STATUS physxDevGetCudaOrdinalWrapper(int *cudaDevOrdinal);

/**
 *  physxDevSet
 *  Sets the proper registry location so the PhysX engine
 *  will use the device specified.  If you want the engine
 *  to automatically choose the which device to use, set it to -1.
 */
PHYSX_DEV_DLL_PRIVATE_API PHYSX_DEV_STATUS physxDevSet(PhysXDevHandle devHandle);

/**
 *  physxDevGet
 *  Returns the PhysX GPU device that the PhysX Engine
 *  will use.  If the device is -1, the engine will 
 *  automatically choose which GPU to use.
 *
 *  This function handles the R177/R180 detection first, then decides accordingly
 *
 *  if(180+)
 *      if(GPU Enabled) ? get NVAPI sel : -1
 *  else (177)
 *      if regkey ? regkey value : -1 (PHYSX_DEV_LEGACY_MODE_GPU_HANDLE returned)
 */
PHYSX_DEV_DLL_API PHYSX_DEV_STATUS physxDevGet(PhysXDevHandle *devHandle);

/** 
 *  physxDevGetHwSelection
 *  Returns what the PhysX Engine will use as a hardware
 *  device.  
 *  "GPU", "PPU", "NoAcceleration" are possible values
 */
PHYSX_DEV_DLL_API PHYSX_DEV_STATUS physxDevGetHwSelection(char *hwSelection, unsigned int len);
PHYSX_DEV_STATUS physxDevGetHwSelectionWrapper(char *hwSelection, unsigned int len);

/** 
 *  physxDevSetHwSelection
 *  Sets what the PhysX Engine will use as a hardware
 *  device.  
 *  "GPU", "PPU", "NoAcceleration" are possible values
 */
PHYSX_DEV_DLL_PRIVATE_API PHYSX_DEV_STATUS physxDevSetHwSelection(char *hwSelection);


/**
 *  physxDevErrorString
 *  Returns the error string describing the error status
 */
PHYSX_DEV_DLL_API char *physxDevErrorString(PHYSX_DEV_STATUS status);

#endif

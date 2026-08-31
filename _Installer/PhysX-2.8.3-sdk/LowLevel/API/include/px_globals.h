#ifndef PXD_INIT_H
#define PXD_INIT_H

#include "px_config.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*!
\file
PhysX Low-level, Memory management
*/

/************************************************************************/
/* Memory management                                                    */
/************************************************************************/

typedef void* (*PxdUserMalloc)(size_t);
typedef void (*PxdUserFree)(void*);
typedef void* (*PxdDebugUserMalloc)(size_t,const char*,PxUInt);
typedef void (*PxdDebugUserFree)(void*,const char*,PxUInt);

/************************************************************************/
/* Error Handling                                                       */
/************************************************************************/


typedef enum PxdErrorCode_
{
	PXD_ERROR_NO_ERROR = 0,
	PXD_ERROR_INVALID_PARAMETER,
	PXD_ERROR_INVALID_PARAMETER_SIZE,
	PXD_ERROR_INTERNAL_ERROR,
	PXD_ERROR_NOT_IMPLEMENTED,
	PXD_ERROR_NO_CONTEXT,
	PXD_ERROR_NO_TASK_MANAGER
} PxdErrorCode;
	
typedef void (*PxdUserLog)(PxdErrorCode code, const char* description);

/*!
Return last error code. By reading this value you also reset the
last error, so that next time it will return PXD_ERROR_NO_ERROR
unless another error has been reported.
*/
PXD_EXPORT_FUNCTION PxdErrorCode PxdGetLastError();

typedef struct PxdDesc_
{
	/*!
	Default user allocator
	*/
	PxdUserMalloc		userMalloc;

	/*!
	Default user deallocator
	*/
	PxdUserFree			userFree;

	/*!
	User log object.
	Optional.
	*/
	PxdUserLog			userLog;

	/*!
	Debug user allocator. Used in debug mode to allow more fine-grained
	logging of memory requests.
	Optional, if none is provided the default user allocator will be used.
	*/
	PxdDebugUserMalloc	debugUserMalloc;

	/*!
	Debug user deallocator. Used in debug mode to allow more fine-grained
	logging of memory requests.
	Optional, if none is provided the default user deallocator will be used.
	*/
	PxdDebugUserFree	debugUserFree;

	/*!
	CUDA device. If -1, CUDA will not be started
	*/

	PxI32				cudaDevice;

	/*!
	CUDA heap sizes. If zero, no CUDA heap will be allocated
	*/
	PxU32				cudaLocalHeapSize;
	PxU32				cudaRemoteHeapSize;

	bool				perSceneBatching;

} PxdDesc;

/*!
Initialize low-leveld initialization descriptor with reasonable defaults. 
Does not initialize memory manager or error stream.
*/
PXD_EXPORT_FUNCTION void PxdSetDefaults(PxdDesc* desc);

/*!
Initialize low-level implementation. Requires a PxdDesc containing at least 
normal memory management functions.
*/
PXD_EXPORT_FUNCTION void PxdInit(PxdDesc* desc);


/*!
Shut down low-level implementation.
*/
PXD_EXPORT_FUNCTION void PxdTerm();

#ifdef __cplusplus
}
#endif

#endif


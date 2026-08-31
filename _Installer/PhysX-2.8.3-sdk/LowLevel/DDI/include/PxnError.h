#ifndef PXN_ERROR_H
#define PXN_ERROR_H

#include "px_config.h"
#include "px_globals.h"
#include "PxnPropertyTypes.h"
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>

// Some helper macros
#if defined(_DEBUG)
#	if!defined(PXD_COMPILER_CUDA)
#		define PXN_ASSERT(x) assert(x)
#	else
#		define PXN_ASSERT(x) {(void)(x);}	// make sure expr is evaluated on CUDA, avoid unused var kvetching
#	endif
#else
#	define PXN_ASSERT(x) {}	
#endif

void PxnSetLog(PxdUserLog log);

// allow us to use PxnErrorReport in PxgMirrored.h, which is indirectly included in cu files
// and compiled by the CUDA compiler
extern "C"
{
void PxnErrorReport(PxdErrorCode errCode, const char* fmt, ...);
}

#ifdef PXD_COMPILER_VC
#define snprintf(buffer,fmt,size,arg) _snprintf((buffer),(fmt),(size),(arg))
#endif

/* shouldn't really be inlines! */
inline void reportError(const char* error)
{
	PxnErrorReport(PXD_ERROR_INTERNAL_ERROR, "Internal error: %s: \n",error);
	PXN_ASSERT(0);
}

inline void reportUnimplemented(const char* fn, const char* comment = 0)
{
	if(comment)
		PxnErrorReport(PXD_ERROR_NOT_IMPLEMENTED, "Unimplemented Function: %s: %s\n",fn, comment);
	else
		PxnErrorReport(PXD_ERROR_NOT_IMPLEMENTED, "Unimplemented Function: %s\n",fn);
	PXN_ASSERT(0);
}

inline void reportInvalidHandle(const char* fn)
{
	PxnErrorReport(PXD_ERROR_INVALID_PARAMETER, "Invalid Handle: %s\n",fn);
	PXN_ASSERT(0);
}

inline void reportInvalidTypeCombination(const char* fn)
{
	PxnErrorReport(PXD_ERROR_INVALID_PARAMETER, "Invalid handle/type combination: %s\n",fn);
	PXN_ASSERT(0);
}

inline void reportInvalidSize(const char* fn)
{
	PxnErrorReport(PXD_ERROR_INVALID_PARAMETER_SIZE, "Invalid data size: %s\n", fn);
	PXN_ASSERT(0);
}

inline void reportInvalidContext(const char* fn)
{
	PxnErrorReport(PXD_ERROR_NO_CONTEXT, "Invalid Context set: %s\n", fn);
	PXN_ASSERT(0);
}

#define NO_SET_INT(_type)																\
void Pxd ## _type ## SetInt(Pxd ## _type ,  Pxd ## _type ## Property, PxInt)			\
{																						\
	reportError("Pxd" #_type "SetInt: " #_type "has no property of type Int");			\
}

#define NO_GET_INT(_type)																\
PxInt Pxd ## _type ## GetInt(Pxd ## _type ,  Pxd ## _type ## Property)				\
{																						\
	reportError("Pxd" #_type "GetInt: " #_type "has no property of type Int");			\
	return 0;																			\
}


#define NO_SET_FLOAT(_type)																\
void Pxd ## _type ## SetFloat(Pxd ## _type ,  Pxd ## _type ## Property, PxFloat)		\
{																						\
	reportError("Pxd" #_type "SetFloat: " #_type "has no property of type Float");		\
}


#define NO_GET_FLOAT(_type)																\
PxFloat Pxd ## _type ## GetFloat(Pxd ## _type ,  Pxd ## _type ## Property)			\
{																						\
	reportError("Pxd" #_type "GetFloat: " #_type "has no property of type Float");		\
	return 0;																			\
}

#define NO_SET_PROPERTY(_type)															\
void Pxd ## _type ## SetProperty(Pxd ## _type ,  Pxd ## _type ## Property, void *)	\
{																						\
	reportError("Pxd" #_type "SetFloat: " #_type "has no untyped property");			\
}

#define NO_GET_PROPERTY(_type)															\
void * Pxd ## _type ## GetProperty(Pxd ## _type ,  Pxd ## _type ## Property)			\
{																						\
	reportError("Pxd" #_type "GetFloat: " #_type "has no untyped property");			\
	return 0;																			\
}

#define NO_SET_VEC(_type)																\
void Pxd ## _type ## SetVec(Pxd ## _type ,  Pxd ## _type ## Property, PxdVector)		\
{																						\
	reportError("Pxd" #_type "SetVec: " #_type "has no property of type Vec");			\
}

#define NO_GET_VEC(_type)																\
PxdVector Pxd ## _type ## GetVec(Pxd ## _type ,  Pxd ## _type ## Property)			\
{																						\
	reportError("Pxd" #_type "GetVec: " #_type "has no property of type Vec");			\
	PxdVector v = {0,0,0};																\
	return v;																			\
}

#define NO_SET_QUAT(_type)																\
void Pxd ## _type ## SetQuat(Pxd ## _type ,  Pxd ## _type ## Property, PxdQuaternion)	\
{																						\
	reportError("Pxd" #_type "SetQuat: " #_type "has no property of type Quat");		\
}


#define NO_GET_QUAT(_type)																\
PxdQuaternion Pxd ## _type ## GetQuat(Pxd ## _type ,  Pxd ## _type ## Property)			\
{																						\
	reportError("Pxd" #_type "GetQuat: " #_type "has no property of type Quat");		\
	PxdQuaternion q = {0,0,0,1};														\
	return q;																			\
}

#define NO_SET_TRANSFORM(_type)																\
void Pxd ## _type ## SetTransform(Pxd ## _type ,  Pxd ## _type ## Property, PxdTransform)	\
{																							\
	reportError("Pxd" #_type "SetTransform: " #_type "has no property of type Transform");	\
}

#define NO_GET_TRANSFORM(_type)																\
PxdTransform Pxd ## _type ## GetTransform(Pxd ## _type ,  Pxd ## _type ## Property)		\
{																							\
	reportError("Pxd" #_type "GetTransform: " #_type "has no property of type Transform");	\
	PxdTransform t = {{0,0,0}, {0,0,0,1}};													\
	return t;																				\
}

#ifdef _DEBUG
#define VALIDATE_HANDLE(_struct, _handle, _fn)										\
	PxnContext * ctx = PxnContext::findHandleContext(_handle);						\
	if(ctx==0)																		\
	{																				\
		reportInvalidHandle(_fn);													\
		return;																		\
	}																				\
	Pxn##_struct *obj = ctx->find ## _struct ## V(_handle);							\
	if(obj==0)																		\
	{																				\
	    reportInvalidHandle(_fn);													\
		return;																		\
	}
#else
#define VALIDATE_HANDLE(_struct, _handle, _fn)										\
	PxnContext * ctx = PxnContext::findHandleContext(_handle);						\
	Pxn##_struct * obj = ctx->find ## _struct ## V(_handle);
#endif

#ifdef _DEBUG
#define VALIDATE_HANDLE_RETVAL(_struct, _handle, _fn, _retval)						\
	PxnContext * ctx = PxnContext::findHandleContext(_handle);						\
	if(ctx==0)																		\
	{																				\
		reportInvalidHandle(_fn);													\
		return _retval;																\
	}																				\
	Pxn##_struct *obj = ctx->find ## _struct ## V(_handle);							\
	if(obj==0)																		\
	{																				\
	    reportInvalidHandle(_fn);													\
		return _retval;																\
	}
#else
#define VALIDATE_HANDLE_RETVAL(_struct, _handle, _fn, _retval)						\
	PxnContext * ctx = PxnContext::findHandleContext(_handle);						\
	Pxn##_struct *obj = ctx->find ## _struct ## V(_handle);							
#endif


#ifdef _DEBUG
#define VALIDATE_TYPE(_fn, _struct, _property, _type)						\
{																			\
	if((g##_struct##PropertyType[_property][1] & PXN_PROPERTY_TYPE_##_type) == 0 )	\
	{																		\
		reportInvalidTypeCombination(_fn);									\
		return;																\
	}																		\
	if(!INTERNAL_TYPE_CHECK(_struct, _property))							\
	{																		\
		reportError("Internal type-map mismatch");							\
		return;																\
	}																		\
}
#else
#define VALIDATE_TYPE(_fn, _struct, _property, _type)
#endif

#ifdef _DEBUG
#define VALIDATE_TYPE_RETVAL(_fn, _struct, _property, _type, _retval)		\
{																			\
	if((g##_struct##PropertyType[_property][1] & PXN_PROPERTY_TYPE_##_type) == 0 )	\
	{																		\
		reportInvalidTypeCombination(_fn);									\
		return _retval;														\
	}																		\
	if(!INTERNAL_TYPE_CHECK(_struct, _property))							\
	{																		\
		reportError("Internal type-map mismatch");							\
		return _retval;														\
	}																		\
}
#else
#define VALIDATE_TYPE_RETVAL(_fn, _struct, _property, _type, _retval)		
#endif


#ifdef _DEBUG
#define INTERNAL_TYPE_CHECK(_struct, _property) (g##_struct##PropertyType[_property][0] == _property)
#else
#define INTERNAL_TYPE_CHECK(_struct, _property)	(1)
#endif


#ifdef _DEBUG
#define VALIDATE_SIZE(_fn, _size, _type, _count)							\
{																			\
	if(_size < sizeof(_type)*_count)										\
	{																		\
		reportInvalidSize(_fn);												\
	}																		\
}																			

#else
#define VALIDATE_SIZE(_fn, _size, _type, _count)
#endif


#endif

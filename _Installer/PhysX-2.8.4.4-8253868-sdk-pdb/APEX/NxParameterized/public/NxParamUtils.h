#ifndef NX_PARAM_UTILS_H

#define NX_PARAM_UTILS_H
/*
 * Copyright 2009-2010 NVIDIA Corporation.  All rights reserved.
 *
 * NOTICE TO USER:
 *
 * This source code is subject to NVIDIA ownership rights under U.S. and
 * international Copyright laws.  Users and possessors of this source code
 * are hereby granted a nonexclusive, royalty-free license to use this code
 * in individual and commercial software.
 *
 * NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE
 * CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR
 * IMPLIED WARRANTY OF ANY KIND.  NVIDIA DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOURCE CODE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.
 * IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOURCE CODE.
 *
 * U.S. Government End Users.   This source code is a "commercial item" as
 * that term is defined at  48 C.F.R. 2.101 (OCT 1995), consisting  of
 * "commercial computer  software"  and "commercial computer software
 * documentation" as such terms are  used in 48 C.F.R. 12.212 (SEPT 1995)
 * and is provided to the U.S. Government only as a commercial end item.
 * Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through
 * 227.7202-4 (JUNE 1995), all U.S. Government End Users acquire the
 * source code with only those rights set forth herein.
 *
 * Any use of this source code in individual and commercial software must
 * include, in the user documentation and internal comments to the code,
 * the above Disclaimer and U.S. Government End Users Notice.
 */
// utility methods to operate on NxParameterized data.
#include <PsShare.h>
#include "NxParameterized.h"

namespace NxParameterized
{

class Interface;
class Handle;
class Traits;

/*
\brief Recursively finds a parameter with the given name

This method will recursively search through not only this parameterized but all referenced
parameterized objects as well.  It sets the handle and returns the NxParameterized::Interface in which the name was found.

\param longName	contains the name of the parameter to be found
the longName will work with arrays, structs, and included references

\param outHandle will contain the output handle that provides access to the specified parameter

\returns the NxParameterized::Interface pointer in which the parameter is contained (this could be different than the top level NxParameterized::Interface if the parameter is contained in an included reference)
**/
PX_INLINE Interface * findParam(const Interface &i,const char *longName, Handle &outHandle);


/**
\brief Container for results of getParamList
*/
struct ParamResult
{
public:
	/**
	\brief Constructor
	*/
	ParamResult(const char *name,
				const char *longName,
				const char *className,
				const char *instanceName,
				const Handle &handle,
				physx::PxI32 arraySize,
				DataType type)
		: mArraySize(arraySize),
		mInstanceName(instanceName),
		mClassName(className),
		mName(name),
		mLongName(longName),
		mHandle(handle),
		mDataType(type)
	{}

	/**
	\brief size of array (if parameter is array)
	*/
	physx::PxI32 mArraySize;

	/**
	\brief Name of parameter's parent object
	*/
	const char *mInstanceName;

	/**
	\brief Name of NxParameterized-class of parameter's parent object
	*/
	const char *mClassName;

	/**
	\brief The name of the parameter
	*/
	const char *mName;

	/**
	\brief The fully qualified 'long' name of the parameter
	*/
	const char *mLongName;

	/**
	\brief Use this handle to access the parameter in question
	*/
	Handle mHandle;

	/**
	\brief The type of parameter
	*/
	DataType mDataType;
};

/**
\brief A helper method to retrieve the list of all parameters relative to an interface.

\param [in]  i the input interface 
\param [in]  className is an optional class name to match to.  If this is null, it will return all parameters.
\param [in]  paramName is an optional parameter name to match to.  If this is null, it will return all parameters.
\param [out] count the number of parameters found
\param [in]  recursive if true the search will recurse into every included reference, not just the top parameterized class
\param [in]  classesOnly if true the search will only match against class names
\param [in]  traits typically the APEX traits class, used to allocate the ParamResult, see NxApexSDK::getParameterizedTraits()

\note The return pointer is allocated by the NxParameterized Traits class and should be freed by calling releaseParamList()

*/
PX_INLINE const ParamResult * getParamList(const Interface &i,
								 const char *className,	// name of class to match
								 const char *paramName, // name of parameter to match
								 physx::PxU32 &count,
								 bool recursive,
								 bool classesOnly,
								 NxParameterized::Traits *traits);

PX_INLINE void				releaseParamList(physx::PxU32 resultCount,const ParamResult *results,NxParameterized::Traits *traits);

/// helper function to get an NxParameterized array size
PX_INLINE bool getParamArraySize(const Interface &pm, const char *name, physx::PxI32 &arraySize);

/**
\brief Callback container for getNamedReferences
*/
class NamedReferenceInterface
{
public:
	/**
	\brief Callback

	Calls back to the user with any named reference (not included references) in the NxParameterized::Interface
	If the user returns a NULL pointer, than the original named reference is left alone.
	If the user returns a non-NULL pointer, than the named reference is replaced with the 'const char *' returned.
	*/
	virtual const char * namedReferenceCallback(const char *className,const char *namedReference,Handle &handle) = 0;
};

/// Calls back for every non-included named reference.
PX_INLINE physx::PxU32 getNamedReferences(const Interface &i,
										  NamedReferenceInterface &namedReference,
										  bool recursive);


/// helper function to get an NxParameterized value
PX_INLINE bool getParamBool(const Interface &pm, const char *name, bool &val);
/// helper function to set an NxParameterized value
PX_INLINE bool setParamBool(Interface &pm, const char *name, bool val) ;

/// helper function to get an NxParameterized value
PX_INLINE bool getParamString(const Interface &pm, const char *name, const char *&val);
/// helper function to set an NxParameterized value
PX_INLINE bool setParamString(Interface &pm, const char *name, const char *val) ;

/// helper function to get an NxParameterized value
PX_INLINE bool getParamEnum(const Interface &pm, const char *name, const char *&val);
/// helper function to set an NxParameterized value
PX_INLINE bool setParamEnum(Interface &pm, const char *name, const char *val) ;

/// helper function to get an NxParameterized value
PX_INLINE bool getParamRef(const Interface &pm, const char *name, NxParameterized::Interface *&val);
/// helper function to set an NxParameterized value
PX_INLINE bool setParamRef(Interface &pm, const char *name, NxParameterized::Interface * val) ;

/// helper function to get an NxParameterized value
PX_INLINE bool getParamI8(const Interface &pm, const char *name, physx::PxI8 &val);
/// helper function to set an NxParameterized value
PX_INLINE bool setParamI8(Interface &pm, const char *name, physx::PxI8 val) ;

/// helper function to get an NxParameterized value
PX_INLINE bool getParamI16(const Interface &pm, const char *name, physx::PxI16 &val);
/// helper function to set an NxParameterized value
PX_INLINE bool setParamI16(Interface &pm, const char *name, physx::PxI16 val) ;

/// helper function to get an NxParameterized value
PX_INLINE bool getParamI32(const Interface &pm, const char *name, physx::PxI32 &val);
/// helper function to set an NxParameterized value
PX_INLINE bool setParamI32(Interface &pm, const char *name, physx::PxI32 val) ;

/// helper function to get an NxParameterized value
PX_INLINE bool getParamI64(const Interface &pm, const char *name, physx::PxI64 &val);
/// helper function to set an NxParameterized value
PX_INLINE bool setParamI64(Interface &pm, const char *name, physx::PxI64 val) ;

/// helper function to get an NxParameterized value
PX_INLINE bool getParamU8(const Interface &pm, const char *name, physx::PxU8 &val);
/// helper function to set an NxParameterized value
PX_INLINE bool setParamU8(Interface &pm, const char *name, physx::PxU8 val) ;

/// helper function to get an NxParameterized value
PX_INLINE bool getParamU16(const Interface &pm, const char *name, physx::PxU16 &val);
/// helper function to set an NxParameterized value
PX_INLINE bool setParamU16(Interface &pm, const char *name, physx::PxU16 val) ;

/// helper function to get an NxParameterized value
PX_INLINE bool getParamU32(const Interface &pm, const char *name, physx::PxU32 &val);
/// helper function to set an NxParameterized value
PX_INLINE bool setParamU32(Interface &pm, const char *name, physx::PxU32 val) ;

/// helper function to get an NxParameterized value
PX_INLINE bool getParamU64(const Interface &pm, const char *name, physx::PxU64 &val);
/// helper function to set an NxParameterized value
PX_INLINE bool setParamU64(Interface &pm, const char *name, physx::PxU64 val) ;

/// helper function to get an NxParameterized value
PX_INLINE bool getParamF32(const Interface &pm, const char *name, physx::PxF32 &val);
/// helper function to set an NxParameterized value
PX_INLINE bool setParamF32(Interface &pm, const char *name, physx::PxF32 val) ;

/// helper function to get an NxParameterized value
PX_INLINE bool getParamF64(const Interface &pm, const char *name, physx::PxF64 &val);
/// helper function to set an NxParameterized value
PX_INLINE bool setParamF64(Interface &pm, const char *name, physx::PxF64 val) ;

/// helper function to get an NxParameterized value
PX_INLINE bool getParamVec2(const Interface &pm, const char *name, physx::PxVec2 &val);
/// helper function to set an NxParameterized value
PX_INLINE bool setParamVec2(Interface &pm, const char *name, const physx::PxVec2 &val) ;

/// helper function to get an NxParameterized value
PX_INLINE bool getParamVec3(const Interface &pm, const char *name, physx::PxVec3 &val);
/// helper function to set an NxParameterized value
PX_INLINE bool setParamVec3(Interface &pm, const char *name, const physx::PxVec3 &val) ;

/// helper function to get an NxParameterized value
PX_INLINE bool getParamVec4(const Interface &pm, const char *name, physx::PxVec4 &val);
/// helper function to set an NxParameterized value
PX_INLINE bool setParamVec4(Interface &pm, const char *name, const physx::PxVec4 &val) ;

/// helper function to get an NxParameterized value
PX_INLINE bool getParamQuat(const Interface &pm, const char *name, physx::PxQuat &val);
/// helper function to set an NxParameterized value
PX_INLINE bool setParamQuat(Interface &pm, const char *name, const physx::PxQuat &val) ;

/// helper function to get an NxParameterized value
PX_INLINE bool getParamMat33(const Interface &pm, const char *name, physx::PxMat33 &val);
/// helper function to set an NxParameterized value
PX_INLINE bool setParamMat33(Interface &pm, const char *name, const physx::PxMat33 &val) ;

/// helper function to get an NxParameterized value
PX_INLINE bool getParamMat34(const Interface &pm, const char *name, physx::PxMat44 &val);
/// helper function to set an NxParameterized value
PX_INLINE bool setParamMat34(Interface &pm, const char *name, const physx::PxMat44 &val) ;

/// helper function to get an NxParameterized value
PX_INLINE bool getParamMat44(const Interface &pm, const char *name, physx::PxMat44 &val);
/// helper function to set an NxParameterized value
PX_INLINE bool setParamMat44(Interface &pm, const char *name, const physx::PxMat44 &val) ;

/// helper function to get an NxParameterized value
PX_INLINE bool getParamBounds3(const Interface &pm, const char *name, physx::PxBounds3 &val);
/// helper function to set an NxParameterized value
PX_INLINE bool setParamBounds3(Interface &pm, const char *name, const physx::PxBounds3 &val) ;

};


#include "NxParamUtils.inl"

#endif

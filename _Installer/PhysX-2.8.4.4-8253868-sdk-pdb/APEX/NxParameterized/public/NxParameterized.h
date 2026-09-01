#ifndef NX_APEX_PARAMETERIZED_H
#define NX_APEX_PARAMETERIZED_H

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

/**
 * APEX uses a framework called NxParameterized for storing asset and actor data. 
 * NxParameterized classes provide reflection on every parameter they include making it 
 * effective for serialization and auto-generation of user interfaces for tools.<br>
 *
 * NxParameterized stores data in C++ classes which are auto-generated from their 
 * descriptions written in a special domain specific language (DSL). Description files 
 * contain information on internal layout and various high-level metadata of corresponding 
 * classes and their members. All generated classes implement special interface for 
 * run time reflection and modification of data.
 * */

/*!
\file
\brief NxParameterized classes
*/

#include <PxSimpleTypes.h>
#include <PxVec2.h>
#include <PxVec3.h>
#include <PxVec4.h>
#include <PxQuat.h>
#include <PxBounds3.h>
#include <PxMat33.h>
#include <PxMat44.h>
#include "NxParameterizedTraits.h"
#include <new> // for placement new

PX_PUSH_PACK_DEFAULT

namespace NxParameterized
{

class Interface;


/**
\brief Various errors that may be returned from NxParameterized calls
*/
enum ErrorType
{
   ERROR_NONE = 0,
   ERROR_TYPE_NOT_SUPPORTED,
   ERROR_INDEX_OUT_OF_RANGE,
   ERROR_INVALID_PARAMETER_NAME,
   ERROR_INVALID_PARAMETER_HANDLE,
   ERROR_CAST_FAILED,
   ERROR_INVALID_ENUM_VAL,
   ERROR_SYNTAX_ERROR_IN_NAME,
   ERROR_IS_LEAF_NODE,
   ERROR_RESULT_BUFFER_OVERFLOW,
   ERROR_NAME_DOES_NOT_MATCH_DEFINITION,
   ERROR_NOT_AN_ARRAY,
   ERROR_ARRAY_SIZE_IS_FIXED,
   ERROR_ARRAY_RESIZING_IS_NOT_SUPPORTED,
   ERROR_ARRAY_IS_TOO_SMALL,
   ERROR_INVALID_ARRAY_DIMENSION,
   ERROR_INVALID_ARRAY_SIZE,
   ERROR_PARAMETER_HANDLE_DOES_NOT_MATCH_CLASS,
   ERROR_MEMORY_ALLOCATION_FAILURE,
   ERROR_INVALID_REFERENCE_INCLUDE_HINT,
   ERROR_INVLAID_REFERENCE_VALUE,
   ERROR_PARAMETER_HANDLE_NOT_INITIALIZED,
   ERROR_PARAMETER_DEFINITIONS_DO_NOT_MATCH,
   ERROR_HANDLE_MISSING_INTERFACE_POINTER,
   ERROR_HANDLE_INVALID_INTERFACE_POINTER,
   ERROR_INVALID_CALL_ON_NAMED_REFERENCE,
   ERROR_NOT_IMPLEMENTED
};

/**
\brief These types are supported in NxParameterized schemas
*/    
enum DataType
{
    TYPE_UNDEFINED = 0,

    /**
    \brief Array type, size may be static or dynamic
    \see Definition::arraySizeIsFixed(), Handle::resizeArray(), Handle::getArraySize()
    */
    TYPE_ARRAY,

    TYPE_STRUCT,

    TYPE_BOOL,
    /**
    \brief String type, represented by a const char pointer
    \see Handle::getParamString(), Handle::setParamString()
    */
    TYPE_STRING,

    /**
    \brief Enum type, represented by a const char pointer
    \see Definition::numEnumVals(), Definition::enumVal(), Handle::getParamEnum(), Handle::setParamEnum()
    */
    TYPE_ENUM,

    /**
    \brief Reference type, may be a named or included reference

    References are intended to be used in instances where a class needs either a 
    named reference (an emitter asset references an IOFX and IOS asset), or an 
    included reference (a destructible asset that serializes an APEX render mesh).
    References may also used to create a unions within a class.  Each reference will
    contain one or more variants to allow for unions.
        
    \see Handle::initParamRef(), Definition::numRefVariants(), Definition::refVariantVal(), Handle::initParamRef(), Handle::getParamRef(), Handle::setParamRef()
    */
    TYPE_REF,

    TYPE_I8,
    TYPE_I16,
    TYPE_I32,
    TYPE_I64,

    TYPE_U8,
    TYPE_U16,
    TYPE_U32,
    TYPE_U64,

    TYPE_F32,
    TYPE_F64,

	TYPE_VEC2,
    TYPE_VEC3,
	TYPE_VEC4,
    TYPE_QUAT,
    TYPE_MAT33,
    TYPE_MAT34,
    TYPE_BOUNDS3,
    TYPE_MAT44,

	TYPE_POINTER
};

/**
\brief returns the data type in string form
*/
const char *typeToStr(DataType type);

/**
\brief returns the data type contained in the string
*/
DataType strToType(const char *str);

/**
\brief Provides hints about the parameter definition

\see Definition
*/
class Hint
{
    public:

    /**
    \brief Returns the name of the hint
    */
    virtual const char *    name(void) const = 0;

    /**
    \brief Returns the type of the hint
    */    
    virtual DataType         type(void) const = 0;

    /**
    \brief Returns the unsigned 64-bit value of the hint
    \note Undefined results if the type != TYPE_U64
    */
    virtual    physx::PxU64             asUInt(void) const = 0;
    
    /**
    \brief Returns the 64-bit floating point value of the hint
    \note Undefined results if the type != TYPE_FU64
    */    
    virtual physx::PxF64             asFloat(void) const = 0;

    /**
    \brief Returns the const character pointer for hint
    \note Undefined results if the type != TYPE_STRING
    */
    virtual const char *    asString(void) const = 0;

    private:
};

/**
\brief Provides information about a parameter
*/
class Definition
{
public:
    /**
    \brief Returns the number of hints in the parameter Definition
    */
    virtual physx::PxI32                             numHints(void) const = 0;

    /**
    \brief Returns the Hint located at the index
    \returns NULL if index >= Hint::numHints()
    */
    virtual const Hint *                    hint(physx::PxI32 index) const = 0;

    /**
    \brief Returns the Hint that matches the input name
    \returns NULL if name does not match any of the Definition's hints
    */
    virtual const Hint *                    hint(const char *name) const = 0;

	/**
	\brief Store parameter hints
	\warning Only for internal use
	*/
    virtual void                            setHints(const Hint **hints, physx::PxI32 n) = 0;

	/**
	\brief Add parameter hint
	\warning Only for internal use
	*/
	virtual void                            addHint(Hint *hint) = 0;
    
    /**
    \brief Returns this Definition's parent Definition
    \returns NULL if at the top level
    */
    virtual const Definition *    parent(void) const = 0;

    /**
    \brief Returns this Definition's top most ancestor
    */
    virtual const Definition *    root(void) const = 0;

    /**
    \brief Returns the name of the parameter 
    */
    virtual const char *                    name(void) const = 0;

    /**
    \brief Returns the long name of the parameter 
    If the parameter is a member of a struct or an array the long name will contain these
    parent names
    */
    virtual const char *                    longName(void) const = 0;

    /**
    \brief Returns the name of parameter's struct type
	\returns NULL if not struct
    */
    virtual const char *                    structName(void) const = 0;

	/**
    \brief Returns the parameter type
    */
    virtual DataType                         type(void) const = 0;

    /**
    \brief Returns the number of variants this parameter could be
    A reference is sometimes a union of different types, each different type is referred to as 
    a "variant". Variants can be used in either included or named references.
    */
    virtual    physx::PxI32                             numRefVariants(void) const = 0;

    /**
    \brief Get the string value of the reference variant
    */
    virtual    const char *                     refVariantVal(physx::PxI32 index) const = 0;

    /**
    \brief Returns the number of enums for the parameter
    */
    virtual physx::PxI32                             numEnumVals(void) const = 0;

    /**
    \brief Given the enum string, get the enum val index
    \returns -1 if input enum_val is not found
    */
    virtual    physx::PxI32                             enumValIndex( const char * enum_val ) const = 0;

    /**
    \brief Returns the Enum string located at the index
    \returns NULL if index >= Hint::numEnumVals()
    */
    virtual const char *                    enumVal(physx::PxI32 index) const = 0;

	/**
	\brief Store possible enum values for TYPE_ENUM parameter
	\warning Only for internal use
	*/
    virtual void                             setEnumVals(const char **enum_vals, physx::PxI32 n) = 0;

	/**
	\brief Add new enum value to a list of possible enum values for TYPE_ENUM parameter
	\warning Only for internal use
	*/
    virtual void                             addEnumVal(const char *enum_val) = 0;

    /**
    \brief Returns the number of dimensions of a static array
    */
    virtual physx::PxI32 arrayDimension(void) const = 0;

    /**
    \brief Returns the array size of a static array
    \returns 0 for dynamic arrays
    */
    virtual physx::PxI32 arraySize(physx::PxI32 dimension = 0) const = 0;

    /**
    \brief Used to determine if an array is static or dynamic
    */
    virtual bool arraySizeIsFixed(void) const = 0;

	/**
	\brief Set size of static array
	\warning Only for internal use
	*/
    virtual bool setArraySize(physx::PxI32 size) = 0; // -1 if the size is not fixed

	/**
	\brief Used to determine if parameter is aggregate (TYPE_STRUCT or TYPE_ARRAY) or not.
	*/
    virtual bool isLeaf(void) const = 0;
    
    /**
    \brief Used to determine if reference is included or not.
    */
    virtual bool isIncludedRef(void) const = 0;

    /**
    \brief Returns the number of children (for a struct or static array)
    */
    virtual physx::PxI32 numChildren(void) const = 0;

	/**
	\brief Access definition of i-th child parameter.
	*/
    virtual const Definition *child(physx::PxI32 index) const = 0;

	/**
	\brief Access definition of child parameter with given name
	\warning Only used with TYPE_STRUCT
	*/
    virtual const Definition *child(const char *name, physx::PxI32 &index) const = 0;

	/**
	\brief Store definitions of child parameters
	\warning Only for internal use
	*/
	virtual void setChildren(Definition **children, physx::PxI32 n) = 0;

	/**
	\brief Add definition of one morechild parameter
	\warning Only for internal use
	*/
    virtual void addChild(Definition *child) = 0;

    /**
    \brief Set indices of child handles which must be released when downsizing array
    */
    virtual void setDynamicHandleIndicesMap(const physx::PxU8 *indices, physx::PxU32 numIndices) = 0;

    /**
    \brief Get indices of child handles which must be released when downsizing array
    */
    virtual const physx::PxU8 * getDynamicHandleIndicesMap(physx::PxU32 &outNumIndices) const = 0;
};

/**
\brief Provides access to individual parameters within the NxParameterized object
*/
class Handle
{
public:

    enum { MAX_DEPTH = 16 };

    /**
    \brief The constructor takes a pointer (if the user requires an instance before the ::NxParameterized::Interface exists)
    */
    PX_INLINE Handle(::NxParameterized::Interface *iface);

    /**
    \brief The constructor takes a reference
    */
    PX_INLINE Handle(::NxParameterized::Interface &iface);

    /**
    \brief The constructor takes a const interface
    \note WARNING: this is evil because the const is not preserved...
    */
    PX_INLINE Handle(const ::NxParameterized::Interface &iface);

    /**
    \brief Copy constructor
    */
    PX_INLINE Handle(const Handle &param_handle);

    /**
    \brief This constructor allows the user to create a handle that initially points at a particular parameter in the instance
    */
    PX_INLINE Handle(::NxParameterized::Interface &instance, const char *longName);

    /**
    \brief Get the parameter Definition for the handle's parameter
    */
    PX_INLINE const Definition *parameterDefinition(void) const { return(mParameterDefinition); }
    
    /**
    \brief Get the depth of the handle within the NxParameterized object
    */
    PX_INLINE physx::PxI32 numIndexes(void) const { return(mNumIndexes); }

    /**
    \brief Get the index at the specified depth within the NxParameterized object
    */
    PX_INLINE physx::PxI32 index(physx::PxI32 i) const { return(mIndexList[i]); }

    /**
    \brief Reduce the handle's depth within the NxParameterized object
    */
    PX_INLINE physx::PxI32 popIndex(physx::PxI32 levels = 1);

    PX_INLINE ErrorType set(const ::NxParameterized::Interface *instance, const Definition *root, const char *child_long_name);
    PX_INLINE ErrorType set(const ::NxParameterized::Interface *instance, const char *child_long_name);
    
    /**
    \brief Get the parameter specified by longName and set the Handle to point it
    Given a long name like "mystruct.somearray[10].foo", it will return
    a handle to that specific parameter.  The handle can then be used to
    set/get values, as long as it's a handle to a leaf node.
    \note this method will not work if an included reference's child is included in the longName
    */
    PX_INLINE ErrorType getParameter(const char *longName);

    /**
    \brief Set the depth of the handle within the handle's parameter

    The set method is useful for accessing indices within an array of parameters or members
    within a struct.
    */
    PX_INLINE ErrorType set(physx::PxI32 child_index);

    /**
    \brief Get a child handle of this handle
    Sets handle to point to a child of this handle, with child_long_name being
    relative to the current long name.
    */
    PX_INLINE ErrorType getChildHandle(const ::NxParameterized::Interface *instance, const char *child_long_name, Handle &outHandle);

    /**
    \brief Get a child handle of this handle
    Sets handle to point to a direct child of this handle.  Works with structs and arrays.
    */
    PX_INLINE ErrorType getChildHandle(physx::PxI32 index, Handle &outHandle);

    /**
    \brief Returns the long name of the parameter, with indexes into str.
    \returns false if the long name didn't fit in str.
    */
    PX_INLINE bool getLongName(char *str, physx::PxU32 max_str_len);

    /**
    \brief Reset all of the state data for the handle
    */
    PX_INLINE void reset();

	/**
	\brief Does handle correspond to an existing Interface?
	*/
    PX_INLINE bool isValid(void) const { return(mIsValid); }

	/**
	\brief Same as isValid
	*/
    PX_INLINE operator bool() const { return(isValid()); }

	/**
	\brief Return user data stored in handle
	*/
    PX_INLINE const void *userData(void) const { return(mUserData); }

	/**
	\brief Return user data stored in handle
	*/
    PX_INLINE void *userData(void) { return(mUserData); }

	/**
	\brief Store user data in handle
	*/
    PX_INLINE void setUserData(void *user_data) { mUserData = user_data; }

	/**
	\brief Get associated NxParameterized object
	*/
    PX_INLINE ::NxParameterized::Interface * getInterface(void) const { return mInterface; };

	/**
	\brief Set associated NxParameterized object
	*/
    PX_INLINE void setInterface(::NxParameterized::Interface *iface) { mInterface = iface; };

	/**
	\brief Set associated NxParameterized object
	*/
	PX_INLINE void setInterface(const ::NxParameterized::Interface *iface) { mInterface = (::NxParameterized::Interface *)iface; };

    /**
    \brief Initialize a Reference parameter

    \param [in] chosenRefStr This string must be one of the strings returned from 
    Definition::refVariantVal()

    \see Definition::numRefVariants(), Definition::refVariantVal()
    */
    PX_INLINE ErrorType initParamRef(const char *chosenRefStr = 0);

    /**
    \brief Store this Handle's parameter to a string
    \param [in] buf this buffer is used to store any strings that need to be constructed
    \param [in] bufSize size of buf
    \param [out] ret this contains a pointer to the value as a string, it may or may not 
    be equal to buf, depending if the value had to be constructed dynamically or if it already exists as a static string
    */
    PX_INLINE ErrorType valueToStr(char *buf, physx::PxU32 bufSize, const char *&ret);
    
    /**
    \brief Store the string to this Handle's parameter
    \returns ERROR_TYPE_NOT_SUPPORTED if the Handle's parameter type is an array, struct, or reference
    \returns ERROR_NONE if the Handle's parameter type is a simple type (u32, i32, vec3, etc)
    */
    PX_INLINE ErrorType strToValue(const char *str, const char **endptr);

    /**
    \brief Resize the array that the Handle points to
    */
    ErrorType resizeArray(physx::PxI32 new_size);

    /// Get the array size for the given array dimension
    PX_INLINE ErrorType getArraySize(physx::PxI32 &size, physx::PxI32 dimension = 0) const;

    /// Swap two elements of an array
    PX_INLINE ErrorType swapArrayElements(physx::PxU32 firstElement, physx::PxU32 secondElement);

    // These functions wrap the raw(Get|Set)XXXXX() methods.  They deal with
    // error handling and casting.
    PX_INLINE ErrorType getParamBool(bool &val) const ;
    PX_INLINE ErrorType setParamBool(bool val) ;
    PX_INLINE ErrorType getParamBoolArray(bool *array, physx::PxI32 n, physx::PxI32 offset = 0) const ;
    PX_INLINE ErrorType setParamBoolArray(const bool *array, physx::PxI32 n, physx::PxI32 offset= 0) ;

    PX_INLINE ErrorType getParamString(const char *&val) const ;
    PX_INLINE ErrorType setParamString(const char *val) ;
    PX_INLINE ErrorType getParamStringArray(char **array, physx::PxI32 n, physx::PxI32 offset = 0) const ;
    PX_INLINE ErrorType setParamStringArray(const char **array, physx::PxI32 n, physx::PxI32 offset= 0) ;

    PX_INLINE ErrorType getParamEnum(const char *&val) const ;
    PX_INLINE ErrorType setParamEnum(const char *val) ;
    PX_INLINE ErrorType getParamEnumArray(char **array, physx::PxI32 n, physx::PxI32 offset = 0) const ;
    PX_INLINE ErrorType setParamEnumArray(const char **array, physx::PxI32 n, physx::PxI32 offset= 0) ;

    PX_INLINE ErrorType getParamRef(::NxParameterized::Interface *&val) const ;
    PX_INLINE ErrorType setParamRef(::NxParameterized::Interface * val) ;
    PX_INLINE ErrorType getParamRefArray(::NxParameterized::Interface **array, physx::PxI32 n, physx::PxI32 offset = 0) const ;
    PX_INLINE ErrorType setParamRefArray(::NxParameterized::Interface **array, physx::PxI32 n, physx::PxI32 offset = 0) ;

    PX_INLINE ErrorType getParamI8(physx::PxI8 &val) const ;
    PX_INLINE ErrorType setParamI8(physx::PxI8 val) ;
    PX_INLINE ErrorType getParamI8Array(physx::PxI8 *array, physx::PxI32 n, physx::PxI32 offset = 0) const ;
    PX_INLINE ErrorType setParamI8Array(const physx::PxI8 *val, physx::PxI32 n, physx::PxI32 offset= 0) ;

    PX_INLINE ErrorType getParamI16(physx::PxI16 &val) const ;
    PX_INLINE ErrorType setParamI16(physx::PxI16 val) ;
    PX_INLINE ErrorType getParamI16Array(physx::PxI16 *array, physx::PxI32 n, physx::PxI32 offset = 0) const ;
    PX_INLINE ErrorType setParamI16Array(const physx::PxI16 *val, physx::PxI32 n, physx::PxI32 offset= 0) ;

    PX_INLINE ErrorType getParamI32(physx::PxI32 &val) const ;
    PX_INLINE ErrorType setParamI32(physx::PxI32 val) ;
    PX_INLINE ErrorType getParamI32Array(physx::PxI32 *array, physx::PxI32 n, physx::PxI32 offset = 0) const ;
    PX_INLINE ErrorType setParamI32Array(const physx::PxI32 *val, physx::PxI32 n, physx::PxI32 offset= 0) ;

    PX_INLINE ErrorType getParamI64(physx::PxI64 &val) const ;
    PX_INLINE ErrorType setParamI64(physx::PxI64 val) ;
    PX_INLINE ErrorType getParamI64Array(physx::PxI64 *array, physx::PxI32 n, physx::PxI32 offset = 0) const ;
    PX_INLINE ErrorType setParamI64Array(const physx::PxI64 *val, physx::PxI32 n, physx::PxI32 offset= 0) ;

    PX_INLINE ErrorType getParamU8(physx::PxU8 &val) const ;
    PX_INLINE ErrorType setParamU8(physx::PxU8 val) ;
    PX_INLINE ErrorType getParamU8Array(physx::PxU8 *array, physx::PxI32 n, physx::PxI32 offset = 0) const ;
    PX_INLINE ErrorType setParamU8Array(const physx::PxU8 *val, physx::PxI32 n, physx::PxI32 offset= 0) ;

    PX_INLINE ErrorType getParamU16(physx::PxU16 &val) const ;
    PX_INLINE ErrorType setParamU16(physx::PxU16 val) ;
    PX_INLINE ErrorType getParamU16Array(physx::PxU16 *array, physx::PxI32 n, physx::PxI32 offset = 0) const ;
    PX_INLINE ErrorType setParamU16Array(const physx::PxU16 *array, physx::PxI32 n, physx::PxI32 offset= 0) ;

    PX_INLINE ErrorType getParamU32(physx::PxU32 &val) const ;
    PX_INLINE ErrorType setParamU32(physx::PxU32 val) ;
    PX_INLINE ErrorType getParamU32Array(physx::PxU32 *array, physx::PxI32 n, physx::PxI32 offset = 0) const ;
    PX_INLINE ErrorType setParamU32Array(const physx::PxU32 *array, physx::PxI32 n, physx::PxI32 offset= 0) ;

    PX_INLINE ErrorType getParamU64(physx::PxU64 &val) const ;
    PX_INLINE ErrorType setParamU64(physx::PxU64 val) ;
    PX_INLINE ErrorType getParamU64Array(physx::PxU64 *array, physx::PxI32 n, physx::PxI32 offset = 0) const ;
    PX_INLINE ErrorType setParamU64Array(const physx::PxU64 *array, physx::PxI32 n, physx::PxI32 offset= 0) ;

    PX_INLINE ErrorType getParamF32(physx::PxF32 &val) const ;
    PX_INLINE ErrorType setParamF32(physx::PxF32 val) ;
    PX_INLINE ErrorType getParamF32Array(physx::PxF32 *array, physx::PxI32 n, physx::PxI32 offset = 0) const ;
    PX_INLINE ErrorType setParamF32Array(const physx::PxF32 *array, physx::PxI32 n, physx::PxI32 offset= 0) ;

    PX_INLINE ErrorType getParamF64(physx::PxF64 &val) const ;
    PX_INLINE ErrorType setParamF64(physx::PxF64 val) ;
    PX_INLINE ErrorType getParamF64Array(physx::PxF64 *array, physx::PxI32 n, physx::PxI32 offset = 0) const ;
    PX_INLINE ErrorType setParamF64Array(const physx::PxF64 *array, physx::PxI32 n, physx::PxI32 offset= 0) ;

    PX_INLINE ErrorType setParamVec2(const physx::PxVec2 &val) ;
    PX_INLINE ErrorType getParamVec2(physx::PxVec2 &val) const ;
    PX_INLINE ErrorType getParamVec2Array(physx::PxVec2 *array, physx::PxI32 n, physx::PxI32 offset = 0) const ;
    PX_INLINE ErrorType setParamVec2Array(const physx::PxVec2 *array, physx::PxI32 n, physx::PxI32 offset= 0) ;

    PX_INLINE ErrorType setParamVec3(const physx::PxVec3 &val) ;
    PX_INLINE ErrorType getParamVec3(physx::PxVec3 &val) const ;
    PX_INLINE ErrorType getParamVec3Array(physx::PxVec3 *array, physx::PxI32 n, physx::PxI32 offset = 0) const ;
    PX_INLINE ErrorType setParamVec3Array(const physx::PxVec3 *array, physx::PxI32 n, physx::PxI32 offset= 0) ;

    PX_INLINE ErrorType setParamVec4(const physx::PxVec4 &val) ;
    PX_INLINE ErrorType getParamVec4(physx::PxVec4 &val) const ;
    PX_INLINE ErrorType getParamVec4Array(physx::PxVec4 *array, physx::PxI32 n, physx::PxI32 offset = 0) const ;
    PX_INLINE ErrorType setParamVec4Array(const physx::PxVec4 *array, physx::PxI32 n, physx::PxI32 offset= 0) ;

    PX_INLINE ErrorType setParamQuat(const physx::PxQuat &val) ;
    PX_INLINE ErrorType getParamQuat(physx::PxQuat &val) const ;
    PX_INLINE ErrorType getParamQuatArray(physx::PxQuat *array, physx::PxI32 n, physx::PxI32 offset = 0) const ;
    PX_INLINE ErrorType setParamQuatArray(const physx::PxQuat *array, physx::PxI32 n, physx::PxI32 offset= 0) ;

    PX_INLINE ErrorType setParamMat33(const physx::PxMat33 &val) ;
    PX_INLINE ErrorType getParamMat33(physx::PxMat33 &val) const ;
    PX_INLINE ErrorType getParamMat33Array(physx::PxMat33 *array, physx::PxI32 n, physx::PxI32 offset = 0) const ;
    PX_INLINE ErrorType setParamMat33Array(const physx::PxMat33 *array, physx::PxI32 n, physx::PxI32 offset= 0) ;
    
    PX_INLINE ErrorType setParamMat34(const physx::PxMat44 &val) ;
    PX_INLINE ErrorType getParamMat34(physx::PxMat44 &val) const ;
    PX_INLINE ErrorType getParamMat34Array(physx::PxMat44 *array, physx::PxI32 n, physx::PxI32 offset = 0) const ;
    PX_INLINE ErrorType setParamMat34Array(const physx::PxMat44 *array, physx::PxI32 n, physx::PxI32 offset= 0) ;
    
    PX_INLINE ErrorType setParamMat44(const physx::PxMat44 &val) ;
    PX_INLINE ErrorType getParamMat44(physx::PxMat44 &val) const ;
    PX_INLINE ErrorType getParamMat44Array(physx::PxMat44 *array, physx::PxI32 n, physx::PxI32 offset = 0) const ;
    PX_INLINE ErrorType setParamMat44Array(const physx::PxMat44 *array, physx::PxI32 n, physx::PxI32 offset= 0) ;

    PX_INLINE ErrorType setParamBounds3(const physx::PxBounds3 &val) ;
    PX_INLINE ErrorType getParamBounds3(physx::PxBounds3 &val) const ;
    PX_INLINE ErrorType getParamBounds3Array(physx::PxBounds3 *array, physx::PxI32 n, physx::PxI32 offset = 0) const ;
    PX_INLINE ErrorType setParamBounds3Array(const physx::PxBounds3 *array, physx::PxI32 n, physx::PxI32 offset= 0) ;

	/// Template version of setParamXxx
    template <typename T> PX_INLINE ErrorType setParam(const T &val)
    {
        PX_ALWAYS_ASSERT();
        return ::NxParameterized::ERROR_NOT_IMPLEMENTED;
    }

	/// Template version of getParamXxx
    template <typename T> PX_INLINE ErrorType getParam(T &val) const
    {
        PX_ALWAYS_ASSERT();
        return ::NxParameterized::ERROR_NOT_IMPLEMENTED;
    }

	/// Template version of getParamXxxArray
    template <typename T> PX_INLINE ErrorType getParamArray(T *array, physx::PxI32 n, physx::PxI32 offset = 0) const
    {
        PX_ALWAYS_ASSERT();
        return ::NxParameterized::ERROR_NOT_IMPLEMENTED;
    }

	/// Template version of setParamXxxArray
    template <typename T> PX_INLINE ErrorType setParamArray(const T *array, physx::PxI32 n, physx::PxI32 offset = 0)
    {
        PX_ALWAYS_ASSERT();
        return ::NxParameterized::ERROR_NOT_IMPLEMENTED;
    }

private:

    PX_INLINE void pushIndex(physx::PxI32 index);

    bool mIsValid;
    physx::PxI32 mNumIndexes;
    physx::PxI32 mIndexList[MAX_DEPTH];
    const Definition *mParameterDefinition;
    void *mUserData;
    ::NxParameterized::Interface    *mInterface;
};

/// A callback class for notification just prior to serialization
class SerializationCallback
{
public:
	/// Callback method
    virtual void preSerialize( void *userData = NULL  ) = 0;
};


/**
\brief Represents the interface to the NxParameterized object
*/
class Interface
{
    friend class Handle;
public:

    /**
    \brief Destroys the NxParameterized object and all nested dynamic objects contained within
    */
    virtual void destroy() = 0;

    /**
    \brief Initializes all parameters to their default value
    */
    virtual void initDefaults(void) = 0;

    /**
    \brief Get the class name
    */
    virtual const char * className(void) const = 0;
    
    /**
    \brief Sets the class name
    
    This method is used for named references. The input name should be one of the possible variants.
    \see Definition::numRefVariants(), Definition::refVariantVal()
    */
    virtual void setClassName(const char *name) = 0;

    /**
    \brief Get the name of the NxParameterized object
    This method is used for named references. The name will typically specify an asset to be loaded.
    */
    virtual const char * name(void) const = 0;
    
    /**
    \brief Set the name of the NxParameterized object
    This method is used for named references. The name will typically specify an asset to be loaded.
    */
    virtual void setName(const char *name) = 0;

    /**
    \brief Get the class version
    */
    virtual physx::PxU32 version(void) const = 0;

    /**
    \brief Get the class checksum.
    \param bits contains the number of bits contained in the checksum
    \returns A pointer to a constant array of physx::PxU32 values representing the checksum
    */    
    virtual const physx::PxU32 * checksum(physx::PxU32 &bits) const = 0;

    /**
    \brief Get the number of parameters contained in the NxParameterized object
    */
    virtual physx::PxI32 numParameters(void) = 0;

    /**
    \brief Get the definition of i-th parameter
    */
    virtual const Definition *parameterDefinition(physx::PxI32 index) = 0;

    /**
    \brief Get definition of root structure
    */
    virtual const Definition *rootParameterDefinition(void) = 0;

    /**
    \brief Get definition of root structure
    */
	virtual const Definition *rootParameterDefinition(void) const = 0;

    /**
    \brief Set the Handle to point to the parameter specified by longName
    Given a long name like "mystruct.somearray[10].foo", it will return
    a handle to that specific parameter.  The handle can then be used to
    set/get values, as long as it's a handle to a leaf node.
    \note this method will not work if an included reference's child is included in the longName
    */
    virtual ErrorType getParameterHandle(const char *longName, Handle  &handle) const = 0;

    /**
    \brief Set the Handle to point to the parameter specified by longName
    Given a long name like "mystruct.somearray[10].foo", it will return
    a handle to that specific parameter.  The handle can then be used to
    set/get values, as long as it's a handle to a leaf node.
    \note this method will not work if an included reference's child is included in the longName
    */
    virtual ErrorType getParameterHandle(const char *longName, Handle  &handle) = 0;

    /// An application may set a callback function that is called immediately before serialization
    virtual void setSerializationCallback(SerializationCallback *cb, void *userData = NULL) = 0;

	/// Called prior by Serializer to serialization
    virtual void callPreSerializeCallback() const = 0;


    /**
    \brief Compares two NxParameterized objects
    \param [in] obj The other ::NxParameterized::Interface object (this == obj)
    \param [out] handlesOfInequality If the return value is False, these handles will contain the path to where definition or data is not identical
    \param [in] numHandlesOfInequality The number of handles that can be written to.
    \returns true if parameter definition tree is equal as well as parameter values
    */
    virtual bool equals(const ::NxParameterized::Interface &obj, Handle* handlesOfInequality = NULL, physx::PxU32 numHandlesOfInequality = 0) const = 0;

    /**
    \brief Checks if object satisfies schema constraints
    \param [out] invalidHandles If the return value is False, these handles will contain the path to invalid data
    \param [in] numInvalidHandles The number of handles that can be written to.
    \returns true if all values satisfy constraints
    */
    virtual bool areParamsOK(Handle *invalidHandles = NULL, physx::PxU32 numInvalidHandles = 0) = 0;

    /**
    \brief Copies an NxParameterized object
    \param [in] src the src NxParameterized object will be copied to this object.  It must be of the same type (class name).
    */
    virtual ErrorType copy(const ::NxParameterized::Interface &src) = 0;

protected:
	/// Initialize a Reference parameter
    virtual ErrorType initParamRef(const Handle &handle, const char *chosenRefStr = 0) = 0;

    // These functions wrap the raw(Get|Set)XXXXX() methods.  They deal with
    // error handling and casting.
    virtual ErrorType getParamBool(const Handle &handle, bool &val) const = 0;
    virtual ErrorType setParamBool(const Handle &handle, bool val) = 0;
    virtual ErrorType getParamBoolArray(const Handle &handle, bool *array, physx::PxI32 n, physx::PxI32 offset = 0) const = 0;
    virtual ErrorType setParamBoolArray(const Handle &handle, const bool *array, physx::PxI32 n, physx::PxI32 offset= 0) = 0;

    virtual ErrorType getParamString(const Handle &handle, const char *&val) const = 0;
    virtual ErrorType setParamString(const Handle &handle, const char *val) = 0;
    virtual ErrorType getParamStringArray(const Handle &handle, char **array, physx::PxI32 n, physx::PxI32 offset = 0) const = 0;
    virtual ErrorType setParamStringArray(const Handle &handle, const char **array, physx::PxI32 n, physx::PxI32 offset= 0) = 0;

    virtual ErrorType getParamEnum(const Handle &handle, const char *&val) const = 0;
    virtual ErrorType setParamEnum(const Handle &handle, const char *val) = 0;
    virtual ErrorType getParamEnumArray(const Handle &handle, char **array, physx::PxI32 n, physx::PxI32 offset = 0) const = 0;
    virtual ErrorType setParamEnumArray(const Handle &handle, const char **array, physx::PxI32 n, physx::PxI32 offset= 0) = 0;

    virtual ErrorType getParamRef(const Handle &handle, ::NxParameterized::Interface *&val) const = 0;
    virtual ErrorType setParamRef(const Handle &handle, ::NxParameterized::Interface * val) = 0;
    virtual ErrorType getParamRefArray(const Handle &handle, ::NxParameterized::Interface **array, physx::PxI32 n, physx::PxI32 offset = 0) const = 0;
    virtual ErrorType setParamRefArray(const Handle &handle, /*const*/ ::NxParameterized::Interface **array, physx::PxI32 n, physx::PxI32 offset = 0) = 0;

    virtual ErrorType getParamI8(const Handle &handle, physx::PxI8 &val) const = 0;
    virtual ErrorType setParamI8(const Handle &handle, physx::PxI8 val) = 0;
    virtual ErrorType getParamI8Array(const Handle &handle, physx::PxI8 *array, physx::PxI32 n, physx::PxI32 offset = 0) const = 0;
    virtual ErrorType setParamI8Array(const Handle &handle, const physx::PxI8 *val, physx::PxI32 n, physx::PxI32 offset= 0) = 0;

    virtual ErrorType getParamI16(const Handle &handle, physx::PxI16 &val) const = 0;
    virtual ErrorType setParamI16(const Handle &handle, physx::PxI16 val) = 0;
    virtual ErrorType getParamI16Array(const Handle &handle, physx::PxI16 *array, physx::PxI32 n, physx::PxI32 offset = 0) const = 0;
    virtual ErrorType setParamI16Array(const Handle &handle, const physx::PxI16 *val, physx::PxI32 n, physx::PxI32 offset= 0) = 0;

    virtual ErrorType getParamI32(const Handle &handle, physx::PxI32 &val) const = 0;
    virtual ErrorType setParamI32(const Handle &handle, physx::PxI32 val) = 0;
    virtual ErrorType getParamI32Array(const Handle &handle, physx::PxI32 *array, physx::PxI32 n, physx::PxI32 offset = 0) const = 0;
    virtual ErrorType setParamI32Array(const Handle &handle, const physx::PxI32 *val, physx::PxI32 n, physx::PxI32 offset= 0) = 0;

    virtual ErrorType getParamI64(const Handle &handle, physx::PxI64 &val) const = 0;
    virtual ErrorType setParamI64(const Handle &handle, physx::PxI64 val) = 0;
    virtual ErrorType getParamI64Array(const Handle &handle, physx::PxI64 *array, physx::PxI32 n, physx::PxI32 offset = 0) const = 0;
    virtual ErrorType setParamI64Array(const Handle &handle, const physx::PxI64 *val, physx::PxI32 n, physx::PxI32 offset= 0) = 0;

    virtual ErrorType getParamU8(const Handle &handle, physx::PxU8 &val) const = 0;
    virtual ErrorType setParamU8(const Handle &handle, physx::PxU8 val) = 0;
    virtual ErrorType getParamU8Array(const Handle &handle, physx::PxU8 *array, physx::PxI32 n, physx::PxI32 offset = 0) const = 0;
    virtual ErrorType setParamU8Array(const Handle &handle, const physx::PxU8 *val, physx::PxI32 n, physx::PxI32 offset= 0) = 0;

    virtual ErrorType getParamU16(const Handle &handle, physx::PxU16 &val) const = 0;
    virtual ErrorType setParamU16(const Handle &handle, physx::PxU16 val) = 0;
    virtual ErrorType getParamU16Array(const Handle &handle, physx::PxU16 *array, physx::PxI32 n, physx::PxI32 offset = 0) const = 0;
    virtual ErrorType setParamU16Array(const Handle &handle, const physx::PxU16 *array, physx::PxI32 n, physx::PxI32 offset= 0) = 0;

    virtual ErrorType getParamU32(const Handle &handle, physx::PxU32 &val) const = 0;
    virtual ErrorType setParamU32(const Handle &handle, physx::PxU32 val) = 0;
    virtual ErrorType getParamU32Array(const Handle &handle, physx::PxU32 *array, physx::PxI32 n, physx::PxI32 offset = 0) const = 0;
    virtual ErrorType setParamU32Array(const Handle &handle, const physx::PxU32 *array, physx::PxI32 n, physx::PxI32 offset= 0) = 0;

    virtual ErrorType getParamU64(const Handle &handle, physx::PxU64 &val) const = 0;
    virtual ErrorType setParamU64(const Handle &handle, physx::PxU64 val) = 0;
    virtual ErrorType getParamU64Array(const Handle &handle, physx::PxU64 *array, physx::PxI32 n, physx::PxI32 offset = 0) const = 0;
    virtual ErrorType setParamU64Array(const Handle &handle, const physx::PxU64 *array, physx::PxI32 n, physx::PxI32 offset= 0) = 0;

    virtual ErrorType getParamF32(const Handle &handle, physx::PxF32 &val) const = 0;
    virtual ErrorType setParamF32(const Handle &handle, physx::PxF32 val) = 0;
    virtual ErrorType getParamF32Array(const Handle &handle, physx::PxF32 *array, physx::PxI32 n, physx::PxI32 offset = 0) const = 0;
    virtual ErrorType setParamF32Array(const Handle &handle, const physx::PxF32 *array, physx::PxI32 n, physx::PxI32 offset= 0) = 0;

    virtual ErrorType getParamF64(const Handle &handle, physx::PxF64 &val) const = 0;
    virtual ErrorType setParamF64(const Handle &handle, physx::PxF64 val) = 0;
    virtual ErrorType getParamF64Array(const Handle &handle, physx::PxF64 *array, physx::PxI32 n, physx::PxI32 offset = 0) const = 0;
    virtual ErrorType setParamF64Array(const Handle &handle, const physx::PxF64 *array, physx::PxI32 n, physx::PxI32 offset= 0) = 0;

    virtual ErrorType setParamVec2(const Handle &handle, const physx::PxVec2 &val) = 0;
    virtual ErrorType getParamVec2(const Handle &handle, physx::PxVec2 &val) const = 0;
    virtual ErrorType getParamVec2Array(const Handle &handle, physx::PxVec2 *array, physx::PxI32 n, physx::PxI32 offset = 0) const = 0;
    virtual ErrorType setParamVec2Array(const Handle &handle, const physx::PxVec2 *array, physx::PxI32 n, physx::PxI32 offset= 0) = 0;

	virtual ErrorType setParamVec3(const Handle &handle, const physx::PxVec3 &val) = 0;
    virtual ErrorType getParamVec3(const Handle &handle, physx::PxVec3 &val) const = 0;
    virtual ErrorType getParamVec3Array(const Handle &handle, physx::PxVec3 *array, physx::PxI32 n, physx::PxI32 offset = 0) const = 0;
    virtual ErrorType setParamVec3Array(const Handle &handle, const physx::PxVec3 *array, physx::PxI32 n, physx::PxI32 offset= 0) = 0;

    virtual ErrorType setParamVec4(const Handle &handle, const physx::PxVec4 &val) = 0;
    virtual ErrorType getParamVec4(const Handle &handle, physx::PxVec4 &val) const = 0;
    virtual ErrorType getParamVec4Array(const Handle &handle, physx::PxVec4 *array, physx::PxI32 n, physx::PxI32 offset = 0) const = 0;
    virtual ErrorType setParamVec4Array(const Handle &handle, const physx::PxVec4 *array, physx::PxI32 n, physx::PxI32 offset= 0) = 0;

	virtual ErrorType setParamQuat(const Handle &handle, const physx::PxQuat &val) = 0;
    virtual ErrorType getParamQuat(const Handle &handle, physx::PxQuat &val) const = 0;
    virtual ErrorType getParamQuatArray(const Handle &handle, physx::PxQuat *array, physx::PxI32 n, physx::PxI32 offset = 0) const = 0;
    virtual ErrorType setParamQuatArray(const Handle &handle, const physx::PxQuat *array, physx::PxI32 n, physx::PxI32 offset= 0) = 0;

	virtual ErrorType setParamMat33(const Handle &handle, const physx::PxMat33 &val) = 0;
	virtual ErrorType getParamMat33(const Handle &handle, physx::PxMat33 &val) const = 0;
	virtual ErrorType getParamMat33Array(const Handle &handle, physx::PxMat33 *array, physx::PxI32 n, physx::PxI32 offset = 0) const = 0;
	virtual ErrorType setParamMat33Array(const Handle &handle, const physx::PxMat33 *array, physx::PxI32 n, physx::PxI32 offset= 0) = 0;

	virtual ErrorType setParamMat44(const Handle &handle, const physx::PxMat44 &val) = 0;
    virtual ErrorType getParamMat44(const Handle &handle, physx::PxMat44 &val) const = 0;
    virtual ErrorType getParamMat44Array(const Handle &handle, physx::PxMat44 *array, physx::PxI32 n, physx::PxI32 offset = 0) const = 0;
    virtual ErrorType setParamMat44Array(const Handle &handle, const physx::PxMat44 *array, physx::PxI32 n, physx::PxI32 offset= 0) = 0;

    virtual ErrorType setParamBounds3(const Handle &handle, const physx::PxBounds3 &val) = 0;
    virtual ErrorType getParamBounds3(const Handle &handle, physx::PxBounds3 &val) const = 0;
    virtual ErrorType getParamBounds3Array(const Handle &handle, physx::PxBounds3 *array, physx::PxI32 n, physx::PxI32 offset = 0) const = 0;
    virtual ErrorType setParamBounds3Array(const Handle &handle, const physx::PxBounds3 *array, physx::PxI32 n, physx::PxI32 offset= 0) = 0;

	/**
	\brief Store value of parameter into a string
	\see Handle::valueToStr
	*/
    virtual ErrorType valueToStr(const Handle &handle, char *buf, physx::PxU32 bufSize, const char *&ret) = 0;

	/**
	\brief Read value of parameter from string
	\see Handle::strToValue
	*/
    virtual ErrorType strToValue(Handle &handle,const char *str, const char **endptr) = 0; // assigns this string to the value

    /**
    \brief Resize array parameter
	\see Handle::resizeArray
    */
    virtual ErrorType resizeArray(const Handle &array_handle, physx::PxI32 new_size) = 0;

    /**
    \brief Get size of array parameter
	\see Handle::getArraySize
    */
	virtual ErrorType getArraySize(const Handle &array_handle, physx::PxI32 &size, physx::PxI32 dimension = 0) const = 0;

    /**
    \brief Swap two elements of an array
	\see Handle::swapArrayElements
    */
    virtual ErrorType swapArrayElements(const Handle &array_handle, physx::PxU32 firstElement, physx::PxU32 secondElement) = 0;
};

#include "NxParameterized.inl" // inline the NxParamterHandle methods.

}; // end of namespace


PX_POP_PACK

#endif

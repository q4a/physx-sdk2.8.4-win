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

/*!
\file
\brief NxParameterized type X-macro template
\note See http://en.wikipedia.org/wiki/C_preprocessor#X-Macros for more details
*/


// NX_PARAMETERIZED_TYPE(type_name, enum_name, c_type)

//class Interface;

#pragma warning(push)
#pragma warning(disable:4555)
PX_PUSH_PACK_DEFAULT

#ifndef NX_PARAMETERIZED_TYPES_ONLY_SIMPLE_TYPES
NX_PARAMETERIZED_TYPE(Array, ARRAY, void *)
NX_PARAMETERIZED_TYPE(Struct, STRUCT, void *)
NX_PARAMETERIZED_TYPE(Ref, REF, NxParameterized::Interface *)
#endif

NX_PARAMETERIZED_TYPE(Bool, BOOL, bool)

#ifndef NX_PARAMETERIZED_TYPES_NO_STRING_TYPES
NX_PARAMETERIZED_TYPE(String, STRING, const char *)
NX_PARAMETERIZED_TYPE(Enum, ENUM, const char *)
#endif

NX_PARAMETERIZED_TYPE(I8, I8, physx::PxI8)
NX_PARAMETERIZED_TYPE(I16, I16, physx::PxI16)
NX_PARAMETERIZED_TYPE(I32, I32, physx::PxI32)
NX_PARAMETERIZED_TYPE(I64, I64, physx::PxI64)

NX_PARAMETERIZED_TYPE(U8, U8, physx::PxU8)
NX_PARAMETERIZED_TYPE(U16, U16, physx::PxU16)
NX_PARAMETERIZED_TYPE(U32, U32, physx::PxU32)
NX_PARAMETERIZED_TYPE(U64, U64, physx::PxU64)

NX_PARAMETERIZED_TYPE(F32, F32, physx::PxF32)
NX_PARAMETERIZED_TYPE(F64, F64, physx::PxF64)

NX_PARAMETERIZED_TYPE(Vec2, VEC2, physx::PxVec2)
NX_PARAMETERIZED_TYPE(Vec3, VEC3, physx::PxVec3)
NX_PARAMETERIZED_TYPE(Vec4, VEC4, physx::PxVec4)
NX_PARAMETERIZED_TYPE(Quat, QUAT, physx::PxQuat)
NX_PARAMETERIZED_TYPE(Bounds3,BOUNDS3,physx::PxBounds3)
NX_PARAMETERIZED_TYPE(Mat33, MAT33, physx::PxMat33)
NX_PARAMETERIZED_TYPE(Mat44, MAT44, physx::PxMat44)

#ifndef NX_PARAMETERIZED_TYPES_NO_LEGACY_TYPES
NX_PARAMETERIZED_TYPE(Mat34, MAT34, physx::PxMat44)
#endif

#ifdef NX_PARAMETERIZED_TYPES_ONLY_SIMPLE_TYPES
#   undef NX_PARAMETERIZED_TYPES_ONLY_SIMPLE_TYPES
#endif

#ifdef NX_PARAMETERIZED_TYPES_NO_STRING_TYPES
#   undef NX_PARAMETERIZED_TYPES_NO_STRING_TYPES
#endif

#ifdef NX_PARAMETERIZED_TYPES_NO_LEGACY_TYPES
#	undef NX_PARAMETERIZED_TYPES_NO_LEGACY_TYPES
#endif


#pragma warning(pop)
PX_POP_PACK

#undef NX_PARAMETERIZED_TYPE

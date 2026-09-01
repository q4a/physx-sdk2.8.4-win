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
\brief Serializer::SerializePlatform and Serializer::DeserializedData inline implementation
*/

//Check binary compatibility of compiler versions
PX_INLINE bool DoCompilerVersMatch(SerializePlatform::CompilerType t, physx::PxU32 v1, physx::PxU32 v2)
{
	PX_FORCE_PARAMETER_REFERENCE(t);
	if( SerializePlatform::ANY_VERSION == v1 || SerializePlatform::ANY_VERSION == v2 )
		return true;
	
	//In future we should distinguish compiler versions which have different ABI
	//but now we are optimistic

	return true;
}

//Check binary compatibility of OS versions
PX_INLINE bool DoOsVersMatch(SerializePlatform::OsType t, physx::PxU32 v1, physx::PxU32 v2)
{
	PX_FORCE_PARAMETER_REFERENCE(t);
	if( SerializePlatform::ANY_VERSION == v1 || SerializePlatform::ANY_VERSION == v2 )
		return true;
	
	return true; //See comment for doCompilerVersMatch
}

PX_INLINE SerializePlatform::SerializePlatform()
	: archType(ARCH_LAST),
	compilerType(COMP_LAST),
	compilerVer(ANY_VERSION),
	osType(OS_LAST),
	osVer(ANY_VERSION)
{}

PX_INLINE SerializePlatform::SerializePlatform(ArchType archType_, CompilerType compType_, physx::PxU32 compVer_, OsType osType_, physx::PxU32 osVer_)
	: archType(archType_),
	compilerType(compType_),
	compilerVer(compVer_),
	osType(osType_),
	osVer(osVer_)
{}

PX_INLINE bool SerializePlatform::operator ==(const SerializePlatform &p) const
{
	return archType == p.archType
		&& compilerType == p.compilerType
		&& osType == p.osType
		&& DoCompilerVersMatch(compilerType, compilerVer, p.compilerVer)
		&& DoOsVersMatch(osType, osVer, p.osVer);
}

PX_INLINE bool SerializePlatform::operator !=(const SerializePlatform &p) const
{
	return !(*this == p);
}

PX_INLINE Serializer::DeserializedData::DeserializedData()
	: objs(0), nobjs(0), traits(0) {}

PX_INLINE Serializer::DeserializedData::DeserializedData(const Serializer::DeserializedData &data)
{
	*this = data;
}

PX_INLINE Serializer::DeserializedData &Serializer::DeserializedData::operator =(const Serializer::DeserializedData &rhs)
{
	init(rhs.traits, rhs.objs, rhs.nobjs);
	return *this;
}

PX_INLINE void Serializer::DeserializedData::clear()
{
	if ( objs && objs != buf ) //Memory was allocated?
	{
		PX_ASSERT(traits);
		traits->free(objs);
	}
}

PX_INLINE Serializer::DeserializedData::~DeserializedData()
{
	clear();
}

PX_INLINE void Serializer::DeserializedData::init(Traits *traits_, ::NxParameterized::Interface **objs_, physx::PxU32 nobjs_)
{
	init(traits_, nobjs_);
	::memcpy(objs, objs_, nobjs * sizeof(::NxParameterized::Interface *));
}

PX_INLINE void Serializer::DeserializedData::init(Traits *traits_, physx::PxU32 nobjs_)
{
	clear();

	traits = traits_;
	nobjs = nobjs_;

	//Allocate memory if buf is too small
	objs = nobjs <= bufSize
		? buf
		: (::NxParameterized::Interface **)traits->alloc(nobjs * sizeof(::NxParameterized::Interface *));
}

PX_INLINE physx::PxU32 Serializer::DeserializedData::size() const
{
	return nobjs;
}

PX_INLINE ::NxParameterized::Interface *&Serializer::DeserializedData::operator[](physx::PxU32 i)
{
	PX_ASSERT( i < nobjs );
	return objs[i];
}

PX_INLINE ::NxParameterized::Interface *Serializer::DeserializedData::operator[](physx::PxU32 i) const
{
	PX_ASSERT( i < nobjs );
	return objs[i];
}

PX_INLINE void Serializer::DeserializedData::getObjects(::NxParameterized::Interface **outObjs)
{
	::memcpy(outObjs, objs, nobjs * sizeof(::NxParameterized::Interface *));
}

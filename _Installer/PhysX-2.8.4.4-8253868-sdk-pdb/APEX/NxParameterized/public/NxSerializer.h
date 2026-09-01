#ifndef NX_SERIALIZER_H
#define NX_SERIALIZER_H

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
\brief NxParameterized serializer class
*/


#include "PsShare.h"
#include "PxAssert.h"
#include "NxParameterizedTraits.h"

namespace NxParameterized
{

PX_PUSH_PACK_DEFAULT

/**
\brief Platform descriptor

This class describes target serialization platform which consists of processor architecture, compiler and OS.
*/
struct SerializePlatform
{
	/** 
	\brief Processor architectures enumeration
	*/
	typedef enum
	{
		ARCH_GEN = 0,
		ARCH_X86,
		ARCH_X86_64,
		ARCH_PPC,
		ARCH_CELL,
		ARCH_LAST
	} ArchType;

	/**
	\brief Platform processor architecture
	*/
	ArchType archType;

	/** 
	\brief Compilers enumeration
	*/
	typedef enum
	{
		COMP_GEN = 0,
		COMP_GCC,
		COMP_VC,
		COMP_MW,
		COMP_LAST
	} CompilerType;

	/**
	\brief Platform compiler
	*/
	CompilerType compilerType;

	/**
	\brief Platform compiler version
	*/
	physx::PxU32 compilerVer;

	/** 
	\brief OSes enumeration
	*/
	typedef enum
	{
		OS_WINDOWS = 0,
		OS_LINUX,
		OS_PS3,
		OS_MACOSX,
		OS_XBOX,
		OS_GEN,
		OS_LAST
	} OsType;

	/**
	\brief Platform OS
	*/
	OsType osType;

	/**
	\brief Platform OS version
	*/
	physx::PxU32 osVer;

	/**
	\brief This value identfies that version is unknown
	*/
	static const physx::PxU32 ANY_VERSION = (physx::PxU32)-1;

	/**
	\brief Get current (native) platform
	*/
	static const SerializePlatform &GetCurrentPlatform();

	/**
	\brief Get platform by human-readable name
	\param [in] name platform name
	\param [out] platform Platform corresponding to name

	Name format: compiler + compiler version (if needed) + architecture.
	Supported names: VcWin32, VcWin64, VcXbox, GccPs3, Pib.
	*/
	static bool GetPlatform(const char *name, SerializePlatform &platform);

	PX_INLINE SerializePlatform();

	PX_INLINE SerializePlatform(ArchType archType, CompilerType compType, physx::PxU32 compVer, OsType osType, physx::PxU32 osVer);

	/**
	\brief Checks if platforms are binary-compatible
	*/
	PX_INLINE bool operator ==(const SerializePlatform &p) const;

	/**
	\brief Checks if platforms are binary-incompatible
	*/
	PX_INLINE bool operator !=(const SerializePlatform &p) const;
};

class Interface;
class Traits;
struct SerializePlatform;

/**
\brief Interface to use when compressing or decompressing parameterized data streams.
*/
class SerializerCompression
{
public:
	/**
	\brief Returns true if this memory represents compressed data
	*/
	virtual bool isCompressedData(const void *mem, physx::PxU32 dlen) = 0;

	/**
	\brief Compress data
	*/
	virtual void *compressData(const void *mem, physx::PxU32 inLen, physx::PxU32 &outLen, Traits *traits) = 0;

	/**
	\brief Decompress data
	*/
	virtual void *decompressData(const void *mem, physx::PxU32 inLen, physx::PxU32 &outLen, Traits *traits) = 0;

	/**
	\brief Release memory returned by compressData or decompressData
	*/
	virtual void releaseCompressedData(void *mem, Traits *traits) = 0;
};

/**
\brief Interface class for serializer-deserializer of NxParameterized objects

Serializer serializes and deserializes one or more NxParameterized objects to file using various output formats
(see SerializeType).
*/
class Serializer
{
public:

	/**
	\brief Status enums that the Serializer methods may return
	*/
	enum ErrorType
	{
		ERROR_NONE = 0,
		ERROR_UNKNOWN,
		ERROR_STREAM_ERROR,
		ERROR_OBJECT_HAS_NO_ROOT_PARAMETER_DEFINITION,
		ERROR_INVALID_ARRAY,
		ERROR_VAL2STRING_FAILED,
		ERROR_STRING2VAL_FAILED,
		ERROR_INVALID_FILE_FORMAT,
		ERROR_INVALID_PARAMETERIZED_OBJECT,
		ERROR_ARRAY_INDEX_OUT_OF_RANGE,
		ERROR_UNKNOWN_STRUCT_MEMBER,
		ERROR_UNKNOWN_DATA_TYPE,
		ERROR_UNKNOWN_XML_TAG,
		ERROR_TYPE_MISMATCH,
		ERROR_NOT_IMPLEMENTED,
		ERROR_INVALID_PLATFORM,
		ERROR_INVALID_INTERNAL_PTR,
		ERROR_INVALID_VERSION,
		ERROR_INVALID_CHECKSUM,
		ERROR_MEMORY_ALLOCATION_FAILURE,
		ERROR_INVALID_ENUM,
		ERROR_INVALID_VALUE,
		ERROR_OBJECT_CREATION_FAILED,
		ERROR_INVALID_PARAM_HANDLE,
		ERROR_CONVERSION_FAILED,
		ERROR_UNALIGNED_MEMORY,
		ERROR_NULL_OBJECT,
		ERROR_INVALID_RELOC
	};

	/**
	\brief The supported serialization formats
	*/
	enum SerializeType
	{
		/// serialize in XML format.
		NST_XML = 0,

		/// serialize in a binary format
		NST_BINARY,

		NST_LAST
	};

	/**
	\brief Get type of stream (binary or xml)
	\param [in] stream stream to be analyzed
	*/
	static SerializeType peekSerializeType(physx::PxFileBuf &stream);

	/**
	\brief Get stream native platform
	\param [in] stream stream to be analyzed
	\param [out] platform stream native platform
	*/
	static ErrorType peekPlatform(physx::PxFileBuf &stream, SerializePlatform &platform);

	virtual ~Serializer() {}

	/**
	\brief Set platform to use in platform-dependent serialization
	\param [in] platform target platform

	\warning Currently this is used only in binary serializer

	Application running on target platforms may potentially make use of extremely fast
	inplace deserialization (using method deserializeInplace) on files which were serialized
	for this platform.
	*/
	virtual ErrorType setTargetPlatform(const SerializePlatform &platform) = 0;

	/**
	\brief Serialize array of NxParameterized-objects to a stream
	\param [in] stream the stream to which the object will be serialized
	\param [in] objs NxParameterized-objects which will be serialized
	\param [in] nobjs number of objects
	\param [in] doSerializeMetadata set this to store object metadata in file
	\param [in] doCompression set this to compress data

	\warning doCompression and doSerializeMetadata are not implemented
	\warning Serialized file may depend on selected target platform
	*/
	virtual ErrorType serialize(
		physx::PxFileBuf &stream,
		const ::NxParameterized::Interface **objs,
		physx::PxU32 nobjs,
		bool doSerializeMetadata = false,
		bool doCompression = false) = 0;

	/**
	\brief Peek number of NxParameterized-objects in stream with serialized data
	\param [in] stream the stream from which the object will be deserialized
	\param [out] numObjects number of objects

	\warning Not all streams support peeking
	*/
	virtual ErrorType peekNumObjects(physx::PxFileBuf &stream, physx::PxU32 &numObjects) = 0;

	/**
	\brief Peek number of NxParameterized-objects in stream with serialized data
	\param [in] stream the stream from which objects will be deserialized
	\param [in] classNames pointer to buffer for resulting names
	\param [in,out] numClassNames limit on number of returned classNames; number of returned names

	\warning User is responsible for releasing every element of classNames via Traits::strfree()
	*/
	virtual ErrorType peekClassNames(physx::PxFileBuf &stream, char **classNames, physx::PxU32 &numClassNames) = 0;

	/**
	\brief Peek number of NxParameterized-objects in memory buffer with serialized data
	\param [in] data pointer to memory buffer
	\param [in] dataLen length of memory buffer
	\param [out] numObjects number of objects
	*/
	virtual ErrorType peekNumObjectsInplace(const void *data, physx::PxU32 dataLen, physx::PxU32 &numObjects) = 0;

	/**
	\brief Container for results of deserialization

	DeserializedData holds array of NxParameterized objects obtained during deserialization.
	*/
	class DeserializedData
	{
	public:

		PX_INLINE DeserializedData();

		PX_INLINE ~DeserializedData();

		/**
		\brief Copy constructor
		*/
		PX_INLINE DeserializedData(const DeserializedData &data);

		/**
		\brief Assignment operator
		*/
		PX_INLINE DeserializedData &operator =(const DeserializedData &rhs);

		/**
		\brief Allocate memory for values
		*/
		PX_INLINE void init(Traits *traits_, physx::PxU32 nobjs_);

		/**
		\brief Allocate memory and set values
		*/
		PX_INLINE void init(Traits *traits_, ::NxParameterized::Interface **objs_, physx::PxU32 nobjs_);

		/**
		\brief Number of objects in a container
		*/
		PX_INLINE physx::PxU32 size() const;

		/**
		\brief Access individual object in container
		*/
		PX_INLINE ::NxParameterized::Interface *&operator[](physx::PxU32 i);

		/**
		\brief Const-access individual object in container
		*/
		PX_INLINE ::NxParameterized::Interface *operator[](physx::PxU32 i) const;

		/**
		\brief Read all NxParameterized objects in container to buffer outObjs
		\warning outObjs must be large enough to hold all contained objects
		*/
		PX_INLINE void getObjects(::NxParameterized::Interface **outObjs);

	private:
		static const physx::PxU32 bufSize = 8;
		::NxParameterized::Interface *buf[bufSize]; //For small number of objects

		::NxParameterized::Interface **objs;

		physx::PxU32 nobjs;

		Traits *traits;

		void clear();
	};

	/**
	\brief Deserialize a stream into one or more NxParameterized objects
	\param [in] stream the stream from which objects will be deserialized
	\param [out] desData storage for deserialized data
	*/
	virtual ErrorType deserialize(physx::PxFileBuf &stream, DeserializedData &desData) = 0;

	/**
	\brief Deserialize a stream into one or more NxParameterized objects
	\param [in] stream the stream from which objects will be deserialized
	\param [out] desData storage for deserialized data
	\param [out] isUpdated true if any legacy object was updated, false otherwise
	*/
	virtual ErrorType deserialize(physx::PxFileBuf &stream, DeserializedData &desData, bool &isUpdated) = 0;

	/**
	\brief Deserialize memory buffer into one or more NxParameterized objects
	\param [in] data pointer to serialized data
	\param [in] dataLen length of serialized data
	\param [out] desData storage for deserialized data

	\warning Currently only binary serializer supports inplace deserialization
	\warning Memory must be aligned to 8 byte boundary
	*/
	virtual ErrorType deserializeInplace(void *data, physx::PxU32 dataLen, DeserializedData &desData) = 0;

	/**
	\brief Deserialize memory buffer into one or more NxParameterized objects
	\param [in] data pointer to serialized data
	\param [in] dataLen length of serialized data
	\param [out] desData storage for deserialized data
	\param [out] isUpdated true if any legacy object was updated, false otherwise

	\warning Currently only binary serializer supports inplace deserialization
	*/
	virtual ErrorType deserializeInplace(void *data, physx::PxU32 dataLen, DeserializedData &desData, bool &isUpdated) = 0;

	/**
	\brief Release deserializer and any memory allocations associated with it
	*/
	virtual void release() = 0;

	/**
	\brief setSerializerCompression sets the interface to use when compressing or decompressing parameterized data streams.
	\param [serializerCompression] A pointer to a serializer compression interface.

	\warning Currently this does not work
	*/
	virtual void setSerializerCompression(SerializerCompression *serializerCompression) = 0;
};

#include "NxSerializer.inl"

PX_POP_PACK

}; // end of namespace

#endif

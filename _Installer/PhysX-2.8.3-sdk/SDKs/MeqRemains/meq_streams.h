/*************************************************************************
*
* Copyright 2010 NVIDIA Corporation
*
*			www.nvidia.com
*
************************************************************************/

#ifndef MEQ_STREAMS_H
#define MEQ_STREAMS_H

#include "FoundationSDK.h"
#include "NxFoundationSDK.h"
#include "NxSimpleTypes.h"
#include "Nxf.h"
#include "NxVec3.h"
#include "NxPlane.h"

#include "meq_string.h"

#include <stdio.h>

#ifdef __PPCGEKKO__
#define MEQ_INPUT_BUFFER_GRANULARITY 4064
#define MEQ_OUTPUT_BUFFER_SIZE 4064
#else
#define MEQ_INPUT_BUFFER_GRANULARITY 32768
#define MEQ_OUTPUT_BUFFER_SIZE 32768
#endif

namespace meq {

	/*
	Stream replacement has several objectives:
	- Centralize endianess conversion
	- Replace serialization in conversion to binary data
	- Centralize buffering, which today is handled in stream implementation *huh*
	- Make common base for file stream and socket, today they are separate for some reason
	- Unify agent/debugger network communication
	- Make it easier to implement custom streams

	- Input/output buffer size 1024 as stream member, not allocated.
	*/
	class InputStream{
	public:
		InputStream () : bufferPos(0), validDataLength(0), blocking(true),
			currentStringLength(-1), bufferSize(0), bufferData(NULL), managedBuffer(false), bigEndianInput(false){
		}

		virtual ~InputStream(){
#ifdef __PPCGEKKO__
			if (bufferData != NULL && !managedBuffer)
				NxAlignedFree(bufferData);
#else		
			if (bufferData != NULL && !managedBuffer)
				NxFoundation::FoundationSDK::getInstance().getAllocator().free(bufferData);
#endif
		}
		
		//////////////////////////////////////////////////////////////////////////
		/*!
		Sets whether the stream should be blocking or not. A blocking stream will
		always wait until enough data is available to read a requested value. A
		non-blocking stream may return false on read methods, as an indication
		that not enough data was present to immediately read the value.

		\param blocking
		Set this to true if the input stream is to be blocking, and false otherwise.

		*/
		//////////////////////////////////////////////////////////////////////////
		void setBlocking(bool blocking){
			this->blocking = blocking;
		}

		//////////////////////////////////////////////////////////////////////////
		/*!
		Returns whether the stream is blocking or not.

		\return
		True if the stream is blocking, false otherwise.

		*/
		//////////////////////////////////////////////////////////////////////////
		bool isBlocking(){
			return blocking;
		}

   		//////////////////////////////////////////////////////////////////////////
   		/*!
 		Sets what endianness to expect from the input stream.
 
 		\param bigEndian
 		Set this to true if the input stream is big endian, and false otherwise.
 
 		*/
 		//////////////////////////////////////////////////////////////////////////
 		/*inline void setBigEndianInput(bool bigEndian) {
 			bigEndianInput = bigEndian;
 		}*/
 
 		//////////////////////////////////////////////////////////////////////////
 		/*!
 		Returns what endianness the stream is currently treated as
 
 		\return
 		True if input stream is treated as big endian, false otherwise
 
 		*/
 		//////////////////////////////////////////////////////////////////////////
 		/*inline bool hasBigEndianInput() {
 			return bigEndianInput;
 		}*/
 
		//////////////////////////////////////////////////////////////////////////
		/*!
		Returns whether there potentially is more data in the stream to read
		If this method returns true, it is still not guaranteed that a read
		call will be successful. hasMoreData will return true even if the input
		buffer is empty, if there is reason to expect more data to arrive (e.g.
		if a socket is still connected)

		\return
		True if there may be more data, false otherwise

		*/
		//////////////////////////////////////////////////////////////////////////
		virtual bool hasMoreData() = 0;

		//////////////////////////////////////////////////////////////////////////
		/*!
		Reads a boolean (1 byte) value from the stream
		The call will fail if there is currently no data to read and the stream is
		in non-blocking mode.

		\param result
		The read value will be put in this variable

		\return
		True if the value was read properly, false otherwise

		*/
		//////////////////////////////////////////////////////////////////////////
		inline bool readBool(bool& result) {
			if (!buffer(1)) return false;

			unsigned char charBool;
			if (!readBuffered(&charBool, 1))
				return false;
			result = (charBool != 0);
			return true;
		}

		//////////////////////////////////////////////////////////////////////////
		/*!
		Reads an 8-bit integer (1 byte) value from the stream
		The call will fail if there is currently no data to read and the stream is
		in non-blocking mode.

		\param result
		The read value will be put in this variable

		\return
		True if the value was read properly, false otherwise

		*/
		//////////////////////////////////////////////////////////////////////////
		inline bool readInt8(NxI8& result) {
			if (!buffer(1)) return false;

			return readBuffered((unsigned char*)&result, 1);
		}

		//////////////////////////////////////////////////////////////////////////
		/*!
		Reads an unsigned 8-bit integer value from the stream
		The call will fail if there is currently no data to read and the stream is
		in non-blocking mode.

		\param result
		The read value will be put in this variable

		\return
		True if the value was read properly, false otherwise

		*/
		//////////////////////////////////////////////////////////////////////////
		inline bool readUInt8(NxU8& result) {
			if (!buffer(1)) return false;

			return readBuffered((unsigned char*)&result, 1);
		}

		//////////////////////////////////////////////////////////////////////////
		/*!
		Reads a 16-bit integer value from the stream
		The call will fail if there is currently no data to read and the stream is
		in non-blocking mode.

		\param result
		The read value will be put in this variable

		\return
		True if the value was read properly, false otherwise

		*/
		//////////////////////////////////////////////////////////////////////////
		inline bool readInt16(NxI16& result) {
			if (!buffer(2)) return false;

			if (!readBuffered((unsigned char*)&result, 2))
				return false;
			/*if (bigEndianInput) result = fromBigEndian(result);
			else result = fromLittleEndian(result);*/
			return true;
		}

		//////////////////////////////////////////////////////////////////////////
		/*!
		Reads an unsigned 16-bit integer value from the stream
		The call will fail if there is currently no data to read and the stream is
		in non-blocking mode.

		\param result
		The read value will be put in this variable

		\return
		True if the value was read properly, false otherwise

		*/
		//////////////////////////////////////////////////////////////////////////
		inline bool readUInt16(NxU16& result) {
			if (!buffer(2)) return false;

			if (!readBuffered((unsigned char*)&result, 2))
				return false;
			/*if (bigEndianInput) result = fromBigEndian(result);
			else result = fromLittleEndian(result);*/
			return true;
		}

		//////////////////////////////////////////////////////////////////////////
		/*!
		Reads a 32-bit integer value from the stream
		The call will fail if there is currently no data to read and the stream is
		in non-blocking mode.

		\param result
		The read value will be put in this variable

		\return
		True if the value was read properly, false otherwise

		*/
		//////////////////////////////////////////////////////////////////////////
		inline bool readInt32(NxI32& result) {
			if (!buffer(4)) return false;

			if (!readBuffered((unsigned char*)&result, 4))
				return false;
			/*if (bigEndianInput) result = fromBigEndian(result);
			else result = fromLittleEndian(result);*/
			return true;
		}

		//////////////////////////////////////////////////////////////////////////
		/*!
		Reads an unsigned 32-bit integer value from the stream
		The call will fail if there is currently no data to read and the stream is
		in non-blocking mode.

		\param result
		The read value will be put in this variable

		\return
		True if the value was read properly, false otherwise

		*/
		//////////////////////////////////////////////////////////////////////////
		inline bool readUInt32(NxU32& result) {
			if (!buffer(4)) return false;

			if (!readBuffered((unsigned char*)&result, 4))
				return false;
			/*if (bigEndianInput) result = fromBigEndian(result);
			else result = fromLittleEndian(result);*/
			return true;
		}

		//////////////////////////////////////////////////////////////////////////
		/*!
		Reads a real value from the stream
		The call will fail if there is currently no data to read and the stream is
		in non-blocking mode.

		\param result
		The read value will be put in this variable

		\return
		True if the value was read properly, false otherwise

		*/
		//////////////////////////////////////////////////////////////////////////
		inline bool readReal(NxReal& result) {
			if (!buffer(4)) return false;

			NxU32 tempResult;
			if (!readBuffered((unsigned char*)&tempResult, 4))
				return false;

			/*if (bigEndianInput) tempResult = fromBigEndian(tempResult);
			else tempResult = fromLittleEndian(tempResult);*/
			result = *(NxReal*)&tempResult;
			return true;
		}

		//////////////////////////////////////////////////////////////////////////
		/*!
		Reads a vector from the stream
		The call will fail if there is currently no data to read and the stream is
		in non-blocking mode.

		\param result
		The read vector will be put in this variable

		\return
		True if the vector was read properly, false otherwise

		*/
		//////////////////////////////////////////////////////////////////////////
		inline bool readVector(NxVec3& result) {
			if (!buffer(12)) return false;

			if (!readReal(result.x)) return false;
			if (!readReal(result.y)) return false;
			if (!readReal(result.z)) return false;
			return true;
		}

		//////////////////////////////////////////////////////////////////////////
		/*!
		Reads a direction from the stream
		The call will fail if there is currently no data to read and the stream is
		in non-blocking mode.

		\param result
		The read direction will be put in this variable

		\return
		True if the direction was read properly, false otherwise

		*/
		//////////////////////////////////////////////////////////////////////////
		/*inline bool readDirection(Direction& result) {
			if (!buffer(12)) return false;

			Vector vector;
			if (!readReal(vector.x)) return false;
			if (!readReal(vector.y)) return false;
			if (!readReal(vector.z)) return false;
			result.set(vector, true);
			return true;
		}*/

		//////////////////////////////////////////////////////////////////////////
		/*!
		Reads a point from the stream
		The call will fail if there is currently no data to read and the stream is
		in non-blocking mode.

		\param result
		The read point will be put in this variable

		\return
		True if the point was read properly, false otherwise

		*/
		//////////////////////////////////////////////////////////////////////////
		/*inline bool readPoint(Point& result) {
			if (!buffer(12)) return false;

			if (!readReal(result.x)) return false;
			if (!readReal(result.y)) return false;
			if (!readReal(result.z)) return false;
			return true;
		}*/

		//////////////////////////////////////////////////////////////////////////
		/*!
		Reads a matrix from the stream
		The call will fail if there is currently no data to read and the stream is
		in non-blocking mode.

		\param result
		The read matrix will be put in this variable

		\return
		True if the matrix was read properly, false otherwise

		*/
		//////////////////////////////////////////////////////////////////////////
		inline bool readMatrix(NxMat33& result) {
			if (!buffer(36)) return false;

			if (!readReal(result(0, 0))) return false;
			if (!readReal(result(0, 1))) return false;
			if (!readReal(result(0, 2))) return false;
			if (!readReal(result(1, 0))) return false;
			if (!readReal(result(1, 1))) return false;
			if (!readReal(result(1, 2))) return false;
			if (!readReal(result(2, 0))) return false;
			if (!readReal(result(2, 1))) return false;
			if (!readReal(result(2, 2))) return false;
			return true;
		}

		//////////////////////////////////////////////////////////////////////////
		/*!
		Reads a frame from the stream
		The call will fail if there is currently no data to read and the stream is
		in non-blocking mode.

		\param result
		The read frame will be put in this variable

		\return
		True if the frame was read properly, false otherwise

		*/
		//////////////////////////////////////////////////////////////////////////
		/*inline bool readFrame(Frame& result) {
			if (!buffer(48)) return false;

			if (!readMatrix(result.base)) return false;
			if (!readPoint(result.origin)) return false;
			return true;
		}*/

		//////////////////////////////////////////////////////////////////////////
		/*!
		The call will fail if there is currently no data to read and the stream is
		in non-blocking mode.

		\param result
		The read plane will be put in this variable

		\return
		True if the plane was read properly, false otherwise

		*/
		//////////////////////////////////////////////////////////////////////////
		inline bool readPlane(NxPlane& result) {
			if (!buffer(16)) return false;

			if (!readReal(result.normal.x)) return false;
			if (!readReal(result.normal.y)) return false;
			if (!readReal(result.normal.z)) return false;
			if (!readReal(result.d)) return false;
			return true;
		}

		//////////////////////////////////////////////////////////////////////////
		/*!
		Reads an AABB value from the stream
		The call will fail if there is currently no data to read and the stream is
		in non-blocking mode.

		\param result
		The read AABB will be put in this variable

		\return
		True if the AABB was read properly, false otherwise

		*/
		//////////////////////////////////////////////////////////////////////////
		/*inline bool readAABB(AABB& result) {
			if (!buffer(24)) return false;

			Point origin;
			Vector extent;
			if (!readPoint(origin)) return false;
			if (!readVector(extent)) return false;
			result.setOrigin(origin);
			result.setExtent(extent);
			return true;
		}*/

		//////////////////////////////////////////////////////////////////////////
		/*!
		Reads a OBB from the stream
		The call will fail if there is currently no data to read and the stream is
		in non-blocking mode.

		\param result
		The read OBB will be put in this variable

		\return
		True if the OBB was read properly, false otherwise

		*/
		//////////////////////////////////////////////////////////////////////////
		/*inline bool readOBB(OBB& result) {
			if (!buffer(60)) return false;

			Matrix base;
			Point origin;
			Vector extent;
			if (!readMatrix(base)) return false;
			if (!readPoint(origin)) return false;
			if (!readVector(extent)) return false;
			result.setBase(base);
			result.setOrigin(origin);
			result.setExtent(extent);	   
			return true;
		}*/

		//////////////////////////////////////////////////////////////////////////
		/*!
		Reads a string from the stream
		The call will fail if there is currently no data to read and the stream is
		in non-blocking mode.

		\param result
		The read string will be put in this variable

		\return
		True if the string was read properly, false otherwise

		*/
		//////////////////////////////////////////////////////////////////////////
		inline bool readString(String& result) {
			if (currentStringLength == -1)
			{
				if (!buffer(4)) return false;

				if (!readInt32(currentStringLength)) return false;
			}

			if (!buffer(currentStringLength)) return false;

			// @@@ Should not read the bytes one by one
			result = "";
			for (NxI32 i=0; i<currentStringLength; i++){
				NxU8 inputChar;
				if (!readUInt8(inputChar)) return false;
				result += inputChar;
			}

			currentStringLength = -1;

			return true;
		}

		//////////////////////////////////////////////////////////////////////////
		/*!
		Reads a data block from the stream
		The call will fail if there is currently no data to read and the stream is
		in non-blocking mode.

		\param data
		A pointer to a block of memory where the read data is to be put. This 
		must point to an allocated block at least as big as length.

		\param length
		The number of bytes of data to read

		\return
		True if the data was read properly, false otherwise

		*/
		//////////////////////////////////////////////////////////////////////////
		inline bool readData(unsigned char* data, unsigned int length) {
			if (!buffer(length)) return false;
			return readBuffered(data, length);
		}

	protected:
		void growBuffer (NxU32 newSize){
			if ((NxI32)newSize<bufferSize || managedBuffer)
				return;

			newSize = ((newSize/MEQ_INPUT_BUFFER_GRANULARITY)+1)*MEQ_INPUT_BUFFER_GRANULARITY;

			bufferSize = newSize;
#ifdef __PPCGEKKO__
			if (bufferData == NULL)
				bufferData = (NxU8*)NxAlignedMalloc(bufferSize, 32);
			else
				bufferData = (NxU8*)NxAlignedReAlloc(bufferData, bufferSize, 32);
#else
			if (bufferData == NULL)
				bufferData = (NxU8*)NxFoundation::FoundationSDK::getInstance().getAllocator().malloc(bufferSize, NX_MEMORY_VRDData);
			else
				bufferData = (NxU8*)NxFoundation::FoundationSDK::getInstance().getAllocator().realloc(bufferData, bufferSize);
#endif
		}

		inline bool buffer(unsigned int length)
		{
			if ((NxI32)length > bufferSize-bufferPos && !managedBuffer)
			{
				memmove(bufferData, bufferData+bufferPos, validDataLength-bufferPos);
				validDataLength -= bufferPos;
				bufferPos = 0;
				if ((NxI32)length > bufferSize-bufferPos)
					growBuffer(length);
			}
			if ((NxI32)length > bufferSize-bufferPos)
				return false;
			if (blocking)
			{
				// @@@ Yield?
				while((validDataLength < bufferPos + (NxI32)length) && hasMoreData())
					validDataLength += read(bufferData+validDataLength, bufferSize-validDataLength);
			} else {
				validDataLength += read(bufferData+validDataLength, bufferSize-validDataLength);
			}
			return bufferPos+(NxI32)length <= validDataLength;
		}

		inline bool readBuffered(unsigned char* data, unsigned int length) {
			if (length == 0)
				return true;

			if ((NxI32)length > validDataLength-bufferPos || bufferData == NULL)
				return false;

			memcpy(data, bufferData+bufferPos, length);
			bufferPos += length;
			return true;
		}

		//////////////////////////////////////////////////////////////////////////
		/*!
		Reads data from the input stream. This method is to be overloaded by
		subclasses to perform the actual data read.

		\param data
		Pointer to a destination block of memory to put the read data in

		\param length
		Amount of data to read, in bytes

		\return
		Number of bytes actually read. This can be lower than length if the
		end of the stream was encountered, or not enough data exists in the input
		stream.
		*/
		//////////////////////////////////////////////////////////////////////////
		virtual int read(unsigned char* data, unsigned int length) = 0;

		//////////////////////////////////////////////////////////////////////////
		/*!
		Specifies if the InputStream baseclass should keep a read buffer of its
		own, or if this is managed by the subclass. If managedBuffer is true, the
		InputStream baseclass assumes that the bufferSize, bufferPos and 
		bufferData members are managed by the subclass. This is useful in cases
		where the input itself actually already is some kind of memory buffer,
		and no extra buffering is needed. See MemoryInputBuffer for implementation
		examples.
		*/
		//////////////////////////////////////////////////////////////////////////
		bool managedBuffer;

		//////////////////////////////////////////////////////////////////////////
		/*!
		The size of the read buffer. If the buffer is managed, this should be
		set to the size of the input buffer. If the buffer is not managed by the
		subclass, this should not be touched.
		*/
		//////////////////////////////////////////////////////////////////////////
		NxI32 bufferSize;

		//////////////////////////////////////////////////////////////////////////
		/*!
		The current position in the read buffer. Incremented automatically by
		the InputStream baseclass, but should be reset explicity by the subclass
		when needed if the buffer is managed.
		*/
		//////////////////////////////////////////////////////////////////////////
		NxI32 bufferPos;

		//////////////////////////////////////////////////////////////////////////
		/*!
		Pointer to the input buffer. If the buffer is not managed, this will be
		automatically allocated. Otherwise it needs to be explicitly set by the
		subclass.
		*/
		//////////////////////////////////////////////////////////////////////////
		NxU8 *bufferData;

		
		NxI32 validDataLength;
		bool blocking;
		NxI32 currentStringLength;
		bool bigEndianInput; //keep this so that the size of this class is the same as for the VRD app
	};


	class OutputStream{
	public:
		OutputStream () : bigEndianOutput(false) {
		}

		virtual ~OutputStream() {};

		//////////////////////////////////////////////////////////////////////////
		/*!
		Sets what endianness to write to the output stream.

		\param bigEndian
		Set this to true if the output stream is to be big endian, and false 
		otherwise.

		*/
		//////////////////////////////////////////////////////////////////////////
		/*void setBigEndianOutput(bool bigEndian){
			bigEndianOutput = bigEndian;
		}*/

		//////////////////////////////////////////////////////////////////////////
		/*!
		Returns what endianness the stream is written as

		\return
		True if output stream is written as big endian, false otherwise

		*/
		//////////////////////////////////////////////////////////////////////////
		/*bool hasBigEndianOutput(){
			return bigEndianOutput;
		}*/

		//////////////////////////////////////////////////////////////////////////
		/*!
		Returns whether it is possible to write more data to the stream. This may
		return false in cases like when the end of a memory buffer has reached,
		or a socket has been closed.

		\return
		True if there may be more data, false otherwise

		*/
		//////////////////////////////////////////////////////////////////////////
		virtual bool canWriteMoreData() = 0;

		//////////////////////////////////////////////////////////////////////////
		/*!
		Writes a boolean (1-byte) value to the stream

		\param value
		The value to write

		\return
		True if the value was written properly, false otherwise

		*/
		//////////////////////////////////////////////////////////////////////////
		inline bool writeBool(bool value){
			return writeUInt8(value?1:0);
		}

		//////////////////////////////////////////////////////////////////////////
		/*!
		Writes an 8-bit integer value to the stream

		\param value
		The value to write

		\return
		True if the value was written properly, false otherwise

		*/
		//////////////////////////////////////////////////////////////////////////
		inline bool writeInt8(NxI8 value){
			/*if (bigEndianOutput) value = toBigEndian(value);
			else value = toLittleEndian(value);*/
			return writeBuffered((unsigned char*)&value, 1) == 1;
		}

		//////////////////////////////////////////////////////////////////////////
		/*!
		Writes an unsigned 8-bit integer value to the stream

		\param value
		The value to write

		\return
		True if the value was written properly, false otherwise

		*/
		//////////////////////////////////////////////////////////////////////////
		inline bool writeUInt8(NxU8 value){
			/*if (bigEndianOutput) value = toBigEndian(value);
			else value = toLittleEndian(value);*/
			return writeBuffered((unsigned char*)&value, 1) == 1;
		}

		//////////////////////////////////////////////////////////////////////////
		/*!
		Writes a 16-bit integer value to the stream

		\param value
		The value to write

		\return
		True if the value was written properly, false otherwise

		*/
		//////////////////////////////////////////////////////////////////////////
		inline bool writeInt16(NxI16 value){
			/*if (bigEndianOutput) value = toBigEndian(value);
			else value = toLittleEndian(value);*/
			return writeBuffered((unsigned char*)&value, 2) == 2;
		}

		//////////////////////////////////////////////////////////////////////////
		/*!
		Writes an unsigned 16-bit integer value to the stream

		\param value
		The value to write

		\return
		True if the value was written properly, false otherwise

		*/
		//////////////////////////////////////////////////////////////////////////
		inline bool writeUInt16(NxU16 value){
			/*if (bigEndianOutput) value = toBigEndian(value);
			else value = toLittleEndian(value);*/
			return writeBuffered((unsigned char*)&value, 2) == 2;
		}

		//////////////////////////////////////////////////////////////////////////
		/*!
		Writes a 32-bit integer value to the stream

		\param value
		The value to write

		\return
		True if the value was written properly, false otherwise

		*/
		//////////////////////////////////////////////////////////////////////////
		inline bool writeInt32(NxI32 value){
			/*if (bigEndianOutput) value = toBigEndian(value);
			else value = toLittleEndian(value);*/
			return writeBuffered((unsigned char*)&value, 4) == 4;
		}

		//////////////////////////////////////////////////////////////////////////
		/*!
		Writes an unsigned 32-bit integer value to the stream

		\param value
		The value to write

		\return
		True if the value was written properly, false otherwise

		*/
		//////////////////////////////////////////////////////////////////////////
		inline bool writeUInt32(NxU32 value){
			/*if (bigEndianOutput) value = toBigEndian(value);
			else value = toLittleEndian(value);*/
			return writeBuffered((unsigned char*)&value, 4) == 4;
		}

		//////////////////////////////////////////////////////////////////////////
		/*!
		Writes a real value to the stream

		\param value
		The value to write

		\return
		True if the value was written properly, false otherwise

		*/
		//////////////////////////////////////////////////////////////////////////
		inline bool writeReal(NxReal value){
			return writeUInt32(*(NxU32*)&value);
		}

		//////////////////////////////////////////////////////////////////////////
		/*!
		Writes a vector to the stream

		\param value
		The vector to write

		\return
		True if the vector was written properly, false otherwise

		*/
		//////////////////////////////////////////////////////////////////////////
		inline bool writeVector(const NxVec3& value){
			if (!writeReal(value.x)) return false;
			if (!writeReal(value.y)) return false;
			if (!writeReal(value.z)) return false;
			return true;
		}

		//////////////////////////////////////////////////////////////////////////
		/*!
		Writes a direction to the stream

		\param value
		The direction to write

		\return
		True if the direction was written properly, false otherwise

		*/
		//////////////////////////////////////////////////////////////////////////
		/*inline bool writeDirection(const Direction& value){
			if (!writeReal(value.getX())) return false;
			if (!writeReal(value.getX())) return false;
			if (!writeReal(value.getX())) return false;
			return true;
		}*/

		//////////////////////////////////////////////////////////////////////////
		/*!
		Writes a point to the stream

		\param value
		The point to write

		\return
		True if the point was written properly, false otherwise

		*/
		//////////////////////////////////////////////////////////////////////////
		/*inline bool writePoint(const Point& value){
			if (!writeReal(value.x)) return false;
			if (!writeReal(value.y)) return false;
			if (!writeReal(value.z)) return false;
			return true;
		}*/

		//////////////////////////////////////////////////////////////////////////
		/*!
		Writes a matrix to the stream

		\param value
		The matrix to write

		\return
		True if the matrix was written properly, false otherwise

		*/
		//////////////////////////////////////////////////////////////////////////
		inline bool writeMatrix(const NxMat33& value)
		{
			if (!writeReal(value(0, 0))) return false;
			if (!writeReal(value(0, 1))) return false;
			if (!writeReal(value(0, 2))) return false;
			if (!writeReal(value(1, 0))) return false;
			if (!writeReal(value(1, 1))) return false;
			if (!writeReal(value(1, 2))) return false;
			if (!writeReal(value(2, 0))) return false;
			if (!writeReal(value(2, 1))) return false;
			if (!writeReal(value(2, 2))) return false;
			return true;
		}

		//////////////////////////////////////////////////////////////////////////
		/*!
		Writes a frame to the stream

		\param value
		The frame to write

		\return
		True if the frame was written properly, false otherwise

		*/
		//////////////////////////////////////////////////////////////////////////
		inline bool writeFrame(const NxMat34& value){
			if (!writeMatrix(value.M)) return false;
			if (!writeVector(value.t)) return false;
			return true;
		}

		//////////////////////////////////////////////////////////////////////////
		/*!
		Writes a plane to the stream

		\param value
		The plane to write

		\return
		True if the plane was written properly, false otherwise

		*/
		//////////////////////////////////////////////////////////////////////////
		inline bool writePlane(const NxPlane& value){
			if (!writeReal(value.normal.x)) return false;
			if (!writeReal(value.normal.y)) return false;
			if (!writeReal(value.normal.z)) return false;
			if (!writeReal(value.d)) return false;
			return true;
		}

		//////////////////////////////////////////////////////////////////////////
		/*!
		Writes a AABB to the stream

		\param value
		The AABB to write

		\return
		True if the AABB was written properly, false otherwise

		*/
		//////////////////////////////////////////////////////////////////////////
		/*inline bool writeAABB(const AABB& value){
			if (!writePoint(value.getOrigin())) return false;
			if (!writeVector(value.getExtent())) return false;
			return true;
		}*/

		//////////////////////////////////////////////////////////////////////////
		/*!
		Writes a OBB to the stream

		\param value
		The OBB to write

		\return
		True if the OBB was written properly, false otherwise

		*/
		//////////////////////////////////////////////////////////////////////////
		/*inline bool writeOBB(const OBB& value){
			if (!writeMatrix(value.getBase())) return false;
			if (!writePoint(value.getOrigin())) return false;
			if (!writeVector(value.getExtent())) return false;
			return true;
		}*/

		//////////////////////////////////////////////////////////////////////////
		/*!
		Writes a string to the stream

		\param value
		The string to write

		\return
		True if the string was written properly, false otherwise

		*/
		//////////////////////////////////////////////////////////////////////////
		inline bool writeString(const meq::String &value){
			if (!writeUInt32(value.getLength()))
				return false;
			return writeData((unsigned char*)value.getArray(), value.getLength());
		}

		//////////////////////////////////////////////////////////////////////////
		/*!
		Writes a data block to the stream

		\param data
		A pointer to the block of memory that is to be written. This 
		must point to an allocated block at least as big as length.

		\param length
		The number of bytes of data to write

		\return
		True if the data was written properly, false otherwise

		*/
		//////////////////////////////////////////////////////////////////////////
		inline bool writeData(const unsigned char* data, unsigned int length){
			return writeBuffered(data, length) == length;
		}

		virtual bool flush() = 0;
		virtual int writeBuffered(const unsigned char* data, unsigned int length) = 0;

		bool bigEndianOutput;
	};
}

#endif // MEQ_STREAMS_H

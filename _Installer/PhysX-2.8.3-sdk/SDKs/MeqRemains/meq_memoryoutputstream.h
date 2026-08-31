/*************************************************************************
*
* Copyright 2010 NVIDIA Corporation
*
*			www.nvidia.com
*
************************************************************************/

#ifndef MEQ_MEMORYOUTPUTSTREAM_H
#define MEQ_MEMORYOUTPUTSTREAM_H

#include "NxSimpleTypes.h"
#include "meq_streams.h"

namespace meq {
	/*
	The memory output stream is also more general. It will always allocate
	it's own buffer internally and public interface is always void pointer
	and byte count. The buffer is cleared on open, so the same output stream
	can be used several times without allocations by closing it and reopening
	it.
	*/
	class /*MEQ_PUBLIC_CLASS*/  MemoryOutputStream : public OutputStream {
	public:
		//MEQ_DECLARE_MMOBJECT(MemoryOutputStream);

		MemoryOutputStream() : memoryBuffer(NULL), currentPos(0), bufferSize(0), customBuffer(false) {
		}

		~MemoryOutputStream(){
			close();
		}

		//////////////////////////////////////////////////////////////////////////
		/*!
		Opens an internally allocated block of memory for output. 
		*/
		//////////////////////////////////////////////////////////////////////////
		void open(){
			close();
			customBuffer = false;
			memoryBuffer = (unsigned char*)NxFoundation::FoundationSDK::getInstance().getAllocator().malloc(MEQ_OUTPUT_BUFFER_SIZE, NX_MEMORY_VRDData);
			bufferSize = MEQ_OUTPUT_BUFFER_SIZE;
		}

		//////////////////////////////////////////////////////////////////////////
		/*!
		Opens an externally allocated block of memory for output. 
		*/
		//////////////////////////////////////////////////////////////////////////
		void open(unsigned char* buffer, NxI32 length){
			close();
			customBuffer = true;
			memoryBuffer = buffer;
			bufferSize = length;
		}

		//////////////////////////////////////////////////////////////////////////
		/*!
		Closes an open stream
		*/
		//////////////////////////////////////////////////////////////////////////
		void close(){
			if (!customBuffer && memoryBuffer != NULL)
				NxFoundation::FoundationSDK::getInstance().getAllocator().free(memoryBuffer);
			memoryBuffer = NULL;
			currentPos = 0;
			bufferSize = 0;
		}

		//////////////////////////////////////////////////////////////////////////
		/*!
		Returns a pointer to the block of memory currently used for output.
		Note that this memory may be reallocated when data is written to the
		stream, which will invalidate the pointer returned by this call. 

		\return
		A pointer to the current output data 
		*/
		//////////////////////////////////////////////////////////////////////////
		void* getBuffer() const {
			return memoryBuffer;
		}

		//////////////////////////////////////////////////////////////////////////
		/*!
		Returns the size of the output data, i.e. the number of bytes written to 
		the output stream.

		\return
		Size of the output data, in bytes
		*/
		//////////////////////////////////////////////////////////////////////////
		unsigned int getSize() const
		{
			return currentPos;
		}


		virtual bool flush(){
			return true;
		}

		virtual int writeBuffered(const unsigned char* data, unsigned int length){
			return write(data, length);
		}

		//////////////////////////////////////////////////////////////////////////
		/*!
		Returns whether it is possible to write more data to the stream or not.
		As long as there is an open stream, it will be possible to write more
		data, since it will automatically reallocate its memory buffer when 
		needed.

		\return
		True if it is possible to write more data (i.e. the stream is open), 
		false otherwise
		*/
		//////////////////////////////////////////////////////////////////////////
		bool canWriteMoreData(){
			if (customBuffer)
				return memoryBuffer != NULL && currentPos<bufferSize;
			else
				return memoryBuffer != NULL;
		}
	protected:
		virtual int write(const unsigned char* data, unsigned int length){
			if (memoryBuffer == NULL || bufferSize == 0)
				return 0;
			if ((NxI32)length>bufferSize-currentPos)
			{
				if (customBuffer)
				{
					length = bufferSize-currentPos;
				} else {
					while ((NxI32)length>bufferSize-currentPos) bufferSize *= 2;
					unsigned char* oldBuffer = memoryBuffer;
					// Is realloc in the UserAllocator implemented in the samples broken? 
					// If I use realloc here, ICE something reports a leak and crashes on exit -- Anders Stenberg
					memoryBuffer = (unsigned char*)NxFoundation::FoundationSDK::getInstance().getAllocator().malloc(bufferSize, NX_MEMORY_VRDData);
					memcpy(memoryBuffer, oldBuffer, currentPos);
					NxFoundation::FoundationSDK::getInstance().getAllocator().free(oldBuffer);
				}
			}
			memcpy(memoryBuffer+currentPos, data, length);
			currentPos += length;
			return length;
		}

		unsigned char *memoryBuffer;
		NxI32 currentPos;
		NxI32 bufferSize;
		bool customBuffer;
	};

}//meq
#endif //MEQ_MEMORYINPUTSTREAM_H

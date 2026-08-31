/*************************************************************************
*
* Copyright 2010 NVIDIA Corporation
*
*			www.nvidia.com
*
************************************************************************/

#ifndef MEQ_MEMORYINPUTSTREAM_H
#define MEQ_MEMORYINPUTSTREAM_H

#include "meq_streams.h"

namespace meq {

#define MEQ_MEMORYINPUTSTREAM_MAXLENGTH 2147483647

	//////////////////////////////////////////////////////////////////////////
	/*!
		\class MemoryInputStream

		Stream class for reading from a memory buffer.

		\sa InputStream
	*/
	//////////////////////////////////////////////////////////////////////////
	/*
	The memory input stream has been more general since last version.
	It operated on void pointer instead of growable memory buffer and
	should be more usable. Seek possibility removed since there's
	no real need for it. You can always create another memory stream
	instead.
	*/
	class /*MEQ_PUBLIC_CLASS*/ MemoryInputStream : public InputStream {
	public:
		//MEQ_DECLARE_MMOBJECT(MemoryInputStream);

		MemoryInputStream() {
			managedBuffer = true;
		}

		//////////////////////////////////////////////////////////////////////////
		/*!
		Opens a block of memory for input. 

		\param buffer
		A pointer to the block of memory to read from. This must point to an 
		allocated block at least as big as size.

		\param size
		Size of the input memory block, in bytes
		*/
		//////////////////////////////////////////////////////////////////////////
		void open(const void* buffer, unsigned int size){
			bufferData = (unsigned char *)buffer;
			validDataLength = size;
			bufferSize = size;
			bufferPos = 0;
		}

		//////////////////////////////////////////////////////////////////////////
		/*!
		Closes an open stream
		*/
		//////////////////////////////////////////////////////////////////////////
		void close(){
			bufferData = NULL;
			validDataLength = 0;
			bufferSize = 0;
			bufferPos = 0;
		}

		//////////////////////////////////////////////////////////////////////////
		/*!
		Changes the block of memory to use as input. This should most probably
		be followed or preceded by a call to setSize.

		\param buffer
		A pointer to the block of memory to read from. This must point to an 
		allocated block at least as big as the set size for the buffer.
		*/
		//////////////////////////////////////////////////////////////////////////
		void setBuffer(void* buffer){
			bufferData = (unsigned char *)buffer;
			bufferPos = 0;
		}

		//////////////////////////////////////////////////////////////////////////
		/*!
		Returns a pointer to the block of memory currently used as input

		\return
		A pointer to the current input data 
		*/
		//////////////////////////////////////////////////////////////////////////
		void* getBuffer() const{
			return bufferData;
		}

		//////////////////////////////////////////////////////////////////////////
		/*!
		Changes the size of the input data.

		\param size
		Size of the input memory block, in bytes
		*/
		//////////////////////////////////////////////////////////////////////////
		void setSize(unsigned int size){
			validDataLength = size;
			bufferSize = size;
			if (bufferPos > (NxI32)size)
				bufferPos = size;
		}
		
		//////////////////////////////////////////////////////////////////////////
		/*!
		Returns the size of the input data.

		\return
		Size of the input memory block, in bytes
		*/
		//////////////////////////////////////////////////////////////////////////
		unsigned int getSize() const {
			return bufferSize;
		}

		//////////////////////////////////////////////////////////////////////////
		/*!
		Returns whether there is more data in the input block or not. Will return
		false when the end of the stream, as specified by size, has been reached.

		\return
		True if there is more data in the input data block, false otherwise
		*/
		//////////////////////////////////////////////////////////////////////////
		bool hasMoreData(){
			return bufferPos < validDataLength;
		}
	protected:
		virtual int read(unsigned char* data, unsigned int length){
			// The source memory buffer already acts as input buffer, so
			// we already have all the available input data buffered.
			return 0;
		}
	};
}
#endif


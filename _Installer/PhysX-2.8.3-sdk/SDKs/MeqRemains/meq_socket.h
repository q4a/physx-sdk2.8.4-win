/*************************************************************************
*
* Copyright 2010 NVIDIA Corporation
*
*			www.nvidia.com
*
************************************************************************/

#ifndef MEQ_SOCKET_H
#define MEQ_SOCKET_H

#include "meq_string.h"
#include "meq_streams.h"

namespace meq{
	/*
	Socket now uses centralized buffering and blocking functionality.
	*/
	class Socket : public InputStream, public OutputStream{
	public:
		//MEQ_DECLARE_MMOBJECT(Socket);

		Socket () : currentBufferPos(0){
		#ifdef __PPCGEKKO__
		buffer = NULL;
		#endif
		}

		//////////////////////////////////////////////////////////////////////////
		/*!
		Opens a network socket for input and/or output

		\param host
		Name of the host to connect to. This can be an IP, URL, etc

		\param port
		The port to connect to on the remote host

		\return
		True if the connection was successful, false otherwise
		*/
		//////////////////////////////////////////////////////////////////////////
		bool connect(const char* host, unsigned int port){
			this->host = host;
			this->port = port;
			return connectInternal(host, port);
		}

		//////////////////////////////////////////////////////////////////////////
		/*!
		Disconnects an open socket
		*/
		//////////////////////////////////////////////////////////////////////////
		void disconnect(){
			disconnectInternal();
			host = "";
			port = 0;
		}

		//////////////////////////////////////////////////////////////////////////
		/*!
		Returns whether the socket is currently open (connected) or not.

		\return
		True if the socket is connected, false otherwise
		*/
		//////////////////////////////////////////////////////////////////////////
		virtual bool isConnected() const = 0;

		//////////////////////////////////////////////////////////////////////////
		/*!
		Returns the name of the connected host. This is the same as the string 
		that was supplied to the connect call.

		\return
		The name of the connected host
		*/
		//////////////////////////////////////////////////////////////////////////
		const char* getHost() const {
			return host.getArray();
		}

		//////////////////////////////////////////////////////////////////////////
		/*!
		Returns the port of the connected host. This is the same as the port 
		that was supplied to the connect call.

		\return
		The port of the connected host
		*/
		//////////////////////////////////////////////////////////////////////////
		unsigned int getPort() const {
			return port;
		}



		//////////////////////////////////////////////////////////////////////////
		/*!
		Flushes the output stream. Until the stream is flushed, there is no
		guarantee that the written data has actually reached the destination
		storage. Flush forces all buffered data to be sent to the output.

		\return
		True if the flush was successful, false otherwise

		*/
		//////////////////////////////////////////////////////////////////////////
		virtual bool flush(){
			NxI32 totalBytesWritten = 0;
			NxI32 bytesWritten = 1;
			while (totalBytesWritten < currentBufferPos &&
				   bytesWritten > 0)
			{
				bytesWritten = write(buffer+totalBytesWritten, currentBufferPos-totalBytesWritten);
				if (bytesWritten>0)
					totalBytesWritten += bytesWritten;
			}
			bool successful = (totalBytesWritten == currentBufferPos);
			currentBufferPos = 0;
			return successful;
		}

		//////////////////////////////////////////////////////////////////////////
		/*!
		Writes data to the output stream. This method is to be overloaded by
		subclasses to perform the actual data write.

		\param data
		Pointer to a  block of data to write to the stream

		\param length
		Amount of data to write, in bytes

		\return
		Number of bytes actually written. This could be lower than length if the
		end of the stream was encountered, but will succeed in most situations.
		*/
		//////////////////////////////////////////////////////////////////////////
		virtual int write(const unsigned char* data, unsigned int length) = 0;

	protected:

		inline int writeBuffered(const unsigned char* data, unsigned int length){
			NxI32 bytesWritten = 0;
			while(length>(unsigned int)(MEQ_OUTPUT_BUFFER_SIZE-currentBufferPos)){
				NxI32 currentChunk = MEQ_OUTPUT_BUFFER_SIZE-currentBufferPos;
				memcpy(buffer+currentBufferPos, data+bytesWritten, currentChunk);
				currentBufferPos = MEQ_OUTPUT_BUFFER_SIZE;
				bool flushed = flush();
				if (!flushed)
				{
					disconnect();
					return bytesWritten;
				}
				bytesWritten += currentChunk;
				length -= currentChunk;
			}
			if (length>0) {
				memcpy(buffer+currentBufferPos, data+bytesWritten, length);
				bytesWritten += length;
				currentBufferPos += length;
			}
			if (currentBufferPos == MEQ_OUTPUT_BUFFER_SIZE)
			{
				bool flushed = flush();
				if (!flushed)
				{
					disconnect();
					return bytesWritten;
				}
			}
			return bytesWritten;
		}

		virtual bool connectInternal(const char* address, unsigned int port) = 0;
		virtual void disconnectInternal() = 0;

		String host;
		unsigned int port;
		
		NxI32 currentBufferPos;

#ifdef __PPCGEKKO__
			 
		__attribute__ ((aligned(32))) NxU8* buffer;
#else
		NxU8 buffer[MEQ_OUTPUT_BUFFER_SIZE];
#endif

	}; 
}

#endif


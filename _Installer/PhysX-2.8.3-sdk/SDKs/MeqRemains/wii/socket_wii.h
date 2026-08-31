/*----------------------------------------------------------------------------*\
|
|							  NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#ifndef NX_SOCKET_WII_H
#define NX_SOCKET_WII_H

#include <revolution/hio2.h>

//#include "NxSimpleTypes.h"
#include "meq_socket.h"
#include "socket_wii_impl.h"

enum HIO2_MSG
{
	HIO2_ARK_CONNECT,
	HIO2_REQ_CONNECT,
	HIO2_ARK_SEND,
	HIO2_REQ_SEND,
	HIO2_ARK_DISCONNECT
};



namespace meq{

struct ConnectInfo;

	class SocketWII:public Socket{
	public:

		SocketWII();

		virtual ~SocketWII();

		bool hasMoreData()
		{
			return isConnected();
		}

		bool canWriteMoreData()
		{
			return isConnected();
		}


		bool isConnected() const
		{
			return connectedFlag;

		}



	public:
		virtual bool connectInternal(const char* szAddress, unsigned int port);

		virtual void disconnectInternal();

		virtual int read(unsigned char* pData, unsigned int len);

		virtual int write(const unsigned char* pData, unsigned int len);

		SOCKET_WII sock;
		bool connectedFlag;
		bool initFlag;

	public:


};
}

#endif



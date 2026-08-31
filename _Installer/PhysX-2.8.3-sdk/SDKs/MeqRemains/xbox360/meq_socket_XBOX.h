/*************************************************************************
*
* Copyright 2010 NVIDIA Corporation
*
*			www.nvidia.com
*
************************************************************************/

#ifndef MEQ_SOCKET_XBOX_H
#define MEQ_SOCKET_XBOX_H

//#include "meq_config.h"
#include "NxSimpleTypes.h"
#include "meq_socket.h"

#if !defined(_XBOX)
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#	include <winsock.h>
#	pragma comment( lib, "wsock32" )
#else
#	include "NxXBOX.h"
#endif



namespace meq{
	class SocketXBOX : public Socket{
	public:
		//MEQ_DECLARE_MMOBJECT(SocketXBOX);

		SocketXBOX(){
			sock = INVALID_SOCKET;

#if defined(_XBOX)
			//We need to init the xbox network libraries, being sure to instruct them to use insecure 
			// communication so we are allowed to talk to the developers PC...

			XNetStartupParams xnsp;
			memset(&xnsp, 0, sizeof(xnsp));
			xnsp.cfgSizeOfStruct = sizeof(XNetStartupParams);
			xnsp.cfgFlags = XNET_STARTUP_BYPASS_SECURITY;
			INT err = XNetStartup(&xnsp);
#endif

			WORD vreq;
			WSADATA wsaData;
			vreq = MAKEWORD(2,2);
			if (WSAStartup(vreq, &wsaData)!=0){
				//MEQ_ASSERT(false, "Failed to init socket");
			}

			connectedFlag=false;
		}

		virtual ~SocketXBOX(){
			flush();
			disconnect();
		}

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
			/* Use a lazy connected flag, seems to work fine. MSG_PEEK is
			not supported on the xenon. Should be faster as well...
						
			char buffer;
			int mode=1;
			ioctlsocket(sock, FIONBIO, (u_long*)&mode);
			int got = recv(sock, &buffer, 1, MSG_PEEK);
			mode = blocking?0:1;
			int error = WSAGetLastError();
			ioctlsocket(sock, FIONBIO, (u_long*)&mode);
			if (got == 0)
				return false;
			return got != SOCKET_ERROR || error == WSAEWOULDBLOCK;*/

			return connectedFlag;
		}


	protected:
		virtual bool connectInternal(const char* szAddress, unsigned int port){

			sockaddr_in sa;
			memset(&sa, 0, sizeof(sockaddr_in));
			sa.sin_family = AF_INET;
			sa.sin_port = htons(port);

#if !defined(_XBOX)
			hostent *hp;

			//get host
			hp = gethostbyname(szAddress);
			if(!hp) {
				in_addr a; a.s_addr = inet_addr(szAddress);
				hp = gethostbyaddr((const char*)&a, sizeof(in_addr), AF_INET);
				if(!hp) return false;
			}
			memcpy(&sa.sin_addr, hp->h_addr_list[0], hp->h_length);
#else
			// The xbox does not support dns lookups with gethostbyname()... hmmm

			WSAEVENT hEvent = WSACreateEvent();
			XNDNS * pxndns = NULL;
			INT err = XNetDnsLookup(szAddress, hEvent, &pxndns);
			WaitForSingleObject(hEvent, INFINITE);

			if ((pxndns->iStatus == 0)&&(pxndns->cina>0))
				memcpy(&sa.sin_addr,&(pxndns->aina[0]),sizeof(in_addr));
			else
				{
				unsigned long a=inet_addr(szAddress);
				memcpy(&sa.sin_addr,&a,sizeof(in_addr));
				}

			if(pxndns!=NULL)
				XNetDnsRelease(pxndns);
#endif

			//connect
			sock = socket(AF_INET, SOCK_STREAM, 0);
			if (sock == INVALID_SOCKET) return false;

			if(::connect(sock, (sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR)
			{
				closesocket(sock);
				return false;
			}
			int mode=blocking?0:1;
			ioctlsocket(sock, FIONBIO, (u_long*)&mode);

			connectedFlag=true;

			return true;
		}

		virtual void disconnectInternal(){
			if (sock != INVALID_SOCKET)
			{
				closesocket(sock);
				sock = INVALID_SOCKET;
			}
			connectedFlag=false;
		}

		virtual int read(unsigned char* pData, unsigned int len){
			NxI32 bytesReceived = recv(sock, (char*)pData, (int)len, 0);
			if (bytesReceived<0)
				{
				bytesReceived = 0;
				connectedFlag=false;
				}

			return bytesReceived;
		}

		virtual int write(const unsigned char* pData, unsigned int len){
			size_t sent = 0;
			while ((sent = send(sock, (char*)pData, (int)len, 0))== SOCKET_ERROR){
				if (WSAGetLastError() != WSAEWOULDBLOCK){
					connectedFlag=false;
					return 0;
				}
			}
			return (int)sent;
		}

		SOCKET sock;
		bool connectedFlag;
	};
}

#endif



/*************************************************************************
*
* Copyright 2010 NVIDIA Corporation
*
*			www.nvidia.com
*
************************************************************************/

#ifndef MEQ_SOCKET_WIN32_H
#define MEQ_SOCKET_WIN32_H

//#include "meq_config.h"
#include "NxSimpleTypes.h"
#include "meq_socket.h"

#define SUPPORTS_SOCKETS	(WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)	// todo: implement

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock.h>
#if SUPPORTS_SOCKETS
#pragma comment( lib, "wsock32" )
#endif


namespace meq{
	class SocketWin32 : public Socket{
	public:
		//MEQ_DECLARE_MMOBJECT(SocketWin32);

		SocketWin32(){
			#if SUPPORTS_SOCKETS
			sock = INVALID_SOCKET;

			WORD vreq;
			WSADATA wsaData;
			vreq = MAKEWORD(2,2);
			if (WSAStartup(vreq, &wsaData)!=0){
				//MEQ_ASSERT(false, "Failed to init socket");
			}
			#endif
			connectedFlag=false;
		}

		virtual ~SocketWin32(){
			#if SUPPORTS_SOCKETS
			flush();
			disconnect();
			#endif
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
			#if SUPPORTS_SOCKETS
			sockaddr_in sa;
			memset(&sa, 0, sizeof(sockaddr_in));
			sa.sin_family = AF_INET;
			sa.sin_port = htons(port);

			hostent *hp;

			//get host
			hp = gethostbyname(szAddress);
			if(!hp) {
				in_addr a; a.s_addr = inet_addr(szAddress);
				hp = gethostbyaddr((const char*)&a, sizeof(in_addr), AF_INET);
				if(!hp) return false;
			}
			memcpy(&sa.sin_addr, hp->h_addr_list[0], hp->h_length);

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
			#else
			return false;
			#endif
		}

		virtual void disconnectInternal(){
			#if SUPPORTS_SOCKETS
			if (sock != INVALID_SOCKET)
			{
				closesocket(sock);
				sock = INVALID_SOCKET;
			}
			#endif
			connectedFlag=false;
		}

		virtual int read(unsigned char* pData, unsigned int len){
			#if SUPPORTS_SOCKETS
			NxI32 bytesReceived = recv(sock, (char*)pData, (int)len, 0);
			if (bytesReceived<0)
				{
				bytesReceived = 0;
				connectedFlag=false;
				}

			return bytesReceived;
			#else
			return 0;
			#endif
		}

		virtual int write(const unsigned char* pData, unsigned int len){
			#if SUPPORTS_SOCKETS
			size_t sent = 0;
			while ((sent = send(sock, (char*)pData, (int)len, 0))== SOCKET_ERROR){
				if (WSAGetLastError() != WSAEWOULDBLOCK){
					connectedFlag=false;
					return 0;
				}
			}
			return (int)sent;
			#else
			return 0;
			#endif
		}

		#if SUPPORTS_SOCKETS
		SOCKET sock;
		#endif
		bool connectedFlag;
	};
}

#endif



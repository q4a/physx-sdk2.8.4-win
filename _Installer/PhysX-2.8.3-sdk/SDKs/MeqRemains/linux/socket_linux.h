/*----------------------------------------------------------------------------*\
|
|							  AGEIA PhysX Technology
|
|							     www.ageia.com
|
\*----------------------------------------------------------------------------*/

#ifndef NX_SOCKET_LINUX_H
#define NX_SOCKET_LINUX_H

#include "NxSimpleTypes.h"
#include "meq_socket.h"

#if !defined(__native_client__)

#include <netinet/in.h>
//#include <netex/errno.h>
#if defined (__APPLE__)
#include <errno.h>
#elif defined (__QNX__) || defined (__TIZEN__)
#include <errno.h>
#include <sys/socket.h>
#elif defined(LINUX)
#include <asm/errno.h>
#include <linux/socket.h>
#endif
//#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>

#endif

namespace meq{
	class SocketLinux : public Socket{
	public:

		SocketLinux(){
			connectedFlag=false;
		}

		virtual ~SocketLinux(){
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
/*						
			char buffer;
			int mode=1; // non-blocking
			setsockopt(sock, SOL_SOCKET, SO_NBIO, (int*)&mode, sizeof(int));
			int got = recv(sock, &buffer, 1, MSG_PEEK);
			mode = blocking?0:1;

			setsockopt(sock, SOL_SOCKET, SO_NBIO, (int*)&mode, sizeof(int));
			if (got == 0)
				return false;
			return got != SO_ERROR || sys_net_errno == SYS_NET_EWOULDBLOCK;
*/
			return connectedFlag;

		}


	protected:
		virtual bool connectInternal(const char* szAddress, unsigned int port){
#if !defined(__native_client__)
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

			if (sock == -1) return false;

			if(::connect(sock, (sockaddr*)&sa, sizeof(sa)) != 0)
			{
			  //socketclose(sock);
			  shutdown(sock, SHUT_RDWR);
				return false;
			}
			//int mode=blocking?0:1;
			//setsockopt(sock, SOL_SOCKET, SO_NBIO, (int*)&mode, sizeof(int));
			int mode = fcntl(sock, F_GETFL, 0);
			if (!blocking)
			  mode |= O_NONBLOCK;
			else
			  mode &= ~O_NONBLOCK;
			fcntl(sock, F_SETFL, &mode);

			connectedFlag=true;

			return true;
#else
			return false;
#endif
		}

		virtual void disconnectInternal(){
#if !defined(__native_client__)
		  //socketclose(sock);
		        shutdown(sock, SHUT_RDWR);
			connectedFlag=false;
#endif
		}

		virtual int read(unsigned char* pData, unsigned int len){
#if !defined(__native_client__)
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
#if !defined(__native_client__)
			size_t sent = 0;
			while ((sent = send(sock, (char*)pData, (int)len, 0)) < 0){
			  int err;
			  socklen_t errlen = sizeof(err);
			  getsockopt(sock, SOL_SOCKET, SO_ERROR, &err, &errlen);
				if (err != EAGAIN){
					connectedFlag=false;
					return 0;
				}
			}
			return sent;
#else
			return 0;
#endif
		}

		int sock;
		bool connectedFlag;
	};
}

#endif



/* NVIDIA/SCE CONFIDENTIAL
   NVIDIA PhysX(TM) SDK for PlayStation(R)3 Version PS3_PhysX_283_B2_forSDK330_hlsrc
*/
/*----------------------------------------------------------------------------*\
|
|							  NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#ifndef NX_SOCKET_PS3_H
#define NX_SOCKET_PS3_H

#include "NxSimpleTypes.h"
#include "meq_socket.h"

#include <netinet/in.h>
#include <netex/errno.h>
#include <netex/net.h>
#include <netex/libnetctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cell/sysmodule.h>
#include <sys/timer.h>

namespace meq{
	class SocketCell : public Socket{
	public:

		SocketCell(){
			initFlag		= false;
			connectedFlag	= false;
		}

		virtual ~SocketCell(){
			if(connectedFlag)
			{
				flush();
				disconnect();
			}
			if(initFlag)
			{
				sys_net_finalize_network();
			}
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

			int iReturn;
			iReturn=cellSysmoduleLoadModule(CELL_SYSMODULE_NET);

			// E See if the module is already loaded
			if (iReturn!=CELL_OK && iReturn!=CELL_SYSMODULE_ERROR_DUPLICATED) {
				printf("PhysX Net (VRD): error loading CELL_SYSMODULE_NET module: %08x\n",
					   iReturn);
			} else {
				if ( 0 != sys_net_initialize_network() )
				{
					printf("PhysX Net (VRD): Failed to intialize network with sys_net_initialize_network()\n");
				} 
				else
				{
					initFlag = true;
				}
				if ( 0 != cellNetCtlInit() )
				{
					printf("PhysX Net (VRD): Failed to intialize network control with cellNetCtlInit()\n");
				} 
				else
				{
					int state;
					cellNetCtlGetState(&state);
					int countdown = 25;

					if(state != CELL_NET_CTL_STATE_IPObtained) {
						printf("PhysX Net (VRD): Network not ready yet! wait...\n");
						
						while(state != CELL_NET_CTL_STATE_IPObtained)
						{
							sys_timer_usleep(200000);
							if(countdown-- < 0)
							{
								printf("PhysX Net (VRD): Network seems not connected!\n");
								break;
							}
							cellNetCtlGetState(&state);
						}
					}
					cellNetCtlTerm();
				}
			}

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

			timeval timeout = {5,0};
			setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(struct timeval));
			
			if(::connect(sock, (sockaddr*)&sa, sizeof(sa)) != 0)
			{
				socketclose(sock);
				return false;
			}
			int mode=blocking?0:1;
			setsockopt(sock, SOL_SOCKET, SO_NBIO, (int*)&mode, sizeof(int));

			connectedFlag=true;

			return true;
		}

		virtual void disconnectInternal(){
			flush();
			socketclose(sock);
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
			while ((sent = send(sock, (char*)pData, (int)len, 0)) < 0){
				if (sys_net_errno != SYS_NET_EWOULDBLOCK){
					connectedFlag=false;
					return 0;
				}
			}
			return sent;
		}

		int sock;
		bool connectedFlag;
		bool initFlag;
	};

}

#endif





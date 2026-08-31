#ifndef SOCKET_WII_IMPL_H
#define SOCKET_WII_IMPL_H

#ifdef __PPCGEKKO__
	#define HOST_SIDE_SOCKET 0
#else
	#define HOST_SIDE_SOCKET 1
#endif


#if !HOST_SIDE_SOCKET
typedef long SOCKET_WII;

SOCKET_WII socket();
int connect(SOCKET_WII s);
int closesocket(SOCKET_WII s);
int send(SOCKET_WII s, const char* buf, int len);
int recv(SOCKET_WII s, char* buf, int len);

BOOL HIO2Enum( HIO2DeviceType type );
void HIO2Receive( HIO2Handle h );
void HIO2Dma( HIO2Handle h );
void HIO2Disconnect( HIO2Handle h );
int HIO2Notify( HIO2NotifyEvent event, void* param );
#else
BOOL LoadProxyLib( const char* libname );
void RunProxy();
#endif

#endif

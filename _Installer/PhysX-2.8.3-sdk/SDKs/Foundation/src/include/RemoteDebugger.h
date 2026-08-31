/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#ifndef NX_FOUNDATION_REMOTEDEBUGGER
#define NX_FOUNDATION_REMOTEDEBUGGER

#include "Allocateable.h"
#include "NxRemoteDebugger.h"
#include "Array.h"

#ifndef NX_DISABLE_REMOTE_DEBUG

#include "meq_streams.h"

#include "meq_socket_impl.h"

#define NX_DBG_MAGIC_NUMBER 0xfa1af31

class NxActor;


//////////////////////////////////////////////////////////////////////////
/*!
	Bitfield entries describe the names of the masks for bitfields
*/
struct NxBitfieldEntry
{
	NxU32		mask;
	const char*	name;
};

//////////////////////////////////////////////////////////////////////////
/*!
	RemoteDebug supplies functionality for writing debug info to a stream
	to be read by the remote debugger application.
*/
//////////////////////////////////////////////////////////////////////////
class RemoteDebugger : public NxRemoteDebugger, public NxFoundation::NxAllocateable
{
public:

	RemoteDebugger ();
	~RemoteDebugger ();

	void connect(const char* host, unsigned int port, NxU32 eventMask);
	
	void connect(meq::InputStream *inputStream, meq::OutputStream *outputStream, NxU32 eventMask);

	void disconnect();

	//////////////////////////////////////////////////////////////////////////
	/*!
		Flushes the output stream, i.e. forces it to write/send any queued
		data.
	*/
	//////////////////////////////////////////////////////////////////////////
	void flush();

	//////////////////////////////////////////////////////////////////////////
	/*!
		Returns whether the debugger is connected or not
	*/
	//////////////////////////////////////////////////////////////////////////
	bool isConnected();

	//////////////////////////////////////////////////////////////////////////
	/*!
		Write a frame break to the output stream.
	*/
	//////////////////////////////////////////////////////////////////////////
	void frameBreak();

	//////////////////////////////////////////////////////////////////////////
	/*!
		Create an object in the output stream, given an object identifier,
		object type and object classname.
	*/
	//////////////////////////////////////////////////////////////////////////
	void createObject(void *object, NxRemoteDebuggerObjectType type, const char *className, NxU32 mask);

	//////////////////////////////////////////////////////////////////////////
	/*!
		Remove an object in the output stream, given an object identifier.
	*/
	//////////////////////////////////////////////////////////////////////////
	void removeObject(void *object, NxU32 mask);

	//////////////////////////////////////////////////////////////////////////
	/*!
		Add an object as child to another object.
	*/
	//////////////////////////////////////////////////////////////////////////
	void addChild(void *object, void *child, NxU32 mask);

	//////////////////////////////////////////////////////////////////////////
	/*!
		Remove a child object from another object.
	*/
	//////////////////////////////////////////////////////////////////////////
	void removeChild(void *object, void *child, NxU32 mask);

	//////////////////////////////////////////////////////////////////////////
	/*!
		Write a real parameter to output stream, given a parent object
		identifier and parameter name. The create argument
		must be true at parameter creation, and should be false for all
		consecutive writes of the same parameter for optimal stream size.
	*/
	//////////////////////////////////////////////////////////////////////////
	void writeParameter(const NxReal &parameter, void *object, bool create, const char *name, NxU32 mask);

	//////////////////////////////////////////////////////////////////////////
	/*!
		Write a NxU32 parameter to output stream, given a parent object
		identifier and parameter name. The create argument
		must be true at parameter creation, and should be false for all
		consecutive writes of the same parameter for optimal stream size.
	*/
	//////////////////////////////////////////////////////////////////////////
	void writeParameter(const NxU32 &parameter, void *object, bool create, const char *name, NxU32 mask);

	//////////////////////////////////////////////////////////////////////////
	/*!
		Write a Vector parameter to output stream, given a parent object
		identifier and parameter name. The create argument
		must be true at parameter creation, and should be false for all
		consecutive writes of the same parameter for optimal stream size.
	*/
	//////////////////////////////////////////////////////////////////////////
	void writeParameter(const NxVec3 &parameter, void *object, bool create, const char *name, NxU32 mask);

	//////////////////////////////////////////////////////////////////////////
	/*!
		Write a Plane parameter to output stream, given a parent object
		identifier and parameter name. (Parameter name 
		needs only to be supplied at parameter creation.)
	*/
	//////////////////////////////////////////////////////////////////////////
	void writeParameter(const NxPlane &parameter, void *object, bool create, const char *name, NxU32 mask);

	//////////////////////////////////////////////////////////////////////////
	/*!
		Write a NxMat34 parameter to output stream, given a parent object
		identifier and parameter name. The create argument
		must be true at parameter creation, and should be false for all
		consecutive writes of the same parameter for optimal stream size.
	*/
	//////////////////////////////////////////////////////////////////////////
	void writeParameter(const NxMat34 &parameter, void *object, bool create, const char *name, NxU32 mask);

	//////////////////////////////////////////////////////////////////////////
	/*!
		Write a NxMat33 parameter to output stream, given a parent object
		identifier and parameter name. The create argument
		must be true at parameter creation, and should be false for all
		consecutive writes of the same parameter for optimal stream size.
	*/
	//////////////////////////////////////////////////////////////////////////
	void writeParameter(const NxMat33 &parameter, void *object, bool create, const char *name, NxU32 mask);

	//////////////////////////////////////////////////////////////////////////
	/*!
		Write a Binary parameter to output stream, given a parent object
		identifier and parameter name. The first 4 bytes
		must be an NxU32 containing the data size, including those 4 size
		bytes. The data is assumed to be of the same endianness as the output
		stream. The create argument must be true at parameter creation, and 
		should be false for all	consecutive writes of the same parameter 
		for optimal stream size.
	*/
	//////////////////////////////////////////////////////////////////////////
	void writeParameter(const NxU8 *parameter, void *object, bool create, const char *name, NxU32 mask);

	//////////////////////////////////////////////////////////////////////////
	/*!
		Write a string parameter to output stream, given a parent object
		identifier and parameter name. The create argument
		must be true at parameter creation, and should be false for all
		consecutive writes of the same parameter for optimal stream size.
	*/
	//////////////////////////////////////////////////////////////////////////
	void writeParameter(const char *parameter, void *object, bool create, const char *name, NxU32 mask);

	//////////////////////////////////////////////////////////////////////////
	/*!
		Write a boolean parameter to output stream, given a parent object
		identifier and parameter name. The create argument
		must be true at parameter creation, and should be false for all
		consecutive writes of the same parameter for optimal stream size.
	*/
	//////////////////////////////////////////////////////////////////////////
	void writeParameter(const bool &parameter, void *object, bool create, const char *name, NxU32 mask);

	//////////////////////////////////////////////////////////////////////////
	/*!
		Write a object id parameter to output stream, given a parent object
		identifier and parameter name. The create argument
		must be true at parameter creation, and should be false for all
		consecutive writes of the same parameter for optimal stream size.
	*/
	//////////////////////////////////////////////////////////////////////////
	void writeParameter(const void *parameter, void *object, bool create, const char *name, NxU32 mask);
	

	//////////////////////////////////////////////////////////////////////////
	/*!
		Write a contact report.  This single call replaces two add child calls
		and five set parameters. 
	*/	
	void writeContactReport(const void *object, const void *shape1, const void *shape2, const NxVec3& point, const NxVec3& axis, NxReal separation, NxU32 mask);
	

	//////////////////////////////////////////////////////////////////////////
	/*!
		Write an update for an actor in as compact a way as possible.  
		There may be a lot of these sent for a scene with lots of dynamic objects
		so this is a semi-necessary stream compaction device.
	*/	
	void writeActorUpdate(	const void *object
							, const void *bounds
							, const NxMat34& frame
							, const NxVec3& linearVelocity
							, const NxVec3& angularVelocity
							, const NxVec3& centerOfMass
							, const NxVec3&	boundsCenter
							, const NxVec3&	boundsExtents
							, NxU32 mask );
		
	//////////////////////////////////////////////////////////////////////////
	/*!
		Write a stream version event.  Most likely, the stream does this
		for you.
	*/						
	void writeStreamVersion();

	//////////////////////////////////////////////////////////////////////////
	/*!
		Write a bitfield mask description out for this object's parameters.
	*/
	void writeBitfieldDescription( const char* paramName, const void *object, NxBitfieldEntry* inEntries, NxU32 inEntryCount, NxU32 mask );

	//////////////////////////////////////////////////////////////////////////
	/*!
	Sets the mask which is used to filter out events from the Visual Remote 
	Debugger stream.
	*/
	//////////////////////////////////////////////////////////////////////////
	void setMask(NxU32 mask);

	//////////////////////////////////////////////////////////////////////////
	/*!
		Returns the mask the Visual Remote Debugger stream is filtered with.
	*/
	//////////////////////////////////////////////////////////////////////////
	NxU32 getMask();

	void *getPickedObject();
	NxVec3 getPickPoint();

	void registerEventListener(NxRemoteDebuggerEventListener* eventListener);
	void unregisterEventListener(NxRemoteDebuggerEventListener* eventListener);


	void setOutputStream(meq::OutputStream* outputStream);

	meq::OutputStream* getOutputStream();

	void setInputStream(meq::InputStream* inputStream);

	meq::InputStream* getInputStream();

protected:
#if defined(WIN32)
	meq::SocketWin32 socket;
#elif defined(_XBOX)
	meq::SocketXBOX socket;
#elif defined(__CELLOS_LV2__)
	meq::SocketCell socket;
#elif defined(LINUX) || defined(__APPLE__) || defined(__native_client__) || defined(__flash__) || defined(__webgl__)
	meq::SocketLinux socket;
#elif defined(__PPCGEKKO__)
	meq::SocketWII socket;
#elif defined(CAFE)
	meq::SocketWIIU socket;
#endif
	meq::OutputStream *outputStream;
	meq::InputStream *inputStream;
	NxU32 eventMask;

	NxActor *pickedActor;
	NxVec3 pickPoint;

	NxFoundation::NxArraySDK<NxRemoteDebuggerEventListener*> eventListeners;

	static NxU32 hashFunction(const char* input);
};

#endif // NX_DISABLE_REMOTE_DEBUG

#endif // NX_FOUNDATION_REMOTEDEBUGGER


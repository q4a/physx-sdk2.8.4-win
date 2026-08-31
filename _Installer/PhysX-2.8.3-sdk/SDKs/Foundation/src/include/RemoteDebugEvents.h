/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#ifndef NX_FOUNDATION_REMOTEDEBUGEVENTS
#define NX_FOUNDATION_REMOTEDEBUGEVENTS

#include "meq_streams.h"
#include "NxRemoteDebugger.h"
//#include "meq_endian.h"
#include "meq_string.h"

enum NxRemoteDebuggerEventType
{
	NX_DBG_EVENTTYPE_FRAME_BREAK = 0,
	NX_DBG_EVENTTYPE_CREATE_OBJECT = 1,
	NX_DBG_EVENTTYPE_REMOVE_OBJECT = 2,
	NX_DBG_EVENTTYPE_ADD_CHILD = 3,
	NX_DBG_EVENTTYPE_REMOVE_CHILD  = 4,
	NX_DBG_EVENTTYPE_SET_PARAMETER = 5,
	NX_DBG_EVENTTYPE_ERROR = 6,
	NX_DBG_EVENTTYPE_CONTACTREPORT = 7,
	NX_DBG_EVENTTYPE_ACTORUPDATE = 8,
	NX_DBG_EVENTTYPE_STREAMVERSION = 9,
	NX_DBG_EVENTTYPE_BITFLAGS = 10,
};

enum NxRemoteDebuggerFeedbackEventType
{
	NX_DBG_EVENTTYPE_REQUEST_EVENTS = 42,
	NX_DBG_EVENTTYPE_NEW_PICK = 43,
	NX_DBG_EVENTTYPE_PICK_MOVED = 44
};

enum NxRemoteDebuggerParameterType
{
	NX_DBG_PARAMTYPE_undefined = 0,
	NX_DBG_PARAMTYPE_real = 1,
	NX_DBG_PARAMTYPE_NxU32 = 2,
	NX_DBG_PARAMTYPE_Vector = 3,
	NX_DBG_PARAMTYPE_Plane = 4,
	NX_DBG_PARAMTYPE_Frame = 5,
	NX_DBG_PARAMTYPE_Matrix = 6,
	NX_DBG_PARAMTYPE_Binary = 7,
	NX_DBG_PARAMTYPE_String = 8,
	NX_DBG_PARAMTYPE_bool = 9,
	NX_DBG_PARAMTYPE_objectId = 10
};

#include "meq_string.h"

class NxCreateObjectEventData
{
public:
	//NX_DECLARE_MMOBJECT(CreateObjectEventData);

	NxU8 eventType;
	NxU32 size;
	NxU8 objectType;
	NxU32 objectID;
	meq::String name;

	NxCreateObjectEventData () {}

	NxCreateObjectEventData (NxU8 objectType, NxU32 objectID, const char* name)
		: eventType(NX_DBG_EVENTTYPE_CREATE_OBJECT),
			objectType(objectType),
			objectID(objectID),
			name(name){
		size = NxU32(sizeof(objectType) + sizeof(objectID) + strlen(name)+4);
	}

	void read (meq::InputStream *input){
		input->readUInt8(eventType);
		input->readUInt32(size);
		input->readUInt8(objectType);
		input->readUInt32(objectID);
		input->readString(name);
	}

	void write (meq::OutputStream *output){
		output->writeUInt8(eventType);
		output->writeUInt32(size);
		output->writeUInt8(objectType);
		output->writeUInt32(objectID);
		output->writeString(name);
	}
};

struct NxRemoveObjectEventData
{
public:
	//NX_DECLARE_MMOBJECT(RemoveObjectEventData);

	NxU8 eventType;
	NxU32 size;
	NxU32 objectID;

	NxRemoveObjectEventData () {}

	NxRemoveObjectEventData (NxU32 objectID)
		: eventType(NX_DBG_EVENTTYPE_REMOVE_OBJECT),
			objectID(objectID){
		size = sizeof(objectID);
	}

	void read (meq::InputStream *input){
		input->readUInt8(eventType);
		input->readUInt32(size);
		input->readUInt32(objectID);
	}

	void write (meq::OutputStream *output){
		output->writeUInt8(eventType);
		output->writeUInt32(size);
		output->writeUInt32(objectID);
	}
};

struct NxAddChildEventData
{
public:
	//NX_DECLARE_MMOBJECT(AddChildEventData);

	NxU8 eventType;
	NxU32 size;
	NxU32 objectID;
	NxU32 childID;

	NxAddChildEventData () {}

	NxAddChildEventData (NxU32 objectID, NxU32 childID)
		: eventType(NX_DBG_EVENTTYPE_ADD_CHILD),
			objectID(objectID),
			childID(childID){
		size = sizeof(objectID) + sizeof(childID);
	}

	void read (meq::InputStream *input){
		input->readUInt8(eventType);
		input->readUInt32(size);
		input->readUInt32(objectID);
		input->readUInt32(childID);
	}

	void write (meq::OutputStream *output){
		output->writeUInt8(eventType);
		output->writeUInt32(size);
		output->writeUInt32(objectID);
		output->writeUInt32(childID);
	}
};

struct NxRemoveChildEventData
{
public:
	//NX_DECLARE_MMOBJECT(RemoveChildEventData);

	NxU8 eventType;
	NxU32 size;
	NxU32 objectID;
	NxU32 childID;

	NxRemoveChildEventData () {}

	NxRemoveChildEventData (NxU32 objectID, NxU32 childID)
		: eventType(NX_DBG_EVENTTYPE_REMOVE_CHILD),
			objectID(objectID),
			childID(childID){
		size = sizeof(objectID) + sizeof(childID);
	}

	void read (meq::InputStream *input){
		input->readUInt8(eventType);
		input->readUInt32(size);
		input->readUInt32(objectID);
		input->readUInt32(childID);
	}

	void write (meq::OutputStream *output){
		output->writeUInt8(eventType);
		output->writeUInt32(size);
		output->writeUInt32(objectID);
		output->writeUInt32(childID);
	}
};

struct NxSetParameterEventData
{
public:
	//NX_DECLARE_MMOBJECT(SetParameterEventData);

	NxU8 eventType;
	NxU32 size;
	NxU32 objectID;
	NxU32 nameHash;
	NxU8 parameterType;
	NxU32 dataLength;
	const char *name;

	NxSetParameterEventData () {}

	NxSetParameterEventData (NxU32 objectID, NxU8 parameterType, NxU32 dataLength, NxU32 nameHash, const char *name)
		: eventType(NX_DBG_EVENTTYPE_SET_PARAMETER),
			objectID(objectID),
			nameHash(nameHash),
			parameterType(parameterType),
			dataLength(dataLength),
			name(name){
		size = NxU32(sizeof(objectID) + sizeof(nameHash) + sizeof(parameterType) + dataLength + strlen(name)+4);
	}

	void read (meq::InputStream *input){
		input->readUInt8(eventType);
		input->readUInt32(size);
		input->readUInt32(objectID);
		input->readUInt32(nameHash);
		input->readUInt8(parameterType);
	}

	void write (meq::OutputStream *output)
	{
		output->writeUInt8(eventType);
		output->writeUInt32(size);
		output->writeUInt32(objectID);
		output->writeUInt32(nameHash);
		output->writeUInt8(parameterType);
	}
};

struct NxContactReportEventData
{
	NxU8		eventType;
	NxU32		size;
	NxU32		objectID;
	NxU32		shape1ID;
	NxU32		shape2ID;
	NxVec3		point;
	NxVec3		axis;
	NxReal		separation;
	
	NxContactReportEventData () {}

	NxContactReportEventData (NxU32 nobjectID, NxU32 nshape1ID, NxU32 nshape2ID, NxVec3 npoint, NxVec3 naxis, NxReal nseparation )
		: eventType(NX_DBG_EVENTTYPE_CONTACTREPORT),
			objectID(nobjectID),
			shape1ID(nshape1ID),
			shape2ID(nshape2ID),
			point(npoint),
			axis(naxis),
			separation(nseparation){
		size = NxU32( sizeof(objectID) + sizeof(shape1ID) + sizeof(shape2ID) + sizeof(point) + sizeof(axis) + sizeof(separation) );
	}
	
	void read (meq::InputStream *input){
		input->readUInt8(eventType);
		input->readUInt32(size);
		input->readUInt32(objectID);
		input->readUInt32(shape1ID);
		input->readUInt32(shape2ID);
		input->readVector(point);
		input->readVector(axis);
		input->readReal(separation);
	}

	void write (meq::OutputStream *output)
	{
		output->writeUInt8(eventType);
		output->writeUInt32(size);
		output->writeUInt32(objectID);
		output->writeUInt32(shape1ID);
		output->writeUInt32(shape2ID);
		output->writeVector(point);
		output->writeVector(axis);
		output->writeReal(separation);
	}
};

struct NxActorUpdateEventData
{
	NxU8		eventType;
	NxU32		size;
	NxU32		objectID;
	NxU32		boundsID;
	NxMat34		frame;
	NxVec3		linearVelocity;
	NxVec3		angularVelocity;
	NxVec3		centerOfMass;
	NxVec3		boundsCenter;
	NxVec3		boundsExtents;
	NxActorUpdateEventData(){}
	NxActorUpdateEventData( NxU32		nobjectID
							, NxU32		nboundsID
							, NxMat34	nframe
							, NxVec3	nlinearVelocity
							, NxVec3	nangularVelocity
							, NxVec3	ncenterOfMass
							, NxVec3	nboundsCenter
							, NxVec3	nboundsExtents )
		: eventType			( NX_DBG_EVENTTYPE_ACTORUPDATE )
		, objectID			( nobjectID )
		, boundsID			( nboundsID )
		, frame				( nframe )
		, linearVelocity	( nlinearVelocity )
		, angularVelocity	( nangularVelocity )
		, centerOfMass		( ncenterOfMass )
		, boundsCenter		( nboundsCenter )
		, boundsExtents		( nboundsExtents )
	{
		size = sizeof( objectID )
			+ sizeof( boundsID )
			+ sizeof( frame.M )
			+ sizeof( frame.t )
			+ sizeof( linearVelocity )
			+ sizeof( angularVelocity )
			+ sizeof( centerOfMass )
			+ sizeof( boundsCenter )
			+ sizeof( boundsExtents );
	}
		
	
	void read (meq::InputStream *input){
		input->readUInt8	( eventType);
		input->readUInt32	( size);
		input->readUInt32	( objectID);
		input->readUInt32	( boundsID );
		input->readMatrix	( frame.M );
		input->readVector	( frame.t );
		input->readVector	( linearVelocity );
		input->readVector	( angularVelocity );
		input->readVector	( centerOfMass );
		input->readVector	( boundsCenter );
		input->readVector	( boundsExtents );
	}

	void write (meq::OutputStream *output)
	{
		output->writeUInt8	( eventType );
		output->writeUInt32	( size );
		output->writeUInt32	( objectID );
		output->writeUInt32	( boundsID );
		output->writeMatrix	( frame.M );
		output->writeVector	( frame.t );
		output->writeVector	( linearVelocity );
		output->writeVector	( angularVelocity );
		output->writeVector	( centerOfMass );
		output->writeVector	( boundsCenter );
		output->writeVector	( boundsExtents );
	}
};

struct NxStreamVersionEventData
{
	NxU8		eventType;
	NxU32		size;
	NxU32		version;
	static const NxU32 s_StreamVersion = 1;
	NxStreamVersionEventData()
		: eventType( NX_DBG_EVENTTYPE_STREAMVERSION )
		, version( s_StreamVersion )
	{
		size = sizeof( NxU32 );
	}
	
	void read (meq::InputStream *input){
		input->readUInt8	( eventType );
		input->readUInt32	( size );
		input->readUInt32	( version );
	}

	void write (meq::OutputStream *output)
	{
		output->writeUInt8	( eventType );
		output->writeUInt32	( size );
		output->writeUInt32	( version );
	}
};

//Variable length struct
struct NxBitfieldDescription
{
	NxU8		eventType;
	NxU32		size;
	NxU32		objectID;	//Id of the object this pertains to.
	NxU32		paramHash;	//Hash of the parameter name
	NxU32		count;		//Number of bit flags described
	//Each bitfield entry is an NxU32 describing the bit mask
	//followed by a string name to use for that bit mask.
	NxBitfieldDescription() {}
	NxBitfieldDescription(NxU32 inSize, NxU32 inId, NxU32 inParamHash, NxU32 inCount ) 
		: eventType( NX_DBG_EVENTTYPE_BITFLAGS )
		, size( inSize )
		, objectID( inId )
		, paramHash( inParamHash )
		, count( inCount )
	{}
	void read (meq::InputStream *input){
		input->readUInt8	( eventType );
		input->readUInt32	( size );
		input->readUInt32	( objectID );
		input->readUInt32	( paramHash );
		input->readUInt32	( count );
	}
	void write (meq::OutputStream *output)
	{
		output->writeUInt8	( eventType );
		output->writeUInt32	( size );
		output->writeUInt32	( objectID );
		output->writeUInt32	( paramHash );
		output->writeUInt32	( count );
	}
};

#undef swap

#endif


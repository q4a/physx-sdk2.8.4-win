#ifndef PXD_BROADPHASE_H
#define PXD_BROADPHASE_H

#include "px_config.h"
#include "px_task.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*!
\file
Broad phase interface.
*/

/************************************************************************/
/* Volumes																*/
/************************************************************************/

typedef PxdHandle PxdVolume; 
typedef PxdHandle PxdShape;
typedef PxdHandle PxdContext;

/*!
Type of broad phase volume. Used when creating a new volume 
and cannot be changed afterwards.
*/
typedef enum PxdVolumeType_
{
	/*!
	Invalid volume type
	*/
	PXD_VOLUME_TYPE_NONE = 0,
	/*! 
	Volume is axis-aligned bounding box.
	*/
	PXD_VOLUME_TYPE_AABB,
	/*!
	Volume is oriented bounding box. NOT IMPLEMENTED
	*/
	PXD_VOLUME_TYPE_OBB,
	/*!
	Volume is the bounding box of a body shape.
	*/
	PXD_VOLUME_TYPE_SHAPE
} PxdVolumeType;


/*!
Per volume properties
*/
typedef enum PxdVolumeProperty_
{
	/*!
	The origin of the bounding volume (1 PxdVector). 
	Not valid for objects of type PX_VOLUME_TYPE_SHAPE.
	*/
	PXD_VOLUME_ORIGIN,
	/*!
	The extent of the bounding volume (1 PxdVector)
	For a volume of type AABB this corresponds to the extent along the 
	X, Y and Z axis (in that order). 
	For an OBB it represents the extent within the OBB frame, defined by 
	PXD_VOLUME_ORIGIN and PXD_VOLUME_ORIENTATION. 
	Not valid for volumes of type PX_VOLUME_TYPE_SHAPE.
	*/
	PXD_VOLUME_EXTENT,
	/*!
	The volume orientation quaternion, only used for OBB volumes (4 PxFloats). 
	NOT IMPLEMENTED
	*/
	PXD_VOLUME_ORIENTATION,
	/*!
	The body shape from which the volume is derived. 
	Only valid for volumes of type PX_VOLUME_TYPE_SHAPE
	*/
	PXD_VOLUME_SHAPE,
	/*!
	The volume group identifier (1 PxInt). 
	*/
	PXD_VOLUME_GROUP,
	/*!
	Volume user data (void*). This pointer will be included in overlap 
	reports if the PXD_BROAD_PHASE_REPORT_USER_DATA flag is set.
	*/
	PXD_VOLUME_USER_DATA

} PxdVolumeProperty;


/*!
Descriptor used when creating a broad phase volume.

Initialize it to zero/default by using PxdVolumeInitDesc(PxdVolumeDesc*)
*/
typedef struct PxdVolumeDesc_
{
	/*!
	Volume type.
	\sa PxdVolumeType
	*/
	PxdVolumeType type;

	/*!
	Volume group
	\sa PXD_VOLUME_GROUP
	*/
	PxInt group;

	/*!
	Volume user data
	\sa PXD_VOLUME_USER_DATA
	*/
	void* userdata;

	union
	{
		struct AABB
		{
			/*!
			Volume origin.
			Used when volumeType is PXD_VOLUME_TYPE_AABB
			*/
			PxdVector origin;

			/*!
			Volume extent.
			Vector from center to the positive corner along all axes.
			Used when volumeType is PXD_VOLUME_TYPE_AABB
			*/
			PxdVector extent;
		} aabb;
		/*!
		Shape volume should be connected to. Should be the identifier of a 
		body shape.
		Used when volumeType is PXD_VOLUME_TYPE_SHAPE
		*/
		PxdShape shape;
	} volumeType ;
} PxdVolumeDesc;

/*!
Initialize a volume descriptor
\param desc
Volume descriptor to initialize
*/
PXD_EXPORT_FUNCTION void PxdVolumeInitDesc(PxdVolumeDesc* desc);

/*!
Create a new volume
\param context
Context handle
\param desc
Volume descriptor
\return
Volume identifier. Zero on failure.
*/
PXD_EXPORT_FUNCTION PxdVolume PxdVolumeCreate(PxdContext context, PxdVolumeDesc* desc);

/*!
Destroy already existing volume.
If volume isn't a valid volume, an assert will be raised in debug,
nothing will happen in release.

\param volume
Volume identifier
*/
PXD_EXPORT_FUNCTION void PxdVolumeDestroy(PxdVolume volume);

/*!
Return type for existing volume.

\param volume
Volume identifier
\return
Volume type identifier. PXD_VOLUME_TYPE_NONE if volume identifier is invalid.
*/
PXD_EXPORT_FUNCTION PxdVolumeType PxdVolumeGetType(PxdVolume volume);

/*!
\addtogroup Volume property set/get
@{
*/
PXD_EXPORT_FUNCTION void PxdVolumeSetInt(PxdVolume volume, PxdVolumeProperty property, PxInt value);
PXD_EXPORT_FUNCTION PxInt PxdVolumeGetInt(PxdVolume volume, PxdVolumeProperty property);

PXD_EXPORT_FUNCTION void PxdVolumeSetFloat(PxdVolume volume, PxdVolumeProperty property, PxFloat value);
PXD_EXPORT_FUNCTION PxFloat PxdVolumeGetFloat(PxdVolume volume, PxdVolumeProperty property);

PXD_EXPORT_FUNCTION void PxdVolumeSetProperty(PxdVolume volume, PxdVolumeProperty property, void* data, size_t size);
PXD_EXPORT_FUNCTION void PxdVolumeGetProperty(PxdVolume volume, PxdVolumeProperty property, void* data, size_t size);

PXD_EXPORT_FUNCTION void PxdVolumeSetVec(PxdVolume Volume, PxdVolumeProperty property, PxdVector v);
PXD_EXPORT_FUNCTION PxdVector PxdVolumeGetVec(PxdVolume Volume, PxdVolumeProperty property);

PXD_EXPORT_FUNCTION void PxdVolumeSetQuat(PxdVolume Volume, PxdVolumeProperty property, PxdQuaternion q);
PXD_EXPORT_FUNCTION PxdQuaternion PxdVolumeGetQuat(PxdVolume Volume, PxdVolumeProperty property);

PXD_EXPORT_FUNCTION void PxdVolumeSetTransform(PxdVolume Volume, PxdVolumeProperty property, PxdTransform t);
PXD_EXPORT_FUNCTION PxdTransform PxdVolumeGetTransform(PxdVolume Volume, PxdVolumeProperty property);


/*!
@}
*/

/************************************************************************/
/* Broad phase control                                                  */
/************************************************************************/

/*!
After the broad phase has been updated, an array of
PxdBroadPhaseOverlapReport structs can be retrieved for created and 
destroyed overlaps.
*/
typedef struct PxdBroadPhaseOverlap_
{
	/*!
	Volume identifier for one of the volumes in this overlap.
	This is only available if PXD_BROAD_PHASE_REPORT_IDENTIFIER
	flag is set. Undefined otherwise.
	*/
	PxdVolume volume0;
	/*!
	Volume identifier for the other volume in this overlap.
	This is only available if PXD_BROAD_PHASE_REPORT_IDENTIFIER
	flag is set. Undefined otherwise.
	*/
	PxdVolume volume1;
	/*!
	User data for one of the volumes in this overlap.
	This is only available if PXD_BROAD_PHASE_REPORT_USER_DATA
	flag is set. Undefined otherwise.
	*/
	void* userdata0;
	/*!
	User data for the other volume in this overlap.
	This is only available if PXD_BROAD_PHASE_REPORT_USER_DATA
	flag is set. Undefined otherwise.
	*/
	void* userdata1;
} PxdBroadPhaseOverlap;

/*!
Per broad phase properties/flags
*/
typedef enum PxdBroadPhaseProperty_ 
{
	/*! 
	Flag to include identifier in overlap report (1 PxInt - flag)
	*/
	PXD_BROAD_PHASE_REPORT_IDENTIFIER,
	/*! 
	Flag to include user data in overlap report (1 PxInt - flag)
	*/	
	PXD_BROAD_PHASE_REPORT_USER_DATA,
	/*! 
	Flag to report created overlaps (1 PxInt - flag)
	*/
	PXD_BROAD_PHASE_REPORT_CREATED_OVERLAPS,
	/*! 
	Flag to report destroyed overlaps (1 PxInt - flag)
	*/
	PXD_BROAD_PHASE_REPORT_DESTROYED_OVERLAPS,
	/*! 
	Number of created overlaps during last update (1 PxInt, read-only)
	*/
	PXD_BROAD_PHASE_CREATED_OVERLAP_COUNT,
	/*! 
	An array of PxdBroadPhaseOverlap structs representing
	the created overlaps. This data is only available if 
	PXD_BROAD_PHASE_REPORT_CREATED_OVERLAPS flag is set.
	*/
	PXD_BROAD_PHASE_CREATED_OVERLAPS,			
	/*! 
	Number of destroyed overlaps during last update (1 PxInt, read-only)
	*/
	PXD_BROAD_PHASE_DESTROYED_OVERLAP_COUNT,
	/*! 
	An array of PxdBroadPhaseOverlap structs representing
	the destroyed overlaps. This data is only available if 
	PXD_BROAD_PHASE_REPORT_DESTROYED_OVERLAPS flag is set.
	*/
	PXD_BROAD_PHASE_DESTROYED_OVERLAPS,
	/*
	Defines the up-axis for the broad phase. (1 PxInt, 0 = X, 1 = Y, 2 = Z)
	This is only used with the multi SAP context.
	*/
	PXD_BROAD_PHASE_UP_AXIS,
	/*
	Defines the number of grid-cells along X. (1 PxInt)
	Must be a power of 2.
	This is only used with the multi SAP context.
	*/
	PXD_BROAD_PHASE_X_GRID_CELLS_COUNT,
	/*
	Defines the number of grid-cells along Y. (1 PxInt)
	Must be a power of 2.
	This is only used with the multi SAP context.
	*/
	PXD_BROAD_PHASE_Y_GRID_CELLS_COUNT,
	/*
	Defines the size of the world along X.
	This is only used with the multi SAP context.
	*/
	PXD_BROAD_PHASE_X_SIZE,
	/*
	Defines the size of the world along Y.
	This is only used with the multi SAP context.
	*/
	PXD_BROAD_PHASE_Y_SIZE,
	/*
	Defines the origin of the broadphase
	This is only used with the SPU multi SAP context.
	*/
	PXD_BROAD_PHASE_ORIGIN,
} PxdBroadPhaseProperty;

/*!
Start an update pass of the broad phase for given context.
When the task is finished (PxdTaskWait returns) overlap reports can be
fetched.

\param context
Context identifier
\return 
Returns a task identifier. Zero on failure.
*/
PXD_EXPORT_FUNCTION PxdTask PxdBroadPhaseUpdate(PxdContext context);

/*!
Get the number of new overlap reports of given type (created/removed).
Only valid to call after a successful broad phase update until the beginnig 
of next update.

\param context
Context identifier
\param overlapType
Type of overlap reports (PXD_BROAD_PHASE_CREATED_OVERLAP_COUNT/
PXD_BROAD_PHASE_DESTROYED_OVERLAP_COUNT)
\return
Returns the number of overlap reports that are fetchable.
*/
PXD_EXPORT_FUNCTION PxUInt PxdBroadPhaseGetOverlapCount(PxdContext context, PxdBroadPhaseProperty overlapType);

/*!
Fetch overlap reports of given type (created/removed).
The caller is responsible for allocating a buffer for overlaps of sufficient
size to fit at least the number of overlaps reported by 
PxdBroadPhaseGetOverlapCount.

\param context
Context identifier
\param overlapType
Type of overlap reports (PXD_BROAD_PHASE_CREATED_OVERLAP/
PXD_BROAD_PHASE_DESTROYED_OVERLAP)
\param overlaps
Preallocated buffer to fill with overlap reports
\return
Returns true on successful filling of the buffer, false on error.
*/
PXD_EXPORT_FUNCTION PxBool PxdBroadPhaseGetOverlaps(PxdContext context, PxdBroadPhaseProperty overlapType, PxdBroadPhaseOverlap* overlaps);

/*!
\addtogroup Broad phase control property set/get
@{
*/
PXD_EXPORT_FUNCTION void PxdBroadPhaseSetInt(PxdContext context, PxdBroadPhaseProperty property, PxInt value);
PXD_EXPORT_FUNCTION PxInt PxdBroadPhaseGetInt(PxdContext context, PxdBroadPhaseProperty property);

PXD_EXPORT_FUNCTION void PxdBroadPhaseSetFloat(PxdContext context, PxdBroadPhaseProperty property, PxFloat value);
PXD_EXPORT_FUNCTION PxFloat PxdBroadPhaseGetFloat(PxdContext context, PxdBroadPhaseProperty property);

PXD_EXPORT_FUNCTION void PxdBroadPhaseSetProperty(PxdContext context, PxdBroadPhaseProperty property, void* value, size_t size);
PXD_EXPORT_FUNCTION void PxdBroadPhaseGetProperty(PxdContext context, PxdBroadPhaseProperty property, void* value, size_t size);

PXD_EXPORT_FUNCTION void PxdBroadPhaseSetVec(PxdContext context, PxdBroadPhaseProperty property, PxdVector v);
PXD_EXPORT_FUNCTION PxdVector PxdBroadPhaseGetVec(PxdContext context, PxdBroadPhaseProperty property);

PXD_EXPORT_FUNCTION void PxdBroadPhaseSetQuat(PxdContext context, PxdBroadPhaseProperty property, PxdQuaternion q);
PXD_EXPORT_FUNCTION PxdQuaternion PxdBroadPhaseGetQuat(PxdContext context, PxdBroadPhaseProperty property);

PXD_EXPORT_FUNCTION void PxdBroadPhaseSetTransform(PxdContext context, PxdBroadPhaseProperty property, PxdTransform t);
PXD_EXPORT_FUNCTION PxdTransform PxdBroadPhaseGetTransform(PxdContext context, PxdBroadPhaseProperty property);


/*!
@}
*/

#ifdef __cplusplus
}
#endif

#endif

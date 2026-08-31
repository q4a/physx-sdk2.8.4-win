#ifndef PXS_CELLSPUTHRESHOLDTABLE_SHARED_H
#define PXS_CELLSPUTHRESHOLDTABLE_SHARED_H

#define THRESHOLD_STREAM_MEMORY_MANAGER_USER_TYPE (24)

#define THRESHOLD_STREAM_NEW_GROUP_MARKER (0xffffffff)
#define THRESHOLD_STREAM_END_MARKER (0xfffffffe)
#define THRESHOLD_STREAM_LINK_MARKER (0xfffffffd)

struct CellThresholdStreamItem {
	uint32_t body0_groupMarker_endMarker_or_linkMaker;
	uint32_t body1_or_nextBuffer;
	float normalForce;
	float threshold;
};

#define	THRESHOLD_STREAM_ITEMS_PER_BUFFER (CELL_MEM_SLAB_SIZE/sizeof(CellThresholdStreamItem))
//#define	THRESHOLD_STREAM_ITEMS_PER_BUFFER (16)

#endif  // PXS_CELLSPUTHRESHOLDTABLE_SHARED_H

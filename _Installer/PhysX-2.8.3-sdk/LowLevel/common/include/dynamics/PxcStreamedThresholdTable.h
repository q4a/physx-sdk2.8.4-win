#ifndef PXS_STREAMEDTHRESHOLDTABLE_H
#define PXS_STREAMEDTHRESHOLDTABLE_H

#include "PxcBaseThresholdTable.h"

#ifdef __CELLOS_LV2__
#include "dynamics/CellSPUThresholdTable.shared.h"
#endif

template <class BodyPtr>
class PxcStreamedThresholdTable : public PxcBaseThresholdTable<BodyPtr>
{
private:
	struct AddForceStreamItem
	{
		BodyPtr body0, body1;
		PxFloat force;
		PxFloat threshold;
	};

	PxcArray<AddForceStreamItem> mAddForceStream;

	BodyPtr mCurrentBody0;
	BodyPtr mCurrentBody1;
	float mCurrentNormalForce;
	float mCurrentThreshold;
	bool mIsEmpty;

public:

	void init(PxU32 bodyPairCount, PxU32 addForceQueueSize)
	{
		mAddForceStream.reserve(addForceQueueSize);
		mAddForceStream.clear();

		mCurrentBody0 = NULL;
		mCurrentBody1 = NULL;
		mCurrentNormalForce = 0.0f;
		mCurrentThreshold = 0.0f;
		mIsEmpty = true;

		PxcBaseThresholdTable<BodyPtr>::initTable(bodyPairCount);
	}

	// called in last iteration of solver. Solver only needs to build force table.

	void addForce(BodyPtr body0, BodyPtr body1, PxFloat force, PxFloat threshold)
	{

		if (mCurrentBody0 == body0 && mCurrentBody1 == body1) {
			
			mCurrentNormalForce+=force;
			mCurrentThreshold = threshold;

		} else {
			
			flush();
	
			mCurrentBody0 = body0;
			mCurrentBody1 = body1;
			mCurrentNormalForce = force;
			mCurrentThreshold = threshold;

			mIsEmpty = false;

		}
	}

	void flush()
	{
		if (!mIsEmpty) {
			AddForceStreamItem streamItem = {mCurrentBody0, mCurrentBody1, mCurrentNormalForce, mCurrentThreshold};
			mAddForceStream.insert(streamItem);
		}
	}

	template <class BodyPair>
	void getThresholdPairs(PxcArray<BodyPair> &accumulator)
	{

		for(PxU32 i=0;i<mAddForceStream.getSize();i++)
		{
			AddForceStreamItem& item = mAddForceStream[i];
			PxcStreamedThresholdTable<BodyPtr>::addForceInternal(item.body0, item.body1, item.force, item.threshold);
		}
		mAddForceStream.clear();

		PxcStreamedThresholdTable<BodyPtr>::getThresholdPairsInternal(accumulator);
	}

#ifdef __CELLOS_LV2__

	template <class BodyPair>
	void getSPUThresholdPairs(PxcArray<BodyPair> &accumulator, CellThresholdStreamItem* stream, PxU32 bodyPairCount)
	{
		if (stream != NULL) {

			PxcBaseThresholdTable<BodyPtr>::initTable(bodyPairCount);

			bool done = false;
			while(!done) {
				if (stream->body0_groupMarker_endMarker_or_linkMaker == THRESHOLD_STREAM_NEW_GROUP_MARKER) {

					//printf("groupEndMarker\n");
					getThresholdPairsInternal(accumulator);				
					stream++;
					PxcBaseThresholdTable<BodyPtr>::initTable(bodyPairCount);

				} else if (stream->body0_groupMarker_endMarker_or_linkMaker == THRESHOLD_STREAM_END_MARKER) {

					//printf("endMarker\n");
					done = true;

				} else if (stream->body0_groupMarker_endMarker_or_linkMaker == THRESHOLD_STREAM_LINK_MARKER) {

					//printf("linkMarker %x\n", stream->body1_or_nextBuffer);
					stream = (CellThresholdStreamItem*)stream->body1_or_nextBuffer;

				} else {

					//printf("item: %x %x %f %f\n", stream->body0_groupMarker_endMarker_or_linkMaker, stream->body1_or_nextBuffer, stream->normalForce, stream->threshold);
					addForceInternal((BodyPtr)(stream->body0_groupMarker_endMarker_or_linkMaker), (BodyPtr)(stream->body1_or_nextBuffer), stream->normalForce, stream->threshold);
					stream++;

				}
			}
		}

	}
#endif

};

#endif

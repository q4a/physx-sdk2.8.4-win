#ifndef PXS_BROADPHASE_ENDPOINTARRAY_H
#define PXS_BROADPHASE_ENDPOINTARRAY_H

struct PxsBpEndPoint
{
	PxsBpValue mPosition; //floating point value encoded as a sortable integer.
	PxU32 mSmallVolume; //top bit indicates min/max

	PXD_FORCE_INLINE PxU32 getSmallVolume() const
	{
		return mSmallVolume & ~0x80000000;
	}

	PXD_FORCE_INLINE void setSmallVolume(PxU32 a)
	{
		PXN_ASSERT(!(a & 0x80000000));

		mSmallVolume = a | (mSmallVolume & 0x80000000);
	}


	PXD_FORCE_INLINE void setMin()
	{
		mSmallVolume |= 0x80000000;
	}

	PXD_FORCE_INLINE void setMax()
	{
		mSmallVolume &= ~0x80000000;
	}

	PXD_FORCE_INLINE PxIBool isMin() const
	{
		return mSmallVolume & 0x80000000;
	}

	PXD_FORCE_INLINE PxIBool isMax() const
	{
		return !isMin();
	}
};


class PxsBroadPhaseEndPointArray
{
public:

	PxsBroadPhaseEndPointArray(PxsBroadPhasePairMapBase<PxsBpSmallVolume> &pairMap, PxcArray<PxsBpSmallVolume> &smallVolumes);
	~PxsBroadPhaseEndPointArray();

	template<const PxU32 AX> void updateEndPointsAx(PxU32 smallIdx, PxsBpValue newMin, PxsBpValue newMax); 

	void dump();
	void dumpPairBucketCounts();
	bool isConsistent();

	//note: the arrays are destroyed/changed and contain endpoint indexes
	void purgeEndPoints(PxU32 ax, PxcArray<PxU32> &endPointIndices);
	//assumes other two axes have already been purged.
#ifdef PXS_ENABLE_OPT_BP_VOLUME_DELETION
	void purgeEndPointsAndPairs(PxU32 ax, PxcArray<PxU32> &endPointIndices, bool fastMassiveDeletionPath);
#else
	void purgeEndPointsAndPairs(PxU32 ax, PxcArray<PxU32> &endPointIndices);
#endif

	//note: the arrays are destroyed/changed and contain endpoints for insert
	void insertEndPoints(PxU32 ax, PxcArray<PxsBpEndPoint> &endPoints);
	//assumes other two axes have already been inserted
	void insertEndPointsAndPairs(PxU32 ax, PxcArray<PxsBpEndPoint> &endPoints);

//private:

	PxcArray<PxsBpEndPoint>		mEndPoints[3];
	
	PxsBroadPhasePairMapBase<PxsBpSmallVolume>	&mPairMap;
	PxcArray<PxsBpSmallVolume>	&mSmallVolumes;

	bool checkEndPointsSorted(PxU32 ax);
	bool checkPairsBrute();
	bool checkEndPointIndices();
	bool checkDuplicatePairs();


	//Axis specific version with template?
	template<const PxU32 AX> void minMovingUp(PxU32 epIdx, PxsBpValue newMinValue, PxsBpValue newMaxValue,
		PxsBpValue oldMinValue, PxsBpValue oldMaxValue);
	template<const PxU32 AX> void minMovingDown(PxU32 epIdx, PxsBpValue newMinValue, PxsBpValue newMaxValue);
	template<const PxU32 AX> void maxMovingUp(PxU32 epIdx, PxsBpValue newMinValue, PxsBpValue newMaxValue);
	template<const PxU32 AX> void maxMovingDown(PxU32 epIdx, PxsBpValue newMinValue, PxsBpValue newMaxValue,
		PxsBpValue oldMinValue, PxsBpValue oldMaxValue);

	PXD_FORCE_INLINE bool intersects(PxU32 ax, PxU32 aIdx, PxU32 bIdx)
	{
		static PxU32 otherAx[3][2] = {{1,2}, {0,2}, {0,1}};
		//TODO: look at this and see if optimizer is smart.
		PxU32 axA = otherAx[ax][0];
		PxU32 axB = otherAx[ax][1];
		// PT: could use this, and could also extract it from the loop:
//		PxU32 axA = (1  << ax) & 3;
//		PxU32 axB = (1  << axA) & 3;

			if(
			(mSmallVolumes[aIdx].mMaxEPs[axA] < mSmallVolumes[bIdx].mMinEPs[axA]) ||
			(mSmallVolumes[aIdx].mMinEPs[axA] > mSmallVolumes[bIdx].mMaxEPs[axA]) ||

			(mSmallVolumes[aIdx].mMaxEPs[axB] < mSmallVolumes[bIdx].mMinEPs[axB]) ||
			(mSmallVolumes[aIdx].mMinEPs[axB] > mSmallVolumes[bIdx].mMaxEPs[axB])
			)
			return false;

		return true;
	}

	template<const PxU32 AX> PXD_FORCE_INLINE bool intersectsSV(PxU32 aIdx, PxU32 bIdx){ PXN_ASSERT(0); }

	//Comparitors

	class PxsEpRemoveComparitor
	{
	public:
		PxsEpRemoveComparitor(){}

		PXD_FORCE_INLINE bool greater(PxU32 a, PxU32 b)
		{
			return a > b;
		}
		PXD_FORCE_INLINE bool greaterEqual(PxU32 a, PxU32 b)
		{
			return a >= b;
		}

		PXD_FORCE_INLINE bool lessEqual(PxU32 a, PxU32 b)
		{
			return a <= b;
		}
	};

	//we sort the items for insert into reverse order.
	class PxsEpInsertComparitor
	{
	public:
		PxsEpInsertComparitor(){}

		PXD_FORCE_INLINE bool greater(PxsBpEndPoint &a, PxsBpEndPoint &b)
		{
			if(a.mPosition == b.mPosition)
			{
				//max should always come before min, since we insert backwards.
				return a.isMin() && b.isMax();
			}
			else
				return a.mPosition < b.mPosition;
		}

		PXD_FORCE_INLINE bool less(PxsBpEndPoint &a, PxsBpEndPoint &b)
		{
			return !greaterEqual(a, b);
		}

		PXD_FORCE_INLINE bool greaterEqual(PxsBpEndPoint &a, PxsBpEndPoint &b)
		{
			if(a.mPosition == b.mPosition)
			{
				//min should always come before max, since we insert backwards.
				//perf shouldnt matter here(we could probably simplify this) as greaterEqual is only
				//currently used to check validity.
				return ((a.isMin() && b.isMin()) || (a.isMax() && b.isMax()) || (a.isMin() && b.isMax()));
			}
			else
				return a.mPosition < b.mPosition;
		}

		PXD_FORCE_INLINE bool lessEqual(PxsBpEndPoint &a, PxsBpEndPoint &b)
		{
			return !greater(a, b);
		}
	};

	PxcArrayQuickSort<PxsBpEndPoint, PxsEpInsertComparitor>  mEpInsertSorter;
	PxcArrayQuickSort<PxU32, PxsEpRemoveComparitor>  mEpRemoveSorter;

	//used for open volume tracking on insert/delete:

	PxcBitMap mTempBitMap1;
	PxcBitMap mTempBitMap2;
	
};


template<> PXD_FORCE_INLINE bool PxsBroadPhaseEndPointArray::intersectsSV<0>(PxU32 aIdx, PxU32 bIdx)
{
		if(
		(mSmallVolumes[aIdx].mMaxEPs[1] < mSmallVolumes[bIdx].mMinEPs[1]) |
		(mSmallVolumes[aIdx].mMinEPs[1] > mSmallVolumes[bIdx].mMaxEPs[1]) |

		(mSmallVolumes[aIdx].mMaxEPs[2] < mSmallVolumes[bIdx].mMinEPs[2]) |
		(mSmallVolumes[aIdx].mMinEPs[2] > mSmallVolumes[bIdx].mMaxEPs[2])
		)
		return false;

	return true;
}

template<> PXD_FORCE_INLINE bool PxsBroadPhaseEndPointArray::intersectsSV<1>(PxU32 aIdx, PxU32 bIdx)
{
		if(
		(mSmallVolumes[aIdx].mMaxEPs[0] < mSmallVolumes[bIdx].mMinEPs[0]) |
		(mSmallVolumes[aIdx].mMinEPs[0] > mSmallVolumes[bIdx].mMaxEPs[0]) |

		(mSmallVolumes[aIdx].mMaxEPs[2] < mSmallVolumes[bIdx].mMinEPs[2]) |
		(mSmallVolumes[aIdx].mMinEPs[2] > mSmallVolumes[bIdx].mMaxEPs[2])
		)
		return false;

	return true;
}

template<> PXD_FORCE_INLINE bool PxsBroadPhaseEndPointArray::intersectsSV<2>(PxU32 aIdx, PxU32 bIdx)
{
		if(
		(mSmallVolumes[aIdx].mMaxEPs[0] < mSmallVolumes[bIdx].mMinEPs[0]) |
		(mSmallVolumes[aIdx].mMinEPs[0] > mSmallVolumes[bIdx].mMaxEPs[0]) |

		(mSmallVolumes[aIdx].mMaxEPs[1] < mSmallVolumes[bIdx].mMinEPs[1]) |
		(mSmallVolumes[aIdx].mMinEPs[1] > mSmallVolumes[bIdx].mMaxEPs[1])
		)
		return false;

	return true;
}

template<const PxU32 AX> PXD_FORCE_INLINE
void PxsBroadPhaseEndPointArray::minMovingUp(PxU32 epIdx, PxsBpValue newMinValue, PxsBpValue newMaxValue,
		PxsBpValue oldMinValue, PxsBpValue oldMaxValue)
{
	PxU32 newIdx = epIdx;
	PxU32 lastIdx = newIdx;

	if(newMinValue > mEndPoints[AX][newIdx + 1].mPosition)
	{
		PxsBpEndPoint savedEP = mEndPoints[AX][epIdx];

		PxU32 smallVolIdx = mEndPoints[AX][epIdx].getSmallVolume();
		PxU32 group = mSmallVolumes[smallVolIdx].mGroup;
		//Min moves up passing maxes, which destroys overlaps.

		//Scan for insertion point (we have sentinals to avoid going off the end)
		//we rely on mins moving up to not go past equal values. For example the
		// sentinal on delete.
		do
		{
			newIdx++;

			PxU32 newSmallVolIdx = mEndPoints[AX][newIdx].getSmallVolume();

			if(mEndPoints[AX][newIdx].isMax())
			{
				PxU32 newGroup = mSmallVolumes[newSmallVolIdx].mGroup;

				if(group!=newGroup)
				{
					if(intersectsSV<AX>(smallVolIdx, newSmallVolIdx))
					{
						PxU32 adjMinIndex = mSmallVolumes[newSmallVolIdx].mMinEPs[AX];
						PxsBpValue adjMinValue = mEndPoints[AX][adjMinIndex].mPosition;

						//were we intersecting on this axes before?
						if(oldMaxValue >= adjMinValue)
						{
							mPairMap.destroyBpPair(smallVolIdx, newSmallVolIdx);
						}
					}
				}

				//move endpoint down
				mSmallVolumes[newSmallVolIdx].mMaxEPs[AX] = lastIdx;
			}
			else
			{
				mSmallVolumes[newSmallVolIdx].mMinEPs[AX] = lastIdx;
			}

			mEndPoints[AX][lastIdx] = mEndPoints[AX][newIdx];

			lastIdx = newIdx;
		}while(newMinValue > mEndPoints[AX][newIdx + 1].mPosition);

		mEndPoints[AX][newIdx] = savedEP;
		mSmallVolumes[smallVolIdx].mMinEPs[AX] = newIdx;
	}
}

template<const PxU32 AX> PXD_FORCE_INLINE
void PxsBroadPhaseEndPointArray::minMovingDown(PxU32 epIdx, PxsBpValue newMinValue, PxsBpValue newMaxValue)
{
	PxU32 newIdx = epIdx;
	PxU32 lastIdx = newIdx;

	if(newMinValue <= mEndPoints[AX][newIdx - 1].mPosition)
	{
		PxsBpEndPoint savedEP = mEndPoints[AX][epIdx];

		PxU32 smallVolIdx = mEndPoints[AX][epIdx].getSmallVolume();
		PxU32 group = mSmallVolumes[smallVolIdx].mGroup;
		//Min moves down passing maxes, which creates overlaps.

		//Scan for insertion point, (we have sentinals to avoid going off the end))
		//TODO: what about infinite volumes and going past these?
		do
		{
			newIdx--;

			PxU32 newSmallVolIdx = mEndPoints[AX][newIdx].getSmallVolume();

			if(mEndPoints[AX][newIdx].isMax())
			{
				PxU32 newGroup = mSmallVolumes[newSmallVolIdx].mGroup;

				if(group!=newGroup)
				{
					if(intersectsSV<AX>(smallVolIdx, newSmallVolIdx))
					{
						PxU32 adjMinIndex = mSmallVolumes[newSmallVolIdx].mMinEPs[AX];
						PxsBpValue adjMinValue = mEndPoints[AX][adjMinIndex].mPosition;

						if(newMaxValue >= adjMinValue)
							mPairMap.createBpPair(smallVolIdx, newSmallVolIdx);
					}
				}

				mSmallVolumes[newSmallVolIdx].mMaxEPs[AX] = lastIdx;
			}
			else
			{
				mSmallVolumes[newSmallVolIdx].mMinEPs[AX] = lastIdx;
			}
		
			mEndPoints[AX][lastIdx] = mEndPoints[AX][newIdx];

			lastIdx = newIdx;
		}while(newMinValue <= mEndPoints[AX][newIdx - 1].mPosition);

			//place moved endpoint (should we skip if no move?)
		mEndPoints[AX][newIdx] = savedEP;
		mSmallVolumes[smallVolIdx].mMinEPs[AX] = newIdx;
	}

}

template<const PxU32 AX> PXD_FORCE_INLINE
void PxsBroadPhaseEndPointArray::maxMovingUp(PxU32 epIdx, PxsBpValue newMinValue, PxsBpValue newMaxValue)
{
	PxU32 newIdx = epIdx;
    PxU32 lastIdx = newIdx;
	
	if(newMaxValue >= mEndPoints[AX][newIdx + 1].mPosition)
	{
		PxsBpEndPoint savedEP = mEndPoints[AX][epIdx];

		PxU32 smallVolIdx = mEndPoints[AX][epIdx].getSmallVolume();
		PxU32 group = mSmallVolumes[smallVolIdx].mGroup;
		//Max moves up passing mins, which creates overlaps.

		//Scan for insertion point (we have sentinals to avoid going off the end)
		//We rely on max moving up not going past equal values for delete(so it doesnt go past the sentinal).
		do
		{
			newIdx++;

			PxU32 newSmallVolIdx = mEndPoints[AX][newIdx].getSmallVolume();

			if(mEndPoints[AX][newIdx].isMin())
			{
				
				PxU32 newGroup = mSmallVolumes[newSmallVolIdx].mGroup;

				if(group!=newGroup)
				{
					if(intersectsSV<AX>(smallVolIdx, newSmallVolIdx))
					{

						PxU32 adjMaxIndex = mSmallVolumes[newSmallVolIdx].mMaxEPs[AX];
						PxsBpValue adjMaxValue = mEndPoints[AX][adjMaxIndex].mPosition;

						if(newMinValue <= adjMaxValue)
							mPairMap.createBpPair(smallVolIdx, newSmallVolIdx);
					}
				}

				//move endpoint down
				mSmallVolumes[newSmallVolIdx].mMinEPs[AX] = lastIdx;
			}
			else
			{
				mSmallVolumes[newSmallVolIdx].mMaxEPs[AX] = lastIdx;
			}

			mEndPoints[AX][lastIdx] = mEndPoints[AX][newIdx];
		
			lastIdx = newIdx;
		}while(newMaxValue >= mEndPoints[AX][newIdx + 1].mPosition);

			//place moved endpoint (should we skip if no move?)
		mEndPoints[AX][newIdx] = savedEP;
		mSmallVolumes[smallVolIdx].mMaxEPs[AX] = newIdx;
	}
}

template<const PxU32 AX> PXD_FORCE_INLINE
void PxsBroadPhaseEndPointArray::maxMovingDown(PxU32 epIdx, PxsBpValue newMinValue, PxsBpValue newMaxValue,
		PxsBpValue oldMinValue, PxsBpValue oldMaxValue)
{
	PxU32 newIdx = epIdx;
	PxU32 lastIdx = newIdx;

	if(newMaxValue < mEndPoints[AX][newIdx - 1].mPosition)
	{
		PxsBpEndPoint savedEP = mEndPoints[AX][epIdx];

		PxU32 smallVolIdx = mEndPoints[AX][epIdx].getSmallVolume();
		PxU32 group = mSmallVolumes[smallVolIdx].mGroup;
		//max moves down passing mins, which destroys overlaps.

		//Scan for insertion point (we have sentinals to avoid going off the end)
		//TODO: what about infinit volumes and going past the sentinals.
		do
		{
			newIdx--;

			PxU32 newSmallVolIdx = mEndPoints[AX][newIdx].getSmallVolume();

			if(mEndPoints[AX][newIdx].isMin())
			{
				PxU32 newGroup = mSmallVolumes[newSmallVolIdx].mGroup;

				if(group!=newGroup)
				{
					if(intersectsSV<AX>(smallVolIdx, newSmallVolIdx))
					{
						PxU32 adjMaxIndex = mSmallVolumes[newSmallVolIdx].mMaxEPs[AX];
						PxsBpValue adjMaxValue = mEndPoints[AX][adjMaxIndex].mPosition;

						//were we intersecting on this axes before?
						if(oldMinValue <= adjMaxValue)
						{
							mPairMap.destroyBpPair(smallVolIdx, newSmallVolIdx);
						}
					}
				}
			
				//move endpoint up
				mSmallVolumes[newSmallVolIdx].mMinEPs[AX] = lastIdx;
			}
			else
			{
				mSmallVolumes[newSmallVolIdx].mMaxEPs[AX] = lastIdx;
			}

			mEndPoints[AX][lastIdx] = mEndPoints[AX][newIdx];

			lastIdx = newIdx;

		}while(newMaxValue < mEndPoints[AX][newIdx - 1].mPosition);

		//place moved endpoint (should we skip if no move?)
		mEndPoints[AX][newIdx] = savedEP;
		mSmallVolumes[smallVolIdx].mMaxEPs[AX] = newIdx;
	}
}


template<const PxU32 AX>
PXD_FORCE_INLINE void PxsBroadPhaseEndPointArray::updateEndPointsAx(PxU32 smallIdx, PxsBpValue newMin, PxsBpValue newMax)
{

	PxU32 minEP = mSmallVolumes[smallIdx].mMinEPs[AX];
	PxU32 maxEP = mSmallVolumes[smallIdx].mMaxEPs[AX];

	PxsBpValue oldMin = mEndPoints[AX][minEP].mPosition;
	PxsBpValue oldMax = mEndPoints[AX][maxEP].mPosition;

	if(newMin < oldMin)
	{
		minMovingDown<AX>( minEP, newMin, newMax);
		
		minEP = mSmallVolumes[smallIdx].mMinEPs[AX]; //must update, since it gets moved
		maxEP = mSmallVolumes[smallIdx].mMaxEPs[AX];
		mEndPoints[AX][minEP].mPosition = newMin;

		if(newMax < oldMax)
			maxMovingDown<AX>( maxEP, newMin, newMax, oldMin, oldMax);
		else if(newMax > oldMax)
			maxMovingUp<AX>( maxEP, newMin, newMax);

		maxEP = mSmallVolumes[smallIdx].mMaxEPs[AX];
		mEndPoints[AX][maxEP].mPosition = newMax;
	}
	else if(newMin > oldMin)
	{
		if(newMax > oldMax)
			maxMovingUp<AX>( maxEP, newMin, newMax);
		else if(newMax < oldMax)
			maxMovingDown<AX>( maxEP, newMin, newMax, oldMin, oldMax);

		minEP = mSmallVolumes[smallIdx].mMinEPs[AX]; //must update, since it gets moved
		maxEP = mSmallVolumes[smallIdx].mMaxEPs[AX];
		mEndPoints[AX][maxEP].mPosition = newMax;

		minMovingUp<AX>( minEP, newMin, newMax, oldMin, oldMax);
		
		minEP = mSmallVolumes[smallIdx].mMinEPs[AX]; //must update, since it gets moved
		mEndPoints[AX][minEP].mPosition = newMin;
	}
	else
	{//no change in min

		if(newMax > oldMax)
			maxMovingUp<AX>( maxEP, newMin, newMax);
		else if(newMax < oldMax)
			maxMovingDown<AX>( maxEP, newMin, newMax, oldMin, oldMax);

		maxEP = mSmallVolumes[smallIdx].mMaxEPs[AX];//must update, since it gets moved
		mEndPoints[AX][maxEP].mPosition = newMax;
	}
}


#endif


#ifndef NX_PHYSICS_COMPARTMENT
#define NX_PHYSICS_COMPARTMENT


#include "NxCompartment.h"
#include "Allocateable.h"
#include "ReadWriteMutex.h"

class AsyncScene;

class NpCompartment : public NxCompartment, public NxFoundation::NxAllocateable, public ReadWriteMutex
	{
	public:
	NpCompartment(AsyncScene * s, NxCompartmentType t, NxU32 d, NxReal c, NxU32 pow, NxU32 f, NxReal ts): 
		managedScene(s),
		type(t),
		deviceCode(d),
		cellSize(c),
		gridPower(pow),
		flags(f),
		timeScale(ts)
		{}

	virtual NxCompartmentType getType() const { return type; }
	virtual NxU32 getDeviceCode() const { return deviceCode; }
	virtual NxReal getGridHashCellSize() const { return cellSize; }
	virtual NxU32 gridHashTablePower() const { return gridPower; }
	virtual void setFlags(NxU32 f);
	virtual NxU32 getFlags() const { return flags; }
	virtual void setTimeScale(NxReal);
	virtual NxReal getTimeScale() const { return timeScale; }
	virtual void setTiming(NxReal maxTimestep, NxU32 maxIter, NxTimeStepMethod method);
	virtual void getTiming(NxReal& maxTimestep, NxU32& maxIter, NxTimeStepMethod& method, NxU32* numSubSteps) const;

	virtual	bool checkResults(bool block);
	virtual	bool fetchResults(bool block);


	NX_INLINE NxCompartmentType getTypeFast() const { return type; }
	NX_INLINE NxU32 getDeviceCodeFast() const { return deviceCode; }
	NX_INLINE NxReal getCellSizeFast() const { return cellSize; }
	NX_INLINE NxU32 getGridPowerFast() const { return gridPower; }
	NX_INLINE NxReal getTimeScaleFast() const { return timeScale; }
	NX_INLINE AsyncScene * getManagedScene() const { return managedScene; }
	//CA: Added for PS3 HSM support
	NvScene* getManagedNvScene() const;

	virtual bool saveToDesc(NxCompartmentDesc& desc) const
		{
		desc.deviceCode = deviceCode;
		desc.gridHashCellSize = cellSize;
		desc.gridHashTablePower = gridPower;
		desc.type = type;
		desc.timeScale = timeScale;
		return true;
		}

	void setUserNotify(NxUserNotify* callback);

	void sendToVRD(bool create, bool fromCtor);
	void removeFromVRD();

	private:
	NxCompartmentType	type;
	NxU32				deviceCode;
	NxReal				cellSize;
	NxU32				gridPower;
	NxU32				flags;
	NxReal				timeScale;
	AsyncScene *		managedScene;	//the managed scene
	};

#endif

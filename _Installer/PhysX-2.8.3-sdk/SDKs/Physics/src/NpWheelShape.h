#ifndef NX_COLLISION_NP_WHEELSHAPE
#define NX_COLLISION_NP_WHEELSHAPE
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "NxWheelShape.h"
#include "NxShape.h"
#include "MacroNpShape.h"

class NxWheelShapeDesc;


class NpWheelShape : public NxWheelShape, public NpShape, public NxFoundation::NxAllocateable
	{
	
	NPSHAPE_DECLARATION(WheelShape)

	public:
	virtual	void saveToDesc(NxWheelShapeDesc&)		const;

	virtual	void setRadius(NxReal);
	virtual	void setSuspensionTravel(NxReal);

	virtual	NxReal getRadius() const;
	virtual	NxReal getSuspensionTravel() const;

	//simulation constants:
	virtual	void setSuspension(NxSpringDesc);
	virtual	void setLongitudalTireForceFunction(NxTireFunctionDesc);
	virtual	void setLateralTireForceFunction(NxTireFunctionDesc);
	virtual	void setInverseWheelMass(NxReal);
	virtual	void setWheelFlags(NxU32);

	virtual	NxSpringDesc getSuspension() const;
	virtual	NxTireFunctionDesc getLongitudalTireForceFunction() const;
	virtual	NxTireFunctionDesc getLateralTireForceFunction() const;
	virtual	NxReal	getInverseWheelMass() const;
	virtual	NxU32	getWheelFlags() const;

	//dynamic inputs:
	virtual	void setMotorTorque(NxReal);
	virtual	void setBrakeTorque(NxReal);
	virtual	void setSteerAngle(NxReal);

	virtual	NxReal getMotorTorque() const;
	virtual	NxReal getBrakeTorque() const;
	virtual	NxReal getSteerAngle() const;


	//setting of internal state variables -- you normally don't want to do this!
	virtual	void setAxleSpeed(NxReal);
	//readout of internal state variables:
	virtual	NxReal getAxleSpeed() const;
	virtual	NxShape * getContact(NxWheelContactData & dest) const;
	//internal:
	virtual		bool		addTensorContribution(InertiaTensorComputer& itc) const;
	bool					loadFromDesc(const NxWheelShapeDesc&);
	virtual		void		sendToVRD(bool create, bool fromCtor, bool callBase);


	virtual		void							setUserWheelContactModify(NxUserWheelContactModify* callback);
	virtual		NxUserWheelContactModify*		getUserWheelContactModify();
	};

#endif

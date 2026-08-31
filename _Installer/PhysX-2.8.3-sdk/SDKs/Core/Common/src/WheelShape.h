#ifndef NX_COLLISION_WHEELSHAPE
#define NX_COLLISION_WHEELSHAPE
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/


#include "CapsuleShape.h"
#include "NxWheelShapeDesc.h"
#include "SpatialVec.h"

//namespace NxCollision {

class WheelShape : public Shape/*,	public Solvable*/// : public CapsuleShape		//inherit low level implementation from capsule shape....hopefully its ok to have this and have its type be set to wheel shape.
{
protected:
	WheelShape(const NvShapeDesc & desc, Scene *, CoreActor& actor, NxU32 uid);
	virtual							~WheelShape();

public:
	//NvShape API:
	// Shape interface
	virtual		Shape&				getShape()			{ return *this;	}
	virtual		const Shape&		getShape()	const	{ return *this;	}
	virtual		operator			Shape&()			{ return *this;	}
	virtual		void				visualize(NxFoundation::DebugRenderable& renderer)													const;
	virtual		bool				raycast(const NxRay& worldRay, NxReal maxDist, NxU32 hintFlags, NxRaycastHit& hit, bool firstHit)	const;
	virtual		bool				checkOverlapSphere(const NxSphere& worldSphere)														const;
	virtual		bool				checkOverlapOBB(const NxBox& worldBox)																const;
	virtual		bool				checkOverlapAABB(const NxBounds3& worldBounds)														const;
	virtual		bool				checkOverlapCapsule(const NxCapsule& worldCapsule)													const;

#ifdef SUPPORT_INTERNAL_RADIUS
	virtual		bool				getInternalRadius(NxReal& radius, const NxVec3& dir)												const;
#endif
	virtual		void				computeLocalBounds(NxBounds3& bounds)																const;
	// Internal methods
	virtual		void				computeWorldBounds(NxBounds3&)	const;
	virtual		void				computeWorldSphere(NxSphere&)	const;
	virtual		void				computeLocalSphere(NxSphere&)	const;
	virtual		bool				hasSpecialBroadphase() const { return true; }

	virtual		bool				isValid() const { return true; };

	//~Shape interface 

	//wheel DDI:

	virtual		void							setWheelRadius(NxReal x) { state.radius = x; }
	virtual		NxReal							getWheelRadius() const { return state.radius; }
	virtual		void							setWheelSuspensionTravel(NxReal x)  { state.suspensionTravel = x; }
	virtual		NxReal							getWheelSuspensionTravel() const { return state.suspensionTravel; }
	virtual		void							setWheelInverseMass(NxReal x) { state.inverseWheelMass = x; }
	virtual		NxReal							getWheelInverseMass() const { return state.inverseWheelMass; }
	virtual		void							setWheelFlags(NxU32 x) { state.wheelFlags = x; }
	virtual		NxU32							getWheelFlags() const { return state.wheelFlags; }
	virtual		void							setWheelMotorTorque(NxReal x) { state.motorTorque = x; }
	virtual		NxReal							getWheelMotorTorque() const { return state.motorTorque; }
	virtual		void							setWheelBrakeTorque(NxReal x) { state.brakeTorque = x; }
	virtual		NxReal							getWheelBrakeTorque() const { return state.brakeTorque; }
	virtual		void							setWheelSteerAngle(NxReal x) { state.steerAngle = x; }
	virtual		NxReal							getWheelSteerAngle() const { return state.steerAngle; }
	virtual		void							setWheelSuspension(NxSpringDesc x) { state.suspension = x; }
	virtual		NxSpringDesc					getWheelSuspension() const { return state.suspension; }
	virtual		void							setWheelLongitudalTireForceFunction(NxTireFunctionDesc x) { state.longitudalTireForceFunction = x; }
	virtual		NxTireFunctionDesc				getWheelLongitudalTireForceFunction() const { return state.longitudalTireForceFunction; }
	virtual		void							setWheelLateralTireForceFunction(NxTireFunctionDesc x) { state.lateralTireForceFunction = x; }
	virtual		NxTireFunctionDesc				getWheelLateralTireForceFunction() const { return state.lateralTireForceFunction; }
	virtual		void							setWheelLngImpulse(NxReal x) { NX_ASSERT(0); }	//not used atm.
	virtual		NxReal							getWheelLngImpulse() const { NX_ASSERT(0); return 0; }	//not used atm.
	virtual		void								setWheelAxleSpeed(NxReal x) { if (state.wheelFlags & NX_WF_AXLE_SPEED_OVERRIDE) { outUserCore.axleSpeed = core.axleSpeed = x; } }	//not used atm.
	virtual		NxReal							getWheelAxleSpeed() const { return outUserCore.axleSpeed; }	//not used atm.
	virtual		NvShape*						getContact(NxWheelContactData & dest) const;	//returns true if we have ground contact, and fills in args in that case.
	virtual		void							setUserWheelContactModify(NxUserWheelContactModify* callback) { state.wheelContactModify = callback; }
	virtual		NxUserWheelContactModify*		getUserWheelContactModify() { return state.wheelContactModify; }

	// Internal methods
	//void evalTireModel(const NxVec3 & ra, const NxVec3 & rb, const NxVec3 & contactVrel, const NxVec3 & lngDir, const NxVec3 & latDir, NxReal cpoint, const NxVec3 & normal, NxReal normalForce, NxReal dt, NxReal oneOverDt, SpatialVec & dest) const;	//this has to be const cause its called from NPPC which has const ptrs, stupidly enough.  We just make whatever we write mutable.
	//Solvable:
	void tick(NxReal dt, NxReal oneOverDt);
	void execTireModel(NxU32 pass);
	void notifyContact(Body ** bodies, const NxVec3 & contactPoint, const NxVec3 & contactNormal, NxReal normalForce, Shape * otherShape, NxMaterialIndex otherShapeMaterialIndex, NxU32 otherShapeFeatureIndex);
	void disableAxCons();
	void disableSusCons();
	NX_INLINE const NxSpringDesc & getSuspension()	const { return state.suspension; }
	NX_INLINE NxU32 getWheelFlagsFast() const { return state.wheelFlags; }//flags from NxWheelShapeFlags
	NX_INLINE NxReal getRadiusFast() const { return state.radius; }
	NX_INLINE NxReal getSuspensionTravelFast() const { return state.suspensionTravel; }
	NX_INLINE void	getWorldSegmentFast(NxSegment & s) const 
	{
		if (getWheelFlagsFast() & NX_WF_EMULATE_LEGACY_WHEEL)
		{
			getAbsPoseFast().M.getColumn(1, s.p1);
			s.p1 *= state.suspensionTravel;
			s.p0 = getAbsPoseFast().t + s.p1;
			s.p1 = getAbsPoseFast().t - s.p1;
			return;
		}
		s.p0 = getAbsPoseFast().t;
		getAbsPoseFast().M.getColumn(1, s.p1);
		s.p1 = getAbsPoseFast().t - s.p1 * (state.radius + state.suspensionTravel);
	}

	NX_INLINE NxU32 getSceneIndex() const { return sceneIndex; }
	NX_INLINE void	setSceneIndex(NxU32 i) { sceneIndex = i; }

	// double buffering
	void swapBuffers();

private:
	//vars written by sim that can be read by the user and thus need to be double buffered:
	struct WheelContact
	{
		NxVec3 contactPoint;
		NxVec3 contactNormal;
		NxVec3 lateralDirection;
		NxVec3 longitudalDirection;
		NxReal normalForce;
		Shape * otherShape;
		NxMaterialIndex otherShapeMaterialIndex;
		NxReal longitudalSlip;
		NxReal lateralSlip;
		NxReal lateralImpulse;
		NxReal longitudalImpulse;
		NxReal axleSpeed;
		NxU32 otherShapeFeatureIndex;
		NxReal contactPosition; //NX_MAX_REAL means no contact.
		NxVec3 lastPoint; // last contact point in otherShape's body local frame (used for sticky friction)
	};

	enum
	{
		SUS_CON1 = 0,
		SUS_CON2,
		FRI_CON1,
		FRI_CON2,
		LAST_CON,
	};

	WheelContact outUserCore, core;		//double buffered

	//user's input:
	NxWheelShapeDesc state;			//Only the lngImpulse and axleSpeed members are ever written by sim, this is the user copy for those.  The sim copies are in core.

	NxReal debugAngle;				//just to visualize rotation
	NxReal groundSpeed;				//internal variable that is now replacing longitudalImpulse as part of the wheel state.

	Body * bodies[2];				//one of them is us, we just store both so we don't need to resort and stuff.
	NxU32 sceneIndex;
	bool	axleSpeedOverride;		//raised when the user set the axle speed manually so we don't overwrite it for 1 frame.

	PxdConstraint constraints[LAST_CON];

	friend class Scene;	// For factory function
};
//}
#endif

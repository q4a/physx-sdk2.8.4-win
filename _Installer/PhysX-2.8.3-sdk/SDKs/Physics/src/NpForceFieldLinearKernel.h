#ifndef NX_PHYSICS_NP_FORCEFIELDLINEARKERNEL
#define NX_PHYSICS_NP_FORCEFIELDLINEARKERNEL

/*----------------------------------------------------------------------------*\
|
|						Public Interface to NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "NxForceFieldLinearKernel.h"
#include "NxForceFieldLinearKernelDesc.h"

class NpForceFieldLinearKernel : public NxForceFieldLinearKernel, public NxFoundation::NxAllocateable, public ReadWriteMutex
	{

	public:
										NpForceFieldLinearKernel(const NxForceFieldLinearKernelDesc& desc, NpScene& npScene);
	virtual								~NpForceFieldLinearKernel();

	virtual		void					parse() const;
	virtual		bool					evaluate(NxVec3 &force, NxVec3 &torque, const NxVec3 &position, const NxVec3 &velocity) const;

	virtual		void					saveToDesc(NxForceFieldLinearKernelDesc& desc);
	
	virtual		NxVec3					getConstant() const;
	virtual		void					setConstant(const NxVec3& constant);

	virtual		NxReal					getTorusRadius() const;
	virtual		void					setTorusRadius(NxReal);
	
	virtual		NxMat33					getPositionMultiplier() const;
	virtual		void					setPositionMultiplier(const NxMat33& positionMultiplier);

	virtual		NxMat33					getVelocityMultiplier() const;
	virtual		void					setVelocityMultiplier(const NxMat33& velocityMultiplier);

	virtual		NxVec3					getPositionTarget () const;
	virtual		void					setPositionTarget (const NxVec3 &);

	virtual		NxVec3					getVelocityTarget () const;
	virtual		void					setVelocityTarget (const NxVec3 &);

	virtual		NxVec3					getFalloffLinear () const;
	virtual		void					setFalloffLinear (const NxVec3 &);

	virtual		NxVec3					getFalloffQuadratic () const;
	virtual		void					setFalloffQuadratic (const NxVec3 &);

	virtual		NxVec3					getNoise () const;
	virtual		void					setNoise (const NxVec3 &);

	virtual		NxScene&				getScene()			const;

	virtual void						setName (const char* name);
	virtual const char*					getName () const;

	virtual		NxU32					getType() const;
	virtual		NxForceFieldKernel*		clone() const;
	virtual		void					update(NxForceFieldKernel& in) const;
	virtual		void					setEpsilon(NxReal eps);

				NxForceFieldCoordinates	getCoordinates() const;
				void					setCoordinates(NxForceFieldCoordinates coordinates);

	private:
				NxForceFieldLinearKernelDesc	mDesc;
				NxForceFieldCoordinates			mCoordinates;
				NxU32							mUpdateCounter;
				NpScene&						mNpScene;
	};

#endif

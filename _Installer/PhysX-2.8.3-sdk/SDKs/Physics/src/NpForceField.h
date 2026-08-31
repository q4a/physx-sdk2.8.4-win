#ifndef NX_PHYSICS_NP_FORCEFIELD
#define NX_PHYSICS_NP_FORCEFIELD

/*----------------------------------------------------------------------------*\
|
|						Public Interface to NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "NxForceField.h"
#include "NxForceFieldKernel.h"
#include "NxForceFieldShapeDesc.h"
#include "NxActor.h"
#include "Allocateable.h"
#include "NpEffector.h"
#include "NpScene.h"

class NxForceFieldKernel;
class NpForceFieldShape;
class NpForceFieldShapeGroup;

typedef void (NpForceField::*TransformAndEval)	(NxVec3& force, NxVec3& torque, const NxMat34& pose, const NxVec3& worldPos, const NxVec3& worldVel, const NxForceFieldKernel& kernel) const;

class NpForceField : public NxForceField, public NxFoundation::NxAllocateable, public ReadWriteMutex
	{

	public:

	typedef NxFoundation::NxArraySDK<NpForceFieldShapeGroup *>	ForceFieldShapeGroups;
	typedef NxFoundation::NxArraySDK<ForceFieldShapeGroups*>	GroupIntersections;
	
										NpForceField(NpScene* scene);
	virtual								~NpForceField();

	virtual		void					saveToDesc(NxForceFieldDesc& desc);

	virtual		NxMat34					getPose() const;
	virtual		void					setPose(const NxMat34& pose);

	virtual		NxActor*				getActor() const;
	virtual		void					setActor(NxActor* actor);

	virtual		void					setForceFieldKernel(NxForceFieldKernel* kernel);
	virtual		NxForceFieldKernel*		getForceFieldKernel();

	virtual		NxForceFieldShapeGroup&	getIncludeShapeGroup(); 
	virtual		void					addShapeGroup(NxForceFieldShapeGroup&);
	virtual		void					removeShapeGroup(NxForceFieldShapeGroup&);
	virtual		NxU32					getNbShapeGroups() const; 
	virtual		void					resetShapeGroupsIterator(); 
	virtual		NxForceFieldShapeGroup*	getNextShapeGroup(); 

	virtual		NxForceFieldCoordinates	getCoordinates() const;
	virtual		void					setCoordinates(NxForceFieldCoordinates coordinates);

	virtual		NxCollisionGroup		getGroup() const;
	virtual		void					setGroup(NxCollisionGroup);

	virtual		NxGroupsMask			getGroupsMask() const;
	virtual		void					setGroupsMask(NxGroupsMask  mask);

	virtual		void					setName(const char* name);
	virtual		const char*				getName() const;

	virtual		NxU32					getFlags() const;
	virtual		void					setFlags(NxU32);

	virtual		void					samplePoints(NxU32 numPoints, const NxVec3* points, const NxVec3* velocities, NxVec3* outForces, NxVec3* outTorques) const;

#if 0
	virtual		NxReal					getFluidScale()		const	{ return mFluidScale;		}
	virtual		void					setFluidScale(NxReal s)		{ mFluidScale = s;			}
	virtual		NxReal					getClothScale()		const	{ return mClothScale;		}
	virtual		void					setClothScale(NxReal s)		{ mClothScale = s;			}
	virtual		NxReal					getSoftBodyScale()	const	{ return mSoftBodyScale;	}
	virtual		void					setSoftBodyScale(NxReal s)	{ mSoftBodyScale = s;		}
	virtual		NxReal					getRigidBodyScale()	const	{ return mRigidBodyScale;	}
	virtual		void					setRigidBodyScale(NxReal s)	{ mRigidBodyScale = s;		}
#endif

	virtual		NxForceFieldType		getFluidType()		const				{ return mFluidType;		}
	virtual		void					setFluidType(NxForceFieldType t)		{ mFluidType = t;			}
	virtual		NxForceFieldType		getClothType()		const				{ return mClothType;		}
	virtual		void					setClothType(NxForceFieldType t)		{ mClothType = t;			}
	virtual		NxForceFieldType		getSoftBodyType()	const				{ return mSoftBodyType;	}
	virtual		void					setSoftBodyType(NxForceFieldType t)		{ mSoftBodyType = t;		}
	virtual		NxForceFieldType		getRigidBodyType()	const				{ return mRigidBodyType;	}
	virtual		void					setRigidBodyType(NxForceFieldType t)	{ mRigidBodyType = t;		}

	virtual		NxForceFieldVariety		getForceFieldVariety()	const					{ return mForceFieldVariety;	}
	virtual		void 					setForceFieldVariety(NxForceFieldVariety mat)	{ mForceFieldVariety = mat;	}

	virtual		NxScene&				getScene()			const;
	void								sendToVRD(bool create);
	void								finishVRDFrame();
	void								removeFromVRD();

	NX_INLINE	const NxBounds3&		getBounds()	const		{ return mBounds;		}
	NX_INLINE	void					invalidateBounds()		{ mValidBounds = false;	}
	NX_INLINE	bool					isBoundsValid()	const	{ return mValidBounds;	}
				bool					updateBounds();
				void					prepareShapes();
				void					prepareGroupIntersections();

	NX_INLINE	BOOL					contains(const NxVec3& pos)							const;
	NX_INLINE	BOOL					contains(const NxVec3& pos, NxU32 groupIndex)		const;
				bool					contains(const NxBounds3& aabb, NxU32 groupIndex)	const;
#if MULTI_THREADED_FF
				void					applyForces(NxReal simulatedTimeFluid, NxReal simulatedTimeCloth, NpForceFieldThreading* threader) const;
#endif
				void					applyForces(NxReal simulatedTimeFluid, NxReal simulatedTimeCloth) const;
				void					visualize(NxFoundation::DebugRenderable& dr)		const;

	NX_INLINE	const NxMat34&			getPoseFast()					const	{ return mPose;					}

	NX_INLINE	NxCollisionGroup		_getGroup()						const	{ return mCollisionGroup;		}
	NX_INLINE	void					_setGroup(NxCollisionGroup group)		{ mCollisionGroup = group;		}
	NX_INLINE	const NxGroupsMask&		_getGroupsMask()				const	{ return mGroupsMask;			}
	NX_INLINE	void					_setGroupsMask(NxGroupsMask mask)		{ mGroupsMask = mask;			}

	static NX_INLINE NxReal				getFFMaterial(const NpScene::NpForceFieldMaterialRow& arr, NxU16 sid)
																				{ 
																					if(sid >= arr.size())
																						return 1.0f;
																					else
																						return arr[sid];	
																				}

	NX_INLINE	NpScene*				getNpScene()					const	{ return mScene;				}

	private:
				NpScene*				mScene;
				
				NxMat34					mPose;
				NxActor*				mActor;

#if 0
  				NxReal					mFluidScale;
  				NxReal					mClothScale;
  				NxReal					mSoftBodyScale;
 				NxReal					mRigidBodyScale;
#endif
				NxForceFieldVariety		mForceFieldVariety;

				NxForceFieldType		mFluidType;
				NxForceFieldType		mClothType;
				NxForceFieldType		mSoftBodyType;
				NxForceFieldType		mRigidBodyType;

				NxForceFieldCoordinates	mCoordinates;
				TransformAndEval		mTransformEval[4];

				NxCollisionGroup		mCollisionGroup;
				NxGroupsMask			mGroupsMask;

				NxU32					mFlags;

				NpForceFieldShapeGroup*	mIncludeGroup;
				ForceFieldShapeGroups	mShapeGroups;
				GroupIntersections		mIntersectingGroups;
				NxI32					mGroupIter;				// current group index
				bool					mGroupsSorted;

				NxForceFieldKernel*		mKernel;				// external, custom kernels can be written at any time
				NxForceFieldKernel*		mKernelClone;			// internal clone, update in simulate call

				NxBounds3				mBounds;				// Bounds around force field
				bool					mValidBounds;
				bool					mHasExclusionGroups;

				void					sortGroups();

				void					transformAndEvalCartesian(NxVec3& force, NxVec3& torque, const NxMat34& pose, const NxVec3& worldPos, const NxVec3& worldVel, const NxForceFieldKernel& kernel) const;
				void					transformAndEvalSpherical(NxVec3& force, NxVec3& torque, const NxMat34& pose, const NxVec3& worldPos, const NxVec3& worldVel, const NxForceFieldKernel& kernel) const;
				void					transformAndEvalCylindrical(NxVec3& force, NxVec3& torque, const NxMat34& pose, const NxVec3& worldPos, const NxVec3& worldVel, const NxForceFieldKernel& kernel) const;
				void					transformAndEvalToroidal(NxVec3& force, NxVec3& torque, const NxMat34& pose, const NxVec3& worldPos, const NxVec3& worldVel, const NxForceFieldKernel& kernel) const;
	
		friend class ForceFieldManager;
#if MULTI_THREADED_FF
		friend class NpForceFieldSubTask;
		friend class NpForceFieldThreading;
#endif
	};

#endif

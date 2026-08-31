#ifndef NX_PHYSICS_NP_FORCEFIELDSHAPE
#define NX_PHYSICS_NP_FORCEFIELDSHAPE
/*----------------------------------------------------------------------------*\
|
|						Public Interface to NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "ReadWriteMutex.h"
#include "NxForceField.h"
#include "NxForceFieldShape.h"

#include "NpForceField.h"
#include "NpForceFieldShapeGroup.h"

class NxForceFieldShapeDesc;

class NpForceFieldShape : public ReadWriteMutex
	{
	public:
									NpForceFieldShape(const NxForceFieldShapeDesc& desc, NpForceFieldShapeGroup& sg);
	virtual							~NpForceFieldShape();

	virtual		NxForceFieldShape*	getNxForceFieldShape()					= 0;
	virtual		void				getBounds(NxBounds3& bounds)	const	= 0;
	virtual		bool				contains(const NxVec3& point)	const	= 0;
	virtual		void				visualize(NxFoundation::DebugRenderable& dr)	const	{}
	virtual		void				prepareShape()											{}

	//virtual		NxForceField*			getForceField() const;
	//virtual		NxForceFieldShapeGroup&	getShapeGroup() const;

	NX_INLINE	NxMat34				_getPose()						const	{ return mPose;												}
	NX_INLINE	void				_setPose(const NxMat34& pose)			{ mPose = pose;	mGroup.invalidateBounds();	}
	NX_INLINE	NxForceField&		_getForceField()						{ return *static_cast<NxForceField*>(mForceField);			}
	NX_INLINE	NxForceField&		_getForceField()	const				{ return *static_cast<NxForceField*>(mForceField);			}

				const char*			_getName()						const;
				void				_setName(const char* name);
	NX_INLINE	NxShapeType			_getType() const						{ return mShapeType;									}


	NX_INLINE	void				getGlobalPose(NxMat34& pose)	const
									{
/*										if(mForceField && mForceField->getActor())
										{
											// PT: TODO: optimize this thing
											NxMat34 tmp = mForceField->getActor()->getGlobalPose();
											pose.multiply(tmp, mPose);	// PT: TODO: check order
										}
										else pose = mPose;*/

										// PT: TODO: optimize this thing.
										if(mForceField)
										{
											pose.multiply(mForceField->getPoseFast(), mPose);

											if(mForceField->getActor())
											{
												NxMat34 tmp = mForceField->getActor()->getGlobalPose();
												pose.multiply(tmp, pose);
											}
										}
										else pose = mPose;
									}
	private:
				NxMat34					mPose;
	protected:
				NpForceField*			mForceField;
				NpForceFieldShapeGroup&	mGroup;

				NxShapeType				mShapeType;
	NX_INLINE	void					invalidateBounds()	 { mGroup.invalidateBounds();	}
				void					saveToDesc(NxForceFieldShapeDesc &, void * userData) const;
	};

#include "NxSphereForceFieldShape.h"
class NxSphereForceFieldShapeDesc;
//class NpSphereForceFieldShape : public NxSphereForceFieldShape, public NpForceFieldShape, public NxFoundation::NxAllocateable
class NpSphereForceFieldShape : public NxFoundation::NxAllocateable, public NpForceFieldShape, public NxSphereForceFieldShape
	{
	public:
									NpSphereForceFieldShape(const NxForceFieldShapeDesc& desc, NpForceFieldShapeGroup& sg);
	virtual							~NpSphereForceFieldShape();

	// NpForceFieldShape
	virtual		NxForceFieldShape*	getNxForceFieldShape()						{ return this;	}
	virtual		void				getBounds(NxBounds3& bounds)	const;
	virtual		bool				contains(const NxVec3& point)	const;
	virtual		void				visualize(NxFoundation::DebugRenderable& dr)	const;
	virtual		void				prepareShape();
	//~NpForceFieldShape

	// NxForceFieldShape
	virtual		NxMat34				getPose() const;
	virtual		void				setPose(const NxMat34 &);

	virtual		NxForceField*			getForceField() const;
	virtual		NxForceFieldShapeGroup&	getShapeGroup() const;

	virtual		const char*			getName () const;
	virtual		void				setName (const char *name);
	
	virtual		NxShapeType			getType () const;
	//~NxForceFieldShape

	// NxSphereForceFieldShape
	virtual		void				setRadius(NxReal radius);
	virtual		NxReal				getRadius()	const;
	//~NxSphereForceFieldShape
	virtual		void				saveToDesc(NxSphereForceFieldShapeDesc&)		const;

	private:
				NxVec3				mCachedCenter;
				float				mCachedR2;
				float				mRadius;
	};

#include "NxBoxForceFieldShape.h"
class NxBoxForceFieldShapeDesc;
//class NpBoxForceFieldShape : public NxBoxForceFieldShape, public NpForceFieldShape, public NxFoundation::NxAllocateable
class NpBoxForceFieldShape : public NxFoundation::NxAllocateable, public NpForceFieldShape, public NxBoxForceFieldShape
	{
	public:
									NpBoxForceFieldShape(const NxForceFieldShapeDesc& desc, NpForceFieldShapeGroup& sg);
	virtual							~NpBoxForceFieldShape();

	// NpForceFieldShape
	virtual		NxForceFieldShape*	getNxForceFieldShape()						{ return this;	}
	virtual		void				getBounds(NxBounds3& bounds)	const;
	virtual		bool				contains(const NxVec3& point)	const;
	virtual		void				visualize(NxFoundation::DebugRenderable& dr)	const;
	virtual		void				prepareShape();
	//~NpForceFieldShape

	// NxForceFieldShape
	virtual		NxMat34				getPose() const;
	virtual		void				setPose(const NxMat34 &);

	virtual		NxForceField*			getForceField() const;
	virtual		NxForceFieldShapeGroup&	getShapeGroup() const;

	virtual		const char*			getName () const;
	virtual		void				setName (const char *name);
	
	virtual		NxShapeType			getType () const;
	//~NxForceFieldShape

	// NxBoxForceFieldShape
	virtual		void				setDimensions(const NxVec3& vec);
	virtual		NxVec3				getDimensions() const;
	//~NxBoxForceFieldShape
	virtual		void				saveToDesc(NxBoxForceFieldShapeDesc&)		const;

	private:
				NxBox				mCachedBox;
				NxVec3				mDimensions;
	};

#include "NxCapsuleForceFieldShape.h"
#include "NxSegment.h"
class NxCapsuleForceFieldShapeDesc;
//class NpCapsuleForceFieldShape : public NxCapsuleForceFieldShape, public NpForceFieldShape, public NxFoundation::NxAllocateable
class NpCapsuleForceFieldShape : public NxFoundation::NxAllocateable, public NpForceFieldShape, public NxCapsuleForceFieldShape
	{
	public:
									NpCapsuleForceFieldShape(const NxForceFieldShapeDesc& desc, NpForceFieldShapeGroup& sg);
	virtual							~NpCapsuleForceFieldShape();

	// NpForceFieldShape
	virtual		NxForceFieldShape*	getNxForceFieldShape()						{ return this;	}
	virtual		void				getBounds(NxBounds3& bounds)	const;
	virtual		bool				contains(const NxVec3& point)	const;
	virtual		void				visualize(NxFoundation::DebugRenderable& dr)	const;
	virtual		void				prepareShape();
	//~NpForceFieldShape

	// NxForceFieldShape
	virtual		NxMat34				getPose() const;
	virtual		void				setPose(const NxMat34 &);

	virtual		NxForceField*			getForceField() const;
	virtual		NxForceFieldShapeGroup&	getShapeGroup() const;

	virtual		const char*			getName () const;
	virtual		void				setName (const char *name);
	
	virtual		NxShapeType			getType () const;
	//~NxForceFieldShape

	// NxCapsuleForceFieldShape
	virtual		void				setDimensions(NxReal radius, NxReal height);
	virtual		void				setRadius(NxReal radius);
	virtual		NxReal				getRadius() const;
	virtual		void				setHeight(NxReal height);
	virtual		NxReal				getHeight() const;
	//~NxCapsuleForceFieldShape

	NX_INLINE	void				getWorldSegmentFast(NxSegment& worldSegment)	const
									{
									NxMat34 globalPose(false);
									getGlobalPose(globalPose);

									NxReal halfHeight = mHeight*0.5f;	// TODO: check definition of "height"
									globalPose.M.getColumn(1, worldSegment.p1);
									worldSegment.p1 *= halfHeight;
									worldSegment.p0 = -worldSegment.p1;
									worldSegment.p0 += globalPose.t;
									worldSegment.p1 += globalPose.t;
									}
	virtual		void				saveToDesc(NxCapsuleForceFieldShapeDesc&)		const;
	private:
				LSS					mCachedCapsule;
				NxReal				mRadius;
				NxReal				mHeight;
	};

#include "NxConvexForceFieldShape.h"
class ConvexMeshRuntime;
class NxConvexForceFieldShapeDesc;
//class NpConvexForceFieldShape : public NxConvexForceFieldShape, public NpForceFieldShape, public NxFoundation::NxAllocateable
class NpConvexForceFieldShape : public NxFoundation::NxAllocateable, public NpForceFieldShape, public NxConvexForceFieldShape
	{
	public:
									NpConvexForceFieldShape(const NxForceFieldShapeDesc& desc, NpForceFieldShapeGroup& sg);
	virtual							~NpConvexForceFieldShape();

	// NpForceFieldShape
	virtual		NxForceFieldShape*	getNxForceFieldShape()						{ return this;	}
	virtual		void				getBounds(NxBounds3& bounds)	const;
	virtual		bool				contains(const NxVec3& point)	const;
	virtual		void				visualize(NxFoundation::DebugRenderable& dr)	const;
	virtual		void				prepareShape();
	//~NpForceFieldShape

	// NxForceFieldShape
	virtual		NxMat34					getPose() const;
	virtual		void					setPose(const NxMat34 &);

	virtual		NxForceField*			getForceField() const;
	virtual		NxForceFieldShapeGroup&	getShapeGroup() const;

	virtual		const char*				getName () const;
	virtual		void					setName (const char *name);
	
	virtual		NxShapeType				getType () const;
	//~NxForceFieldShape
	virtual		void					saveToDesc(NxConvexForceFieldShapeDesc&)		const;
	virtual		ConvexMeshRuntime*		getMesh();

	private:
				IceMaths::Plane*		mCachedPlanes;
	mutable		NxVec3					mInSphere;
	mutable		NxReal					mInRadius;
	mutable		NxU32					mLastCullingPlaneIndex;
				ConvexMeshRuntime*		mConvexMesh;
	};

#endif


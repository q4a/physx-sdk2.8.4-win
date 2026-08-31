#ifndef NX_COLLISION_NP_SHAPE
#define NX_COLLISION_NP_SHAPE
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/
#include "Nxp.h"
#include "ReadWriteMutex.h"
#include "DebugRenderable.h"
class NpActor;
class NvShape;
class NxShapeDesc;
class InertiaTensorComputer;
class NxCCDSkeleton;

class NpShape : public ReadWriteMutex
	{
	public:
								NpShape();
	virtual						~NpShape();		//virtual destructor so we can delete Np*Shape objects ezly.

	NX_INLINE	NpActor *		getNpActor() const { return npActor; }
				void			setNpActor(NpActor * p);
//	NX_INLINE	Shape *			getShape() { return shape; }
	NX_INLINE	NvShape *		getNvShape() {return mShape; }		//This is a bit ugly, but needed for HSM

				void			setName(const char*debugName);
				void			setLocalPose(const NxMat34&);
				void			setLocalPosition(const NxVec3&);
				void			setLocalOrientation(const NxMat33&);

				NxMat34			getLocalPose()			const;
				NxVec3			getLocalPosition()		const;
				NxMat33			getLocalOrientation()	const;
				NxMat34			getGlobalPose()			const;
				NxVec3			getGlobalPosition()		const;
				NxMat33			getGlobalOrientation()	const;
				const char*		getName()				const;


				void			setGlobalPose(const NxMat34&);
				void			setGlobalPosition(const NxVec3&);
				void			setGlobalOrientation(const NxMat33&);
	virtual		bool			addTensorContribution(InertiaTensorComputer& itc) const = 0;
				void			setMaterial(NxMaterialIndex index);
	NX_INLINE	NxReal			getDensity() const {return density;}
	NX_INLINE	NxReal			getMass()    const {return mass;}

	virtual		void			visualize(NxFoundation::DebugRenderable & dr);
				void			setCCDSkeleton(NxCCDSkeleton *s);

	virtual		void			sendToVRD(bool create, bool fromCtor, bool callBase);
				void			removeFromVRD();

	protected:
				void			loadFromDesc(const NxShapeDesc &, void ** userData);
				void			saveToDesc(NxShapeDesc &, void * userData) const;

				bool			emulateOverlapAABBTriangles(const NxBounds3 bounds, NxU32 flags, NxU32& nb, const NxU32*& indices) const;

				NvShape*		mShape;
	private:
				NpActor*		npActor;
				NxReal			density;  // density of shape - used to compute the mass.  unless mass is already >0.0
				NxReal			mass;     // mass of the shape - if <= 0.0 then mass will be computed based on density
	};


#endif

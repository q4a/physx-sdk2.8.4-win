#ifndef NX_COLLISION_RB_ACTOR
#define NX_COLLISION_RB_ACTOR
/*----------------------------------------------------------------------------*\
|
|							  NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "CoreActor.h"
#include "CoreInteraction.h"
#include "NvShape.h"

class Scene;
class NxActor;
class Shape;

#define INVALID_ACTOR_GROUP 0xffff

// This inherits NvShape to mimic a compound for usage above the Nv-layer (HSM etc). This will be removed 
// when the Nv-layer is removed. Below the Nv-layer the compounds should be removed.
class RbActor : public CoreActor, public NvShape
{
public:
	RbActor(Scene& scene, NxU32 actorType, NxActor* nxActor);
	virtual ~RbActor();
	void setActorsInteractionsDirty(CoreInteraction::DirtyFlag flag, const CoreActor* other, bool (*condition)(const PxInteraction&));
	virtual void destroy();

	NX_INLINE NxU32 getActorPublicFlags() const;
	NX_INLINE NxActorGroup getActorGroup() const;
	NX_INLINE Scene* getScene() const;
	NX_INLINE NxU32 getBroadPhaseGroup() const;
	NX_INLINE NxU32 getActorId() const;
	NX_INLINE static bool isRbActor(CoreActor& actor);
	NX_INLINE NxActor* getNxActorFast() const;
	NX_INLINE NxDominanceGroup getActorDominanceGroupFast() const;
	NX_INLINE NxU32 getActorContactReportFlagsFast() const;
	NX_INLINE bool isDying() const { return mIsDying; }
	NX_INLINE void markAsDying() { mIsDying = true; }

	// NvShape implementation. 
	// RbActor & NvShape
	virtual	NxActor* getNxActor() const;
	virtual void setActorPublicFlags(NxU32 f);	
	virtual	void setActorGroup(NxActorGroup g);	
	virtual void setActorDominanceGroup(NxDominanceGroup);
	virtual	void setActorContactReportFlags(NxU32 flags);
	virtual	void resetUserActorPairFiltering();
	virtual	void computeWorldSphere(NxSphere& sphere) const;
	virtual NxU32 getNbShapes()	const;

	// Only to be used above Nv-layer and will be removed when the Nv-layer is removed.
	virtual const NvShape** getNvShapes() const;
	virtual NvShape** getNvShapes();
	virtual	void moveToCompound(NvShape&);
	virtual NvShape* getNvShapeOwner();
	virtual	NvScene* getNvScene() const;
	virtual	NxShape** getNxShapes();
	virtual	NvBody* getNvBody()	const;
	virtual	NxU32 getID() const;
	virtual	NxShapeType getType() const;
	virtual	void release();
	virtual	NvShape* getSourceNvShape() const;
	NX_INLINE virtual void setSourceNvShape(NvShape* srcShape);
	virtual void getWorldBounds(NxBounds3&) const;

	// PxActor implementation
	virtual void setElementCountHint(NxU32 n);

protected:
	// PxActor implementation
	virtual void registerElement(PxElement* element);
	virtual void unregisterElement(PxElement* element);
	virtual void onActivate();
	virtual void onDeactivate();

	NX_INLINE void setBroadPhaseGroup(NxU32 g);
	NX_INLINE void setActorId(NxU32 id);

private:
	NxU32 mActorFlags;
	NxU32 mBpGroup;
	NxU32 mActorId;
	NxU32 mShapeCount;
	NxActorGroup mActorGroup;
	NxDominanceGroup mActorDominanceGroup;
	NxU32 mActorContactReportFlags;
	NxActor* mNxActor;
	Scene* mScene;
	NvShape* mSourceShape;
	bool mIsDying;

	// These are only needed by the NvShape interface and should not be used from below the Nv-layer
	// Use the PxElement list to get the shapes otherwise.
	NxFoundation::NxArraySDK<Shape*> mShapes;
	NxFoundation::NxArraySDK<NxShape*> mNpShapes;


public:
	// NvShape implementation not used.
	virtual		NpShape* getNpShape()						const
	{
		return NULL;
	}
	virtual		NxShape* getNxShape()
	{
		return NULL;
	}
	virtual		void setMaterial(NxMaterialIndex) {}
	virtual		NxMaterialIndex getMaterial()						const
	{
		return 0;
	}
	virtual		NxVec3 getShape2ActorP()					const
	{
		NX_ASSERT(0);
		return NxVec3();
	}
	virtual		NxQuat getShape2ActorQ()					const
	{
		NX_ASSERT(0);
		return NxQuat();
	}
	virtual		void setShape2ActorP(const NxVec3& p)
	{
		NX_ASSERT(0);
	}
	virtual		void setShape2ActorQ(const NxQuat& q)
	{
		NX_ASSERT(0);
	}
	virtual		void setShape2ActorPQ(const NxVec3& p, const NxQuat& q)
	{
		NX_ASSERT(0);
	}
	virtual		void setFlags(NxU32 f)
	{
		NX_ASSERT(0);
	}
	virtual		NxU32 getFlags()							const
	{
		NX_ASSERT(0);
		return 0;
	}
	virtual		void setAsyncFlags(NxU32 f)
	{
		NX_ASSERT(0);
	}
	virtual		NxU32 getAsyncFlags()						const
	{
		NX_ASSERT(0);
		return 0;
	}
	virtual		void setSkinWidth(NxReal)
	{
		NX_ASSERT(0);
	}
	virtual		NxReal getSkinWidth()						const
	{
		NX_ASSERT(0);
		return 0;
	}
	virtual		void setGroup(NxCollisionGroup)
	{
		NX_ASSERT(0);
	}
	virtual		NxCollisionGroup getGroup()							const
	{
		NX_ASSERT(0);
		return 0;
	}
	virtual		void setGroupsMask(const NxGroupsMask&)
	{
		NX_ASSERT(0);
	}
	virtual		NxGroupsMask getGroupsMask()						const
	{
		NX_ASSERT(0);
		return NxGroupsMask();
	}

#ifndef __CELLOS_LV2__
	virtual		void setNonInteractingCompartmentTypes(NxU32 compartmentTypes) { NX_ASSERT(0); }
	virtual		NxU32 getNonInteractingCompartmentTypes() const { NX_ASSERT(0); return 0; }
#endif

	virtual		bool raycast(const NxRay& worldRay, NxReal maxDist, NxU32 hintFlags, NxRaycastHit& hit, bool firstHit=
							 false)	const
	{
		NX_ASSERT(0);
		return false;
	}
	virtual		bool checkOverlapSphere(const NxSphere& worldSphere)		const
	{
		NX_ASSERT(0);
		return false;
	}
	virtual		bool checkOverlapOBB(const NxBox& worldBox)				const
	{
		NX_ASSERT(0);
		return false;
	}
	virtual		bool checkOverlapAABB(const NxBounds3& worldBounds)		const
	{
		NX_ASSERT(0);
		return false;
	}
	virtual		bool checkOverlapCapsule(const NxCapsule& worldCapsule)	const
	{
		NX_ASSERT(0);
		return false;
	}
	virtual		void setCCDSkeleton(NvCCDSkeleton*)
	{
		NX_ASSERT(0);
	}
	virtual		NvCCDSkeleton* getCCDSkeleton() const
	{
		NX_ASSERT(0);
		return NULL;
	}
	virtual		void setDimensions(const NxVec3&)
	{
		NX_ASSERT(0);
	}
	virtual		NxVec3 getBoxDimensions()			const
	{
		NX_ASSERT(0);
		return NxVec3();
	}
	virtual		void setRadius(NxReal radius)
	{
		NX_ASSERT(0);
	}
	virtual		void setHeight(NxReal height)
	{
		NX_ASSERT(0);
	}
	virtual		NxReal getRadius()			const
	{
		NX_ASSERT(0);
		return 0;
	}
	virtual		NxReal getHeight()			const
	{
		NX_ASSERT(0);
		return 0;
	}
	virtual		NxU32 getCapsuleFlags()	const
	{
		NX_ASSERT(0);
		return 0;
	}
	virtual		void setCapsuleFlags(NxU32 f)
	{
		NX_ASSERT(0);
	}
	virtual		void getWorldCapsule(NxCapsule& worldCapsule)	const
	{
		NX_ASSERT(0);
	}
	virtual		NxPlane getPlaneEquation()			const
	{
		NX_ASSERT(0);
		return NxPlane();
	}
	virtual		void setPlaneEquation(const NxPlane&)
	{
		NX_ASSERT(0);
	}
	virtual		void setWheelRadius(NxReal)
	{
		NX_ASSERT(0);
	}
	virtual		NxReal getWheelRadius() const
	{
		NX_ASSERT(0);
		return 0;
	}
	virtual		void setWheelSuspensionTravel(NxReal)
	{
		NX_ASSERT(0);
	}
	virtual		NxReal getWheelSuspensionTravel() const
	{
		NX_ASSERT(0);
		return 0;
	}
	virtual		void setWheelInverseMass(NxReal)
	{
		NX_ASSERT(0);
	}
	virtual		NxReal getWheelInverseMass() const
	{
		NX_ASSERT(0);
		return 0;
	}
	virtual		void setWheelFlags(NxU32)
	{
		NX_ASSERT(0);
	}
	virtual		NxU32 getWheelFlags() const
	{
		NX_ASSERT(0);
		return 0;
	}
	virtual		void setWheelMotorTorque(NxReal)
	{
		NX_ASSERT(0);
	}
	virtual		NxReal getWheelMotorTorque() const
	{
		NX_ASSERT(0);
		return 0;
	}
	virtual		void setWheelBrakeTorque(NxReal)
	{
		NX_ASSERT(0);
	}
	virtual		NxReal getWheelBrakeTorque() const
	{
		NX_ASSERT(0);
		return 0;
	}
	virtual		void setWheelSteerAngle(NxReal)
	{
		NX_ASSERT(0);
	}
	virtual		NxReal getWheelSteerAngle() const
	{
		NX_ASSERT(0);
		return 0;
	}
	virtual		void setWheelSuspension(NxSpringDesc)
	{
		NX_ASSERT(0);
	}
	virtual		NxSpringDesc getWheelSuspension() const
	{
		NX_ASSERT(0);
		return 0;
	}
	virtual		void setWheelLongitudalTireForceFunction(NxTireFunctionDesc)
	{
		NX_ASSERT(0);
	}
	virtual		NxTireFunctionDesc getWheelLongitudalTireForceFunction() const
	{
		NX_ASSERT(0);
		return NxTireFunctionDesc();
	}
	virtual		void setWheelLateralTireForceFunction(NxTireFunctionDesc)
	{
		NX_ASSERT(0);
	}
	virtual		NxTireFunctionDesc getWheelLateralTireForceFunction() const
	{
		NX_ASSERT(0);
		return NxTireFunctionDesc();
	}
	virtual		void setWheelLngImpulse(NxReal)
	{
		NX_ASSERT(0);
	}
	virtual		NxReal getWheelLngImpulse() const
	{
		NX_ASSERT(0);
		return 0;
	}
	virtual		void setWheelAxleSpeed(NxReal)
	{
		NX_ASSERT(0);
	}
	virtual		NxReal getWheelAxleSpeed() const
	{
		NX_ASSERT(0);
		return 0;
	}
	virtual		NvShape* getContact(NxWheelContactData& dest) const
	{
		NX_ASSERT(0);
		return 0;
	}
	virtual		NvConvexMesh* getNvConvexMesh()
	{
		NX_ASSERT(0);
		return 0;
	}
	virtual		const NvConvexMesh* getNvConvexMesh()		const
	{
		NX_ASSERT(0);
		return 0;
	}
	virtual		void setConvexMesh(NvConvexMesh*)
	{
		NX_ASSERT(0);
	}
	virtual		NpConvexMesh* getConvexMesh()
	{
		NX_ASSERT(0);
		return 0;
	}
	virtual		NvTriangleMesh* getNvTriangleMesh()
	{
		NX_ASSERT(0);
		return 0;
	}
	virtual		const NvTriangleMesh* getNvTriangleMesh()		const
	{
		NX_ASSERT(0);
		return 0;
	}
	virtual		void setTriangleMesh(NvTriangleMesh*)
	{
		NX_ASSERT(0);
	}
	virtual		NpTriangleMesh* getTriangleMesh()
	{
		NX_ASSERT(0);
		return 0;
	}
	virtual		bool mapPageInstance(NxU32 pageIndex)
	{
		NX_ASSERT(0);
		return 0;
	}
	virtual		void unmapPageInstance(NxU32 pageIndex)
	{
		NX_ASSERT(0);
	}
	virtual		bool isPageInstanceMapped(NxU32 pageIndex) const
	{
		NX_ASSERT(0);
		return 0;
	}
	virtual		NxU32 getMeshFlags()			const
	{
		NX_ASSERT(0);
		return 0;
	}
	virtual		void setMeshFlags(NxU32 f)
	{
		NX_ASSERT(0);
	}
	virtual		NxMeshPagingMode getMeshPagingMode()	const
	{
		NX_ASSERT(0);
		return NxMeshPagingMode();
	}
	virtual		void setMeshPagingMode(NxMeshPagingMode m)
	{
		NX_ASSERT(0);
	}
	virtual		NxU32 getTriangle(NxTriangle& triangle, NxTriangle* edgeTri, NxU32* returnedFlags,
								  NxTriangleID triangleIndex, bool worldSpaceTranslation=true, bool worldSpaceRotation=
								  true) const
	{
		NX_ASSERT(0);
		return 0;
	}
	virtual		bool overlapAABBTriangles(const NxBounds3 bounds, NxU32 flags,
										  NxUserEntityReport<NxU32>* callback)			const
	{
		NX_ASSERT(0);
		return false;
	}
	virtual		NxF32 getScale()				const
	{
		NX_ASSERT(0);
		return 0;
	}
	virtual		void setScale(NxF32 scale)
	{
		NX_ASSERT(0);
	}
	virtual		NvHeightField* getNvHeightField()
	{
		NX_ASSERT(0);
		return 0;
	}
	virtual		const NvHeightField* getNvHeightField() const
	{
		NX_ASSERT(0);
		return 0;
	}
	virtual		void setHeightField(NvHeightField*)
	{
		NX_ASSERT(0);
	}
	virtual		NpHeightField* getHeightField()
	{
		NX_ASSERT(0);
		return 0;
	}
	virtual		NxReal getHeightScaleV() const
	{
		NX_ASSERT(0);
		return 0;
	}
	virtual		void setHeightScale(NxReal)
	{
		NX_ASSERT(0);
	}
	virtual		NxReal getRowScaleV() const
	{
		NX_ASSERT(0);
		return 0;
	}
	virtual		void setRowScale(NxReal)
	{
		NX_ASSERT(0);
	}
	virtual 	NxReal getColumnScaleV() const
	{
		NX_ASSERT(0);
		return 0;
	}
	virtual 	void setColumnScale(NxReal)
	{
		NX_ASSERT(0);
	}
	virtual		NxMaterialIndex getMaterialIndexHighBitsV()const
	{
		NX_ASSERT(0);
		return 0;
	}
	virtual		void setMaterialIndexHighBits(NxMaterialIndex)
	{
		NX_ASSERT(0);
	}
	virtual		NxMaterialIndex getHoleMaterialV()		const
	{
		NX_ASSERT(0);
		return 0;
	}
	virtual		void setHoleMaterial(NxMaterialIndex)
	{
		NX_ASSERT(0);
	}
	virtual		bool isShapePointOnHeightFieldV(NxReal x, NxReal z) const
	{
		NX_ASSERT(0);
		return false;
	}
	virtual		NxReal getHeightAtShapePointV(NxReal x, NxReal z) const
	{
		NX_ASSERT(0);
		return 0;
	}
	virtual		NxMaterialIndex getMaterialAtShapePointV(NxReal x, NxReal z) const
	{
		NX_ASSERT(0);
		return 0;
	}
	virtual		NxVec3 getNormalAtShapePointV(NxReal x, NxReal z) const
	{
		NX_ASSERT(0);
		return NxVec3();
	}
	virtual		NxVec3 getSmoothNormalAtShapePointV(NxReal x, NxReal z) const
	{
		NX_ASSERT(0);
		return NxVec3();
	}
};


//////////////////////////////////////////////////////////////////////////
NX_INLINE bool RbActor::isRbActor(CoreActor& actor)
{
	return (actor.getPxActorType() == PX_ACTOR_TYPE_DYNAMIC || actor.getPxActorType() == PX_ACTOR_TYPE_STATIC);
}

NX_INLINE NxU32 RbActor::getActorPublicFlags() const
{
	return mActorFlags;
}

NX_INLINE NxActorGroup RbActor::getActorGroup() const
{
	return mActorGroup;
}

NX_INLINE Scene* RbActor::getScene() const
{
	return mScene;
}
NX_INLINE NxU32 RbActor::getBroadPhaseGroup() const
{
	return mBpGroup;
}

NX_INLINE NxU32 RbActor::getActorId() const
{
	return mActorId;
}

NX_INLINE NxActor* RbActor::getNxActorFast() const
{
	return mNxActor;
}

NX_INLINE NxDominanceGroup RbActor::getActorDominanceGroupFast() const
{
	return mActorDominanceGroup;
}

NX_INLINE NxU32 RbActor::getActorContactReportFlagsFast() const
{
	return mActorContactReportFlags;
}

NX_INLINE NvShape* RbActor::getSourceNvShape() const
{
	return mSourceShape;
}

NX_INLINE void RbActor::setSourceNvShape(NvShape* srcShape)
{
	mSourceShape = srcShape;
}

NX_INLINE void RbActor::setBroadPhaseGroup(NxU32 g)
{
	mBpGroup = g;
}

NX_INLINE void RbActor::setActorId(NxU32 id)
{
	mActorId = id;
}
#endif

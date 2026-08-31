#ifndef MACRO_NX_COLLISION_NP_SHAPE
#define MACRO_NX_COLLISION_NP_SHAPE
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "NpShape.h"
#include "NpActor.h"
#include "NvShape.h"
#include "NpCCDSkeleton.h"

#ifndef NP
#define NP(ClassName) Np ## ClassName
#endif



#ifndef __CELLOS_LV2__
#define NI_COMPARTMENT_TYPES_DEFINITION(ClassName) \
void NP(ClassName)::setNonInteractingCompartmentTypes(NxU32 compartmentTypes)	\
	{																			\
	BEGINWRITE																	\
	NpScene* npScene = (NpScene*)&getNpActor()->getScene();						\
	npScene->hardwareAbstraction.onActorChange(getNpActor(), HWA_MA_DIRTY_SHAPE_NI_COMPARTMENT_TYPES); \
	mShape->setNonInteractingCompartmentTypes(compartmentTypes);				\
	}																			\
																				\
NxU32 NP(ClassName)::getNonInteractingCompartmentTypes() const					\
	{																			\
	BEGINREAD																	\
	return mShape->getNonInteractingCompartmentTypes();							\
	}
#else
#define NI_COMPARTMENT_TYPES_DEFINITION(ClassName) \
void NP(ClassName)::setNonInteractingCompartmentTypes(NxU32 compartmentTypes) {}	\
																					\
NxU32 NP(ClassName)::getNonInteractingCompartmentTypes() const { return 0; }
#endif


#define NPSHAPE_DECLARATION(ClassName)													\
																						\
	public:																				\
																						\
				NP(ClassName)();															\
	virtual		~NP(ClassName)();														\
																						\
	virtual		void						setNvShape(NvShape* shape);					\
	virtual		NxActor&					getActor() const;							\
	virtual		void						setGroup(NxCollisionGroup);					\
	virtual		NxCollisionGroup			getGroup()							const;	\
	virtual		void						getWorldBounds(NxBounds3& dest)		const;	\
	virtual		void						setFlag(NxShapeFlag flag, bool value);		\
	virtual		NX_BOOL						getFlag(NxShapeFlag flag)			const;	\
	virtual		void						setLocalPose(const NxMat34&);				\
	virtual		void						setLocalPosition(const NxVec3&);			\
	virtual		void						setLocalOrientation(const NxMat33&);		\
	virtual		void						setGlobalPose(const NxMat34&);				\
	virtual		void						setGlobalPosition(const NxVec3&);			\
	virtual		void						setGlobalOrientation(const NxMat33&);		\
	virtual		void						setMaterial(NxMaterialIndex);				\
	virtual		NxMaterialIndex				getMaterial()						const;	\
	virtual		void						setSkinWidth(NxReal skinWidth);				\
	virtual		NxReal						getSkinWidth()						const;	\
	virtual		const char*					getName()							const;	\
	virtual		void						setName(const char*);						\
	virtual		NxShapeType					getType()							const;	\
	virtual		void						setCCDSkeleton(NxCCDSkeleton *);			\
	virtual		NxCCDSkeleton *				getCCDSkeleton() const;						\
																						\
																						\
	virtual		NxMat34						getLocalPose()			const;				\
	virtual		NxVec3						getLocalPosition()		const;				\
	virtual		NxMat33						getLocalOrientation()	const;				\
																						\
	virtual		NxMat34						getGlobalPose()			const;				\
	virtual		NxVec3						getGlobalPosition()		const;				\
	virtual		NxMat33						getGlobalOrientation()	const;				\
																						\
	virtual		bool						raycast(const NxRay& worldRay, NxReal maxDist, NxU32 hintFlags, NxRaycastHit& hit, bool first_hit)	const;	\
	virtual		bool						checkOverlapSphere(const NxSphere& worldSphere)														const;	\
	virtual		bool						checkOverlapOBB(const NxBox& worldBox)																const;	\
	virtual		bool						checkOverlapAABB(const NxBounds3& worldBounds)														const;	\
	virtual		bool						checkOverlapCapsule(const NxCapsule& worldCapsule)													const;	\
	virtual		void						setGroupsMask(const NxGroupsMask& mask);	\
	virtual		const NxGroupsMask			getGroupsMask()	const;						\
	virtual		void						setNonInteractingCompartmentTypes(NxU32 compartmentTypes); \
	virtual		NxU32						getNonInteractingCompartmentTypes() const;	\
																						\
	virtual		void*						is(NxShapeType type);						\
	virtual		const void*					is(NxShapeType type) const;


/*Shape* NP(ClassName)::getInternalShape()									\
	{																		\
		return mShape;														\
	}																		\*/

#define NPSHAPE_DEFINITION(ClassName)										\
																			\
void NP(ClassName)::setNvShape(NvShape* shape)								\
	{																		\
	mShape = shape;															\
	appData = (void*)shape;													\
	}																		\
																			\
																			\
NxActor& NP(ClassName)::getActor() const									\
	{																		\
	BEGINREAD																\
	return *getNpActor();													\
	}																		\
																			\
NxShapeType NP(ClassName)::getType() const									\
	{																		\
	BEGINREAD																\
	return mShape->getType();												\
	}																		\
																			\
void NP(ClassName)::setGroup(NxCollisionGroup group)						\
	{																		\
	BEGINWRITE																\
	mShape->setGroup(group);												\
	NX_DBG_SET_PARAMETER((NxU32)group, (NpShape*)this, "Collision Group", NX_DBG_EVENTGROUP_BASIC_OBJECTS_STATIC_DATA); \
	NpScene* npScene = (NpScene*)&getNpActor()->getScene();					\
	npScene->hardwareAbstraction.onActorChange(getNpActor(), HWA_MA_DIRTY_SHAPE_GROUP);	\
	}																		\
																			\
NxCollisionGroup NP(ClassName)::getGroup() const							\
	{																		\
	BEGINREAD																\
	return mShape->getGroup();												\
	}																		\
																			\
void NP(ClassName)::getWorldBounds(NxBounds3& dest) const					\
	{																		\
	BEGINREAD																\
	mShape->getWorldBounds(dest);											\
	}																		\
																			\
void NP(ClassName)::setFlag(NxShapeFlag flag, bool value)					\
	{																		\
	if (flag == NX_SF_DISABLE_SCENE_QUERIES)								\
		{																	\
		NxU32 asyncFlags = mShape->getAsyncFlags();							\
		if(value)															\
			asyncFlags |= flag;												\
		else																\
			asyncFlags &= ~flag;											\
		mShape->setAsyncFlags(asyncFlags);									\
		return;																\
		}																	\
	BEGINWRITE																\
	if (flag & NX_TRIGGER_ENABLE)											\
		{																	\
		if (value && !(mShape->getFlags() & NX_TRIGGER_ENABLE))				\
			{																\
			NX_DBG_CREATE_OBJECT((char*)((NpShape*)this)+1, NX_DBG_OBJECTTYPE_GENERIC, "Triggers", NX_DBG_EVENTGROUP_TRIGGERS);	\
			NX_DBG_ADD_CHILD((char*)((NpShape*)this), (char*)((NpShape*)this)+1, NX_DBG_EVENTGROUP_TRIGGERS);								\
			}																\
		else if (!value && (mShape->getFlags() & NX_TRIGGER_ENABLE))		\
			{																\
			NX_DBG_REMOVE_OBJECT((char*)((NpShape*)this)+1, NX_DBG_EVENTGROUP_TRIGGERS);		\
			}																\
		}																	\
	NxU32 flags = 0;														\
	if(value)																\
		flags = mShape->getFlags()|flag;									\
	else																	\
		flags = mShape->getFlags()&~flag;									\
	mShape->setFlags(flags);												\
	NpScene* npScene = (NpScene*)&getNpActor()->getScene();					\
	npScene->hardwareAbstraction.onActorChange(getNpActor(), HWA_MA_DIRTY_SHAPE_FLAGS);	\
	}																		\
																			\
NX_BOOL NP(ClassName)::getFlag(NxShapeFlag flag) const						\
	{																		\
	if (flag == NX_SF_DISABLE_SCENE_QUERIES)								\
		{																	\
		return mShape->getAsyncFlags() & flag;								\
		}																	\
	BEGINREAD																\
	/* PT: the whole point of the NX_BOOL in the interface is to AVOID */	\
	/* the USELESS and SLOW cast to bool, so DON'T DO IT. */				\
	return mShape->getFlags() & flag;										\
	}																		\
																			\
void NP(ClassName)::setLocalPose(const NxMat34&m)							\
	{																		\
	BEGINWRITE																\
	NpShape::setLocalPose(m);												\
	}																		\
																			\
void NP(ClassName)::setLocalPosition(const NxVec3&v)						\
	{																		\
	BEGINWRITE																\
	NpShape::setLocalPosition(v);											\
	}																		\
																			\
void NP(ClassName)::setLocalOrientation(const NxMat33&m)					\
	{																		\
	BEGINWRITE																\
	NpShape::setLocalOrientation(m);										\
	}																		\
																			\
void NP(ClassName)::setGlobalPose(const NxMat34& shape2world)				\
	{																		\
	BEGINWRITE																\
	NpShape::setGlobalPose(shape2world);									\
	}																		\
																			\
void NP(ClassName)::setGlobalPosition(const NxVec3&gv)						\
	{																		\
	BEGINWRITE																\
	NpShape::setGlobalPosition(gv);											\
	}																		\
																			\
void NP(ClassName)::setGlobalOrientation(const NxMat33&gm)					\
	{																		\
	BEGINWRITE																\
	NpShape::setGlobalOrientation(gm);										\
	}																		\
																			\
void NP(ClassName)::setMaterial(NxMaterialIndex index)						\
	{																		\
	BEGINWRITE																\
	mShape->setMaterial(index);												\
	NX_DBG_SET_PARAMETER((NxU32)index, (NpShape*)this, "Material", NX_DBG_EVENTGROUP_BASIC_OBJECTS_STATIC_DATA);\
	NpScene* npScene = (NpScene*)&getNpActor()->getScene();					\
	npScene->hardwareAbstraction.onActorChange(getNpActor(), HWA_MA_DIRTY_SHAPE_MATERIAL); \
	}																		\
																			\
NxMaterialIndex	NP(ClassName)::getMaterial() const							\
	{																		\
	BEGINREAD																\
	return mShape->getMaterial();											\
	}																		\
																			\
void	NP(ClassName)::setSkinWidth(NxReal skinWidth)						\
	{																		\
	BEGINWRITE																\
	mShape->setSkinWidth(skinWidth);										\
	NX_DBG_SET_PARAMETER((NxReal)skinWidth, (NpShape*)this, "Skin Width", NX_DBG_EVENTGROUP_BASIC_OBJECTS_STATIC_DATA);\
	NpScene* npScene = (NpScene*)&getNpActor()->getScene();					\
	npScene->hardwareAbstraction.onActorChange(getNpActor(), HWA_MA_DIRTY_SHAPE_SKIN_WIDTH); \
	}																		\
																			\
NxReal	NP(ClassName)::getSkinWidth()	const								\
	{																		\
	BEGINREAD																\
	return mShape->getSkinWidth();											\
	}																		\
																			\
void NP(ClassName)::setCCDSkeleton(NxCCDSkeleton *s)						\
 	{																		\
 	BEGINWRITE																\
	NpShape::setCCDSkeleton(s);												\
 	}																		\
																			\
NxCCDSkeleton *	NP(ClassName)::getCCDSkeleton() const						\
 	{																		\
 	BEGINREAD																\
	if (mShape->getCCDSkeleton() != NULL)									\
		{																	\
 		return mShape->getCCDSkeleton()->getNxCCDSkeleton();				\
		}																	\
	else																	\
		{																	\
		return NULL;														\
		}																	\
 	}																		\
																			\
const char* NP(ClassName)::getName() const									\
	{																		\
	BEGINREAD																\
	return NpShape::getName();												\
	}																		\
																			\
void NP(ClassName)::setName(const char* debugName)							\
	{																		\
	BEGINWRITE																\
	NpShape::setName(debugName);											\
	}																		\
																			\
NxMat34 NP(ClassName)::getLocalPose() const									\
	{																		\
	BEGINREAD																\
	return NpShape::getLocalPose();											\
	}																		\
																			\
NxVec3 NP(ClassName)::getLocalPosition()		const						\
	{																		\
	BEGINREAD																\
	return NpShape::getLocalPosition();										\
	}																		\
																			\
NxMat33 NP(ClassName)::getLocalOrientation()	const						\
	{																		\
	BEGINREAD																\
	return NpShape::getLocalOrientation();									\
	}																		\
																			\
NxMat34 NP(ClassName)::getGlobalPose()			const						\
	{																		\
	BEGINREAD																\
	return NpShape::getGlobalPose();										\
	}																		\
																			\
NxVec3 NP(ClassName)::getGlobalPosition()		const						\
	{																		\
	BEGINREAD																\
	return NpShape::getGlobalPosition();									\
	}																		\
																			\
NxMat33 NP(ClassName)::getGlobalOrientation()	const						\
	{																		\
	BEGINREAD																\
	return NpShape::getGlobalOrientation();									\
	}																		\
																			\
bool NP(ClassName)::raycast(const NxRay& worldRay, NxReal maxDist, NxU32 hintFlags, NxRaycastHit& hit, bool firstHit)	const	\
	{																		\
	BEGINREAD																\
	return mShape->raycast(worldRay, maxDist, hintFlags, hit, firstHit);	\
	}																		\
																			\
bool NP(ClassName)::checkOverlapSphere(const NxSphere& worldSphere) const	\
	{																		\
	BEGINREAD																\
	return mShape->checkOverlapSphere(worldSphere);							\
	}																		\
																			\
bool NP(ClassName)::checkOverlapOBB(const NxBox& worldBox) const			\
	{																		\
	BEGINREAD																\
	return mShape->checkOverlapOBB(worldBox);								\
	}																		\
																			\
bool NP(ClassName)::checkOverlapAABB(const NxBounds3& worldBounds) const	\
	{																		\
	BEGINREAD																\
	return mShape->checkOverlapAABB(worldBounds);							\
	}																		\
																			\
bool NP(ClassName)::checkOverlapCapsule(const NxCapsule& worldCapsule) const	\
	{																		\
	BEGINREAD																\
	return mShape->checkOverlapCapsule(worldCapsule);						\
	}																		\
																			\
void NP(ClassName)::setGroupsMask(const NxGroupsMask& mask)					\
	{																		\
	BEGINWRITE																\
	NpScene* npScene = (NpScene*)&getNpActor()->getScene();					\
	npScene->hardwareAbstraction.onActorChange(getNpActor(), HWA_MA_DIRTY_SHAPE_GROUPS_MASK); \
	mShape->setGroupsMask(mask);											\
	NX_DBG_SET_PARAMETER((NxU32)(mask.bits0), (NpShape*)this, "Groups Mask bits0", NX_DBG_EVENTGROUP_BASIC_OBJECTS_STATIC_DATA);\
	NX_DBG_SET_PARAMETER((NxU32)(mask.bits1), (NpShape*)this, "Groups Mask bits1", NX_DBG_EVENTGROUP_BASIC_OBJECTS_STATIC_DATA);\
	NX_DBG_SET_PARAMETER((NxU32)(mask.bits2), (NpShape*)this, "Groups Mask bits2", NX_DBG_EVENTGROUP_BASIC_OBJECTS_STATIC_DATA);\
	NX_DBG_SET_PARAMETER((NxU32)(mask.bits3), (NpShape*)this, "Groups Mask bits3", NX_DBG_EVENTGROUP_BASIC_OBJECTS_STATIC_DATA);\
	}																		\
																			\
const NxGroupsMask NP(ClassName)::getGroupsMask() const						\
	{																		\
	BEGINREAD																\
	return mShape->getGroupsMask();											\
	}																		\
																			\
NI_COMPARTMENT_TYPES_DEFINITION(ClassName)									\
																			\
void* NP(ClassName)::is(NxShapeType type)									\
{																			\
	return (type == getType()) ? (void*)this : NULL;						\
}																			\
																			\
const void*	NP(ClassName)::is(NxShapeType type) const						\
{																			\
	return (type == getType()) ? (const void*)this : NULL;					\
}																			\


#endif

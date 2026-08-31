#ifndef MACRO_NX_PHYSICS_NP_JOINT
#define MACRO_NX_PHYSICS_NP_JOINT
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/
#include "NpJoint.h"
#include "NvJoint.h"

#ifndef NP
#define NP(ClassName) Np ## ClassName
#endif

#ifndef NPDESC
#define NPDESC(ClassName) Nx ## ClassName ## Desc
#endif


#define NPJOINT_DECLARATION(ClassName)																\
	public:																							\
								NP(ClassName)(void * userData, NvScene*);							\
	virtual						~NP(ClassName)();													\
																									\
	public:																							\
	virtual		void			getActors(NxActor** actor1, NxActor** actor2);						\
																									\
	virtual		void			setGlobalAnchor(const NxVec3&);										\
	virtual		NxVec3			getGlobalAnchor() const;											\
	virtual		void			setGlobalAxis(const NxVec3&);										\
	virtual		NxVec3			getGlobalAxis() const;												\
																									\
	virtual		NxJointState	getState();															\
																									\
	virtual		void			setBreakable(NxReal maxLForce, NxReal maxAForce);					\
	virtual		void			getBreakable(NxReal & l, NxReal & a);								\
																									\
	virtual		NxReal			getSolverExtrapolationFactor() const;								\
	virtual		void			setSolverExtrapolationFactor(NxReal solverExtrapolationFactor);		\
	virtual		bool			getUseAccelerationSpring() const;									\
	virtual		void			setUseAccelerationSpring(bool b);									\
																									\
	virtual		void			setLimitPoint(const NxVec3 & point, bool pointIsOnBody2 = true);	\
	virtual		bool			getLimitPoint(NxVec3 & worldLimitPoint);							\
	virtual		bool			addLimitPlane(const NxVec3 & normal, const NxVec3 & pointInPlane, NxReal restitution);	\
	virtual		void			purgeLimitPlanes();													\
	virtual		void			resetLimitPlaneIterator();											\
	virtual		bool			hasMoreLimitPlanes();												\
	virtual		bool			getNextLimitPlane(NxVec3 & planeNormal, NxReal & planeD, NxReal * restitution);			\
																									\
	virtual		const char*		getName()				const;										\
	virtual		void			setName(const char*);												\
																									\
	virtual		NxJointType				getType()		const;										\
																									\
	virtual		NxScene&		getScene() const;													\
	virtual		void			setNvJoint(NvJoint	* j);											\
																									\
	virtual		void *			is(NxJointType type);												\
																									\
																									\
	private:																						\
	NX_INLINE	NvJoint*				getJoint() { return joint;	}								\
	NX_INLINE	const NvJoint*			getJoint() const { return joint;}
																									



#define NPJOINT_DEFINITION(ClassName)													\
																						\
NP(ClassName)::NP(ClassName)(void * ud, NvScene* cs)									\
	{																					\
	NP(ClassName)::sendToVRD(true, true, false);										\
	appData = 0;																		\
	joint = 0;																			\
	userData = ud;																		\
	creationScene = cs;																	\
	}																					\
																						\
NP(ClassName)::~NP(ClassName)()															\
	{																					\
	if(joint)																			\
		joint->release();																\
		specificRemoveFromVRD();														\
	}																					\
																						\
																						\
void NP(ClassName)::getActors(NxActor** actor1, NxActor** actor2)						\
	{																					\
	BEGINREAD																			\
	*actor1 = actors[0];																\
	*actor2 = actors[1];																\
	}																					\
																						\
void NP(ClassName)::setGlobalAnchor(const NxVec3& d)									\
	{																					\
	BEGINWRITE																			\
	NpJoint::setGlobalAnchor(d);														\
	}																					\
																						\
void NP(ClassName)::setGlobalAxis(const NxVec3& axisIn)									\
	{																					\
	BEGINWRITE																			\
	NpJoint::setGlobalAxis(axisIn);														\
	}																					\
																						\
NxJointType NP(ClassName)::getType() const												\
	{																					\
	BEGINREAD																			\
	return getJoint()->getType();														\
	}																					\
																						\
																						\
void NP(ClassName)::setBreakable(NxReal l, NxReal a)									\
	{																					\
	NpJoint::setBreakable(l, a);														\
	}																					\
																						\
void NP(ClassName)::getBreakable(NxReal & l, NxReal & a)								\
	{																					\
	BEGINREAD																			\
	getJoint()->getBreakForce(l, a);													\
	}																					\
																						\
void NP(ClassName)::setSolverExtrapolationFactor(NxReal solverExtrapolationFactor)		\
	{																					\
	NpJoint::setSolverExtrapolationFactor(solverExtrapolationFactor);					\
	}																					\
																						\
NxReal NP(ClassName)::getSolverExtrapolationFactor() const								\
	{																					\
	BEGINREAD																			\
	return getJoint()->getSolverExtrapolationFactor();									\
	}																					\
																						\
void NP(ClassName)::setUseAccelerationSpring(bool b)									\
	{																					\
	NpJoint::setUseAccelerationSpring(b);												\
	}																					\
																						\
bool NP(ClassName)::getUseAccelerationSpring() const									\
	{																					\
	BEGINREAD																			\
	return getJoint()->getUseAccelerationSpring();										\
	}																					\
																						\
NxJointState NP(ClassName)::getState()													\
	{																					\
	BEGINREAD																			\
	return getJoint()->getState();														\
	}																					\
																						\
void NP(ClassName)::setLimitPoint(const NxVec3 & point, bool onBody2)					\
	{																					\
	BEGINWRITE																			\
	getJoint()->setLimitPoint(point, onBody2);											\
	if (NX_DBG_IS_CONNECTED())															\
		{																				\
		for (NxU32 i=0; i<limitPlaneCount; ++i)											\
			{																			\
			NX_DBG_REMOVE_OBJECT(((NxU8*)(NpJoint*)this+1+i), NX_DBG_EVENTGROUP_JOINTS_DATA); \
			}																			\
		NxMat34 actorPose;																\
		actorPose.id();																	\
		if (onBody2 && actors[1])														\
			actorPose = actors[1]->getGlobalPose();										\
		else if (!onBody2 && actors[0])													\
			actorPose = actors[0]->getGlobalPose();										\
		NX_DBG_CREATE_PARAMETER(actorPose%point, (NpJoint*)this, "Limit point", NX_DBG_EVENTGROUP_JOINTS); \
		NX_DBG_CREATE_PARAMETER((NxU32)(onBody2?2:1), (NpJoint*)this, "Limit actor", NX_DBG_EVENTGROUP_JOINTS); \
		}																				\
	wakeUp();																			\
	}																					\
																						\
bool NP(ClassName)::getLimitPoint(NxVec3 & worldLimitPoint)								\
	{																					\
	BEGINREAD																			\
	return getJoint()->getLimitPoint(worldLimitPoint) != 0;								\
	}																					\
																						\
bool NP(ClassName)::addLimitPlane(const NxVec3 & normal, const NxVec3 & pointInPlane, NxReal restitution)	\
	{																					\
	return NpJoint::addLimitPlane(normal, pointInPlane, restitution) != 0;				\
	}																					\
																						\
void NP(ClassName)::resetLimitPlaneIterator()											\
	{																					\
	BEGINWRITE																			\
	getJoint()->resetLimitPlaneIterator();												\
	}																					\
																						\
bool NP(ClassName)::hasMoreLimitPlanes()												\
	{																					\
	BEGINREAD																			\
	return getJoint()->hasMoreLimitPlanes() != 0;										\
	}																					\
																						\
bool NP(ClassName)::getNextLimitPlane(NxVec3 & planeNormal, NxReal & planeD, NxReal * restitution)			\
	{																					\
	BEGINREAD																			\
	return getJoint()->getNextLimitPlane(planeNormal, planeD, restitution) != 0;		\
	}																					\
																						\
const char* NP(ClassName)::getName() const												\
	{																					\
	BEGINREAD																			\
	return NpPhysicsSDK::getName((NpJoint*)this);										\
	}																					\
																						\
void NP(ClassName)::setName(const char* debugName)										\
	{																					\
	BEGINWRITE																			\
	NpPhysicsSDK::setName((NpJoint*)this, debugName);									\
	NX_DBG_SET_PARAMETER(debugName, (NpJoint*)this, "Name", NX_DBG_EVENTGROUP_JOINTS);  \
	}																					\
																						\
void NP(ClassName)::purgeLimitPlanes()													\
	{																					\
	BEGINWRITE																			\
	if (NX_DBG_IS_CONNECTED())															\
		{																				\
		for (NxU32 i=0; i<limitPlaneCount; ++i)											\
			{																			\
			NX_DBG_REMOVE_OBJECT(((NxU8*)(NpJoint*)this+1+i), NX_DBG_EVENTGROUP_JOINTS_DATA); \
			}																			\
		}																				\
	limitPlaneCount = 0;																\
	getJoint()->purgeLimitPlanes();														\
	}																					\
																						\
NxVec3	NP(ClassName)::getGlobalAnchor() const											\
	{																					\
	BEGINREAD																			\
	NxVec3 t;																			\
	NpJoint::getGlobalAnchor(t);														\
	return t;																			\
	}																					\
																						\
NxVec3	NP(ClassName)::getGlobalAxis() const											\
	{																					\
	BEGINREAD																			\
	NxVec3 t;																			\
	NpJoint::getGlobalAxis(t);															\
	return t;																			\
	}																					\
																						\
NxScene& NP(ClassName)::getScene()	const												\
	{																					\
	BEGINREAD																			\
	return *NpJoint::getNpScene();														\
	}																					\
																						\
void NP(ClassName)::setNvJoint(NvJoint * j)												\
	{																					\
	joint = j;																			\
	appData = (void*)j;																	\
	}																					\
																						\
void * NP(ClassName)::is(NxJointType type)												\
{																						\
	return (type == getType()) ? (void*)this : NULL;									\
}																						\
	
#endif

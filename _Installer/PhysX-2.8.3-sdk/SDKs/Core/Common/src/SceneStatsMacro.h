#ifndef NX_PHYSICS_SCENE_STATS_MACRO
#define NX_PHYSICS_SCENE_STATS_MACRO
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/
#ifdef NX_ENABLE_SCENE_STATS2
#define INIT_NX_SCENESTATS2 	NX_CREATE_SCENE_STAT(ActivePairs, 0xffffffff, true)					\
	NX_CREATE_SCENE_STAT(TotalPairs, 0xffffffff, true)												\
	NX_CREATE_SCENE_STAT(TotalFilteredPairs, 0xffffffff, true)										\
	NX_CREATE_SCENE_STAT(ActiveContactNotificationPairs, StatActivePairs, true)						\
	NX_CREATE_SCENE_STAT(TotalContactNotificationPairs, StatTotalPairs, true)	 					\
	NX_CREATE_SCENE_STAT(ActiveContactModificationPairs, StatActivePairs, true)						\
	NX_CREATE_SCENE_STAT(TotalContactModificationPairs, StatTotalPairs, true)	 					\
	\
	NX_CREATE_SCENE_STAT(TotalContacts, 0xffffffff, true)											\
	NX_CREATE_SCENE_STAT(TotalActorsInAwakeGroups, StatTotalActors, true)							\
	\
	NX_CREATE_SCENE_STAT(TotalActors, 0xffffffff, true)												\
	NX_CREATE_SCENE_STAT(ActiveDynamicActors, 0xffffffff, true)										\
	NX_CREATE_SCENE_STAT(TotalDynamicActors, StatTotalActors, true)									\
	NX_CREATE_SCENE_STAT(ActiveCompoundActors, StatActiveDynamicActors, true)						\
	NX_CREATE_SCENE_STAT(TotalCompoundActors, StatTotalActors, true)								\
	NX_CREATE_SCENE_STAT(ActiveCompoundActors3Plus, StatActiveCompoundActors, true)					\
	NX_CREATE_SCENE_STAT(TotalCompoundActors3Plus, StatTotalCompoundActors, true)					\
	NX_CREATE_SCENE_STAT(ActiveCompoundActors10Plus, StatActiveCompoundActors3Plus, true)			\
	NX_CREATE_SCENE_STAT(TotalCompoundActors10Plus, StatTotalCompoundActors3Plus, true)				\
	NX_CREATE_SCENE_STAT(TotalKinematicActors, StatTotalActors, true)								\
	\
	NX_CREATE_SCENE_STAT(TotalShapes, 0xffffffff, true)												\
	\
	NX_CREATE_SCENE_STAT(TotalStaticShapes, StatTotalShapes, true)									\
	NX_CREATE_SCENE_STAT(TotalMirroredShapes, 0xffffffff, true)										\
	NX_CREATE_SCENE_STAT(ActiveDynamicShapes, StatTotalDynamicShapes, true)							\
	NX_CREATE_SCENE_STAT(TotalDynamicShapes, StatTotalShapes, true)									\
	NX_CREATE_SCENE_STAT(ActiveTriggerShapes, 0xffffffff, false)									\
	NX_CREATE_SCENE_STAT(TotalTriggerShapes, 0xffffffff, false)										\
	NX_CREATE_SCENE_STAT(TotalCCDEnabledShapes, StatTotalShapes, true)								\
	\
	NX_CREATE_SCENE_STAT(ActiveBoxShapes, StatActiveDynamicShapes, false)							\
	NX_CREATE_SCENE_STAT(TotalBoxShapes, StatTotalShapes, false)									\
	NX_CREATE_SCENE_STAT(ActiveSphereShapes, StatActiveDynamicShapes, false)						\
	NX_CREATE_SCENE_STAT(TotalSphereShapes, StatTotalShapes, false)									\
	NX_CREATE_SCENE_STAT(ActiveCapsuleShapes, StatActiveDynamicShapes, false)						\
	NX_CREATE_SCENE_STAT(TotalCapsuleShapes, StatTotalShapes, false)								\
	NX_CREATE_SCENE_STAT(ActiveConvexShapes, StatActiveDynamicShapes, true)							\
	NX_CREATE_SCENE_STAT(TotalConvexShapes, StatTotalShapes, true)									\
	NX_CREATE_SCENE_STAT(ActiveBigConvexShapes, StatActiveDynamicShapes, true)						\
	NX_CREATE_SCENE_STAT(TotalBigConvexShapes, StatTotalShapes, true)								\
	NX_CREATE_SCENE_STAT(ActiveWheelShapes, StatActiveDynamicShapes, false)							\
	NX_CREATE_SCENE_STAT(TotalWheelShapes, StatTotalShapes, false)									\
	NX_CREATE_SCENE_STAT(ActivePlaneShapes, StatActiveDynamicShapes, false)							\
	NX_CREATE_SCENE_STAT(TotalPlaneShapes, StatTotalShapes, false)									\
	NX_CREATE_SCENE_STAT(ActiveHeightfieldShapes, StatActiveDynamicShapes, false)					\
	NX_CREATE_SCENE_STAT(TotalHeightfieldShapes, StatTotalShapes, false)							\
	NX_CREATE_SCENE_STAT(ActiveTriMeshShapes, StatActiveDynamicShapes, false)						\
	NX_CREATE_SCENE_STAT(TotalTriMeshShapes, StatTotalShapes, false)								\
	\
	NX_CREATE_SCENE_STAT(ActiveColliders, 0xffffffff, true)											\
	\
	NX_CREATE_SCENE_STAT(ActiveSwBoxBoxColliders, StatActiveColliders, false)						\
	NX_CREATE_SCENE_STAT(ActiveSwBoxSphereColliders, StatActiveColliders, false)					\
	NX_CREATE_SCENE_STAT(ActiveSwBoxCapsuleColliders, StatActiveColliders, false)					\
	NX_CREATE_SCENE_STAT(ActiveSwBoxConvexColliders, StatActiveColliders, false)					\
	NX_CREATE_SCENE_STAT(ActiveSwBoxWheelColliders, StatActiveColliders, false)						\
	NX_CREATE_SCENE_STAT(ActiveSwBoxPlaneColliders, StatActiveColliders, false)						\
	NX_CREATE_SCENE_STAT(ActiveSwBoxHeightfieldColliders, StatActiveColliders, false)				\
	NX_CREATE_SCENE_STAT(ActiveSwBoxTriMeshColliders, StatActiveColliders, false)					\
	NX_CREATE_SCENE_STAT(ActiveSwSphereSphereColliders, StatActiveColliders, false)					\
	NX_CREATE_SCENE_STAT(ActiveSwSphereCapsuleColliders, StatActiveColliders, false)				\
	NX_CREATE_SCENE_STAT(ActiveSwSphereConvexColliders, StatActiveColliders, false)					\
	NX_CREATE_SCENE_STAT(ActiveSwSphereWheelColliders, StatActiveColliders, false)					\
	NX_CREATE_SCENE_STAT(ActiveSwSpherePlaneColliders, StatActiveColliders, false)					\
	NX_CREATE_SCENE_STAT(ActiveSwSphereHeightfieldColliders, StatActiveColliders, false)			\
	NX_CREATE_SCENE_STAT(ActiveSwSphereTriMeshColliders, StatActiveColliders, false)	 			\
	NX_CREATE_SCENE_STAT(ActiveSwCapsuleCapsuleColliders, StatActiveColliders, false)	 			\
	NX_CREATE_SCENE_STAT(ActiveSwCapsuleConvexColliders, StatActiveColliders, false)	 			\
	NX_CREATE_SCENE_STAT(ActiveSwCapsuleWheelColliders, StatActiveColliders, false)	 				\
	NX_CREATE_SCENE_STAT(ActiveSwCapsulePlaneColliders, StatActiveColliders, false)	 				\
	NX_CREATE_SCENE_STAT(ActiveSwCapsuleHeightfieldColliders, StatActiveColliders, false)			\
	NX_CREATE_SCENE_STAT(ActiveSwCapsuleTriMeshColliders, StatActiveColliders, false)	 			\
	NX_CREATE_SCENE_STAT(ActiveSwConvexConvexColliders, StatActiveColliders, false)	 				\
	NX_CREATE_SCENE_STAT(ActiveSwConvexWheelColliders, StatActiveColliders, false)		 			\
	NX_CREATE_SCENE_STAT(ActiveSwConvexPlaneColliders, StatActiveColliders, false)		 			\
	NX_CREATE_SCENE_STAT(ActiveSwConvexHeightfieldColliders, StatActiveColliders, false)			\
	NX_CREATE_SCENE_STAT(ActiveSwConvexTriMeshColliders, StatActiveColliders, false)				\
	NX_CREATE_SCENE_STAT(ActiveSwWheelWheelColliders, StatActiveColliders, false)					\
	NX_CREATE_SCENE_STAT(ActiveSwWheelPlaneColliders, StatActiveColliders, false)					\
	NX_CREATE_SCENE_STAT(ActiveSwWheelHeightfieldColliders, StatActiveColliders, false)				\
	NX_CREATE_SCENE_STAT(ActiveSwWheelTriMeshColliders, StatActiveColliders, false)					\
	\
	NX_CREATE_SCENE_STAT(ActiveHwBoxBoxColliders, StatActiveColliders, false)						\
	NX_CREATE_SCENE_STAT(ActiveHwBoxSphereColliders, StatActiveColliders, false)					\
	NX_CREATE_SCENE_STAT(ActiveHwBoxCapsuleColliders, StatActiveColliders, false)					\
	NX_CREATE_SCENE_STAT(ActiveHwBoxConvexColliders, StatActiveColliders, false)					\
	NX_CREATE_SCENE_STAT(ActiveHwBoxWheelColliders, StatActiveColliders, false)						\
	NX_CREATE_SCENE_STAT(ActiveHwBoxPlaneColliders, StatActiveColliders, false)						\
	NX_CREATE_SCENE_STAT(ActiveHwBoxHeightfieldColliders, StatActiveColliders, false)				\
	NX_CREATE_SCENE_STAT(ActiveHwBoxTriMeshColliders, StatActiveColliders, false)					\
	NX_CREATE_SCENE_STAT(ActiveHwSphereSphereColliders, StatActiveColliders, false)					\
	NX_CREATE_SCENE_STAT(ActiveHwSphereCapsuleColliders, StatActiveColliders, false)				\
	NX_CREATE_SCENE_STAT(ActiveHwSphereConvexColliders, StatActiveColliders, false)					\
	NX_CREATE_SCENE_STAT(ActiveHwSphereWheelColliders, StatActiveColliders, false)					\
	NX_CREATE_SCENE_STAT(ActiveHwSpherePlaneColliders, StatActiveColliders, false)					\
	NX_CREATE_SCENE_STAT(ActiveHwSphereHeightfieldColliders, StatActiveColliders, false)			\
	NX_CREATE_SCENE_STAT(ActiveHwSphereTriMeshColliders, StatActiveColliders, false)				\
	NX_CREATE_SCENE_STAT(ActiveHwCapsuleCapsuleColliders, StatActiveColliders, false)				\
	NX_CREATE_SCENE_STAT(ActiveHwCapsuleConvexColliders, StatActiveColliders, false)				\
	NX_CREATE_SCENE_STAT(ActiveHwCapsuleWheelColliders, StatActiveColliders, false)					\
	NX_CREATE_SCENE_STAT(ActiveHwCapsulePlaneColliders, StatActiveColliders, false)					\
	NX_CREATE_SCENE_STAT(ActiveHwCapsuleHeightfieldColliders, StatActiveColliders, false)			\
	NX_CREATE_SCENE_STAT(ActiveHwCapsuleTriMeshColliders, StatActiveColliders, false)				\
	NX_CREATE_SCENE_STAT(ActiveHwConvexConvexColliders, StatActiveColliders, false)					\
	NX_CREATE_SCENE_STAT(ActiveHwConvexWheelColliders, StatActiveColliders, false)					\
	NX_CREATE_SCENE_STAT(ActiveHwConvexPlaneColliders, StatActiveColliders, false)					\
	NX_CREATE_SCENE_STAT(ActiveHwConvexHeightfieldColliders, StatActiveColliders, false)			\
	NX_CREATE_SCENE_STAT(ActiveHwConvexTriMeshColliders, StatActiveColliders, false)				\
	NX_CREATE_SCENE_STAT(ActiveHwWheelWheelColliders, StatActiveColliders, false)					\
	NX_CREATE_SCENE_STAT(ActiveHwWheelPlaneColliders, StatActiveColliders, false)					\
	NX_CREATE_SCENE_STAT(ActiveHwWheelHeightfieldColliders, StatActiveColliders, false)				\
	NX_CREATE_SCENE_STAT(ActiveHwWheelTriMeshColliders, StatActiveColliders, false)					\
	\
	NX_CREATE_SCENE_STAT(TotalColliders, 0xffffffff, true)											\
	NX_CREATE_SCENE_STAT(SwForcedColliders, StatTotalColliders, false)								\
	\
	NX_CREATE_SCENE_STAT(TotalSwBoxBoxColliders, StatTotalColliders, false)							\
	NX_CREATE_SCENE_STAT(TotalSwBoxSphereColliders, StatTotalColliders, false)						\
	NX_CREATE_SCENE_STAT(TotalSwBoxCapsuleColliders, StatTotalColliders, false)						\
	NX_CREATE_SCENE_STAT(TotalSwBoxConvexColliders, StatTotalColliders, false)						\
	NX_CREATE_SCENE_STAT(TotalSwBoxWheelColliders, StatTotalColliders, false)						\
	NX_CREATE_SCENE_STAT(TotalSwBoxPlaneColliders, StatTotalColliders, false)						\
	NX_CREATE_SCENE_STAT(TotalSwBoxHeightfieldColliders, StatTotalColliders, false)					\
	NX_CREATE_SCENE_STAT(TotalSwBoxTriMeshColliders, StatTotalColliders, false)						\
	NX_CREATE_SCENE_STAT(TotalSwSphereSphereColliders, StatTotalColliders, false)					\
	NX_CREATE_SCENE_STAT(TotalSwSphereCapsuleColliders, StatTotalColliders, false)					\
	NX_CREATE_SCENE_STAT(TotalSwSphereConvexColliders, StatTotalColliders, false)					\
	NX_CREATE_SCENE_STAT(TotalSwSphereWheelColliders, StatTotalColliders, false)					\
	NX_CREATE_SCENE_STAT(TotalSwSpherePlaneColliders, StatTotalColliders, false)					\
	NX_CREATE_SCENE_STAT(TotalSwSphereHeightfieldColliders, StatTotalColliders, false)				\
	NX_CREATE_SCENE_STAT(TotalSwSphereTriMeshColliders, StatTotalColliders, false)					\
	NX_CREATE_SCENE_STAT(TotalSwCapsuleCapsuleColliders, StatTotalColliders, false)					\
	NX_CREATE_SCENE_STAT(TotalSwCapsuleConvexColliders, StatTotalColliders, false)					\
	NX_CREATE_SCENE_STAT(TotalSwCapsuleWheelColliders, StatTotalColliders, false)					\
	NX_CREATE_SCENE_STAT(TotalSwCapsulePlaneColliders, StatTotalColliders, false)					\
	NX_CREATE_SCENE_STAT(TotalSwCapsuleHeightfieldColliders, StatTotalColliders, false)				\
	NX_CREATE_SCENE_STAT(TotalSwCapsuleTriMeshColliders, StatTotalColliders, false)					\
	NX_CREATE_SCENE_STAT(TotalSwConvexConvexColliders, StatTotalColliders, false)					\
	NX_CREATE_SCENE_STAT(TotalSwConvexWheelColliders, StatTotalColliders, false)					\
	NX_CREATE_SCENE_STAT(TotalSwConvexPlaneColliders, StatTotalColliders, false)					\
	NX_CREATE_SCENE_STAT(TotalSwConvexHeightfieldColliders, StatTotalColliders, false)				\
	NX_CREATE_SCENE_STAT(TotalSwConvexTriMeshColliders, StatTotalColliders, false)					\
	NX_CREATE_SCENE_STAT(TotalSwWheelWheelColliders, StatTotalColliders, false)						\
	NX_CREATE_SCENE_STAT(TotalSwWheelPlaneColliders, StatTotalColliders, false)						\
	NX_CREATE_SCENE_STAT(TotalSwWheelHeightfieldColliders, StatTotalColliders, false)				\
	NX_CREATE_SCENE_STAT(TotalSwWheelTriMeshColliders, StatTotalColliders, false)					\
	\
	NX_CREATE_SCENE_STAT(TotalHwBoxBoxColliders, StatTotalColliders, false)							\
	NX_CREATE_SCENE_STAT(TotalHwBoxSphereColliders, StatTotalColliders, false)						\
	NX_CREATE_SCENE_STAT(TotalHwBoxCapsuleColliders, StatTotalColliders, false)						\
	NX_CREATE_SCENE_STAT(TotalHwBoxConvexColliders, StatTotalColliders, false)						\
	NX_CREATE_SCENE_STAT(TotalHwBoxWheelColliders, StatTotalColliders, false)						\
	NX_CREATE_SCENE_STAT(TotalHwBoxPlaneColliders, StatTotalColliders, false)						\
	NX_CREATE_SCENE_STAT(TotalHwBoxHeightfieldColliders, StatTotalColliders, false)					\
	NX_CREATE_SCENE_STAT(TotalHwBoxTriMeshColliders, StatTotalColliders, false)						\
	NX_CREATE_SCENE_STAT(TotalHwSphereSphereColliders, StatTotalColliders, false)					\
	NX_CREATE_SCENE_STAT(TotalHwSphereCapsuleColliders, StatTotalColliders, false)					\
	NX_CREATE_SCENE_STAT(TotalHwSphereConvexColliders, StatTotalColliders, false)					\
	NX_CREATE_SCENE_STAT(TotalHwSphereWheelColliders, StatTotalColliders, false)					\
	NX_CREATE_SCENE_STAT(TotalHwSpherePlaneColliders, StatTotalColliders, false)					\
	NX_CREATE_SCENE_STAT(TotalHwSphereHeightfieldColliders, StatTotalColliders, false)				\
	NX_CREATE_SCENE_STAT(TotalHwSphereTriMeshColliders, StatTotalColliders, false)					\
	NX_CREATE_SCENE_STAT(TotalHwCapsuleCapsuleColliders, StatTotalColliders, false)					\
	NX_CREATE_SCENE_STAT(TotalHwCapsuleConvexColliders, StatTotalColliders, false)					\
	NX_CREATE_SCENE_STAT(TotalHwCapsuleWheelColliders, StatTotalColliders, false)					\
	NX_CREATE_SCENE_STAT(TotalHwCapsulePlaneColliders, StatTotalColliders, false)					\
	NX_CREATE_SCENE_STAT(TotalHwCapsuleHeightfieldColliders, StatTotalColliders, false)				\
	NX_CREATE_SCENE_STAT(TotalHwCapsuleTriMeshColliders, StatTotalColliders, false)					\
	NX_CREATE_SCENE_STAT(TotalHwConvexConvexColliders, StatTotalColliders, false)					\
	NX_CREATE_SCENE_STAT(TotalHwConvexWheelColliders, StatTotalColliders, false)					\
	NX_CREATE_SCENE_STAT(TotalHwConvexPlaneColliders, StatTotalColliders, false)					\
	NX_CREATE_SCENE_STAT(TotalHwConvexHeightfieldColliders, StatTotalColliders, false)				\
	NX_CREATE_SCENE_STAT(TotalHwConvexTriMeshColliders, StatTotalColliders, false)					\
	NX_CREATE_SCENE_STAT(TotalHwWheelWheelColliders, StatTotalColliders, false)						\
	NX_CREATE_SCENE_STAT(TotalHwWheelPlaneColliders, StatTotalColliders, false)						\
	NX_CREATE_SCENE_STAT(TotalHwWheelHeightfieldColliders, StatTotalColliders, false)				\
	NX_CREATE_SCENE_STAT(TotalHwWheelTriMeshColliders, StatTotalColliders, false)					\
	\
	NX_CREATE_SCENE_STAT(TotalTriggers, 0xffffffff, true)											\
	\
	NX_CREATE_SCENE_STAT(SwBoxBoxTriggers, StatTotalTriggers, false)								\
	NX_CREATE_SCENE_STAT(SwBoxSphereTriggers, StatTotalTriggers, false)								\
	NX_CREATE_SCENE_STAT(SwBoxCapsuleTriggers, StatTotalTriggers, false)							\
	NX_CREATE_SCENE_STAT(SwBoxConvexTriggers, StatTotalTriggers, false)								\
	NX_CREATE_SCENE_STAT(SwBoxWheelTriggers, StatTotalTriggers, false)								\
	NX_CREATE_SCENE_STAT(SwBoxPlaneTriggers, StatTotalTriggers, false)								\
	NX_CREATE_SCENE_STAT(SwBoxHeightfieldTriggers, StatTotalTriggers, false)						\
	NX_CREATE_SCENE_STAT(SwBoxTriMeshTriggers, StatTotalTriggers, false)							\
	NX_CREATE_SCENE_STAT(SwSphereSphereTriggers, StatTotalTriggers, false)							\
	NX_CREATE_SCENE_STAT(SwSphereCapsuleTriggers, StatTotalTriggers, false)							\
	NX_CREATE_SCENE_STAT(SwSphereConvexTriggers, StatTotalTriggers, false)							\
	NX_CREATE_SCENE_STAT(SwSphereWheelTriggers, StatTotalTriggers, false)							\
	NX_CREATE_SCENE_STAT(SwSpherePlaneTriggers, StatTotalTriggers, false)							\
	NX_CREATE_SCENE_STAT(SwSphereHeightfieldTriggers, StatTotalTriggers, false)						\
	NX_CREATE_SCENE_STAT(SwSphereTriMeshTriggers, StatTotalTriggers, false)							\
	NX_CREATE_SCENE_STAT(SwCapsuleCapsuleTriggers, StatTotalTriggers, false)						\
	NX_CREATE_SCENE_STAT(SwCapsuleConvexTriggers, StatTotalTriggers, false)							\
	NX_CREATE_SCENE_STAT(SwCapsuleWheelTriggers, StatTotalTriggers, false)							\
	NX_CREATE_SCENE_STAT(SwCapsulePlaneTriggers, StatTotalTriggers, false)							\
	NX_CREATE_SCENE_STAT(SwCapsuleHeightfieldTriggers, StatTotalTriggers, false)					\
	NX_CREATE_SCENE_STAT(SwCapsuleTriMeshTriggers, StatTotalTriggers, false)						\
	NX_CREATE_SCENE_STAT(SwConvexConvexTriggers, StatTotalTriggers, false)							\
	NX_CREATE_SCENE_STAT(SwConvexWheelTriggers, StatTotalTriggers, false)							\
	NX_CREATE_SCENE_STAT(SwConvexPlaneTriggers, StatTotalTriggers, false)							\
	NX_CREATE_SCENE_STAT(SwConvexHeightfieldTriggers, StatTotalTriggers, false)						\
	NX_CREATE_SCENE_STAT(SwConvexTriMeshTriggers, StatTotalTriggers, false)							\
	NX_CREATE_SCENE_STAT(SwWheelWheelTriggers, StatTotalTriggers, false)							\
	NX_CREATE_SCENE_STAT(SwWheelPlaneTriggers, StatTotalTriggers, false)							\
	NX_CREATE_SCENE_STAT(SwWheelHeightfieldTriggers, StatTotalTriggers, false)						\
	NX_CREATE_SCENE_STAT(SwWheelTriMeshTriggers, StatTotalTriggers, false)							\
	\
	NX_CREATE_SCENE_STAT(HwBoxBoxTriggers, StatTotalTriggers, false)								\
	NX_CREATE_SCENE_STAT(HwBoxSphereTriggers, StatTotalTriggers, false)								\
	NX_CREATE_SCENE_STAT(HwBoxCapsuleTriggers, StatTotalTriggers, false)							\
	NX_CREATE_SCENE_STAT(HwBoxConvexTriggers, StatTotalTriggers, false)								\
	NX_CREATE_SCENE_STAT(HwBoxWheelTriggers, StatTotalTriggers, false)								\
	NX_CREATE_SCENE_STAT(HwBoxPlaneTriggers, StatTotalTriggers, false)								\
	NX_CREATE_SCENE_STAT(HwBoxHeightfieldTriggers, StatTotalTriggers, false)						\
	NX_CREATE_SCENE_STAT(HwBoxTriMeshTriggers, StatTotalTriggers, false)							\
	NX_CREATE_SCENE_STAT(HwSphereSphereTriggers, StatTotalTriggers, false)							\
	NX_CREATE_SCENE_STAT(HwSphereCapsuleTriggers, StatTotalTriggers, false)							\
	NX_CREATE_SCENE_STAT(HwSphereConvexTriggers, StatTotalTriggers, false)							\
	NX_CREATE_SCENE_STAT(HwSphereWheelTriggers, StatTotalTriggers, false)							\
	NX_CREATE_SCENE_STAT(HwSpherePlaneTriggers, StatTotalTriggers, false)							\
	NX_CREATE_SCENE_STAT(HwSphereHeightfieldTriggers, StatTotalTriggers, false)						\
	NX_CREATE_SCENE_STAT(HwSphereTriMeshTriggers, StatTotalTriggers, false)							\
	NX_CREATE_SCENE_STAT(HwCapsuleCapsuleTriggers, StatTotalTriggers, false)						\
	NX_CREATE_SCENE_STAT(HwCapsuleConvexTriggers, StatTotalTriggers, false)							\
	NX_CREATE_SCENE_STAT(HwCapsuleWheelTriggers, StatTotalTriggers, false)							\
	NX_CREATE_SCENE_STAT(HwCapsulePlaneTriggers, StatTotalTriggers, false)							\
	NX_CREATE_SCENE_STAT(HwCapsuleHeightfieldTriggers, StatTotalTriggers, false)					\
	NX_CREATE_SCENE_STAT(HwCapsuleTriMeshTriggers, StatTotalTriggers, false)						\
	NX_CREATE_SCENE_STAT(HwConvexConvexTriggers, StatTotalTriggers, false)							\
	NX_CREATE_SCENE_STAT(HwConvexWheelTriggers, StatTotalTriggers, false)							\
	NX_CREATE_SCENE_STAT(HwConvexPlaneTriggers, StatTotalTriggers, false)							\
	NX_CREATE_SCENE_STAT(HwConvexHeightfieldTriggers, StatTotalTriggers, false)						\
	NX_CREATE_SCENE_STAT(HwConvexTriMeshTriggers, StatTotalTriggers, false)							\
	NX_CREATE_SCENE_STAT(HwWheelWheelTriggers, StatTotalTriggers, false)							\
	NX_CREATE_SCENE_STAT(HwWheelPlaneTriggers, StatTotalTriggers, false)							\
	NX_CREATE_SCENE_STAT(HwWheelHeightfieldTriggers, StatTotalTriggers, false)						\
	NX_CREATE_SCENE_STAT(HwWheelTriMeshTriggers, StatTotalTriggers, false)							\
	\
	NX_CREATE_SCENE_STAT(TotalMeshPageInstances, 0xffffffff, true)									\
	NX_CREATE_SCENE_STAT(MappedMeshPageInstances, StatTotalMeshPageInstances, true)					\
	NX_CREATE_SCENE_STAT(AutoPageMappings, 0xffffffff, true)										\
	NX_CREATE_SCENE_STAT(AutoPageUnmappings, 0xffffffff, true)										\
	NX_CREATE_SCENE_STAT(FailedPageMappings, 0xffffffff, false)										\
	\
	NX_CREATE_SCENE_STAT(TotalIslands, 0xffffffff, true)											\
	\
	NX_CREATE_SCENE_STAT(ActiveJoints, 0xffffffff, true)											\
	NX_CREATE_SCENE_STAT(TotalJoints, 0xffffffff, true)												\
	NX_CREATE_SCENE_STAT(DeadJoints, 0xffffffff, false)												\
	NX_CREATE_SCENE_STAT(ActiveD6Joints, StatActiveJoints, true)									\
	NX_CREATE_SCENE_STAT(TotalD6Joints, StatTotalJoints, true)										\
	NX_CREATE_SCENE_STAT(ActiveSphericalJoints, StatActiveJoints, false)							\
	NX_CREATE_SCENE_STAT(TotalSphericalJoints, StatTotalJoints, false)								\
	NX_CREATE_SCENE_STAT(ActiveRevoluteJoints, StatActiveJoints, false)								\
	NX_CREATE_SCENE_STAT(TotalRevoluteJoints, StatTotalJoints, false)								\
	NX_CREATE_SCENE_STAT(ActiveCylindricalJoints, StatActiveJoints, false)							\
	NX_CREATE_SCENE_STAT(TotalCylindricalJoints, StatTotalJoints, false)							\
	NX_CREATE_SCENE_STAT(ActiveDistanceJoints, StatActiveJoints, false)								\
	NX_CREATE_SCENE_STAT(TotalDistanceJoints, StatTotalJoints, false)								\
	NX_CREATE_SCENE_STAT(ActiveFixedJoints, StatActiveJoints, false)								\
	NX_CREATE_SCENE_STAT(TotalFixedJoints, StatTotalJoints, false)									\
	NX_CREATE_SCENE_STAT(ActivePointInPlaneJoints, StatActiveJoints, false)							\
	NX_CREATE_SCENE_STAT(TotalPointInPlaneJoints, StatTotalJoints, false)							\
	NX_CREATE_SCENE_STAT(ActivePointOnLineJoints, StatActiveJoints, false)							\
	NX_CREATE_SCENE_STAT(TotalPointOnLineJoints, StatTotalJoints, false)							\
	NX_CREATE_SCENE_STAT(ActivePrismaticJoints, StatActiveJoints, false)							\
	NX_CREATE_SCENE_STAT(TotalPrismaticJoints, StatTotalJoints, false)								\
	NX_CREATE_SCENE_STAT(ActivePulleyJoints, StatActiveJoints, false)								\
	NX_CREATE_SCENE_STAT(TotalPulleyJoints, StatTotalJoints, false)									\
	\
	NX_CREATE_SCENE_STAT(TotalRaycasts, 0xffffffff, true)											\
	NX_CREATE_SCENE_STAT(TotalOverlapTests, 0xffffffff, true)										\
	NX_CREATE_SCENE_STAT(TotalSweepTests, 0xffffffff, true)											\
	\
	NX_CREATE_SCENE_STAT(TotalFluidEmitters, 0xffffffff, true)										\
	NX_CREATE_SCENE_STAT(TotalFluids, 0xffffffff, true)												\
	NX_CREATE_SCENE_STAT(ActiveFluidParticles, 0xffffffff, true)									\
	NX_CREATE_SCENE_STAT(TotalFluidParticles, 0xffffffff, true)										\
	NX_CREATE_SCENE_STAT(TotalFluidPackets, 0xffffffff, true)										\
	\
	NX_CREATE_SCENE_STAT(ActiveCloths, 0xffffffff, true)											\
	NX_CREATE_SCENE_STAT(TotalCloths, 0xffffffff, true)												\
	NX_CREATE_SCENE_STAT(ActiveTearableClothPieces, 0xffffffff, true)								\
	NX_CREATE_SCENE_STAT(TotalTearableClothPieces, 0xffffffff, true)								\
	NX_CREATE_SCENE_STAT(ActiveClothVertices, 0xffffffff, true)										\
	NX_CREATE_SCENE_STAT(TotalClothVertices, 0xffffffff, true)										\
	NX_CREATE_SCENE_STAT(ActiveAttachedClothVertices, StatActiveClothVertices, true)				\
	NX_CREATE_SCENE_STAT(TotalAttachedClothVertices, StatTotalClothVertices, true)					\
	\
	NX_CREATE_SCENE_STAT(TotalForceFields, 0xffffffff, true)										\
	NX_CREATE_SCENE_STAT(TotalForceFieldShapes, 0xffffffff, true)									\
	NX_CREATE_SCENE_STAT(TotalBoxForceFieldShapes, StatTotalForceFieldShapes, true)					\
	NX_CREATE_SCENE_STAT(TotalSphereForceFieldShapes, StatTotalForceFieldShapes, true)				\
	NX_CREATE_SCENE_STAT(TotalCapsuleForceFieldShapes, StatTotalForceFieldShapes, true)				\
	NX_CREATE_SCENE_STAT(TotalConvexForceFieldShapes, StatTotalForceFieldShapes, true)				\
	NX_CREATE_SCENE_STAT(TotalForceFieldShapeGroups, 0xffffffff, true)								\
	NX_CREATE_SCENE_STAT(TotalForceFieldOverlaps, 0xffffffff, true)									\
	NX_CREATE_SCENE_STAT(TotalForceFieldRbOverlaps, StatTotalForceFieldOverlaps, true)				\
	NX_CREATE_SCENE_STAT(TotalForceFieldFluidOverlaps, StatTotalForceFieldOverlaps, true)			\
	NX_CREATE_SCENE_STAT(TotalForceFieldClothOverlaps, StatTotalForceFieldOverlaps, true)			\
	NX_CREATE_SCENE_STAT(TotalForceFieldSoftBodyOverlaps, StatTotalForceFieldOverlaps, true)		\
	NX_CREATE_SCENE_STAT(TotalForceFieldLinearKernels, 0xffffffff, true)							\
	NX_CREATE_SCENE_STAT(ActiveForceFieldLinearKernels, 0xffffffff, true)							\
	NX_CREATE_SCENE_STAT(ActiveForceFieldCustomKernels, 0xffffffff, true)							\
	\
	NX_CREATE_SCENE_STAT(WarnConvexShapesIgnored, 0xffffffff, false)								\
	NX_CREATE_SCENE_STAT(WarnClothShapesDropped, 0xffffffff, false)									\
	NX_CREATE_SCENE_STAT(WarnAxisConstraintsFailed, 0xffffffff, false)								\
	NX_CREATE_SCENE_STAT(WarnJointDrivesIgnored, 0xffffffff, false)									\
	NX_CREATE_SCENE_STAT(WarnPacketLimitedFluids, 0xffffffff, false)								\
	NX_CREATE_SCENE_STAT(WarnHeightfieldIndicesDropped, 0xffffffff, false)							\
	NX_CREATE_SCENE_STAT(WarnShapeMirrorFailed, 0xffffffff, false)									\
	NX_CREATE_SCENE_STAT(WarnBodyMirrorFailed, 0xffffffff, false)									
#endif

//NVIDIACOPYRIGHTBEGIN
///////////////////////////////////////////////////////////////////////////
// Copyright  2010 NVIDIA Corporation
// All rights reserved. www.nvidia.com
///////////////////////////////////////////////////////////////////////////
//NVIDIACOPYRIGHTEND

#endif

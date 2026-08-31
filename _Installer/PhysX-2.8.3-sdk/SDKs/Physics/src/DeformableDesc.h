#ifndef NX_PHYSICS_DEFORMABLE_DESC
#define NX_PHYSICS_DEFORMABLE_DESC

#include "NxClothDesc.h"
#include "NxSoftBodyDesc.h"

// a common descriptor for cloth and soft bodies

/*----------------------------------------------------------------------------*/

enum DeformableType
{
	DEFORMABLE_TYPE_NONE        = 0,
	DEFORMABLE_TYPE_CLOTH		= 1,
	DEFORMABLE_TYPE_SOFTBODY	= 2,
};

/*----------------------------------------------------------------------------*/

class DeformableDesc
{
public:
	DeformableType deformableType;	// cloth or softbody

	NxClothMesh    *clothMesh;		// cloth only
	NxSoftBodyMesh *softBodyMesh;	// softbody only
	NxMat34 globalPose;
	NxReal thickness;			// cloth only
	NxReal selfCollisionThickness;
	NxReal density;
	NxReal bendingStiffness;	// cloth only
	NxReal volumeStiffness;		// softbody only
	NxReal stretchingStiffness;
	NxReal hardStretchLimitationFactor;
	NxReal dampingCoefficient;
	NxReal friction;
	NxReal pressure;			// cloth only
	NxReal tearFactor;
	NxReal collisionResponseCoefficient;
	NxReal attachmentResponseCoefficient;
	NxReal attachmentTearFactor;
	NxReal toFluidResponseCoefficient;
	NxReal fromFluidResponseCoefficient;
	NxReal minAdhereVelocity;
	NxU32  solverIterations;
	NxU32  hierarchicalSolverIterations;
	NxVec3 externalAcceleration;
	NxVec3 windAcceleration;
	NxReal wakeUpCounter;
	NxReal sleepLinearVelocity;
	NxMeshData meshData;
	NxSoftBodySplitPairData splitPairData;
	NxClothSplitPairData clothSplitPairData;
	NxCollisionGroup collisionGroup;
	NxGroupsMask groupsMask;
	NxBounds3 validBounds;
	NxReal relativeGridSpacing;
	NxU16 forceFieldMaterial;
	NxU32 flags;				// cloth convention
	void* userData;
	const char* name;
	NxCompartment *	compartment;

	NX_INLINE DeformableDesc();

	NX_INLINE void clear();
	NX_INLINE bool isValid() const;

	// conversion
	NX_INLINE void loadFromClothDesc(const NxClothDesc &desc);
	NX_INLINE void saveToClothDesc(NxClothDesc &desc) const;

	NX_INLINE void loadFromSoftBodyDesc(const NxSoftBodyDesc &desc);
	NX_INLINE void saveToSoftBodyDesc(NxSoftBodyDesc &desc) const;

	static NX_INLINE NxU32 ConvertToClothFlags(NxU32 softBodyFlags);
	static NX_INLINE NxU32 ConvertToSoftBodyFlags(NxU32 clothFlags);
};

/*----------------------------------------------------------------------------*/

NX_INLINE DeformableDesc::DeformableDesc()
{
	clear();
}


/*----------------------------------------------------------------------------*/
void DeformableDesc::loadFromClothDesc(const NxClothDesc &desc)
{
	clear();
	deformableType					= DEFORMABLE_TYPE_CLOTH;

	clothMesh						= desc.clothMesh;
	softBodyMesh					= NULL;
	globalPose						= desc.globalPose;
	thickness						= desc.thickness;
	selfCollisionThickness			= desc.selfCollisionThickness;
	density							= desc.density;
	bendingStiffness				= desc.bendingStiffness;
	volumeStiffness					= 0.0f;
	stretchingStiffness				= desc.stretchingStiffness;
	hardStretchLimitationFactor     = desc.hardStretchLimitationFactor;
	dampingCoefficient				= desc.dampingCoefficient;
	friction						= desc.friction;
	pressure						= desc.pressure;
	tearFactor						= desc.tearFactor;
	attachmentTearFactor			= desc.attachmentTearFactor;
	attachmentResponseCoefficient	= desc.attachmentResponseCoefficient;
	collisionResponseCoefficient	= desc.collisionResponseCoefficient;
	toFluidResponseCoefficient      = desc.toFluidResponseCoefficient;
	fromFluidResponseCoefficient    = desc.fromFluidResponseCoefficient;
	minAdhereVelocity				= desc.minAdhereVelocity;
	flags							= desc.flags;
	solverIterations				= desc.solverIterations;
	hierarchicalSolverIterations	= desc.hierarchicalSolverIterations;
	wakeUpCounter					= desc.wakeUpCounter;
	sleepLinearVelocity				= desc.sleepLinearVelocity;
	collisionGroup					= desc.collisionGroup;
	externalAcceleration			= desc.externalAcceleration;
	windAcceleration				= desc.windAcceleration;
	groupsMask						= desc.groupsMask;
	validBounds						= desc.validBounds;
	relativeGridSpacing				= desc.relativeGridSpacing;
	meshData						= desc.meshData;
	clothSplitPairData				= desc.splitPairData;
	splitPairData.setToDefault();
	userData						= desc.userData;
	name							= desc.name;
	compartment						= desc.compartment;
	forceFieldMaterial				= desc.forceFieldMaterial;
}

/*----------------------------------------------------------------------------*/
void DeformableDesc::saveToClothDesc(NxClothDesc &desc) const
{
	desc.setToDefault();

	desc.clothMesh						= clothMesh;
	desc.globalPose						= globalPose;
	desc.thickness						= thickness;
	desc.selfCollisionThickness			= selfCollisionThickness;
	desc.density						= density;
	desc.bendingStiffness				= bendingStiffness;
	desc.stretchingStiffness			= stretchingStiffness;
	desc.hardStretchLimitationFactor    = hardStretchLimitationFactor;
	desc.dampingCoefficient				= dampingCoefficient;
	desc.friction						= friction;
	desc.pressure						= pressure;
	desc.tearFactor						= tearFactor;
	desc.attachmentTearFactor			= attachmentTearFactor;
	desc.attachmentResponseCoefficient	= attachmentResponseCoefficient;
	desc.collisionResponseCoefficient	= collisionResponseCoefficient;
	desc.toFluidResponseCoefficient     = toFluidResponseCoefficient;
	desc.fromFluidResponseCoefficient   = fromFluidResponseCoefficient;
	desc.minAdhereVelocity				= minAdhereVelocity;
	desc.flags							= flags;
	desc.solverIterations				= solverIterations;
	desc.hierarchicalSolverIterations	= hierarchicalSolverIterations;
	desc.wakeUpCounter					= wakeUpCounter;
	desc.sleepLinearVelocity			= sleepLinearVelocity;
	desc.collisionGroup					= collisionGroup;
	desc.externalAcceleration			= externalAcceleration;
	desc.windAcceleration				= windAcceleration;
	desc.groupsMask						= groupsMask;
	desc.validBounds					= validBounds;
	desc.relativeGridSpacing			= relativeGridSpacing;
	desc.meshData						= meshData;
	desc.splitPairData					= clothSplitPairData;
	desc.userData						= userData;
	desc.name							= name;
	desc.compartment					= compartment;
	desc.forceFieldMaterial				= forceFieldMaterial;				
}

/*----------------------------------------------------------------------------*/
void DeformableDesc::loadFromSoftBodyDesc(const NxSoftBodyDesc &desc)
{
	clear();
	deformableType					= DEFORMABLE_TYPE_SOFTBODY;

	clothMesh						= NULL;
	softBodyMesh					= desc.softBodyMesh;
	globalPose						= desc.globalPose;
	thickness						= desc.particleRadius;
	selfCollisionThickness			= desc.selfCollisionThickness;
	density							= desc.density;
	bendingStiffness				= 0.0f;
	volumeStiffness					= desc.volumeStiffness;
	stretchingStiffness				= desc.stretchingStiffness;
	hardStretchLimitationFactor     = desc.hardStretchLimitationFactor;
	dampingCoefficient				= desc.dampingCoefficient;
	friction						= desc.friction;
	pressure						= 0.0f;
	tearFactor						= desc.tearFactor;
	attachmentTearFactor			= desc.attachmentTearFactor;
	attachmentResponseCoefficient	= desc.attachmentResponseCoefficient;
	collisionResponseCoefficient	= desc.collisionResponseCoefficient;
	toFluidResponseCoefficient      = desc.toFluidResponseCoefficient;
	fromFluidResponseCoefficient    = desc.fromFluidResponseCoefficient;
	minAdhereVelocity				= desc.minAdhereVelocity;
	solverIterations				= desc.solverIterations;
	hierarchicalSolverIterations	= 0;  // not supported yet
	wakeUpCounter					= desc.wakeUpCounter;
	sleepLinearVelocity				= desc.sleepLinearVelocity;
	collisionGroup					= desc.collisionGroup;
	externalAcceleration			= desc.externalAcceleration;
	windAcceleration				= NxVec3(0.0f, 0.0f, 0.0f);
	groupsMask						= desc.groupsMask;
	validBounds						= desc.validBounds;
	relativeGridSpacing				= desc.relativeGridSpacing;
	meshData						= desc.meshData;
	splitPairData					= desc.splitPairData;
	clothSplitPairData.setToDefault();
	userData						= desc.userData;
	name							= desc.name;
	compartment						= desc.compartment;
	forceFieldMaterial				= desc.forceFieldMaterial;
	
	// conversion of flags to cloth convention
	flags = ConvertToClothFlags(desc.flags);
}

/*----------------------------------------------------------------------------*/
void DeformableDesc::saveToSoftBodyDesc(NxSoftBodyDesc &desc) const
{
	desc.setToDefault();

	desc.softBodyMesh					= softBodyMesh;
	desc.globalPose						= globalPose;
	desc.particleRadius					= thickness;
	desc.selfCollisionThickness			= selfCollisionThickness;
	desc.density						= density;
	desc.volumeStiffness				= volumeStiffness;
	desc.stretchingStiffness			= stretchingStiffness;
	desc.hardStretchLimitationFactor    = hardStretchLimitationFactor;
	desc.dampingCoefficient				= dampingCoefficient;
	desc.friction						= friction;
	desc.tearFactor						= tearFactor;
	desc.attachmentTearFactor			= attachmentTearFactor;
	desc.attachmentResponseCoefficient	= attachmentResponseCoefficient;
	desc.collisionResponseCoefficient	= collisionResponseCoefficient;
	desc.toFluidResponseCoefficient     = toFluidResponseCoefficient;
	desc.fromFluidResponseCoefficient   = fromFluidResponseCoefficient;
	desc.minAdhereVelocity				= minAdhereVelocity;
	// desc.solverIterations				= solverIterations;	// not supported yet
	desc.wakeUpCounter					= wakeUpCounter;
	desc.sleepLinearVelocity			= sleepLinearVelocity;
	desc.collisionGroup					= collisionGroup;
	desc.externalAcceleration			= externalAcceleration;
	desc.groupsMask						= groupsMask;
	desc.validBounds					= validBounds;
	desc.relativeGridSpacing			= relativeGridSpacing;
	desc.meshData						= meshData;
	desc.splitPairData					= splitPairData;
	desc.userData						= userData;
	desc.name							= name;
	desc.compartment					= compartment;
	desc.forceFieldMaterial				= forceFieldMaterial;
	
	// conversion of flags from cloth convention
	desc.flags = ConvertToSoftBodyFlags(flags);
}

/*----------------------------------------------------------------------------*/
NX_INLINE void DeformableDesc::clear()
{
	// this just clears the descriptor
	// the setToDefault methods are done in the cloth / soft body descriptors
	deformableType = DEFORMABLE_TYPE_NONE;

	clothMesh = NULL;	
	softBodyMesh = NULL;

	globalPose.id();
	thickness = 0.0f;
	selfCollisionThickness = 0.0f;
	density = 0.0f;
	bendingStiffness = 0.0f;
	volumeStiffness = 0.0f;
	
	stretchingStiffness = 0.0f;
	hardStretchLimitationFactor = 1.0f;
	dampingCoefficient = 0.0f;
	friction = 0.0f;
	pressure = 0.0f;
	tearFactor = 0.0f;
	collisionResponseCoefficient = 0.0f;
	attachmentResponseCoefficient = 0.0f;
	attachmentTearFactor = 0.0f;
	toFluidResponseCoefficient = 0.0f;
	fromFluidResponseCoefficient = 0.0f;
	minAdhereVelocity = 0.0f;
	solverIterations = 0;
	hierarchicalSolverIterations = 0;
	externalAcceleration.zero();
	wakeUpCounter = 0.0f;
	sleepLinearVelocity = 0.0f;
	meshData.setToDefault();
	splitPairData.setToDefault();
	clothSplitPairData.setToDefault();
	collisionGroup = 0;
	validBounds.setEmpty();
	relativeGridSpacing = 0.0f;
	flags = 0;
	userData = NULL;
	name = NULL;
	compartment = NULL;
	forceFieldMaterial = 0;
}

/*----------------------------------------------------------------------------*/
NX_INLINE bool DeformableDesc::isValid() const
{
	if (deformableType == DEFORMABLE_TYPE_CLOTH) {
		NxClothDesc clothDesc;
		saveToClothDesc(clothDesc);
		return clothDesc.isValid();
	}
	else if (deformableType == DEFORMABLE_TYPE_SOFTBODY) {
		NxSoftBodyDesc softBodyDesc;
		saveToSoftBodyDesc(softBodyDesc);
		return softBodyDesc.isValid();
	}
	else return false;
}

/*----------------------------------------------------------------------------*/
NX_INLINE NxU32 DeformableDesc::ConvertToClothFlags(NxU32 softBodyFlags)
{ 
	NxU32 clothFlags = 0;
	if (softBodyFlags & NX_SBF_STATIC			   ) clothFlags |= NX_CLF_STATIC;
	if (softBodyFlags & NX_SBF_DISABLE_COLLISION   ) clothFlags |= NX_CLF_DISABLE_COLLISION;
	if (softBodyFlags & NX_SBF_SELFCOLLISION	   ) clothFlags |= NX_CLF_SELFCOLLISION;
	if (softBodyFlags & NX_SBF_VISUALIZATION	   ) clothFlags |= NX_CLF_VISUALIZATION;
	if (softBodyFlags & NX_SBF_GRAVITY             ) clothFlags |= NX_CLF_GRAVITY;
	if (softBodyFlags & NX_SBF_VOLUME_CONSERVATION ) clothFlags |= NX_CLF_BENDING;
	if (softBodyFlags & NX_SBF_DAMPING             ) clothFlags |= NX_CLF_DAMPING;
	if (softBodyFlags & NX_SBF_COLLISION_TWOWAY    ) clothFlags |= NX_CLF_COLLISION_TWOWAY;
	if (softBodyFlags & NX_SBF_TEARABLE            ) clothFlags |= NX_CLF_TEARABLE;
	if (softBodyFlags & NX_SBF_HARDWARE            ) clothFlags |= NX_CLF_HARDWARE;
	if (softBodyFlags & NX_SBF_COMDAMPING		   ) clothFlags |= NX_CLF_COMDAMPING;
	if (softBodyFlags & NX_SBF_VALIDBOUNDS		   ) clothFlags |= NX_CLF_VALIDBOUNDS;
	if (softBodyFlags & NX_SBF_FLUID_COLLISION	   ) clothFlags |= NX_CLF_FLUID_COLLISION;
	if (softBodyFlags & NX_SBF_DISABLE_DYNAMIC_CCD ) clothFlags |= NX_CLF_DISABLE_DYNAMIC_CCD;
	if (softBodyFlags & NX_SBF_ADHERE		 	   ) clothFlags |= NX_CLF_ADHERE;
	if (softBodyFlags & NX_SBF_HARD_STRETCH_LIMITATION ) clothFlags |= NX_CLF_HARD_STRETCH_LIMITATION;

	// copy upper 16 bits too, since they encode settings for the switchable pipeline
	clothFlags |= softBodyFlags & 0xFFFF0000;

	return clothFlags; 
}

/*----------------------------------------------------------------------------*/
NX_INLINE NxU32 DeformableDesc::ConvertToSoftBodyFlags(NxU32 clothFlags)
{ 
	NxU32 softBodyFlags = 0;
	if (clothFlags & NX_CLF_STATIC			    ) softBodyFlags |= NX_SBF_STATIC;
	if (clothFlags & NX_CLF_DISABLE_COLLISION   ) softBodyFlags |= NX_SBF_DISABLE_COLLISION;
	if (clothFlags & NX_CLF_SELFCOLLISION	    ) softBodyFlags |= NX_SBF_SELFCOLLISION;
	if (clothFlags & NX_CLF_VISUALIZATION	    ) softBodyFlags |= NX_SBF_VISUALIZATION;
	if (clothFlags & NX_CLF_GRAVITY             ) softBodyFlags |= NX_SBF_GRAVITY;
	if (clothFlags & NX_CLF_BENDING             ) softBodyFlags |= NX_SBF_VOLUME_CONSERVATION;
	if (clothFlags & NX_CLF_DAMPING             ) softBodyFlags |= NX_SBF_DAMPING;
	if (clothFlags & NX_CLF_COLLISION_TWOWAY    ) softBodyFlags |= NX_SBF_COLLISION_TWOWAY;
	if (clothFlags & NX_CLF_TEARABLE            ) softBodyFlags |= NX_SBF_TEARABLE;
	if (clothFlags & NX_CLF_HARDWARE            ) softBodyFlags |= NX_SBF_HARDWARE;
	if (clothFlags & NX_CLF_COMDAMPING		    ) softBodyFlags |= NX_SBF_COMDAMPING;
	if (clothFlags & NX_CLF_VALIDBOUNDS		    ) softBodyFlags |= NX_SBF_VALIDBOUNDS;
	if (clothFlags & NX_CLF_FLUID_COLLISION		) softBodyFlags |= NX_SBF_FLUID_COLLISION;
	if (clothFlags & NX_CLF_DISABLE_DYNAMIC_CCD	) softBodyFlags |= NX_SBF_DISABLE_DYNAMIC_CCD;
	if (clothFlags & NX_CLF_ADHERE		 	    ) softBodyFlags |= NX_SBF_ADHERE;
	if (clothFlags & NX_CLF_HARD_STRETCH_LIMITATION ) softBodyFlags |= NX_SBF_HARD_STRETCH_LIMITATION;

	// copy upper 16 bits too, since they encode settings for the switchable pipeline
	softBodyFlags |= clothFlags & 0xFFFF0000;

	return softBodyFlags; 
}


#endif

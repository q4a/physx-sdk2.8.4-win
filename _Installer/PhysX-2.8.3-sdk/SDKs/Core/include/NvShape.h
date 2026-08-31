#ifndef NX_COLLISION_NV_SHAPE
#define NX_COLLISION_NV_SHAPE
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Physics.h"
#include "NxBounds3.h"
#include "NxPlane.h"
#include "NxWheelShapeDesc.h"
#include "NxUserEntityReport.h"

class NpShape;
class NxWheelContactData;
class NvTriangleMesh;
class NvRawTriangleMesh;
class NpTriangleMesh;
class NvConvexMesh;
class NpConvexMesh;
class NvHeightField;
class NpHeightField;
class NxShape;
class NvShape;
class NxActor;
class NvBody;
class NvScene;
class NvCCDSkeleton;
class NxSphere;
class NxTriangle;
class NxCapsule;
class NxRay;
struct NxRaycastHit;
class NvMeshQueryContext;
class NxUserWheelContactModify;

// PT: make sure the class below doesn't have a v-table, as some evil people memset it to zero in some places....... 
// AM: we could just add a ctor and remove the memsets!!

// PT: the unions below are super ugly and caused TTP 3845. Consider removing.
class NvShapeDesc
	{
	public:
	NxShapeType				type;
	NvBody	*				body;		//nonzero if dynamic
	NvShape *				compound;	//nonzero if in a compound
	NvCCDSkeleton *			ccdSkeleton;


	// user pointers

	NxActor *				nxActor;
	NxShape *				nxShape;
	NpShape *				npShape;	//not the same as NxShape because the types are not related in this case due to multiple inheritance.


	//features for shape types and compounds:
	NxActorGroup			actorGroup;
	NxDominanceGroup		actorDominanceGroup;
	NxU32					actorContactReportFlags;
	NxU32					actorFlags;

	//the below features are not used for compounds, only specific shape types:
	NxVec3					shape2ActorP;
	NxQuat					shape2ActorQ;
	NxCollisionGroup		collisionGroup;
	NxMaterialIndex			material;
	NxU32					shapeFlags;
	NxReal					skinWidth;
//mask based filtering:
	NxGroupsMask			groupMask;

#ifndef __CELLOS_LV2__
	NxU32					nonInteractingCompartmentTypes;
#endif


	/* subtype specific */
	union
		{
		struct
			{
			// GCC doesn't like classes with constructors in unions.
			//NxVec3 boxDims;
			Nx3F32 boxDims;
			};
		struct
			{
			NxReal	capsuleRadius;
			NxReal	capsuleHeight;
			NxU32	capsuleFlags;
			};
		struct
			{
			NxReal	sphereRadius;
			};
		struct
			{
			// GCC doesn't like classes with constructors in unions.
			//NxPlane plane;
			Nx3F32 planeNormal;
			NxReal planeDistance;
			};
		struct
			{
			NxReal wheelRadius;
			NxReal wheelSuspensionTravel;
			//NxSpringDesc wheelSuspension;// GCC doesn't like classes with constructors in unions.
			NxReal wheelSuspensionSpring;		
			NxReal wheelSuspensionDamper;		
			NxReal wheelSuspensionTargetValue;	
			//NxTireFunctionDesc wheelLongitudalTireForceFunction;// GCC doesn't like classes with constructors in unions.
			NxReal wheelLongitudalTireForceFunctionExtremumSlip, wheelLongitudalTireForceFunctionExtremumValue;
			NxReal wheelLongitudalTireForceFunctionAsymptoteSlip, wheelLongitudalTireForceFunctionAsymptoteValue;
			NxReal wheelLongitudalTireForceFunctionStiffnessFactor;

			//NxTireFunctionDesc wheelLateralTireForceFunction;// GCC doesn't like classes with constructors in unions.
			NxReal wheelLateralTireForceFunctionExtremumSlip, wheelLateralTireForceFunctionExtremumValue;
			NxReal wheelLateralTireForceFunctionAsymptoteSlip, wheelLateralTireForceFunctionAsymptoteValue;
			NxReal wheelLateralTireForceFunctionStiffnessFactor;

			NxReal inverseWheelMass;
			NxU32 wheelFlags;
			NxReal wheelMotorTorque;
			NxReal wheelBrakeTorque;
			NxReal wheelSteerAngle;
			NxUserWheelContactModify* wheelContactModify;
			NxReal wheelLngImpulse;
			NxReal wheelAxleSpeed;
			};
		struct 
			{
			NvTriangleMesh		*triangleMesh;
			NxU32				triangleMeshFlags;
			NxMeshPagingMode	triangleMeshPagingMode;
//#ifdef NX_SUPPORT_MESH_SCALE
	NxReal					triangleMeshScale;
//#endif
			};
		
		struct 
			{
			NvRawTriangleMesh *rawTriMesh;
			};

		struct
			{
			NxU8 *fwConvex;
			NvConvexMesh *convexMesh;
			NxU32			convexMeshFlags;
//#ifdef NX_SUPPORT_CONVEX_SCALE
	NxReal					convexMeshScale;
//#endif
			};
		struct
			{
			NvHeightField *	heightField;
			NxReal			heightScale;
			NxReal			rowScale;
			NxReal			columnScale;
			NxMaterialIndex	materialIndexHighBits;
			NxMaterialIndex	holeMaterial;
			NxU32			meshFlags;
			};

		struct
			{
			NxU32 shapeCountHint;
			};
		/*
		This part doesn't go over descriptors:
		struct
			{
			NxU32	numShapes;
			NvShapeDesc *shapeDescArray;
			};
		*/
		};
	};


class NvShape
	{
	public:
	virtual		void							release() = 0;

	virtual		NxShapeType						getType()							const	= 0;
	virtual		NvBody	*						getNvBody()							const	= 0;
	virtual		NvScene *						getNvScene()						const   = 0;	//AM: added for HSM use
	virtual		NvShape *						getSourceNvShape()					const	= 0;	//EH: added for HSM use
	virtual		void							setSourceNvShape(NvShape* sourceShape)		= 0;

//@	virtual		void							setNpShape(NpShape* npShape) = 0;
	virtual		NpShape*						getNpShape()						const = 0;
	virtual		NxShape**						getNxShapes()					= 0;

	/* COMMENT(djs): we need refs to NxShapes _and_ NpShapes? And we need getNxShapePtr _and_ getNxShape? */
//@	virtual		void							setNxShape(NxShape* nxShape) = 0;		//this is redundant if we also have getNxShapePtr.
	virtual		NxShape*						getNxShape() = 0;
	virtual		NxActor*						getNxActor() const = 0;

	virtual		void							computeWorldSphere(NxSphere&)		const	= 0;	//shape types implement this

	virtual		void							setMaterial(NxMaterialIndex)				= 0;
	virtual		NxMaterialIndex					getMaterial()						const	= 0;

	virtual		NxVec3							getShape2ActorP()					const   = 0;
	virtual		NxQuat							getShape2ActorQ()					const   = 0;
	virtual		void							setShape2ActorP(const NxVec3 & p)			= 0;
	virtual		void							setShape2ActorQ(const NxQuat & q)			= 0;
	virtual		void							setShape2ActorPQ(const NxVec3 & p, const NxQuat & q) = 0;	//combined call needed to avoid computing intermediates twice when both values change.


	/* COMMENT(djs): the flags need to be defined (as in NvJoint for example) */
	// Synchronous: accessed by both simulation and user thread
	virtual		void							setFlags(NxU32 f) = 0;
	virtual		NxU32							getFlags()							const	= 0;
	// Asynchronous: only accessed by user thread, NOT by simulation thread. NX_SF_DISABLE_SCENE_QUERIES.
	virtual		void							setAsyncFlags(NxU32 f)						= 0;
	virtual		NxU32							getAsyncFlags()						const	= 0;

	virtual		void							setSkinWidth(NxReal)						= 0;
	virtual		NxReal							getSkinWidth()						const	= 0;

	virtual		void							setGroup(NxCollisionGroup)					= 0;
	virtual		NxCollisionGroup				getGroup()							const	= 0;


	virtual		void							setGroupsMask(const NxGroupsMask&)			= 0;
	virtual		NxGroupsMask					getGroupsMask()						const	= 0;

#ifndef __CELLOS_LV2__
	virtual		void							setNonInteractingCompartmentTypes(NxU32 compartmentTypes) = 0;
	virtual		NxU32							getNonInteractingCompartmentTypes()	const	= 0;
#endif

	/* COMMENT(djs): flags need to be defined blah blah blah */

	virtual		void							setActorPublicFlags(NxU32 f) = 0;		//this is virtual cause the compound has a special impl right now that shoots the change off to all the child objects.
	virtual		void							setActorGroup(NxActorGroup g) = 0;		//this is virtual cause the compound has a special impl right now that shoots the change off to all the child objects.
	virtual		void							setActorDominanceGroup(NxDominanceGroup g) = 0;
	virtual		void							setActorContactReportFlags(NxU32 flags) = 0;


	virtual		void							resetUserActorPairFiltering() = 0;

	/* COMMENT(djs): an implementation artifact, or a real API call? */

	// Only used by the sync manager
	virtual		NxU32							getID() const = 0;

	virtual		void							getWorldBounds(NxBounds3&)		const	= 0;
	virtual		bool							raycast(const NxRay& worldRay, NxReal maxDist, NxU32 hintFlags, NxRaycastHit& hit, bool firstHit=false)	const	= 0;
	virtual		bool							checkOverlapSphere(const NxSphere& worldSphere)		const		= 0;
	virtual		bool							checkOverlapOBB(const NxBox& worldBox)				const		= 0;
	virtual		bool							checkOverlapAABB(const NxBounds3& worldBounds)		const		= 0;
	virtual		bool							checkOverlapCapsule(const NxCapsule& worldCapsule)	const		= 0;

	virtual		void							setCCDSkeleton(NvCCDSkeleton *) = 0;
	virtual		NvCCDSkeleton *					getCCDSkeleton() const			= 0;


	/* COMMENT(djs): better to preface methods with shape type (getSphereRadius etc). */

	//shape type specific specials:
	//I added empty default implementations for these ones because only the correct type of shape has to implement them.

	//box shape:
	virtual		void							setDimensions(const NxVec3&)	= 0;
	/* QUERY(djs): Why is it evil? And if it is evil why not exorcise it? (and other similar methods) */
	virtual		NxVec3							getBoxDimensions()			const= 0;

	//capsule shape:
	virtual		void							setRadius(NxReal radius)		= 0;
	virtual		void							setHeight(NxReal height)		= 0;
	virtual		NxReal							getRadius()			const		= 0;
	virtual		NxReal							getHeight()			const		= 0;
	virtual		NxU32							getCapsuleFlags()	const		= 0;
	virtual		void							setCapsuleFlags(NxU32 f)		= 0;
	/* COMMENT(djs): if we could, why not do it? */
	virtual		void							getWorldCapsule(NxCapsule& worldCapsule)	const = 0;	//TODO: this is a 'macro' could pull out!
	//plane shape:
	virtual		NxPlane							getPlaneEquation()			const	= 0;
	virtual		void							setPlaneEquation(const NxPlane &)	= 0;
	// wheel Shape:

	virtual		void							setWheelRadius(NxReal) = 0;
	virtual		NxReal							getWheelRadius() const = 0;
	virtual		void							setWheelSuspensionTravel(NxReal)  = 0;
	virtual		NxReal							getWheelSuspensionTravel() const = 0;
	virtual		void							setWheelInverseMass(NxReal) = 0;
	virtual		NxReal							getWheelInverseMass() const = 0;
	virtual		void							setWheelFlags(NxU32) = 0;
	virtual		NxU32							getWheelFlags() const = 0;
	virtual		void							setWheelMotorTorque(NxReal) = 0;
	virtual		NxReal							getWheelMotorTorque() const = 0;
	virtual		void							setWheelBrakeTorque(NxReal) = 0;
	virtual		NxReal							getWheelBrakeTorque() const = 0;
	virtual		void							setWheelSteerAngle(NxReal) = 0;
	virtual		NxReal							getWheelSteerAngle() const = 0;
	virtual		void							setWheelSuspension(NxSpringDesc) = 0;
	virtual		NxSpringDesc					getWheelSuspension() const = 0;
	virtual		void							setWheelLongitudalTireForceFunction(NxTireFunctionDesc) = 0;
	virtual		NxTireFunctionDesc				getWheelLongitudalTireForceFunction() const = 0;
	virtual		void							setWheelLateralTireForceFunction(NxTireFunctionDesc) = 0;
	virtual		NxTireFunctionDesc				getWheelLateralTireForceFunction() const = 0;
	virtual		void							setWheelLngImpulse(NxReal) = 0;
	virtual		NxReal							getWheelLngImpulse() const = 0;
	virtual		void							setWheelAxleSpeed(NxReal) = 0;
	virtual		NxReal							getWheelAxleSpeed() const = 0;
	virtual		NvShape*						getContact(NxWheelContactData & dest) const = 0;	//returns true if we have ground contact, and fills in args in that case.


	

	//sphere shape:								
	//also implements get/setRadius() listed under capsule!

	//convex shape:
	virtual		NvConvexMesh*					getNvConvexMesh()				= 0;
	virtual		const NvConvexMesh*				getNvConvexMesh()		const	= 0;
	virtual		void							setConvexMesh(NvConvexMesh*)	= 0;
	virtual		NpConvexMesh*					getConvexMesh()					= 0;

	//triangle mesh shape:
	virtual		NvTriangleMesh*					getNvTriangleMesh()				= 0;
	virtual		const NvTriangleMesh*			getNvTriangleMesh()		const	= 0;
	virtual		void							setTriangleMesh(NvTriangleMesh*)= 0;

	virtual		NpTriangleMesh*					getTriangleMesh()				= 0;

	virtual		bool							mapPageInstance(NxU32 pageIndex) = 0;
	virtual		void							unmapPageInstance(NxU32 pageIndex) = 0;
	virtual		bool							isPageInstanceMapped(NxU32 pageIndex) const = 0;

	/* COMMENT(djs): need to specify the possible flags */
	virtual		NxU32							getMeshFlags()			const	= 0;
	virtual		void							setMeshFlags(NxU32 f)			= 0;

	virtual		NxMeshPagingMode				getMeshPagingMode()	const					= 0;
	virtual		void							setMeshPagingMode(NxMeshPagingMode m)		= 0;

	virtual		NxU32							getTriangle(NxTriangle& triangle, NxTriangle* edgeTri, NxU32* returnedFlags, NxTriangleID triangleIndex, bool worldSpaceTranslation=true, bool worldSpaceRotation=true) const = 0;
	virtual		bool							overlapAABBTriangles(const NxBounds3 bounds, NxU32 flags, NxUserEntityReport<NxU32>* callback)			const = 0;

//#ifdef NX_SUPPORT_MESH_SCALE
	virtual		NxF32							getScale()				const	= 0;
	virtual		void							setScale(NxF32 scale)			= 0;
//#endif

	//heightfield shape:
	virtual		NvHeightField*					getNvHeightField()				= 0;
	virtual		const NvHeightField*			getNvHeightField() const		= 0;
	virtual		void							setHeightField(NvHeightField*)	= 0;
	virtual		NpHeightField*					getHeightField()				= 0;
	virtual		NxReal							getHeightScaleV() const			= 0;
	virtual		void							setHeightScale(NxReal)			= 0;
	virtual		NxReal							getRowScaleV() const				= 0;
	virtual		void							setRowScale(NxReal)				= 0;
	virtual 	NxReal							getColumnScaleV() const			= 0;
	virtual 	void							setColumnScale(NxReal)			= 0;
	virtual		NxMaterialIndex					getMaterialIndexHighBitsV()const	= 0;
	virtual		void							setMaterialIndexHighBits(NxMaterialIndex)= 0;
	virtual		NxMaterialIndex					getHoleMaterialV()		const	= 0;
	virtual		void							setHoleMaterial(NxMaterialIndex)= 0;

	virtual bool								isShapePointOnHeightFieldV(NxReal x, NxReal z) const = 0;
	virtual NxReal								getHeightAtShapePointV(NxReal x, NxReal z) const = 0;
	virtual NxMaterialIndex						getMaterialAtShapePointV(NxReal x, NxReal z) const = 0;
	virtual NxVec3								getNormalAtShapePointV(NxReal x, NxReal z) const = 0;
	virtual NxVec3								getSmoothNormalAtShapePointV(NxReal x, NxReal z) const = 0;

	//compound shape:
	virtual		const NvShape**					getNvShapes()			const	= 0;	//if its not a compound, it just returns 0.
	virtual		NvShape**						getNvShapes()					= 0;
	virtual		NxU32							getNbShapes()			const	= 0;
	virtual		void							moveToCompound(NvShape &)		= 0;	//moves a noncompound shape that used to be top level to the scene into this compound.
	virtual		NvShape*						getNvShapeOwner()				= 0;	//For compound removal. Returns the owner NvShape. Returns this if NfShape.

	protected:
	~NvShape() {}	//use release() plz.
	};

#endif

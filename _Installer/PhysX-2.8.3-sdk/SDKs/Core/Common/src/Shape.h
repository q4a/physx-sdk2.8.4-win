#ifndef NX_COLLISION_SHAPE
#define NX_COLLISION_SHAPE
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/
#include "Physics.h"
#include "NxBounds3.h"
#include "Allocateable.h"
#ifdef NX_HARDWARE_IMPL
#include "SyncManager.h"
#endif
#include "Opcode.h"
#include "NvShape.h"
#include "CoreElement.h"
#include "RbActor.h"
#include "Body.h"
#include "QP.h"
#include "CoreInteraction.h"

class NxRay;
class NxUserRaycastReport;
class NxShapeDesc;
class InertiaTensorComputer;
class NxSphere;
class NxBox;
class NxShape;
class NxActor;
class SourceCCDSkeleton;
class CCDSkeleton;
class CCDSkeleton2;
//proxy
class NpShape;

namespace NxFoundation
{
	class DebugRenderable;
}

const NxU32 SHAPE_DEBUG_COLOR = 0xffffffff;
const NxU32 TRIGGER_DEBUG_COLOR = 0xffff00ff;

#undef INC_SCENE_STAT
#undef DEC_SCENE_STAT
#ifdef NX_ENABLE_SCENE_STATS2
#define INC_SCENE_STAT(x) scene->stats2.incStat(x)
#define DEC_SCENE_STAT(x) scene->stats2.decStat(x)
#else
#define INC_SCENE_STAT(x)
#define DEC_SCENE_STAT(x)
#endif
/**
Implementation subclass for the various collision shapes.
An instance of a subclass can be created by calling the create*() method of the NxCollision class.
*/
//namespace NxCollision {
class BroadPhase;
class Body;
class PlaneShape;
class SphereShape;
class BoxShape;
class CapsuleShape;
class ConvexShape;
class TriangleMeshShape;
class HeightFieldShape;
class Scene;

struct NxRaycastHit;

class Shape : public NvShape, public CoreElement
	{
	protected:
												Shape(const NvShapeDesc& desc, Scene*, CoreActor& actor, NxU32 uid);
	virtual										~Shape();
	public:
	virtual		NxVec3							getShape2ActorP()					const;
	virtual		NxQuat							getShape2ActorQ()					const;
	virtual		void							setShape2ActorP(const NxVec3& p);
	virtual		void							setShape2ActorQ(const NxQuat& q);
	virtual		void							setShape2ActorPQ(const NxVec3& p, const NxQuat& q);	//combined call needed to avoid computing intermediates twice when both values change.
	virtual		NxActor*						getNxActor() const					{ return nxActor;	}	
	virtual		void							setCCDSkeleton(NvCCDSkeleton*);
	virtual		NvCCDSkeleton*					getCCDSkeleton() const;
	virtual		NxShape**						getNxShapes()					{ return &mNxShape; }
	virtual		NvScene *						getNvScene()	const; //AM: added for HSM use
	virtual		NvShape *						getSourceNvShape() const		{ return sourceShape; } // EH: added for HSM use
	virtual		void							setSourceNvShape(NvShape* srcShape)	{ sourceShape = srcShape; }


	//blank implementations of virtuals specific to certain shape types:
	//box shape:
	virtual		void							setDimensions(const NxVec3&)	{ NX_ASSERT(0); }
	virtual		NxVec3							getBoxDimensions()			const{ NX_ASSERT(0); return NxVec3(); }
	//capsule shape:
	virtual		void							setRadius(NxReal radius)		{ NX_ASSERT(0); }
	virtual		void							setHeight(NxReal height)		{ NX_ASSERT(0); }
	virtual		NxReal							getRadius()			const		{ NX_ASSERT(0); return 0; }
	virtual		NxReal							getHeight()			const		{ NX_ASSERT(0); return 0; }
	virtual		NxU32							getCapsuleFlags()	const		{ NX_ASSERT(0); return 0; }
	virtual		void							setCapsuleFlags(NxU32 f)		{ NX_ASSERT(0); }
	virtual		void							getWorldCapsule(NxCapsule& worldCapsule)	const { NX_ASSERT(0); }
	//plane shape:
	virtual		NxPlane							getPlaneEquation()			const	{ NX_ASSERT(0); return NxPlane(); }
	virtual		void							setPlaneEquation(const NxPlane&)	{ NX_ASSERT(0); }

	//wheel shape:
	virtual		void							setWheelRadius(NxReal) { NX_ASSERT(0); }
	virtual		NxReal							getWheelRadius() const { NX_ASSERT(0); return 0; }
	virtual		void							setWheelSuspensionTravel(NxReal) { NX_ASSERT(0); }
	virtual		NxReal							getWheelSuspensionTravel() const { NX_ASSERT(0); return 0; }
	virtual		void							setWheelInverseMass(NxReal) { NX_ASSERT(0); }
	virtual		NxReal							getWheelInverseMass() const { NX_ASSERT(0); return 0; }
	virtual		void							setWheelFlags(NxU32)  { NX_ASSERT(0); }
	virtual		NxU32							getWheelFlags() const { NX_ASSERT(0); return 0; }
	virtual		void							setWheelMotorTorque(NxReal) { NX_ASSERT(0); }
	virtual		NxReal							getWheelMotorTorque() const { NX_ASSERT(0); return 0; }
	virtual		void							setWheelBrakeTorque(NxReal) { NX_ASSERT(0); }
	virtual		NxReal							getWheelBrakeTorque() const { NX_ASSERT(0); return 0; }
	virtual		void							setWheelSteerAngle(NxReal) { NX_ASSERT(0); }
	virtual		NxReal							getWheelSteerAngle() const { NX_ASSERT(0); return 0; }
	virtual		void							setWheelSuspension(NxSpringDesc) { NX_ASSERT(0); }
	virtual		NxSpringDesc					getWheelSuspension() const  { NX_ASSERT(0); return NxSpringDesc(); }
	virtual		void							setWheelLongitudalTireForceFunction(NxTireFunctionDesc) { NX_ASSERT(0); }
	virtual		NxTireFunctionDesc				getWheelLongitudalTireForceFunction() const  { NX_ASSERT(0); return NxTireFunctionDesc(); }
	virtual		void							setWheelLateralTireForceFunction(NxTireFunctionDesc) { NX_ASSERT(0); }
	virtual		NxTireFunctionDesc				getWheelLateralTireForceFunction() const  { NX_ASSERT(0); return NxTireFunctionDesc(); }
	virtual		void							setWheelLngImpulse(NxReal)  { NX_ASSERT(0); }
	virtual		NxReal							getWheelLngImpulse() const  { NX_ASSERT(0); return 0; }
	virtual		void							setWheelAxleSpeed(NxReal)  { NX_ASSERT(0); }
	virtual		NxReal							getWheelAxleSpeed() const  { NX_ASSERT(0); return 0; }
	virtual		NvShape*						getContact(NxWheelContactData& dest) const { NX_ASSERT(0); return NULL; }

	//sphere shape:								
	//also implements get/setRadius() listed under capsule!
	//convex shape:
	virtual		NvConvexMesh*					getNvConvexMesh()				{ NX_ASSERT(0); return 0; }
	virtual		const NvConvexMesh*				getNvConvexMesh()		const	{ NX_ASSERT(0); return 0; }
	virtual		void							setConvexMesh(NvConvexMesh*)	{ NX_ASSERT(0); }
	virtual		NpConvexMesh*					getConvexMesh()					{ NX_ASSERT(0); return 0; }

	//triangle mesh shape:
	virtual		NvTriangleMesh*					getNvTriangleMesh()				{ NX_ASSERT(0); return 0; }
	virtual		const NvTriangleMesh*			getNvTriangleMesh()		const	{ NX_ASSERT(0); return 0; }
	virtual		void							setTriangleMesh(NvTriangleMesh*){ NX_ASSERT(0); }
	virtual		NpTriangleMesh*					getTriangleMesh()				{ NX_ASSERT(0); return 0; }
	virtual		NxU32							getMeshFlags()			const	{ NX_ASSERT(0); return 0; }
	virtual		void							setMeshFlags(NxU32 f)			{ NX_ASSERT(0); }
	virtual		NxMeshPagingMode				getMeshPagingMode()		const	{ NX_ASSERT(0); return NX_MESH_PAGING_MANUAL; }
	virtual		void							setMeshPagingMode(NxMeshPagingMode m)		{ NX_ASSERT(0); }
	virtual		NxU32							getTriangle(NxTriangle& triangle, NxTriangle* edgeTri, NxU32* returnedFlags, NxTriangleID triangleIndex, bool worldSpaceTranslation, bool worldSpaceRotation) const { NX_ASSERT(0); return 0; }
	virtual		bool							overlapAABBTriangles(const NxBounds3 bounds, NxU32 flags, NxUserEntityReport<NxU32>* callback)			const { NX_ASSERT(0); return false; }

//#ifdef NX_SUPPORT_MESH_SCALE
	virtual		NxF32							getScale()				const	{ NX_ASSERT(0); return 0; }
	virtual		void							setScale(NxF32 scale)			{ NX_ASSERT(0); }
//#endif
 
	//heightfield shape:
	virtual		NvHeightField*					getNvHeightField()				{ NX_ASSERT(0); return 0; };
	virtual		const NvHeightField*			getNvHeightField() const		{ NX_ASSERT(0); return 0; };
	virtual		void							setHeightField(NvHeightField*)	{ NX_ASSERT(0); };
	virtual		NpHeightField*					getHeightField()				{ NX_ASSERT(0); return 0; };
	virtual		NxReal							getHeightScaleV() const			{ NX_ASSERT(0); return 0; };
	virtual		void							setHeightScale(NxReal)			{ NX_ASSERT(0); };
	virtual		NxReal							getRowScaleV() const				{ NX_ASSERT(0); return 0; };
	virtual		void							setRowScale(NxReal)				{ NX_ASSERT(0); };
	virtual 	NxReal							getColumnScaleV() const			{ NX_ASSERT(0); return 0; };
	virtual 	void							setColumnScale(NxReal)			{ NX_ASSERT(0); };
	virtual		NxMaterialIndex					getMaterialIndexHighBitsV() const { NX_ASSERT(0); return 0; };
	virtual		void							setMaterialIndexHighBits(NxMaterialIndex) { NX_ASSERT(0); };
	virtual		NxMaterialIndex					getHoleMaterialV() const			{ NX_ASSERT(0); return 0; };
	virtual		void							setHoleMaterial(NxMaterialIndex){ NX_ASSERT(0); };

	virtual		bool							isShapePointOnHeightFieldV(NxReal x, NxReal z) const { NX_ASSERT(0); return 0; }
	virtual		NxReal							getHeightAtShapePointV(NxReal x, NxReal z) const { NX_ASSERT(0); return 0; }
	virtual		NxMaterialIndex					getMaterialAtShapePointV(NxReal x, NxReal z) const { NX_ASSERT(0); return 0; }
	virtual		NxVec3							getNormalAtShapePointV(NxReal x, NxReal z) const { NX_ASSERT(0); return NxVec3(0,0,0); }
	virtual		NxVec3							getSmoothNormalAtShapePointV(NxReal x, NxReal z) const { NX_ASSERT(0); return NxVec3(0,0,0); }

	//compound shape:
	virtual		const NvShape**					getNvShapes()			const	{ NX_ASSERT(0); return 0; }	//if its not a compound, it just returns 0.
	virtual		NvShape**						getNvShapes()					{ NX_ASSERT(0); return 0; }
	virtual		NxU32							getNbShapes()			const	{ NX_ASSERT(0); return 0; }
	virtual		void							moveToCompound(NvShape&)		{ NX_ASSERT(0); }
	virtual		NvShape*						getNvShapeOwner()				{ return static_cast<NvShape*>(&getRbActor()); }


	enum InternalFlag
	{
		INVALID_BOUNDS					= (1<<0),	// Bounds should be updated

		IN_PRUNER_UPDATE_LIST			= (1<<1),	// The shape is in the Scene::bpShapes list.
													// This is to avoid adding the shapes multiple times.

		IN_PRUNER_UPDATE_LIST_PERSISTENT= (1<<2),	// This shape should stay in the Scene::bpShapes list
													// between frames until explicitly removed. This happens
													// when in an active body. If the body deactivates the shape
													// is removed.

		IN_PRUNER_UPDATE_LIST_VOLATILE	= (1<<3),	// This shape has been added only to be removed from the
													// list next update. The flag prohibits Body::onDeactivate to
													// remove the shape from the list in case it was added by a user update
													// this frame.

		IN_BP_UPDATE_LIST				= (1<<4),	// As with IN_PRUNER_UPDATE_LIST but for BroadPhase::updateList
		IN_BP_UPDATE_LIST_PERSISTENT	= (1<<5),
		IN_BP_UPDATE_LIST_VOLATILE		= (1<<6),

		ACTOR_PAIR_FILTERING_RESET		= (1<<7),

		SHAPE_DEFAULT_FLAGS	= INVALID_BOUNDS
	};


//	NX_INLINE	const Shape&					getActorFast()						const	{ return *this;	}	//temp trick during refactor to keep things compiling.
//	NX_INLINE	Shape&							getActorFast()								{ return *this;	}	//temp trick during refactor to keep things compiling.

	//this is stupid and we only need it for when we need to map from the shape to the NxActors emitted by the contact stream	TODO: somehow get rid of this!
	NX_INLINE	NxActor*						getNxActorFast()							{ return nxActor;	}	
	NX_INLINE	RbActor&						getRbActor() const							{ return static_cast<RbActor&>(getCoreActor()); }


	NX_INLINE	Body*							getBodyFast()						const	{ return body;	}
	NX_INLINE	Scene*							getOwnerSceneFast()					const	{ return scene;	}
				bool							isSleeping()						const	{ return !getRbActor().isActive(); }

	// &&& Remove once Nv-layer is removed
	virtual		void							setActorPublicFlags(NxU32 f);		
	virtual		void							setActorGroup(NxActorGroup g);	
	virtual		void							setActorDominanceGroup(NxDominanceGroup g);		
	virtual		void							setActorContactReportFlags(NxU32 flags);
	virtual		void							resetUserActorPairFiltering();

				void							setGroupNonvirtual(NxCollisionGroup);	//need this so we can call it from ctor
				void							setGroup(NxCollisionGroup g)				{ setGroupNonvirtual(g);								}
	NX_INLINE	NxCollisionGroup				getGroupFast()						const	{ return collisionGroup;								}

	virtual		void							setGroupsMask(const NxGroupsMask& mask);
	NX_INLINE	const NxGroupsMask&				getGroupsMaskFast()					const	{ return groupsMask;									}

#ifndef __CELLOS_LV2__
	virtual		void							setNonInteractingCompartmentTypes(NxU32 compartmentTypes);
	virtual		NxU32							getNonInteractingCompartmentTypes()	const { return nonInteractingCompartmentTypes; }
#endif

	// IMPORTANT: NX_SF_DISABLE_SCENE_QUERIES are not longer stored among the shape flags because it can be asynchronously set,
	//			  hence the simulation may not read that data. Only user thread may access it.
	// Asynchronous: only accessed by user thread, NOT by simulation thread. NX_SF_DISABLE_SCENE_QUERIES.
	virtual		void							setAsyncFlags(NxU32 f);
	virtual		NxU32							getAsyncFlags()						const;
	// Synchronous: accessed by both simulation and user thread
	virtual		void							setFlags(NxU32 f);
	virtual		NxU32							getFlags()							const;

	NX_INLINE	NX_BOOL							getFlagFast(NxShapeFlag flag)		const	{ return shapeFlags & flag;								}
	NX_INLINE	NxU32							getFlagsFast()						const	{ return shapeFlags;						}
	NX_INLINE	NX_BOOL							isTriggerFast()						const	{ return shapeFlags & NX_TRIGGER_ENABLE;	}

				void							setMaterial(NxMaterialIndex);
	NX_INLINE	NxMaterialIndex					getMaterialFast()					const	{ return materialIndex;									}
				void							setSkinWidth(NxReal);
	NX_INLINE	NxReal							getSkinWidthFast()					const	{ return skinWidth;										}

	NX_INLINE	NxShapeType						getTypeFast()						const	{ return type;	}
	NX_INLINE	Shape*							is(NxShapeType castedType)					{ return castedType==type ? this : NULL;				}
	NX_INLINE	PlaneShape*						isPlane()									{ return (PlaneShape*)			is(NX_SHAPE_PLANE);		}
	NX_INLINE	SphereShape*					isSphere()									{ return (SphereShape*)			is(NX_SHAPE_SPHERE);	}
	NX_INLINE	BoxShape*						isBox()										{ return (BoxShape*)			is(NX_SHAPE_BOX);		}
	NX_INLINE	CapsuleShape*					isCapsule()									{ return (CapsuleShape*)		is(NX_SHAPE_CAPSULE);	}
	NX_INLINE	ConvexShape*					isConvex()									{ return (ConvexShape*)			is(NX_SHAPE_CONVEX);	}
	NX_INLINE	TriangleMeshShape*				isTriangleMesh()							{ return (TriangleMeshShape*)	is(NX_SHAPE_MESH);		}
	NX_INLINE	HeightFieldShape*				isHeightField()								{ return (HeightFieldShape*)	is(NX_SHAPE_HEIGHTFIELD);}

	NX_INLINE	const Shape*					is(NxShapeType castedType)			const	{ return castedType==type ? this : NULL;				}
	NX_INLINE	const PlaneShape*				isPlane()							const	{ return (const PlaneShape*)		is(NX_SHAPE_PLANE);		}
	NX_INLINE	const SphereShape*				isSphere()							const	{ return (const SphereShape*)		is(NX_SHAPE_SPHERE);	}
	NX_INLINE	const BoxShape*					isBox()								const	{ return (const BoxShape*)			is(NX_SHAPE_BOX);		}
	NX_INLINE	const CapsuleShape*				isCapsule()							const	{ return (const CapsuleShape*)		is(NX_SHAPE_CAPSULE);	}
	NX_INLINE	const ConvexShape*				isConvex()							const	{ return (const ConvexShape*)		is(NX_SHAPE_CONVEX);	}
	NX_INLINE	const TriangleMeshShape*		isTriangleMesh()					const	{ return (const TriangleMeshShape*)	is(NX_SHAPE_MESH);		}
	NX_INLINE	const HeightFieldShape*			isHeightField()						const	{ return (const HeightFieldShape*)	is(NX_SHAPE_HEIGHTFIELD);}

	virtual		void							visualize(NxFoundation::DebugRenderable& renderer) const;
	// Collision tests
//	virtual		bool							raycast(const NxRay& worldRay, NxReal maxDist, NxU32 hintFlags, NxRaycastHit& hit, bool firstHit=false)	const	{ return false;	}
	virtual		bool							checkOverlapSphere(const NxSphere& worldSphere)															const	{ return false;	}
	virtual		bool							checkOverlapOBB(const NxBox& worldBox)																	const	{ return false;	}
	virtual		bool							checkOverlapAABB(const NxBounds3& worldBounds)															const	{ return false;	}
	virtual		bool							checkOverlapCapsule(const NxCapsule& worldCapsule)														const	{ return false;	}

	NX_INLINE	NxU32							getIDFast()						const	{ return id;								}
	NX_INLINE	const NxMat34&					getAbsPoseFast()				const;
	NX_INLINE	const NxMat34&					getPreviousAbsPoseFast()		const;
	NX_INLINE	const NxQP&						getRelPose()					const	{ return relPose;							}	//TODO: same as getLocalPose()! 
	NX_INLINE	const NxBounds3&				getNewWorldBoundsFast()			const;
	NX_INLINE	NxBounds3						getWorldBoundsFast()			const;
	virtual		void							getWorldBounds(NxBounds3& b)	const	{ b = getWorldBoundsFast();					}
				NxU32							getDebugColor()					const;

	NX_INLINE	NxU32							getPoseTimestamp()				const	{ return poseTimestamp;						}
#ifdef NX_HARDWARE_IMPL
				void                            mark(NxSyncShapeField f)		const;
#endif
//@	NX_INLINE	void							setNpShape(NpShape* npShape)			{ mNpShape = npShape;						}	
	NX_INLINE	NpShape*						getNpShape()					const	{ return mNpShape;							}
	NX_INLINE	const NpShape*					getNpShapeConst()				const	{ return mNpShape;							}

//@	NX_INLINE	void							setNxShape(NxShape* nxShape)			{ mNxShape = nxShape;						}
//	NX_INLINE	NxShape**						getNxShapePtr()							{ return &mNxShape;							}
	NX_INLINE	const NxShape*					getNxShapeConst()				const	{ return mNxShape;							}	//NpShape * should always be the same object as an NxShape *, without any inheritance induced offsets. Yes this is ugly, so are the macros, but I have to be able to retrieve both kinds of ptrs here.
	NX_INLINE	NxShape*						getNxShape()							{ return mNxShape;							}	

	/**
	This should be called on shapes after their pose has changed, and before any CD queries that may involve them.
	Only call if the pose gets really changed, because this will result in a broad phase update, and
	the invalidation of the contact caches!
	*/
				void							sizeChangeNotify();	//called when shape did not move but its size (radius, etc) changed. 
	virtual		void							updateBroadPhase();
	virtual		void							removeFromBroadPhase();
	virtual		bool							hasSpecialBroadphase() const { return false; } // Should return true if the shape has an overloaded updateBroadPhase

	//ccd related:
				void							getCCDSkeletonMemoryNeeds(NxU32 & nTrigs, NxU32 & nEdges, NxU32 & nVerts) const;
				void							appendToCCDSkeleton(CCDSkeleton & dest, NxQP & shape2dest)  const;
				void							appendToCCDSkeleton2(CCDSkeleton2 & dest, NxQP & shape2dest, const NxVec3 & sourceSpaceMotionDir, const NxVec3 & destSpaceMotionDir)  const;
	public:
	NX_INLINE	bool							groupFilteringFast(NxU32 active_groups)	const	{ return (collisionGroup==0xffff) || ((1<<collisionGroup) & active_groups);	}
	NX_INLINE	NxCollisionGroup				getCollisionGroupFast()				const	{ return collisionGroup;	}


	// Implements NvShape
	virtual		void							release();

		// PT: reimplemented for TTP 579
	virtual		NxU32							getID()							const	{ return getIDFast();				}
	virtual		NxShapeType						getType()						const	{ return getTypeFast();				}
	virtual		bool							getFlag(NxShapeFlag flag)		const	{ return getFlagFast(flag)!=0; 		} // force to bool without warning
	virtual		NxMaterialIndex					getMaterial()					const	{ return getMaterialFast();			}
	virtual		NxReal							getSkinWidth()					const	{ return getSkinWidthFast();		}
	virtual		NxCollisionGroup				getCollisionGroup()				const	{ return getCollisionGroupFast();	}
	virtual		NxCollisionGroup				getGroup()						const	{ return getGroupFast();			}
	virtual		NxGroupsMask					getGroupsMask()					const	{ return getGroupsMaskFast();		}
	virtual		NxU32							getActorPublicFlags()			const	{ return getRbActor().getActorPublicFlags();	}
	virtual		bool							isTrigger()						const	{ return isTriggerFast()!=0;		} // force to bool without warning
			// PT: only for fluids!
	virtual		const NxMat34&					getAbsPose()					const	{ return getAbsPoseFast();			}
	virtual		const NxMat34&					getPreviousAbsPose()			const	{ return getPreviousAbsPoseFast();	}
	virtual		NvBody*							getNvBody()						const;

	NX_INLINE	bool							isDying()						const	{ return mIsDying; }

	private:
				mutable NxU32					poseTimestamp;
				mutable NxMat34					absPose;			//!< Pose in world space	// GLOBALPOS
				mutable NxU32					previousPoseTimestamp;
				mutable NxMat34					previousAbsPose;	//!< Maintained for CCD		// GLOBALPOS
				NxQP							relPose;			//!< Pose in actor space
	protected:
				NpShape*						mNpShape;
				NxShape*						mNxShape;			//I can't believe we need two pointers here considering its the same object but until I stop changing around the inheritance structure its safer not to always cast.
				NxReal							skinWidth;
				PxdShape llShape;						// The low level geometry
				PxdShape llBodyShape;					// The low level atom to shape mapping and relative transform
				NxQP	 hlShape2llShape;				// Relative transform from HL shape to LL shape
	private:
				NxActor*						nxActor;			//see getNxActor().
				bool							mIsDying;
	public:
	//variables
	mutable		NxU32							boundsTimestamp;
	mutable		NxBounds3						cachedWorldBounds;	// PT: only needed because of multithread. Yes, this is wasted memory.// GLOBALPOS
				Opcode::Prunable				pruningInterface;	// PT: size should be optimized
	private:
				Body*							body;				//NULL for static shapes!
				Scene*							scene;				//used to be:  actor->scene
	protected:
				NxGroupsMask					groupsMask;
				NxShapeType						type;				// PT: should be 16 bits
				NxU32							id;					// PT: should be 16 bits
				NxCollisionGroup				collisionGroup;		//!< Collision group. Note: 16 bit variable! PT: please keep them down the list to avoid bad align
				NxMaterialIndex					materialIndex;		//!< Material index. Note: 16 bit variable! PT: please keep them down the list to avoid bad align
	mutable		NxU32							internalShapeFlags;	//!< Internal shape flags
				NxU32							shapeFlags;			//!< Old trigger flags, now also mixed with various shape flags (2.1.1)
				NxU32							asyncShapeFlags;	//!< Asynchronously set. So far only set/accessed by user thread. No simulation thread access!

				void*							fakeVtableZero;		//!< hack to emulate missing NxShape
				SourceCCDSkeleton*				ccdSkeleton;		//!< This one pointer variable of any type MUST be between the above and below variable to give a total size of 3 ptrs.
				Shape*							fakeThis;			//!< hack to emulate missing NxShape
				NvShape*						sourceShape;		//!< nonezero if mirrored by HSM
//				NxReal							density;

#ifndef __CELLOS_LV2__
				NxU32							nonInteractingCompartmentTypes;
#endif

	public:

#ifdef SUPPORT_INTERNAL_RADIUS
	virtual		bool							getInternalRadius(NxReal& radius, const NxVec3& dir)	const	{ return false;	}
#endif
	virtual		void							computeLocalBounds(NxBounds3& bounds) const = 0;
	virtual		void							computeWorldBounds(NxBounds3&)	const = 0;	//shape types implement this, the result is written to this->worldBounds.
	virtual		void							computeWorldSphere(NxSphere&)	const = 0;	//shape types implement this
				void							computeWorldOBB(NxBox&)			const;
	virtual		void							computeLocalSphere(NxSphere&)	const = 0;	//shape types implement this

	virtual		bool							mapPageInstance(NxU32 pageIndex)		{ return false;}
	virtual		void							unmapPageInstance(NxU32 pageIndex)		{ NX_ASSERT(0); }
	virtual		bool							isPageInstanceMapped(NxU32 pageIndex) const		{ return false;}

	virtual		bool							isValid() const;

	friend class BroadPhase;	// ###
#ifdef NX_HARDWARE_IMPL
	friend class SyncManager;
#endif

public:
	NX_INLINE	PxdShape	 getLowLevelShape() const { return llShape; }
	NX_INLINE	PxdShape	 getLowLevelBodyShape() const { return llBodyShape; }
		
	virtual		void		 updateLowLevelShapeTransform();

				void		 setElementInteractionsDirty(CoreInteraction::DirtyFlag flag, const CoreElement* other, bool (*condition)(const PxInteraction&));

				bool		 earlyPairFilteringAllowed() const;
	NX_INLINE	bool		 needPageBoundsInteractions() const { return !(getTypeFast() == NX_SHAPE_WHEEL || getTypeFast() == NX_SHAPE_MESH); }

				void		 updateOnRelPoseChange();
	NX_INLINE	void		 clearInternalFlag(InternalFlag flag) { internalShapeFlags &= ~flag; }
	NX_INLINE	void		 raiseInternalFlag(InternalFlag flag) { internalShapeFlags |= flag; }
	NX_INLINE	bool		 readInternalFlag(InternalFlag flag) const { return (internalShapeFlags & flag) != 0; }

protected:
				void 		 initBodyShapeDesc(PxdShapeDesc& desc);
	};

//////////////////////////////////////////////////////////////////////////
NX_INLINE const NxMat34& Shape::getAbsPoseFast() const
{
	if (body && body->getPoseTimestamp() != poseTimestamp)
	{
		NxQP newTrans = body->getActor2WorldNew() * relPose;
		absPose = NxMat34(newTrans.q, newTrans.p);
		poseTimestamp = body->getPoseTimestamp();
	}

	return absPose;
}

NX_INLINE const NxMat34& Shape::getPreviousAbsPoseFast() const
{
	if (body && body->getPoseTimestamp() != previousPoseTimestamp)
	{
		NxQP newTrans = (body->getBody2WorldOld() * !body->getBody2Actor()) * relPose;
		previousAbsPose = NxMat34(newTrans.q, newTrans.p);
		previousPoseTimestamp = body->getPoseTimestamp();
	}

	return previousAbsPose;
}

NX_INLINE const NxBounds3& Shape::getNewWorldBoundsFast() const
{
	// Lazy-recompute bounds
	if(internalShapeFlags & INVALID_BOUNDS || (body && boundsTimestamp != body->getPoseTimestamp()))
	{
		if (body) boundsTimestamp = body->getPoseTimestamp();
		internalShapeFlags &= ~INVALID_BOUNDS;
		computeWorldBounds(cachedWorldBounds);
	}
	return cachedWorldBounds;
}

NX_INLINE NxBounds3 Shape::getWorldBoundsFast() const
{
	// We need to check for the INVALID_BOUNDS flag as well to make sure that the user gets the
	// updated bounds if he changes a shape and requests the world bounds afterwards (without
	// simulating in between)
	if (pruningInterface.IsValid() && !(readInternalFlag(INVALID_BOUNDS)))
	{
		Shape* FakeThis = (Shape*)this;
		return *(const NxBounds3*)FakeThis->pruningInterface.GetPruner()->GetWorldAABB(FakeThis->pruningInterface);
	}
	else
	{
		// Compute API pose
		NxMat34 poseAPI;
		if (body)
		{
			NxMat34 m;
			m.t = getShape2ActorP();
			m.M.fromQuat(getShape2ActorQ());
			poseAPI = NxMat34(NxMat33(body->getActor2WorldQ()),body->getActor2WorldP()) * m;
		}
		else
			poseAPI = NxMat34(NxQuat(getShape2ActorQ()), getShape2ActorP());

		NxBounds3 b;
		computeLocalBounds(b);
		b.transform(poseAPI.M, poseAPI.t);
		return b;
	}
}

#ifdef NX_ENABLE_SCENE_STATS2
#undef INC_SCENE_STAT
#undef DEC_SCENE_STAT
#endif

//	}
#endif

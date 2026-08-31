#ifndef NX_PHYSICS_CLOTH
#define NX_PHYSICS_CLOTH

/*----------------------------------------------------------------------------*\
|
| NVIDIA PhysX Technology
|
| www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#ifdef __CELLOS_LV2__
#define CELL_SPUCLOTH
#endif

#include "Allocateable.h"
#include "NvCloth.h"
#include "ClothCollision.h"
//#include "ClothHash.h"
#include "ClothMesh.h"
#include "DebugRenderable.h"
#include "ClothMetal.h"
#include "ClothFluidCollision.h"
#include "ClothHierarchy.h"
#include "SoftBody.h"
#include "ClothHash.h"
#include "ClothClothing.h"
#include "Nx.h"

#define ORIGINAL_CLOTH_IMPL

#if defined(__CELLOS_LV2__)
	#undef ORIGINAL_CLOTH_IMPL
	#include "CellUtil.h"
	#include "ps3/Cloth_PS3.h"
	#include "ps3/ClothCollision_PS3.h"
	#include "ps3/ClothHierarchy_PS3.h"
	#include "ps3/ClothClothing_PS3.h"
	#include "CellClothConfig.h"
#endif

#ifdef _XBOX
	#undef ORIGINAL_CLOTH_IMPL
	#include "xbox360/Cloth_XBOX.h"
#endif

/*----------------------------------------------------------------------------*/

class ClothManager;
class ClothCooker;
class ClothMesh;

class NvRawBounds;


/*----------------------------------------------------------------------------*/
#ifdef ORIGINAL_CLOTH_IMPL
// Should maintain 16 byte size/alignment of this structure for the SIMD solver.
struct ClothParticle {

	NxVec3 position;		// actual position
	NxU32 pad0;				// Check SIMD solver before moving this padding(it can get trashed).
					
	NxVec3 velocity;		// velocity of the particle (newPosition - position) / timeStep
	NxReal planeD;			// if the vertex collides (see flags), this is the contact plane d

	NxVec3 newPosition;		// predicted position which the solver solves for
	NxReal invMass;			// inverse mass, zero for kinematic particles
	
	NxVec3 attachment;		// if the vertex is attached (see flags), this is the goal position
	NxU32  flags;			// see NxClothVertexFlag in cloth.cpp

	
	NxVec3 impulse;			// accumulated impulse for two way interaction with rigid bodies

	NxVec3 normal;			// if the vertex collides (see flags), this is the contact normal

//	NxU32  parentIndex;		// for supporting welding, partent indices are not defined per particle
							// anymore but per user vertex -> see mParentIndices
#ifndef CELL_SPUCLOTH
	const  Shape *shape;	// colliding or attached shape
#else
	CELL_MM_PTR(const Shape) shape;
#endif
	NxU32  pad1;			// replaces parentIndex
			
	NxVec3 externalNormal;	// the normal we hand back to the user for rendering

#ifndef CELL_SPUCLOTH
	NxU32 pad2;
#else
	NxReal bodyInvMass;
#endif

	NxVec3 originalPosition;// original position of the vertex, used in self collision
	NxU32 pad3;
};
#endif

/*----------------------------------------------------------------------------*/
#ifndef __CELLOS_LV2__
// self collision data structures
struct HashCell {
	NxU32 numParticles;
	int firstParticle;
};

struct SelfCollisionParticle {
	void copyFrom(NxU32 nr, ClothParticle &p);
	void copyTo(ClothParticle &p);
	NxU32  particleNr;
	NxVec3 velocity;	// this is the only quantity that is updated
	NxVec3 position;	// needed for cloth - cloth friction to reconstruct velocities
	NxVec3 originalPosition;// needed to excule topologically close vertices from colliding
	NxReal invMass;
	NxU16  flags;
};
#endif
/*----------------------------------------------------------------------------*/

class Cloth : public NvCloth, public NxFoundation::NxAllocateable
{
	friend class ClothManager;

protected:
	Cloth(ClothManager* owner, NxU32 id);
	virtual	~Cloth();
#ifdef CELL_SPUCLOTH
	void updateAttachments(NxReal dt);
	void updateRemovedParticleStretchingLinks();
#endif

public:
	bool saveToDesc(DeformableDesc& desc) const;

	void   setStretchingStiffness(NxReal stiffness) { stretchingStiffness = stiffness; }
	NxReal getStretchingStiffness() const { return stretchingStiffness; }

	void   setHardStretchLimitationFactor(NxReal factor) { hardStretchLimitationFactor = factor; };
	NxReal getHardStretchLimitationFactor() const { return hardStretchLimitationFactor; };

	// cloth only
	void   setBendingStiffness(NxReal stiffness) { bendingStiffness = stiffness; }
	NxReal getBendingStiffness() const { return bendingStiffness; }

	// soft body only
	void   setVolumeStiffness(NxReal stiffness) { volumeStiffness = stiffness; }
	NxReal getVolumeStiffness() const { return volumeStiffness; }

	// cloth only
	void   setPressure(NxReal pressure) { pressureCoefficient = pressure; }
	NxReal getPressure() const { return pressureCoefficient; }

	void   setTearFactor(NxReal factor) { tearFactor = factor; }
	NxReal getTearFactor() const { return tearFactor; }

	void   setAttachmentTearFactor(NxReal factor) { attachmentTearFactor = factor; }
	NxReal getAttachmentTearFactor() const { return attachmentTearFactor; }

	void   setFriction(NxReal friction) { frictionCoefficient = friction; }
	NxReal getFriction() const { return frictionCoefficient; }
	
	void   setSolverIterations(NxU32 iterations) { solverIterations = iterations; }
	NxU32  getSolverIterations() const { return solverIterations; }
	
	void setHierarchicalSolverIterations(NxU32 iterations) { hierarchicalSolverIterations = iterations; }
	NxU32  getHierarchicalSolverIterations() const { return hierarchicalSolverIterations; }

	// cloth only
	void   setThickness(NxReal thickness) { if(thickness >= 0.0f) mHalfThickness = 0.5f*thickness;    }
	NxReal getThickness() const { return 2.0f * mHalfThickness;    }

	// soft body only -> mapping to thickness
	void   setParticleRadius(NxReal particleRadius) { if(particleRadius >= 0.0f) mHalfThickness = 0.5f*particleRadius;    }
	NxReal getParticleRadius() const { return 2.0f * mHalfThickness;    }

	void   setSelfCollisionThickness(NxReal thickness) { if(thickness >= 0.0f) mSelfCollisionThickness = thickness;    }
	NxReal getSelfCollisionThickness() const { return mSelfCollisionThickness;    }

	NxReal getDensity() const { return mDensity;    }

	NxReal getRelativeGridSpacing() const { return mRelativeGridSpacing;    }

	void   setFlags(NxU32 flags);
	NxU32  getFlags() const { return flags; }

	void   setDampingCoefficient(NxReal dampingCoefficient) { this->dampingCoefficient = dampingCoefficient; }
	NxReal getDampingCoefficient() const { return this->dampingCoefficient; }

	void   setCollisionResponseCoefficient(NxReal coefficient) { this->collisionResponseCoefficient = coefficient; }
	NxReal getCollisionResponseCoefficient() const { return collisionResponseCoefficient; }

	void   setAttachmentResponseCoefficient(NxReal coefficient) { this->attachmentResponseCoefficient = coefficient; }
	NxReal getAttachmentResponseCoefficient() const { return attachmentResponseCoefficient; }

	void   setFromFluidResponseCoefficient(NxReal coefficient) { this->fromFluidResponseCoefficient = coefficient; }
	NxReal getFromFluidResponseCoefficient() const { return fromFluidResponseCoefficient; }

	void   setToFluidResponseCoefficient(NxReal coefficient) { this->toFluidResponseCoefficient = coefficient; }
	NxReal getToFluidResponseCoefficient() const { return toFluidResponseCoefficient; }

	void   setMinAdhereVelocity(NxReal velocity) { minAdhereVelocity = velocity; }
	NxReal getMinAdhereVelocity() const { return minAdhereVelocity; }

	void   setExternalAcceleration(NxVec3 acceleration) { this->externalAcceleration = acceleration; };
	NxVec3 getExternalAcceleration() const { return externalAcceleration; };

	void   setWindAcceleration(NxVec3 acceleration) { this->windAcceleration = acceleration; };
	NxVec3 getWindAcceleration() const { return windAcceleration; };

	NxClothMesh* getClothMesh() const;
	NxSoftBodyMesh* getSoftBodyMesh() const;

	void   setPosition(const NxVec3& position, NxU32 vertexId);
	NxVec3 getPosition(NxU32 vertexId) const;

	void   setPositions(void* buffer, NxU32 byteStride = sizeof(NxVec3));
	void   getPositions(void* buffer, NxU32 byteStride = sizeof(NxVec3));

	void   setVelocity(const NxVec3& position, NxU32 vertexId);
	NxVec3 getVelocity(NxU32 vertexId) const;

	void   setVelocities(void* buffer, NxU32 byteStride = sizeof(NxVec3));
    void   getVelocities(void* buffer, NxU32 byteStride = sizeof(NxVec3));

	void   setConstrainPositions(void* buffer, NxU32 byteStride = sizeof(NxVec3));
	void   setConstrainNormals(void* buffer, NxU32 byteStride = sizeof(NxVec3));
	void   setClothConstrainCoefficients(const NxClothConstrainCoefficients *coefficients, NxU32 byteStride = sizeof(NxClothConstrainCoefficients));
	void   setSoftBodyConstrainCoefficients(const NxSoftBodyConstrainCoefficients *coefficients, NxU32 byteStride = sizeof(NxSoftBodyConstrainCoefficients));

	NxU32  getNumberOfExternalParticles();
	NxU32  getNumberOfInternalParticles();
	NxU32  getStateByteSize();
	NxU32  queryShapePointers();

	void   getIndices(void* buffer);
	bool   changedConnectivityLastStep();
	NxU32  getNumIndices();
	
	void   internalGetIndices(NxU32* buffer);
	void   internalGetPositions(void* buffer, NxU32 byteStride = sizeof(NxVec3));

	NxU32  getShapePointersByteSize() const;
	
	void   getShapePointers(NxShape** buffer,NxU32 *flags);
	void   setShapePointers(NxShape** buffer,unsigned int numShapes);

	void   saveStateToStream(NxStream& stream);
	void   loadStateFromStream(NxStream& stream);

	bool isSleeping() const;
    NxReal getSleepLinearVelocity() const;
    void setSleepLinearVelocity(NxReal threshold);
	void wakeUp(NxReal wakeUpCounterValue);
	void putToSleep();

	void setNpCloth(NpCloth* _npCloth) { npCloth = _npCloth; }
	NpCloth* getNpCloth() { return npCloth; }

	void setNpSoftBody(NpSoftBody* _npSoftBody) { npSoftBody = _npSoftBody; }
	NpSoftBody* getNpSoftBody() { return npSoftBody; }

	void attachToShape(const NvShape *shape, NxU32 attachmentFlags);
	void attachToCollidingShapes(NxU32 attachmentFlags);
	void detachFromShape(const NvShape *shape);
	void attachVertexToShape(NxU32 vertexId, const NvShape *shape, const NxVec3 &localPos, NxU32 attachmentFlags);
	void attachVertexToGlobalPosition(const NxU32 vertexId, const NxVec3 &pos);
	void freeVertex(const NxU32 vertexId);

	void dominateVertex(NxU32 vertexId, NxReal expirationTime, NxReal dominanceWeight);
	NxClothVertexAttachmentStatus getVertexAttachmentStatus(NxU32 vertexId) const;
	NxShape* getVertexAttachmentShape(NxU32 vertexId) const;
	NxVec3 getVertexAttachmentPosition(NxU32 vertexId) const;

	bool attachToCore(NvShape *shape, NxReal impulseThreshold, NxReal penetrationDepth, NxReal maxDeformationDistance);
	void detachFromCore(NvShape *shape);  // no api call, only used in case shape is deleted

	bool tearVertex(const NxU32 vertexId, const NxVec3 &normal);

	int getCollidingShapeIndex(const NvShape *shape);

	bool raycast(const NxRay& worldRay, NxVec3 &hit, NxU32 &vertexId);

	// forces 
	void addForceAtVertex(const NxVec3& force, NxU32 vertexId, NxForceMode mode = NX_FORCE);
	void addForceAtPos(const NxVec3& position, NxReal magnitude, NxReal radius, NxForceMode mode = NX_FORCE);
	void addDirectedForceAtPos(const NxVec3& position, const NxVec3& force, NxReal radius, NxForceMode mode = NX_FORCE);
	virtual NxForceFieldMaterial getForceFieldMaterial()	const	{ return mForceFieldMaterial;	}
	virtual void setForceFieldMaterial(NxForceFieldMaterial mat)	{ mForceFieldMaterial = mat;	}
	

	void updateParticles(const NvClothParticleUpdateData& updateData);
	const NxU8* getFirstPositionByte() const { return reinterpret_cast<const NxU8*>(&mParticles[0].position); }
	const NxU8* getFirstVelocityByte() const { return reinterpret_cast<const NxU8*>(&mParticles[0].velocity); }
	NxU32 getPositionByteStride() const { return sizeof(ClothParticle); }
	NxU32 getVelocityByteStride() const { return sizeof(ClothParticle); }

	bool overlapAABBTriangles(const NxBounds3& bounds, NxU32& nb, const NxU32*& indices)	const;
	bool overlapAABBTetrahedra(const NxBounds3& bounds, NxU32& nb, const NxU32*& indices)	const;

	void setGroup(NxCollisionGroup collisionGroup) { this->collisionGroup = collisionGroup; }
	NxCollisionGroup getGroup() const { return collisionGroup; }

	void setGroupsMask(const NxGroupsMask& groupsMask) { this->groupsMask = groupsMask; }
	const NxGroupsMask* getGroupsMask() const { return &groupsMask; }

	NxMeshData& getMeshData();
	void setMeshData(NxMeshData& meshData);

	NxSoftBodySplitPairData& getSplitPairData();
	void setSplitPairData(NxSoftBodySplitPairData& splitPairData);

	NxClothSplitPairData& getClothSplitPairData();
	void setClothSplitPairData(NxClothSplitPairData& splitPairData);

	void swapSDKBfr();
	void postAdjustAttachments();

	bool loadFromDesc(const DeformableDesc& desc);
	void visualize(NxFoundation::DebugRenderable& dr);

	// implementation of NvMirrorManagerClient
	NvScene* getOwnerScene() const;
	void addStaticShapeToRegion(NvShape &,const NxBounds3&) ;
	void removeStaticShapeFromRegion(NvShape &,const NxBounds3&) ;
	void addDynamicShape(NvShape& shape);
	void addInteractionWithRbShape(NvShape& nvShape, void* userData) { NX_ASSERT(0); }
	void removeInteractionWithRbShape(NvShape& nvShape, void* userData) { NX_ASSERT(0); }
	void removeDynamicShape(NvShape& shape);
	bool isCompatibleWithStaticMesh(NvRawTriangleMesh &) const;
	NxReal getImpulseTransferSmoothingAlpha() const { return 1.0f; }
	NxU32 getClientType() const { return (deformableType == DEFORMABLE_TYPE_CLOTH) ? 
		NV_MIRROR_MANAGER_CLIENT_TYPE_CLOTH : NV_MIRROR_MANAGER_CLIENT_TYPE_SOFTBODY;
	}
	void accessParentScene();
	void passShapeRefCountChanges() { NX_ASSERT(0); }

	// notification
	void notifyReleasePermanentlyMirroredShape(NvShape *shape);

private:
	NpCloth* npCloth;
	NpSoftBody* npSoftBody;

	NxFoundation::DebugRenderable debugRenderable;
//bh: todo: following functions/variables need to be properly integrated into the SDK
public:
	void clear();

	void computeNewState(NxReal dt);
	//void addToHash(ClothHash &hash);
	void generateContacts(/*ClothHash &hash*/);
	void solverStep(NxReal dt);
	void integrateTimeStep(NxReal dt);
#ifndef CELL_SPUCLOTH
	void updateCollision();
#else
	bool updateCollision();
#endif
	void endSimulation();

	void getWorldBoundsFast(NxBounds3& bounds) const;

	bool hasRawBounds() const { return mRawBounds != 0; }
	void createRawBounds();
	void updateRawBounds();
	void releaseRawBounds();
	
	void setValidBounds(const NxBounds3& validBounds);
	void getValidBounds(NxBounds3& validBounds) const;

	void getMovingPoint(NxU32 vertexNr, ClothMovingPoint &mp);
	void getNewPosition(NxU32 vertexNr, NxVec3 &pos);
	void setNewPosition(NxU32 vertexNr, const NxVec3 &pos);
	NxReal getInverseMass(NxU32 vertexNr);

	void clearAttachments();
	void detachFromInternalShape(Shape *shape);

	NxU32 getId() { return mId; }

    NxU32 getLaneUsec() { return 0; }

private:

#ifdef CELL_SPUCLOTH
	ClothInfo CELL_ALIGN(128,clothInfo);
	CellClothPointers CELL_ALIGN(128,ccp);
	ClothHierarchyPointers CELL_ALIGN(16,hierarchyPointers);
	ClothClothingPointers CELL_ALIGN(16,clothingPointers);
	int CELL_ALIGN(16,removedParticleStretchingLinks[SPU_CLOTH_MAX_PARTICLES]);
#endif

	DeformableType deformableType;

	NxReal stretchingStiffness;
	NxReal bendingStiffness;
	NxReal volumeStiffness;
	NxReal dampingCoefficient;
	NxReal collisionResponseCoefficient;
	NxReal attachmentResponseCoefficient;
	NxReal frictionCoefficient;
	NxReal pressureCoefficient;
	NxReal toFluidResponseCoefficient;
	NxReal fromFluidResponseCoefficient;
	NxReal minAdhereVelocity;
	NxReal tearFactor;
	NxReal attachmentTearFactor;
	NxU32  solverIterations;
	NxU32  hierarchicalSolverIterations;
	NxReal hardStretchLimitationFactor;
	NxVec3 externalAcceleration;
	NxVec3 windAcceleration;
	NxReal wakeUpCounter;
	NxReal sleepLinearVelocity;
	NxReal sleepTimer;
	NxU32  flags;
	const char *mName;
	void* mUserData;
	bool mSwapSDKBfr;
	bool mConstraintsFlag;
	bool mParentIndicesFlag;
	bool mIndicesFlag;
	NxU32 mLastIgnoreCount;
	NxForceFieldMaterial mForceFieldMaterial;

    NxCollisionGroup collisionGroup; //!< Collision group. Note: 16 bit variable! please keep them down the list to avoid bad align

	NxGroupsMask groupsMask;

	const static NxU32 bmCenterSpring = 0;
	const static NxU32 bmDihedralAngle = 1;

	NxU32  bendingMethod;

	ClothMesh *clothMesh;					// user input
	NxMeshData meshData;					// user output
	NxSoftBodySplitPairData splitPairData;	// user output
	NxClothSplitPairData clothSplitPairData;	// user output

	ClothHash mClothHash;
	NxFoundation::NxArraySDK<ClothBounds> mBoundsList;


	void init();
	void initPhysics();

	void attachInternalVertexToShape(NxU32 vertexId, const NvShape *shape, const NxVec3 &localPos, NxU32 attachmentFlags);
	void attachInternalVertexToGlobalPosition(const NxU32 vertexId, const NxVec3 &pos);

	void addForceAtPos(const NxVec3& position, const NxVec3& forceDirection, NxReal forceMagnitude, bool bRadial, NxReal radius, NxForceMode mode);

	bool raycastTriangleMesh(const NxRay& worldRay, NxVec3 &hit, NxU32 &vertexId);
	bool raycastTetrahedralMesh(const NxRay& worldRay, NxVec3 &hit, NxU32 &vertexId);

	void collisionDetection(const NxVec3 &offset = NxVec3(0.0f));
	//void addSelfContacts(ClothHash &hash);

	void projectInternalClothConstraints();
	void projectInternalSoftBodyConstraints();
	void updatePlasticity();
	void udpateShapeMatching();

	void resetAttachmentImpulses();
	void projectAttachmentConstraints(NxReal dt);

	void projectClothVelocities();
	void projectSoftBodyVelocities();

	void projectContacts(NxReal dt, NxU32 firstContact = 0);
	//void projectSelfContacts();

	void projectPressure(NxReal dt);

	void dampCOM();
	void adhereToAttachments(NxReal dt);

	void rigidBodyResponseAndSleeping(NxReal dt);
	void contactFriction();
	void updateConstraints();
	void updateClothConstraints();
	void updateSoftBodyConstraints();
	void updateDominantVertices(NxReal dt);

	void getBoundsDiff(NxBounds3*& delPackets, NxBounds3*& newPackets, NxU32& numDel, NxU32& numNew);
	void getBoundsDiffSimple(NxBounds3*& delPackets, NxBounds3*& newPackets, NxU32& numDel, NxU32& numNew);
	void createTearingData();
	bool tearClothMesh();
	bool tearSoftBodyMesh();
	bool splitClothVertex(const int vertexId, const NxVec3 *normal = NULL); 
	bool splitSoftBodyVertex(const int vertexId, const NxVec3 *normal = NULL); 
	void visualizeFlag(NxFoundation::DebugRenderable& dr, NxU32 flag, NxVec3 position, NxReal baseRadius);
	void visualizeCloth(NxFoundation::DebugRenderable& dr);
	void visualizeSoftBody(NxFoundation::DebugRenderable& dr);
	void visualizeHierarchy(NxFoundation::DebugRenderable& dr);
	void visualizeHardStretchLimitation(NxFoundation::DebugRenderable& dr);

	void updateNormals(bool useOriginalPositions = false);
	void updateBounds(NxReal dt = 0.0f);

	void getAreaAndVolume(NxReal &area, NxReal &volume);
	void getNewAreaAndVolume(NxReal &area, NxReal &volume);

	void quickSortVertexRefs(int l, int r);
	void sortVertexRefs() { if (vertexRefs.size() > 1) quickSortVertexRefs(0, vertexRefs.size()-1); }

	// these procedures act on the internal state of the body in contrast to the Body API procedures
	NxVec3 getPointVelocity(const Body *body, const NxVec3 &p);
	void   addImpulseAtPos(Body *body, const NxVec3 &impulse, const NxVec3 &worldPos);

	NxU32 getNumGridCells(const NxBounds3& bounds, NxReal spacing) const;
	bool isInCompartment();

	NxU32  mId;
	NxReal mHalfThickness;
	NxReal mSelfCollisionThickness;
	NxReal mDensity;
	NxMat34 mGlobalPose;
	NxReal mGridSpacing; 
	NxBounds3 mOldBounds;
	NxBounds3 mBounds;
	NxBounds3 mNewBounds;
	NxBounds3 mBoundsAPI; // buffered API data
	NxU32 mFrameCounter;

	// bounds entered into the broadphase
	NvRawBounds* mRawBounds;

	NxFoundation::NxArraySDK<NxShape*> mCollisionShapes;
#ifdef CELL_SPUCLOTH
	NxFoundation::NxArraySDKAlign<CellClothShape,128> mCellCollisionShapes;
#endif
	/* in sw compartment, we need to store collision shape's NvShape pointer
	   This array must be synchronized with mCollisionShapes */
	NxFoundation::NxArraySDK<NvShape*> mCollisionNvShapes;  
	NxU32 mNumCollisionShapes;

	NxBounds3 mValidBounds;
	NxReal mRelativeGridSpacing;
	NxU32 mInitialNumGridCells;

	bool mAdhered;	// in case we adhere to the attachments we don't want ccd collion detection

	ClothManager *mClothManager;

	NxFoundation::NxArraySDKAlign<ClothParticle, 128> mParticles;
#ifdef _XBOX
	ClothParticle_xbox_solver *tmpParticles;
#endif

	NxU32 mNumTriangles;
	NxU32 mNumTetrahedra;
	NxFoundation::NxArraySDK<NxU32> mIndices;
	NxFoundation::NxArraySDK<NxU32> mUserIndices;
	NxFoundation::NxArraySDK<NxU32> mPermutationTable;
	NxFoundation::NxArraySDK<NxU32> mParentIndices;

	NxU32 mLastStepParticleCount;
	NxU32 mCurrentStepParticleCount;

	NxFoundation::NxArraySDKAlign<ClothGlobalInternalConstraint, 32> mInternalConstraints;
	//NxFoundation::NxArraySDK<ClothSelfContact> mSelfContacts;

#ifdef CELL_SPUCLOTH
	// when a particle is attached to a shape
	// p.attachment is in body local space
	// SPU must get this information
	NxFoundation::NxArraySDKAlign<CellClothAttachment,128> mAttachedBodyPoses;
#endif

	// local untangling: mmf test
	void localUntangling();
	NxFoundation::NxArraySDK<NxU32> mNumTrianglesOfParticle;
	NxFoundation::NxArraySDK<NxU32> mFirstTriangleOfParticle;
	NxFoundation::NxArraySDK<NxU32> mTrianglesOfParticle;

	// boundsdiff
	NxFoundation::NxArraySDK<NxBounds3> mDelPackets;
	NxFoundation::NxArraySDK<NxBounds3> mNewPackets;

	// self collision within this piece of cloth: mmf test
	void intraSelfCollision(NxReal dt);

#if NX_USE_FLUID_API
	ClothFluidCollision mFluidCollision;
#endif

	// tearing helper data structure
	ClothTearingData mTearingData;

	// auxiliary vars
	NxReal restArea;
	NxReal restVolume;
	NxFoundation::NxArraySDK<ClothVertexRef> vertexRefs;
	NxFoundation::NxArraySDK<NxU32> AABBTriangles;
	ClothCooker *cooker;

	NxFoundation::NxArraySDK<NxClothSplitPair> mClothSplitPairs;

	// soft body simulation
	NxFoundation::NxArraySDK<NxSoftBodySplitPair> mSoftBodySplitPairs;
	SoftBodyCore mSoftBodyCore;
	NxFoundation::NxArraySDK<NxU8> mSoftBodyDelFlags;

	NxFoundation::NxArraySDK<NxU32> mConstraintCount;

	bool mUseSimpleBoundsDiff;
	// tearing stuff

	struct IndexRef {
		int oldUser;	// the existing user vertex for this triangle
		int newUser;	// differs in case a new user vertex has to be created for the existing one
		int groupNr;	// mapping to an internal vertex
	};

	struct ClothAdjTriangle {
		void set (NxU32 *center, NxU32 outer1, NxU32 outer2, NxU32 *userCenter)
		{ i0 = center; i1 = outer1; i2 = outer2; groupNr = -1; userI0 = userCenter; }
		bool matches(NxU32 j0, NxU32 j1, NxU32 j2) {
			int num = 0;
			if (j0 == i1 || j0 == i2) num++;
			if (j1 == i1 || j1 == i2) num++;
			if (j2 == i1 || j2 == i2) num++;
			return num >= 2;
		}
		bool contains(NxU32 i) const { return i == i1 || i == i2; }
		NxU32 otherIndex(NxU32 i) const { if (i == i1) return i2; else return i1; }
		NxU32 *i0, i1,i2;	// *i0 points into center vertex in mIndices
		NxU32 *userI0;		// for updating user indices
		int groupNr;
		int side;
		NxU32 idx;
	};
	NxFoundation::NxArraySDK<ClothAdjTriangle> mAdjTriangles;

	// adjacent tetrahedron face (for softbodies)
	struct ClothAdjTetrahedronFace
	{
		ClothAdjTetrahedronFace() {};
		ClothAdjTetrahedronFace(NxU32 _i0, NxU32 _i1) {i0 = _i0; i1 = _i1;}
		NxU32 i0, i1;
	};

	// adjacent tetrahedron (for softbodies)
	struct ClothAdjTetrahedron
	{
		NxU32 *i0, i1, i2, i3; // *i0 points into center vertex in mIndices
		NxU32 *userI0;
		int groupNr;
		int side;
		NxU32 idx;
		ClothConstraintReference constraintRef; // only "constraint" is used in software version
		void set(NxU32* center, NxU32 outer1, NxU32 outer2, NxU32 outer3, NxU32 *userCenter)
		{
			i0 = center; i1 = outer1; i2 = outer2; i3 = outer3;
			userI0 = userCenter;
			groupNr = -1;
		}
		bool matches(NxU32 j0, NxU32 j1, NxU32 j2, NxU32 j3)
		{
			int num = 0;
			if(j0 == i1 || j0 == i2 || j0 == i3) num++;
			if(j1 == i1 || j1 == i2 || j1 == i3) num++;
			if(j2 == i1 || j2 == i2 || j2 == i3) num++;
			if(j3 == i1 || j3 == i2 || j3 == i3) num++;
			return num >= 3;
		}
		bool contains(const NxU32 id)
		{
			return i1 == id || i2 == id || i3 == id;
		}
		bool contains(const ClothAdjTetrahedronFace &face, NxU32 &otherVertex) const
		{
			if(face.i0 == i1) {
				if(face.i1 == i2) { 
					otherVertex = i3; return true; 
				} else if(face.i1 == i3) {
					otherVertex = i2; return true;
				} else
					return false;
			} else if(face.i0 == i2) {
				if(face.i1 == i3) { 
					otherVertex = i1; return true; 
				} else if(face.i1 == i1) {
					otherVertex = i3; return true;
				} else
					return false;
			} else if(face.i0 == i3) {
				if(face.i1 == i1) { 
					otherVertex = i2; return true;
				} else if(face.i1 == i2) {
					otherVertex = i1; return true;
				} else
					return false;
			}
			else
				return false;
		}
	};

	class ClothCrackTipCandidateCompare
	{
	public:
		inline bool operator()(const NxU32 &a, const NxU32 &b)
		{
			return a < b;
		}
	};

	/* Force updates */
	struct clothParticleForceUpdate {
	public:
		int index;
		NxReal weight;
		clothParticleForceUpdate(int _index, NxReal _weight) : index(_index), weight(_weight) {};
	};

	struct DominantVertex
	{
		NxU32 id;
		NxReal dominanceIntervalEnd;
	};

	NxFoundation::NxArraySDK<clothParticleForceUpdate> mUpdateParticles;

	/* Self collision data */
	NxFoundation::NxArraySDK<SelfCollisionParticle> mSelfCollisionParticles;

	/* Metal simulation */
	void updateMetalSimulation(NxReal dt);
	ClothMetal mMetal;
#ifdef CELL_SPUCLOTH
	void prepareMetalSimulation(NxReal dt);
	bool mMetalUpdate;
	void prepareTaskArgs();
#endif

	/* Serialization data */
	NxFoundation::NxArraySDK<NxShape*> mInteractingShapes;
	NxFoundation::NxArraySDK<NxU32>    mInteractingShapeFlags;

	/* Vertex dominance */
	NxReal mDominanceTimer;
	NxFoundation::NxArraySDK<DominantVertex> mDominantVertices;

	ClothClothing mClothing;

	// Mesh hierarchy
	ClothHierarchy mHierarchy;
};
/*----------------------------------------------------------------------------*/

#endif

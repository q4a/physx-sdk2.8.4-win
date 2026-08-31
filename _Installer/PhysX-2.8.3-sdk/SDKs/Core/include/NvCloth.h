#ifndef NX_PHYSICS_NV_CLOTH
#define NX_PHYSICS_NV_CLOTH

/*----------------------------------------------------------------------------*\
|
| NVIDIA PhysX Technology
|
| www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "Physics.h"
#include "DebugRenderable.h"
#include "DeformableDesc.h"
#include "NvMirrorManagerClient.h"
#include "NvQueuedEvent.h"

class NvCloth;
class NpCloth;
class NpSoftBody;
class NxMeshData;
class NxSoftBodySplitPairData;
class NxClothSplitPairData;
class NxRay;
class NvShape;
class NvScene;
class NxClothMesh;
class NxShape;
class NxStream;

// cloth version of the fluid's NxParticleUpdateData, currently only used internally by force fields
// !!! NvCloth::updateParticles expects this struct to be filled per particle, not per mesh vertex
class NvClothParticleUpdateData		
{
public:
	NxForceMode	forceMode;
	NxF32*		bufferForce;
	NxU32		bufferForceByteStride;
};

/*----------------------------------------------------------------------------*/

class NvCloth: public NvMirrorManagerClient
{
public:
	virtual	bool saveToDesc(DeformableDesc& desc) const = 0;

	virtual void   setBendingStiffness(NxReal stiffness) = 0;	// NxCloth only
	virtual NxReal getBendingStiffness() const = 0;				// NxCloth only

	virtual void   setVolumeStiffness(NxReal stiffness) = 0;	// NxSoftBody only
	virtual NxReal getVolumeStiffness() const = 0;				// NxSoftBody only

	virtual void   setStretchingStiffness(NxReal stiffness) = 0;
	virtual NxReal getStretchingStiffness() const = 0;

	virtual void setHardStretchLimitationFactor(NxReal factor) = 0;
	virtual NxReal getHardStretchLimitationFactor() const = 0;

	virtual void   setFriction(NxReal friction) = 0;
	virtual NxReal getFriction() const = 0;

	virtual void   setPressure(NxReal pressure) = 0;	// NxCloth only
	virtual NxReal getPressure() const = 0;				// NxCloth only

	virtual void   setTearFactor(NxReal factor) = 0;
	virtual NxReal getTearFactor() const = 0;

	virtual void   setAttachmentTearFactor(NxReal factor) = 0;
	virtual NxReal getAttachmentTearFactor() const = 0;

	virtual void   setThickness(NxReal thickness) = 0;	// NxCloth only
	virtual NxReal getThickness() const = 0;			// NxCloth only

	virtual void   setParticleRadius(NxReal particleRadius) = 0;	// NxSoftBody only
	virtual NxReal getParticleRadius() const = 0;					// NxSoftBody only

	virtual void   setSelfCollisionThickness(NxReal thickness) = 0;
	virtual NxReal getSelfCollisionThickness() const = 0;

	virtual NxReal getDensity() const = 0;

	virtual NxReal getRelativeGridSpacing() const = 0;

	virtual void   setDampingCoefficient(NxReal damping) = 0;
	virtual NxReal getDampingCoefficient() const = 0;

	virtual void   setSolverIterations(NxU32 iterations) = 0;
	virtual NxU32  getSolverIterations() const = 0;

	virtual void   setHierarchicalSolverIterations(NxU32 iterations) = 0;
	virtual NxU32  getHierarchicalSolverIterations() const = 0;

	virtual void   setCollisionResponseCoefficient(NxReal coefficient) = 0;
	virtual NxReal getCollisionResponseCoefficient() const = 0;

	virtual void   setAttachmentResponseCoefficient(NxReal coefficient) = 0;
	virtual NxReal getAttachmentResponseCoefficient() const = 0;

	virtual void   setFromFluidResponseCoefficient(NxReal coefficient) = 0;
	virtual NxReal getFromFluidResponseCoefficient() const = 0;

	virtual void   setToFluidResponseCoefficient(NxReal coefficient) = 0;
	virtual NxReal getToFluidResponseCoefficient() const = 0;

	virtual void   setMinAdhereVelocity(NxReal velocity) = 0;
	virtual NxReal getMinAdhereVelocity() const = 0;

	virtual void   setExternalAcceleration(NxVec3 acceleration) = 0;
	virtual NxVec3 getExternalAcceleration() const = 0;

	virtual void   setWindAcceleration(NxVec3 acceleration) = 0;
	virtual NxVec3 getWindAcceleration() const = 0;

	virtual NxClothMesh* getClothMesh() const = 0;			// NxCloth only
	virtual NxSoftBodyMesh* getSoftBodyMesh() const = 0;	// NxSoftBody only

	virtual void   setPosition(const NxVec3& position, NxU32 vertexId) = 0;
	virtual NxVec3 getPosition(NxU32 vertexId) const = 0;

	virtual void   setPositions(void* buffer, NxU32 byteStride = sizeof(NxVec3)) = 0;
	virtual void   getPositions(void* buffer, NxU32 byteStride = sizeof(NxVec3)) = 0;

	virtual void   setVelocity(const NxVec3& velocity, NxU32 vertexId) = 0;
	virtual NxVec3 getVelocity(NxU32 vertexId) const = 0;

	virtual void   setVelocities(void* buffer, NxU32 byteStride = sizeof(NxVec3)) = 0;
    virtual void   getVelocities(void* buffer, NxU32 byteStride = sizeof(NxVec3)) = 0;

	virtual void   setConstrainPositions(void* buffer, NxU32 byteStride = sizeof(NxVec3)) = 0;
	virtual void   setConstrainNormals(void* buffer, NxU32 byteStride = sizeof(NxVec3)) = 0;
	virtual void   setClothConstrainCoefficients(const NxClothConstrainCoefficients *coefficients, NxU32 byteStride = sizeof(NxClothConstrainCoefficients)) = 0;
	virtual void   setSoftBodyConstrainCoefficients(const NxSoftBodyConstrainCoefficients *coefficients, NxU32 byteStride = sizeof(NxSoftBodyConstrainCoefficients)) = 0;

	virtual NxU32  getNumberOfExternalParticles() = 0;
	virtual NxU32  getNumberOfInternalParticles() = 0;
	virtual NxU32  getStateByteSize() = 0;
	virtual NxU32  queryShapePointers() = 0;

	virtual void   getIndices(void* buffer) = 0;
	virtual bool   changedConnectivityLastStep() = 0;
	virtual NxU32  getNumIndices() = 0;
	
	//Returns the post-cooked indices, added for VRD
	virtual void   internalGetIndices(NxU32* buffer) = 0;
	//Returns the post-cooked positions, added for VRD
	virtual void   internalGetPositions(void* buffer, NxU32 byteStride = sizeof(NxVec3)) = 0;

	virtual void	saveStateToStream(NxStream& stream) = 0;
	virtual void	loadStateFromStream(NxStream& stream) = 0;

	virtual void	getShapePointers(NxShape** shapePointers,NxU32 *flags) = 0;
	virtual void    setShapePointers(NxShape** shapePointers,unsigned int numShapes) = 0;

	virtual bool isSleeping() const = 0;
    virtual NxReal getSleepLinearVelocity() const = 0;
    virtual void setSleepLinearVelocity(NxReal threshold) = 0;
	virtual void wakeUp(NxReal wakeCounterValue=NX_SLEEP_INTERVAL) = 0;
	virtual void putToSleep() = 0;

	virtual void getWorldBoundsFast(NxBounds3& bounds) const = 0;

	virtual void setValidBounds(const NxBounds3& validBounds) = 0;
	virtual void getValidBounds(NxBounds3& validBounds) const = 0;

	virtual void setFlags(NxU32 flags) = 0;
	virtual NxU32 getFlags() const = 0;
	virtual void setNpCloth(NpCloth*) = 0;	// NxCloth only
	virtual NpCloth* getNpCloth() = 0;		// NxCloth only
	virtual void setNpSoftBody(NpSoftBody*) = 0;	// NxSoftBody only
	virtual NpSoftBody* getNpSoftBody() = 0;		// NxSoftBody only
	virtual void attachToShape(const NvShape *shape, NxU32 attachmentFlags) = 0;
	virtual void attachToCollidingShapes(NxU32 attachmentFlags) = 0;
	virtual void detachFromShape(const NvShape *shape) = 0;
	virtual void attachVertexToShape(NxU32 vertexId, const NvShape *shape, const NxVec3 &localPos, NxU32 attachmentFlags) = 0;
	virtual void attachVertexToGlobalPosition(const NxU32 vertexId, const NxVec3 &pos) = 0;
	virtual void freeVertex(const NxU32 vertexId) = 0;
	virtual void dominateVertex(NxU32 vertexId, NxReal expirationTime, NxReal dominanceWeight) = 0;
	virtual NxClothVertexAttachmentStatus getVertexAttachmentStatus(NxU32 vertexId) const = 0;
	virtual NxShape* getVertexAttachmentShape(NxU32 vertexId) const = 0;
	virtual NxVec3 getVertexAttachmentPosition(NxU32 vertexId) const = 0;
	virtual bool attachToCore(NvShape *shape, NxReal impulseThreshold, NxReal penetrationDepth, NxReal maxDeformationDistance) = 0;
	virtual void detachFromCore(NvShape *shape) = 0;  // no api call, only used in case shape is deleted

	virtual bool tearVertex(const NxU32 vertexId, const NxVec3 &normal) = 0;
	virtual bool raycast(const NxRay& worldRay, NxVec3 &hit, NxU32 &vertexId) = 0;
	virtual	void addForceAtVertex(const NxVec3& force, NxU32 vertexId, NxForceMode mode = NX_FORCE) = 0;
	virtual void addForceAtPos(const NxVec3& position, NxReal magnitude, NxReal radius, NxForceMode mode = NX_FORCE) = 0;
	virtual void addDirectedForceAtPos(const NxVec3& position, const NxVec3& force, NxReal radius, NxForceMode mode = NX_FORCE) = 0;
	
	virtual	void updateParticles(const NvClothParticleUpdateData& updateData) = 0;
	virtual const NxU8* getFirstPositionByte() const = 0;
	virtual const NxU8* getFirstVelocityByte() const = 0;
	virtual NxU32  getPositionByteStride() const = 0;
	virtual NxU32  getVelocityByteStride() const = 0;

	virtual NxForceFieldMaterial getForceFieldMaterial() const = 0;
	virtual void setForceFieldMaterial(NxForceFieldMaterial) = 0;

	// NxCloth only
	virtual	bool overlapAABBTriangles(const NxBounds3& bounds, NxU32& nb, const NxU32*& indices)	const = 0;
	// NxSoftBody only
	virtual	bool overlapAABBTetrahedra(const NxBounds3& bounds, NxU32& nb, const NxU32*& indices)	const = 0;

	virtual void setGroup(NxCollisionGroup collisionGroup) = 0;
	virtual NxCollisionGroup getGroup() const = 0;

	virtual void setGroupsMask(const NxGroupsMask& groupsMask) = 0;
	virtual const NxGroupsMask* getGroupsMask() const = 0;

	virtual bool loadFromDesc(const DeformableDesc& desc) = 0;
	virtual NxMeshData& getMeshData() = 0;
	virtual void setMeshData(NxMeshData& meshData) = 0;
	virtual NxSoftBodySplitPairData& getSplitPairData() = 0;
	virtual NxClothSplitPairData& getClothSplitPairData() = 0;
	virtual void setSplitPairData(NxSoftBodySplitPairData& splitPairData) = 0;
	virtual void setClothSplitPairData(NxClothSplitPairData& splitPairData) = 0;
	virtual void visualize(NxFoundation::DebugRenderable& dr) = 0;

	virtual NxU32 getLaneUsec() = 0;
	virtual	NvScene* getOwnerScene() const = 0;
};

/*----------------------------------------------------------------------------*/

#endif

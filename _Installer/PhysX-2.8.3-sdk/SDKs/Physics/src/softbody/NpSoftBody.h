#ifndef NX_PHYSICS_NP_SOFTBODY
#define NX_PHYSICS_NP_SOFTBODY

/*----------------------------------------------------------------------------*\
|
| NVIDIA PhysX Technology
|
| www.nvidia.com
|
\*----------------------------------------------------------------------------*/

#include "ReadWriteMutex.h"
#include "NxSoftBody.h"
#include "DebugRenderable.h"

class NvCloth;
class NpScene;

class NpSoftBody : public NxSoftBody, public NxFoundation::NxAllocateable, public ReadWriteMutex
{
public:
									NpSoftBody(NvCloth* nvCloth, NpScene* npScene);
	virtual							~NpSoftBody();
	void							visualize(NxFoundation::DebugRenderable & dr);
	bool							saveToDesc(NxSoftBodyDesc& desc) const;

	void							setStretchingStiffness(NxReal stiffness);
	NxReal							getStretchingStiffness() const;

	void							setHardStretchLimitationFactor(NxReal factor);
	NxReal							getHardStretchLimitationFactor() const;

	void							setVolumeStiffness(NxReal stiffness);
	NxReal							getVolumeStiffness() const;

	void							setFriction(NxReal friction);
	NxReal							getFriction() const;

	void							setTearFactor(NxReal factor);
	NxReal							getTearFactor() const;

	void							setAttachmentTearFactor(NxReal factor);
	NxReal							getAttachmentTearFactor() const;

	void							setParticleRadius(NxReal particleRadius);
	NxReal							getParticleRadius() const;

	void							setSelfCollisionThickness(NxReal selfCollisionThickness);
	NxReal							getSelfCollisionThickness() const;

	NxReal							getDensity() const;

	NxReal							getRelativeGridSpacing() const;

	void							setDampingCoefficient(NxReal dampingCoefficient);
	NxReal							getDampingCoefficient() const;

    void                            setSolverIterations(NxU32 iterations);
	NxU32                           getSolverIterations() const;

	void							setCollisionResponseCoefficient(NxReal coefficient);
	NxReal							getCollisionResponseCoefficient() const;

	void							setAttachmentResponseCoefficient(NxReal coefficient);
	NxReal							getAttachmentResponseCoefficient() const;

	void							setFromFluidResponseCoefficient(NxReal coefficient);
	NxReal							getFromFluidResponseCoefficient() const;

	void							setToFluidResponseCoefficient(NxReal coefficient);
	NxReal							getToFluidResponseCoefficient() const;

	void							setMinAdhereVelocity(NxReal velocity);
	NxReal							getMinAdhereVelocity() const;

	void							setExternalAcceleration(NxVec3 acceleration);
	NxVec3							getExternalAcceleration() const;

	NxSoftBodyMesh*					getSoftBodyMesh() const;

	void							setPosition(const NxVec3& position, NxU32 vertexId);
	NxVec3							getPosition(NxU32 vertexId) const;

	void							setVelocity(const NxVec3& velocity, NxU32 vertexId);
	NxVec3							getVelocity(NxU32 vertexId) const;	

	void							setPositions(void* buffer, NxU32 byteStride = sizeof(NxVec3));
	void							getPositions(void* buffer, NxU32 byteStride = sizeof(NxVec3));

	void							setVelocities(void* buffer, NxU32 byteStride = sizeof(NxVec3));
    void							getVelocities(void* buffer, NxU32 byteStride = sizeof(NxVec3));

	void                            setConstrainPositions(void* buffer, NxU32 byteStride = sizeof(NxVec3));
	void                            setConstrainNormals(void* buffer, NxU32 byteStride = sizeof(NxVec3));
	void                            setConstrainCoefficients(const NxSoftBodyConstrainCoefficients *coefficients, NxU32 byteStride = sizeof(NxSoftBodyConstrainCoefficients));

	NxU32							getNumberOfParticles();
	NxU32							queryShapePointers();
	
	NxU32							getStateByteSize();

	void							getShapePointers(NxShape** shapePointers,NxU32 *flags);
	void							setShapePointers(NxShape** shapePointers,unsigned int numShapes);

	void							saveStateToStream(NxStream& stream, bool permute);
	void							loadStateFromStream(NxStream& stream);

	bool							isSleeping() const;
    NxReal							getSleepLinearVelocity() const;
    void							setSleepLinearVelocity(NxReal threshold);
	void							wakeUp(NxReal wakeCounterValue);
	void							putToSleep();

	void							getWorldBounds(NxBounds3& bounds) const;

	void							setValidBounds(const NxBounds3& validBounds);
	void							getValidBounds(NxBounds3& validBounds) const;

	void							setFlags(NxU32 flags);
	NxU32							getFlags() const;

	NxScene&						getScene() const;

	NX_INLINE	const NvCloth*		getNvClothConst() const	{ return nvCloth; }
	NX_INLINE	NvCloth*			getNvCloth() { return nvCloth; }

	void							attachToShape(const NxShape *shape, NxU32 attachmentFlags);
	void							attachToCollidingShapes(NxU32 attachmentFlags);
	void							detachFromShape(const NxShape *shape);
	void							attachVertexToShape(NxU32 vertexId, const NxShape *shape, const NxVec3 &localPos, NxU32 attachmentFlags);
	void							attachVertexToGlobalPosition(const NxU32 vertexId, const NxVec3 &pos);
	void							freeVertex(const NxU32 vertexId);

	bool							tearVertex(const NxU32 vertexId, const NxVec3 &normal);

	bool							raycast(const NxRay& worldRay, NxVec3 &hit, NxU32 &vertexId);

	void							addForceAtPos(const NxVec3& position, NxReal magnitude, NxReal radius, NxForceMode mode = NX_FORCE);
	void							addDirectedForceAtPos(const NxVec3& position, const NxVec3& force, NxReal radius, NxForceMode mode = NX_FORCE);

	void							addForceAtVertex(const NxVec3& force, NxU32 vertexId, NxForceMode mode = NX_FORCE);
	bool							overlapAABBTetrahedra(const NxBounds3& bounds, NxU32& nb, const NxU32*& indices) const;

	void							setGroup(NxCollisionGroup collisionGroup);
	NxCollisionGroup				getGroup() const;

	NxForceFieldMaterial			getForceFieldMaterial()	const;
	void 							setForceFieldMaterial(NxForceFieldMaterial mat);

	void							setGroupsMask(const NxGroupsMask& groupsMask);
	const							NxGroupsMask getGroupsMask() const;

	virtual		NxMeshData			getMeshData();
	virtual		void 				setMeshData(NxMeshData& meshData);
	virtual		NxSoftBodySplitPairData		getSplitPairData();
	virtual		void 						setSplitPairData(NxSoftBodySplitPairData& splitPairData);

	const char*						getName() const;
	void							setName(const char*);
	NxCompartment *					getCompartment() const;
	NX_INLINE void					setCompartment(NxCompartment *c) { compartment = c; }

	void							sendToVRD(bool create);
	void							removeFromVRD();
protected:
	NvCloth*						nvCloth;
	NpScene*						npScene;
	NxCompartment *					compartment;
	NpActor*                        coreActor;
};

#endif

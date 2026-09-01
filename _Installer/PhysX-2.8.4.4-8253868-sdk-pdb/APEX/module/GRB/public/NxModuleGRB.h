#ifndef __NX_MODULE_GRB_H__
#define __NX_MODULE_GRB_H__

#include "NxApex.h"

class GrbActorDesc;
class GrbActor;
class GrbConvexMeshDesc;
class GrbCookedConvexMesh;

namespace physx {
namespace apex {

PX_PUSH_PACK_DEFAULT

enum NxModuleGRBLODParameters
{
	/**
		Controls the radius of each particle in the scene.  The lower
		the scale, the larger the radius (the lower the perf cost)
	*/
	NX_GRB_SCALE_PARTICLE_SIZE = 0,
};

enum { NX_MAX_GRB_COUNT = 16 * 1024 };

class NxModuleGRBDesc : public NxApexDesc
{
public:

	/**
	\brief Constructor sets to default.
	*/
	PX_INLINE NxModuleGRBDesc();

	/**
	\brief (re)sets the structure to the default.	
	*/
	PX_INLINE void	setToDefault();

	/**
		Returns true iff an object can be created using this descriptor.
	*/
	PX_INLINE bool	isValid() const;

	/**
		Module configurable parameters.
	*/

	/**
	\brief Size of grid cells used in mesh collision.
	*/
	physx::PxF32 meshCellSize;
	/**
	\brief Collision skin width, as in PhysX.
	*/
	physx::PxF32 skinWidth;
	/**
	\brief The density to use for buld actor creation
	*/
	physx::PxF32 defaultDensity;
	/**
	\brief Maximum number of solver iterations, as in PhysX.
	*/
	physx::PxU32 maxIterations;
	/**
	\brief Maximum linear acceleration
	*/
	physx::PxF32 maxLinAcceleration;
	/**
	\brief Particle spacing
	*/
	physx::PxF32 particleSpacing;
	/**
	\brief visualize GRB particles if true, else box primitive
	*/
	bool visualizeParticles;
    /**
    \brief override automatic GPU detection and selection

    Setting this field to -1 will disable GRB entirely.  Leaving
    this field at it's default value (-2) will allow GRB to use the
    default PhysX GPU as defined by the NVIDIA control panel.  Any
    other value will explicitly specify the GPU ordinal to use for
    all GRB scenes.
    */
    physx::PxI32 gpuDeviceOrdinal;

	/**
	\brief Amount of GPU memory to allocate for GRB use, in MB
	*/
	physx::PxU32 gpuHeapSize;
};

PX_INLINE NxModuleGRBDesc::NxModuleGRBDesc()
{
	setToDefault();
}

PX_INLINE void NxModuleGRBDesc::setToDefault()
{
	NxApexDesc::setToDefault();
	meshCellSize = 2.0f/8.0f;
	skinWidth = 0.01f;
	defaultDensity = 10.0f;
	maxIterations = 4;
	visualizeParticles = false;
	gpuHeapSize = 128;
	particleSpacing = 0.1f;
    maxLinAcceleration = FLT_MAX;
    gpuDeviceOrdinal = -2;
}

PX_INLINE bool NxModuleGRBDesc::isValid() const
{
	return NxApexDesc::isValid() && maxIterations > 0 && gpuHeapSize > 0;
}

class NxModuleGRB : public NxModule
{
public:
	virtual void                        init( const NxModuleGRBDesc& ) = 0;

	virtual void                        setVisualizeParticles( bool ) = 0;
	virtual bool                        getVisualizeParticles() const = 0;

	virtual void                        setMeshCellSize( float cellSize ) = 0;
	virtual void                        setParticleSpacing( float particleSpacing ) = 0;
	virtual void                        setMaxLinAcceleration( float maxLinAcceleration ) = 0;

	virtual GrbActor *                  createActor( const GrbActorDesc&, NxApexScene& ) = 0;
	virtual void                        releaseActor( GrbActor&, NxApexScene& ) = 0;
	virtual void                        setDefaultDensity( physx::PxF32 density ) = 0;

	virtual physx::PxU32          				registerConvexMesh( const GrbConvexMeshDesc& desc, const physx::PxMat34Legacy &pose, const NxApexScene& ) = 0;
	virtual bool						cookConvexMesh( const GrbConvexMeshDesc& desc, const physx::PxMat34Legacy &pose, GrbCookedConvexMesh& stream, const NxApexScene& ) = 0;

	virtual bool                        hasScene( const NxApexScene& ) const = 0;
	virtual bool                        isEnabled() const = 0;
	virtual void                        disableSimulation() = 0;
	virtual void                        enableSimulation() = 0;

protected:
	virtual ~NxModuleGRB() {}
};

#if !defined(_USRDLL)
/* If this module is distributed as a static library, the user must call 
 * instantiateGRBModule() before calling NxApexSDK::createModule("GRB")
 */
void instantiateGRBModule();
#endif

PX_POP_PACK

}} // namespace physx::apex

#endif

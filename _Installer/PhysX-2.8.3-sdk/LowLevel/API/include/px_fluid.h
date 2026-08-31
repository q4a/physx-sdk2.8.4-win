#ifndef PXD_FLUID_H
#define PXD_FLUID_H

#include "px_config.h"
#include "px_task.h"


#ifdef __cplusplus
extern "C"
{
#endif

/*!
\file
Fluid interface.
*/

typedef PxdHandle PxdShape;		// This redefinition of the shape handle is ugly but there are circular
								// dependencies of header files which make this necessary.

/************************************************************************/
/* Fluids                                                               */
/************************************************************************/

/*
Fluid related constants
*/
// Maximum number of particles per fluid
#define PXD_FLUID_PARTICLE_LIMIT		((1<<16)-1)

// Size of fluid packet hash table.
// Has to be a power of 2. Must be at least as large as the packet limit (see further below) but should
// be larger for the hash to be efficient.
#define PXD_FLUID_PACKET_HASH_SIZE		1024

// Maximum number of fluid packets (should be smaller than hash size since a full hash table is not efficient)
#define PXD_FLUID_PACKET_LIMIT			924

// Slack for building the triangle packet hash. Has to be bigger than any epsilons used in collision detection.
// TODO: Move this to PxsFluid if the high level does not need this parameter anymore
#define PXD_FLUID_COLLISION_SLACK		1.0e-3f


typedef PxdHandle PxdFluid;
typedef PxdHandle PxdFluidManager;

/*!
Fluid simulation method types
*/
typedef enum PxdFluidSimulationMethod_
{
	/*!
	Enable simulation of inter particle forces.
	*/
	PXD_F_SPH						= (1<<0),	

	/*!
	Do not simulate inter particle forces.
	*/
	PXD_F_NO_PARTICLE_INTERACTION	= (1<<1)
} PxdFluidSimulationMethod;

/*!
Fluid flags
*/
typedef enum PxdFluidFlag_
{
	/*!
	Fluid affects rigid body on collision.
	*/
	PXD_FF_COLLISION_TWOWAY	= (1<<0),

	/*!
	Project position and velocity of fluid particles to a plane.
	*/
	PXD_FF_PROJECT_TO_PLANE	= (1<<1)
} PxdFluidFlag;

/*!
Fluid properties
*/
typedef enum PxdFluidProperty_
{
	/*!
	Maximal number of particles for the fluid used in the simulation.
	
	1 PxU32, read-only
	*/
	PXD_FLUID_MAX_PARTICLES,
	/*!
	Particle resolution (particle per linear unit) when the fluid is in rest state (relaxed).
	Defines emission density of emitters.

	1 PxFloat, read-only
	*/
	PXD_FLUID_REST_PARTICLES_PER_UNIT,
	/*!
	Target density for the fluid.

	1 PxFloat, read-only
	*/
	PXD_FLUID_REST_DENSITY,
	/*!
	Self density of the fluid in resting state.

	1 PxFloat, read-only
	*/
	PXD_FLUID_SELF_DENSITY,
	/*!
	Radius of influence for particle interaction.
	This parameter is relative to the rest spacing of the particles.

	1 PxFloat, read-only
	*/
	PXD_FLUID_KERNEL_RADIUS_MULTIPLIER,
	/*!
	Maximal distance a particle is allowed to travel within one timestep.
	This parameter is relative to the rest spacing of the particles.

	1 PxFloat, read-only
	*/
	PXD_FLUID_MOTION_LIMIT_MULTIPLIER,
	/*!
	Distance between particles and collision geometry that should be maintained.
	This parameter is relative to the rest spacing of the particles.

	1 PxFloat, positive, read-only
	*/
	PXD_FLUID_COLLISION_DISTANCE_MULTIPLIER,
	/*!
	Parameter to control the parallelization of the fluid.
	
	The spatial domain is divided into so called packets, equal sized cubes, aligned in a grid.
	
	The parameter given defines the 2-logarithm of the edge length of such a packet.
	This parameter is relative to the interaction radius of the particles.

	1 PxU32, read-only
	*/
	PXD_FLUID_PACKET_SIZE_MULTIPLIER_LOG2,
	/*!
	Stiffness of the particle interaction related to the pressure. This factor linearly scales the force
	which acts on particles which are closer to each other than the rest spacing.

	1 PxFloat, positive
	*/
	PXD_FLUID_STIFFNESS,
	/*!
	Parameter to define viscous behaviour of fluid.

	1 PxFloat, positive
	*/
	PXD_FLUID_VISCOSITY,
	/*!
	The surfaceTension of the fluid defines an attractive force between particles.
	Higher values will result in smoother surfaces.

	1 PxFloat, nonnegative
	*/
	PXD_FLUID_SURFACE_TENSION,
	/*!
	Velocity damping constant for all particles.

	1 PxFloat, nonnegative
	*/
	PXD_FLUID_DAMPING,
	/*!
	Timespan for the particle "fade-in".
	This feature is experimental. When a particle is created it has no influence on the simulation. 
	It takes fadeInTime until the particle has full influence. If set to zero, the particle has full 
	influence on the simulation from start.

	1 PxFloat, nonnegative
	*/
	PXD_FLUID_FADE_IN_TIME,
	/*!
	Acceleration to apply to all particles.

	1 PxdVector
	*/
	PXD_FLUID_EXTERNAL_ACCELERATION,
	/*!
	Restitution coefficient for collisions of fluid particles with static geometry.

	1 PxFloat, [0,1]
	*/
	PXD_FLUID_STATIC_COLLISION_RESTITUTION,
	/*!
	"Friction" coefficient for collisions of fluid particles with static geometry.

	1 PxFloat, [0,1]
	*/
	PXD_FLUID_STATIC_COLLISION_ADHESION,
	/*!
	Strength of attraction between the particles and static rigid bodies on collision.

	1 PxFloat, nonnegative
	*/
	PXD_FLUID_STATIC_COLLISION_ATTRACTION,
	/*!
	Restitution coefficient for collisions of fluid particles with dynamic geometry.

	1 PxFloat, [0,1]
	*/
	PXD_FLUID_DYNAMIC_COLLISION_RESTITUTION,
	/*!
	"Friction" coefficient for collisions of fluid particles with dynamic geometry.

	1 PxFloat, [0,1]
	*/
	PXD_FLUID_DYNAMIC_COLLISION_ADHESION,
	/*!
	Strength of attraction between the particles and dynamic rigid bodies on collision.

	1 PxFloat, nonnegative
	*/
	PXD_FLUID_DYNAMIC_COLLISION_ATTRACTION,
	/*!
	Factor for impulse transfer from fluid to colliding rigid body.

	1 PxFloat, nonnegative
	*/
	PXD_FLUID_COLLISION_RESPONSE_COEFFICIENT,
	/*!
	Time step to use for the fluid simulation.

	1 PxFloat, nonnegative
	*/
	PXD_FLUID_SIMULATION_TIME_STEP,
	/*!
	Defines whether or not particle-geometry interactions are considered in the simulation.

	1 PxdFluidSimulationMethod - flag
	*/
	PXD_FLUID_SIMULATION_METHOD,

	/*!
	Defines the plane the fluid particles are projected to.

	1 PxdPlane
	*/
	PXD_FLUID_PROJECTION_PLANE,

	/*!
	Fluid specific flags.

	1 PxU32 - flag
	*/
	PXD_FLUID_FLAG,

	/*!
	Stiction coefficient for collisions of fluid particles with static geometry.

	1 PxFloat, [0,1]
	*/
	PXD_FLUID_STATIC_COLLISION_STICTION,

	/*!
	Stiction coefficient for collisions of fluid particles with dynamic geometry.

	1 PxFloat, [0,1]
	*/
	PXD_FLUID_DYNAMIC_COLLISION_STICTION

} PxdFluidProperty;


/*!
Descriptor for fluids

\sa PxdFluidProperty
*/
typedef struct PxdFluidDesc_
{
	/*!
	Maximal number of particles for the fluid used in the simulation.

	\sa PXD_FLUID_MAX_PARTICLES
	*/
	PxU32			maxParticles;

	/*!
	Particle resolution.

	\sa PXD_FLUID_REST_PARTICLES_PER_UNIT
	*/
	PxFloat			restParticlesPerUnit;

	/*!
	Target density.

	\sa PXD_FLUID_REST_DENSITY
	*/
	PxFloat			restDensity;

	/*!
	Radius of influence for particle interaction.

	\sa PXD_FLUID_KERNEL_RADIUS_MULTIPLIER
	*/
	PxFloat			kernelRadiusMultiplier;

	/*!
	Maximal particle travel distance.

	\sa PXD_FLUID_MOTION_LIMIT_MULTIPLIER
	*/
	PxFloat			motionLimitMultiplier;

	/*!
	Distance between particles and collision geometry.

	\sa PXD_FLUID_COLLISION_DISTANCE_MULTIPLIER
	*/
	PxFloat			collisionDistanceMultiplier;

	/*!
	Size of fluid packets.

	\sa PXD_FLUID_PACKET_SIZE_MULTIPLIER_LOG2
	*/
	PxU32			packetSizeMultiplierLog2;

	/*!
	Stiffness of the particle interaction.

	\sa PXD_FLUID_STIFFNESS
	*/
	PxFloat			stiffness;

	/*!
	Viscosity of fluid.

	\sa PXD_FLUID_VISCOSITY
	*/
	PxFloat			viscosity;

	/*!
	Attractive force between particles.

	\sa PXD_FLUID_SURFACE_TENSION
	*/
	PxFloat			surfaceTension;

	/*!
	Velocity damping.

	\sa PXD_FLUID_DAMPING
	*/
	PxFloat			damping;

	/*!
	Timespan for the particle "fade-in".

	\sa PXD_FLUID_FADE_IN_TIME
	*/
	PxFloat			fadeInTime;

	/*!
	Acceleration to apply to all particles.

	\sa PXD_FLUID_EXTERNAL_ACCELERATION
	*/
	PxdVector		externalAcceleration;

	/*!
	Restitution coefficient.

	\sa PXD_FLUID_STATIC_COLLISION_RESTITUTION
	*/
	PxFloat			staticCollisionRestitution;

	/*!
	"Friction" coefficient.

	\sa PXD_FLUID_STATIC_COLLISION_ADHESION
	*/
	PxFloat			staticCollisionAdhesion;

	/*!
	Strength of attraction between the particles and static rigid bodies on collision.

	\sa PXD_FLUID_STATIC_COLLISION_ATTRACTION
	*/
	PxFloat			staticCollisionAttraction;

	/*!
	Restitution coefficient.

	\sa PXD_FLUID_DYNAMIC_COLLISION_RESTITUTION
	*/
	PxFloat			dynamicCollisionRestitution;

	/*!
	"Friction" coefficient.

	\sa PXD_FLUID_DYNAMIC_COLLISION_ADHESION
	*/
	PxFloat			dynamicCollisionAdhesion;

	/*!
	Strength of attraction between the particles and dynamic rigid bodies on collision.

	\sa PXD_FLUID_DYNAMIC_COLLISION_ATTRACTION
	*/
	PxFloat			dynamicCollisionAttraction;

	/*!
	Impulse transfer factor.

	\sa PXD_FLUID_COLLISION_RESPONSE_COEFFICIENT
	*/
	PxFloat			collisionResponseCoefficient;

	/*!
	Time step to use for the fluid simulation.

	\sa PXD_FLUID_SIMULATION_TIME_STEP
	*/
	PxFloat			simulationTimeStep;

	/*!
	Defines simulation method.

	\sa PXD_FLUID_SIMULATION_METHOD
	*/
	PxdFluidSimulationMethod	simulationMethod;

	/*!
	Projection plane.

	\sa PXD_FLUID_PROJECTION_PLANE
	*/
	PxdPlane		projectionPlane;

	/*!
	Fluid flags.

	\sa PXD_FLUID_FLAG
	*/
	PxU32			flags;
	
} PxdFluidDesc;


/*!
Initialize a fluid descriptor to default values
\param desc
Fluid descriptor to initialize
*/
PXD_EXPORT_FUNCTION void PxdFluidInitDesc(PxdFluidDesc* desc);

/*!
Create fluid
\param desc
Fluid descriptor.
\return
Fluid identifier. Zero on failure.
*/
PXD_EXPORT_FUNCTION PxdFluid PxdFluidCreate(PxdContext context, PxdFluidDesc* desc);

/*!
Destroy fluid
\param fluid
Fluid identifier
*/
PXD_EXPORT_FUNCTION void PxdFluidDestroy(PxdFluid fluid);

/*!
Activate a fluid
\param fluid
Fluid identifier
*/
PXD_EXPORT_FUNCTION void PxdFluidActivate(PxdFluid fluid);

/*!
Deactivate a fluid
\param fluid
Fluid identifier
*/
PXD_EXPORT_FUNCTION void PxdFluidDeactivate(PxdFluid fluid);

/*!
Test whether a fluid is active
\param fluid
Fluid identifier
\return
1 if fluid is active, 0 else
*/
PXD_EXPORT_FUNCTION PxBool PxdFluidIsActive(PxdFluid fluid);


/*!
Structure to define a particle interval that should be updated.
*/
typedef struct PxdFluidParticleUpdateInterval_
{
	PxU16	startIndexSrc;		//! Index pointing to the start of the update data buffer
	PxU16	startIndexDest;		//! Index pointing to the first particle of the interval that should be updated
	PxU16	particleCount;		//! Size of update interval, i.e., number of particles to update
} PxdFluidParticleUpdateInterval;

/*!
Particle flags are used to initiate certain particle updates or to give some 
additional information about the particles.
*/
typedef enum PxdFluidParticleFlag_
{
	//
	// Particle status flags
	//
	/*!
	Particle collided with static shape.
	*/
	PXD_FP_COLLISION_WITH_STATIC	= (1<<0),

	/*!
	Particle collided with dynamic shape.
	*/
	PXD_FP_COLLISION_WITH_DYNAMIC	= (1<<1),

	/*!
	Particle hit drain.
	*/
	PXD_FP_DRAIN					= (1<<2),

	//
	// Particle update flags
	//
	/*!
	Particle should be deleted.
	*/
	PXD_FP_DELETE					= (1<<3),
} PxdFluidParticleFlag;

/*!
Descriptor for fluid particle creation / retrieval
*/
typedef struct PxdFluidParticleDataDesc_
{
	const PxdF32*		position;					//! Particle positions [IN/OUT]
	const PxdF32*		velocity;					//! Particle velocities [IN/OUT]
	const PxdF32*		lifeTime;					//! Particle lifetimes [IN/OUT]
	const PxdF32*		density;					//! Particle densities [OUT]
	const PxU16*		id;							//! Particle ids [IN/OUT]
	const PxU16*		flag;						//! Particle flags [OUT]
	const PxdF32*		collisionNormal;			//! Particle collision normals or, in the case of two way interaction, collision impulses [OUT]
	const PxdShape*		collisionShape;				//! Handle of colliding shape for each particle (zero if no collision) [OUT]

    PxU32				positionByteStride;			//! Separation (in bytes) between consecutive particle positions.
    PxU32				velocityByteStride;			//! Separation (in bytes) between consecutive particle velocities.
    PxU32				lifeTimeByteStride;			//! Separation (in bytes) between consecutive particle lifetimes.
	PxU32				densityByteStride;			//! Separation (in bytes) between consecutive particle densities.
	PxU32				idByteStride;				//! Separation (in bytes) between consecutive particle ids.
	PxU32				flagByteStride;				//! Separation (in bytes) between consecutive particle flags.
	PxU32				collisionNormalByteStride;	//! Separation (in bytes) between consecutive particle collision normals.
	PxU32				collisionShapeByteStride;	//! Separation (in bytes) between consecutive particle collision shapes.
} PxdFluidParticleDataDesc;

/*!
Conversion macros to map particle data which is represented as a byte buffer to the correct type.
*/
#define PXD_GET_PARTICLE_POSITION(src, dest)	const PxdF32* dest = reinterpret_cast<const PxdF32*>(src);
#define PXD_GET_PARTICLE_VELOCITY(src, dest)	const PxdF32* dest = reinterpret_cast<const PxdF32*>(src);
#define PXD_GET_PARTICLE_LIFE_TIME(src, dest)	const PxdF32* dest = reinterpret_cast<const PxdF32*>(src);
#define PXD_GET_PARTICLE_DENSITY(src, dest)		const PxdF32* dest = reinterpret_cast<const PxdF32*>(src);
#define PXD_GET_PARTICLE_ID(src, dest)			const PxU16*  dest = reinterpret_cast<const PxU16*>(src);
#define PXD_GET_PARTICLE_FLAG(src, dest)		const PxU16*  dest = reinterpret_cast<const PxU16*>(src);
#define PXD_GET_PARTICLE_COLLISION_NORMAL(src, dest) const PxdF32* dest = reinterpret_cast<const PxdF32*>(src);
#define PXD_GET_PARTICLE_COLLISION_SHAPE(src, dest)	const PxdShape* dest = reinterpret_cast<const PxdShape*>(src);

/*!
Add particles to fluid (to implement emitters and to define initial particles)
\param fluid
Fluid identifier
\param count
Number of particles to add
\param particles
Description of particles to add
NOTE: Only position, velocity, lifetime and id properties will be written. Any other information will be ignored.
*/
PXD_EXPORT_FUNCTION void PxdFluidAddParticles(PxdFluid fluid, PxUInt count, PxdFluidParticleDataDesc* particles);

/*!
Remove all particles from fluid.
\param fluid
Fluid identifier
*/
PXD_EXPORT_FUNCTION void PxdFluidRemoveAllParticles(PxdFluid fluid);

/*!
Get IDs of deleted fluid particles.
\param fluid
Fluid identifier
\param particleIndices
List of deleted particle IDs
\return
Number of IDs
*/
PXD_EXPORT_FUNCTION PxU32 PxdFluidGetDeletedParticleIds(PxdFluid fluid, const PxU16** particleIds);

/*!
Get number of particles for a fluid.
\param fluid
Fluid identifier
\return
Number of particles
*/
PXD_EXPORT_FUNCTION PxU32 PxdFluidGetParticleCount(PxdFluid fluid);

/*!
Get particle data for a fluid.
\param fluid
Fluid identifier
\param particles
Fluid particle data (memory is allocated in low level)
\return
Number of particles
*/
PXD_EXPORT_FUNCTION PxU32 PxdFluidGetParticles(PxdFluid fluid, PxdFluidParticleDataDesc* particles);


/*!
Descriptor for fluid particle update
*/
typedef struct PxdFluidParticleUpdateDataDesc_
{
	const PxdF32*		deltaVelocity;				//! Particle velocity changes
	const PxU16*		flag;						//! Particle update flags

    PxU32				deltaVelocityByteStride;	//! Separation (in bytes) between consecutive particle velocity changes.
	PxU32				flagByteStride;				//! Separation (in bytes) between consecutive particle update flags.
} PxdFluidParticleUpdateDataDesc;

/*!
Update fluid particles.
\param fluid
Fluid identifier
\param intervalCount
Number of particle index pairs which define the particle intervals to update.
\param updateData
Buffer with particle update information

NOTE: Using this call to set the deletion flag of a particle will not delete the particle immediately. Particles marked with the deletion flag will be removed at the next call of PxdFluidUpdateShapes (see below).
*/
PXD_EXPORT_FUNCTION void PxdFluidUpdateParticles(PxdFluid fluid, const PxdFluidParticleUpdateInterval* updateIntervals, 
												 PxUInt intervalCount, PxdFluidParticleUpdateDataDesc* updateData);


/*!
Descriptor for updated fluid packet shapes
*/
typedef struct PxdFluidShapeUpdateResults_
{	
	const PxdShape*			createdShapes;			//! Handles of newly created fluid packet shapes
	PxU32					createdShapeCount;		//! Number of newly created fluid packet shapes
	const PxdShape*			destroyedShapes;		//! Handles of fluid packet shapes to delete
	PxU32					destroyedShapeCount;	//! Number of fluid packet shapes to delete
} PxdFluidShapeUpdateResults;

/*!
Get created/deleted fluid shapes
\param fluid
Fluid identifier
\param updateResults
Information on created/deleted packets (memory is allocated by low level)
\return
Update task identifier.
*/
PXD_EXPORT_FUNCTION void PxdFluidGetShapesUpdate(PxdFluid fluid, PxdFluidShapeUpdateResults* updateResults);

/*!
Update fluid packets.
\param fluid
Fluid identifier
\return
Update task identifier.
*/
PXD_EXPORT_FUNCTION PxdTask PxdFluidShapesUpdate(PxdFluid fluid);


/*!
Solve fluid dynamics.
\param fluid
Fluid identifier
\param dt
Timestep for simulation
\return
Update task identifier.
*/
PXD_EXPORT_FUNCTION PxdTask PxdFluidDynamicsUpdate(PxdFluid fluid);


/*!
Solve fluid collisions.
\param fluid
Fluid identifier
\return
Update task identifier.
*/
PXD_EXPORT_FUNCTION PxdTask PxdFluidCollisionUpdate(PxdFluid fluid);


/*!
\addtogroup Fluid property set/get
@{
*/
PXD_EXPORT_FUNCTION void PxdFluidSetInt(PxdFluid fluid, PxdFluidProperty property, PxInt value);
PXD_EXPORT_FUNCTION PxInt PxdFluidGetInt(PxdFluid fluid, PxdFluidProperty property);

PXD_EXPORT_FUNCTION void PxdFluidSetFloat(PxdFluid fluid, PxdFluidProperty property, PxFloat value);
PXD_EXPORT_FUNCTION PxFloat PxdFluidGetFloat(PxdFluid fluid, PxdFluidProperty property);

PXD_EXPORT_FUNCTION void PxdFluidSetVec(PxdFluid fluid, PxdFluidProperty property, PxdVector v);
PXD_EXPORT_FUNCTION PxdVector PxdFluidGetVec(PxdFluid fluid, PxdFluidProperty property);

PXD_EXPORT_FUNCTION void PxdFluidSetPlane(PxdFluid fluid, PxdPlane p);
PXD_EXPORT_FUNCTION PxdPlane PxdFluidGetPlane(PxdFluid fluid);


/*!
@}
*/


#ifdef __cplusplus
}
#endif


#endif

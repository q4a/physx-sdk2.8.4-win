#ifndef PXS_FLUID_CONFIG_H
#define PXS_FLUID_CONFIG_H

#include "px_fluid.h"

//
// Support fluids in LowLevel
// If a platform can not support fluids, disable the define below
//
#if NX_USE_FLUID_API
#define PXS_ENABLE_FLUIDS
#endif

#ifdef PXS_ENABLE_FLUIDS


// Memory alignment for fluid data structures [byte]
#ifdef PXD_PLATFORM_XBOX360
#define PXS_FLUID_DATA_ALIGNMENT	128
#else
#define PXS_FLUID_DATA_ALIGNMENT	16
#endif

// Marker for fluid particles with no collision constraints
#define PXS_FLUID_NO_CONSTRAINT		PXD_FLUID_PARTICLE_LIMIT

// Threshold for the angle between two contact constraint surfaces. If the two surfaces form
// a "steep valley" only one of the two constraints will be applied.
#define PXS_FLUID_COLL_VEL_PROJECTION_CROSS_EPSILON		1e-6f

#define PXS_FLUID_COLL_VEL_PROJECTION_PROJ				1e-4f
#define PXS_FLUID_COLL_TRI_DISTANCE						1e-5f
#define PXS_FLUID_COLL_RAY_EPSILON_FACTOR				1e-4f


// Maximum number of fluid particles in a packet that can be handled at a time
#define PXS_FLUID_SUBPACKET_PARTICLE_LIMIT	PXD_FLUID_PARTICLE_LIMIT

// Maximum number of fluid particles in a packet that can be handled at a time for dividing
// a packet into sections and reordering the particles accordingly
#define PXS_FLUID_SUBPACKET_PARTICLE_LIMIT_PACKET_SECTIONS	PXS_FLUID_SUBPACKET_PARTICLE_LIMIT

// Maximum number of fluid particles in a packet that can be handled at a time for SPH dynamics
// calculations, i.e., computation of density & force
#define PXS_FLUID_SUBPACKET_PARTICLE_LIMIT_FORCE_DENSITY	PXS_FLUID_SUBPACKET_PARTICLE_LIMIT

// Maximum number of fluid particles in a packet that can be handled at a time for velocity
// integration
#define PXS_FLUID_SUBPACKET_PARTICLE_LIMIT_VEL_INTEGRATION	PXS_FLUID_SUBPACKET_PARTICLE_LIMIT

// Maximum number of fluid particles in a packet that can be handled at a time for
// detecting and resolving collisions
#if defined(PXD_PLATFORM_WII) || defined(PXD_PLATFORM_WIIU)
#define PXS_FLUID_SUBPACKET_PARTICLE_LIMIT_COLLISION	50
#else
#define PXS_FLUID_SUBPACKET_PARTICLE_LIMIT_COLLISION	300
#endif
// Hash size for the local particle cell hash.
// - Must to be larger than PXS_FLUID_SUBPACKET_PARTICLE_LIMIT_COLLISION
// - Must be a power of 2
#if defined(PXD_PLATFORM_WII) || defined(PXD_PLATFORM_WIIU)
#define PXS_FLUID_LOCAL_HASH_SIZE_MESH_COLLISION		64
#else
#define PXS_FLUID_LOCAL_HASH_SIZE_MESH_COLLISION		512
#endif

// If the number of particles in a packet and the number of particles for each neighboring halo region
// are below this threshold, then no local hash will be constructed and each particle of one packet will be
// tested against each particle of the other packet (for particle-particle interaction only).
//
// Note: Has to be smaller or equal to PXS_FLUID_SUBPACKET_PARTICLE_LIMIT_FORCE_DENSITY
#if defined(PXD_PLATFORM_WII) || defined(PXD_PLATFORM_WIIU)
#define PXS_FLUID_BRUTE_FORCE_PARTICLE_THRESHOLD		32
#else
#define PXS_FLUID_BRUTE_FORCE_PARTICLE_THRESHOLD		100
#endif

// If the number of particles in a packet section and the number of particles in a neighboring halo
// region are below this threshold, then no local hash will be constructed and each particle of the
// packet section will be tested against each particle of the halo region (for particle-particle interaction only).
//
// Note: Has to be smaller or equal to PXS_FLUID_SUBPACKET_PARTICLE_LIMIT_FORCE_DENSITY
#if defined(PXD_PLATFORM_WII) || defined(PXD_PLATFORM_WIIU)
#define PXS_FLUID_BRUTE_FORCE_PARTICLE_THRESHOLD_HALO_VS_SECTION	32
#else
#define PXS_FLUID_BRUTE_FORCE_PARTICLE_THRESHOLD_HALO_VS_SECTION	100
#endif

// Number of fluid packet shapes to run in parallel during collision update.
// NOTE: For PC this is not used yet
#define PXS_FLUID_NUM_PACKETS_PARALLEL_COLLISION		1

// Initial size of triangle mesh collision buffer (for storing indices of colliding triangles)
#define PXS_FLUID_INITIAL_MESH_COLLISION_BUFFER_SIZE	1024


#endif	// PXS_ENABLE_FLUIDS

#endif

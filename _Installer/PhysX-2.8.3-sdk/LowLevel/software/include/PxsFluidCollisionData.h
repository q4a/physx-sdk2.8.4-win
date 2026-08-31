#ifndef PXS_FLUID_COLLISION_DATA_H
#define PXS_FLUID_COLLISION_DATA_H


#include "PxsFluidConfig.h"
#ifdef PXS_ENABLE_FLUIDS

#include "PxcVector.h"
#include "PxcTransform.h"
#include "PxsFluidSpatialHash.h"

#define PXS_FLUID_SHAPE_ID_NO_SHAPE		0	// Default collision shape ID if a fluid particle has no collision.
											// Since a colliding shape is referenced by its handle, this value 
											// needs to be equal to an invalid handle, i.e., 0.

enum PxsFluidConstraintInfo
{
	PXS_FLUID_CONSTRAINT_INVALID		= 0,
	PXS_FLUID_CONSTRAINT_SHAPE_STATIC	= 1,	// Shape associated to constraint is static
	PXS_FLUID_CONSTRAINT_SHAPE_DYNAMIC	= 2		// Shape associated to constraint is dynamic
};

/*!
Fluid particle collision constraint
*/
struct PxsFluidConstraint
{
	PxcVector			normal;		// Contact surface normal
	PxdF32				d;			// Contact point projected on contact normal
	//16

	PxcVector			velocity;
	PxdShape			shapeId;	// Handle of colliding shape
	//32


public:
	PxsFluidConstraint()
	{
		// Do we want to initialize the constraints on creation?
		//setZero();
	}

	PxsFluidConstraint(const PxcVector& _normal, const PxcVector& _p, const PxcVector& _vel, PxdShape _shapeId)
	{
		normal = _normal;
		d = normal.dot(_p);
		velocity = _vel;
		shapeId = _shapeId;
	}

	PXD_FORCE_INLINE void setZero()
	{
		normal.setZero();
		d = 0.0f;
		velocity.setZero();
		shapeId = PXS_FLUID_SHAPE_ID_NO_SHAPE;
	}

	PXD_FORCE_INLINE bool isZero() const
	{
		return (shapeId == PXS_FLUID_SHAPE_ID_NO_SHAPE);
	}

	PXD_FORCE_INLINE PxcVector project(const PxcVector& p)
	{
		return (p + (normal * (d - normal.dot(p))));
	}
};

/*!
Fluid particle collision constraint pair
*/
struct PxsFluidConstraintPair
{
	PxsFluidConstraint	c0;
	PxsFluidConstraint	c1;
	//64
};

/*!
Different types of collision
*/
enum PxsFluidParticleCollisionFlags
{
	// Global collision flags. Used to track the latest collision status of a particle when
	// testing against potentially colliding shapes
	PXS_FLUID_COLL_FLAG_DC		=	(1<<0),		// Discrete collision
	PXS_FLUID_COLL_FLAG_CC		=	(1<<1),		// Continuous collision

	// When testing a particle against a shape, the following collision flags might be used
	PXS_FLUID_COLL_FLAG_LCC		=	(1<<2),		// Discrete collision: Predicted particle position inside discrete region of shape (shape region + collision radius)
	PXS_FLUID_COLL_FLAG_LDC		=	(1<<3),		// Continuous collision: Predicted particle motion vector intersects shape region
	PXS_FLUID_COLL_FLAG_LPROX	=	(1<<4),		// Proximity collision: Predicted particle position inside proximity region of shape (shape region + proximity radius)
	PXS_FLUID_COLL_FLAG_LANY	=	(PXS_FLUID_COLL_FLAG_LCC | PXS_FLUID_COLL_FLAG_LDC | PXS_FLUID_COLL_FLAG_LPROX)
};

/*!
Structure to track collision data for a fluid particle
*/
struct PxsParticleCollData
{
	PxcVector				surfaceNormal;	// Contact normal [world space]
	PxU32					flags;			// Latest collision status
	//16

	PxcVector				surfacePos;		// Contact point on shape surface [world space]
	PxU32					dcNum;			// Number of discrete collisions
	//32

	PxcVector				surfaceVel;		// Velocity of contact point on shape surface [world space]
	PxdF32					ccTime;			// "Time of impact" for continuous collision
	//48

	PxcVector				oldPos;			// Old particle position
	PxdShape				shapeId;		// Handle of colliding shape
	//64
	
	PxcVector				newPos;			// New particle position 
	PxdShape				shapeIdTmp;		// Temporary handle of colliding shape
	//80

	PxcVector				velocity;		// Particle velocity
	PxU16					c0Info;			// Information on first constraint
	PxU16					c1Info;			// Information on second constraint
	//96
	
	PxcVector				shapeImpulseOrNormal;
	PxU32					pad2;
	//112

	PxsFluidConstraint		c0;
	//144

	PxsFluidConstraint		c1;
	//176


public:
	PXD_FORCE_INLINE void init(const PxcVector& particlePos)
	{
		// Initialize values

		surfaceNormal.setZero();
		flags = 0;

		surfacePos.setZero();
		dcNum = 0;

		surfaceVel.setZero();
		ccTime = 1.0f; // No collision assumed.

		oldPos = particlePos;
		shapeId = PXS_FLUID_SHAPE_ID_NO_SHAPE;
		shapeIdTmp = PXS_FLUID_SHAPE_ID_NO_SHAPE;

		c0Info = PXS_FLUID_CONSTRAINT_INVALID;
		c1Info = PXS_FLUID_CONSTRAINT_INVALID;

		shapeImpulseOrNormal.setZero();

		c0.setZero();
		c1.setZero();
	}
};

/*!
Structure to track collision data for a {particle, shape} pair
*/
struct PxsFluidCollShapeData
{
	PxcVector		localOldPos;		//in
	PxdF32			ccTime;				//in/out
	//16

	PxcVector		localNewPos;		//in
	PxU32			localFlags;			//in/out
	//32

	PxcVector		localSurfaceNormal; //out
	PxU32			pad0;
	//48

	PxcVector		localSurfacePos;	//out
	PxU32			pad1;
	//64

	//
	// The following members are used for collision against mesh
	//

	PxcVector		localConstr0Normal;	//out
	PxU32			numConstraints;		//out
	//80

	PxcVector		localConstr0Pos;	//out
	PxU32			dcNum;				//out
	//96

	PxcVector		localConstr1Normal;	//out
	PxFloat			localConstr0Dist;	//out, only used to cache internal results
	//112

	PxcVector		localConstr1Pos;	//out
	PxFloat			localConstr1Dist;	//out, only used to cache internal results
	//128
};


/*!
Structure to cache a local cell hash that was computed for a set of particles
*/
struct PxsFluidLocalCellHash
{
	PxU32				numParticles;		// Number of particles the cell hash is based on
	PxU16*				particleIndices;	// Particle indices (0..numParticles) with respect to the particle array that was used
											// to build the cell hash. Indices are ordered according to cells.
	PxU32				numHashEntries;		// Size of cell hash table
	PxsFluidCell*		hashEntries;		// Hash entry for cells

	bool				isHashValid;		// Marks whether the hash contains valid data or needs to be computed

	PxsFluidLocalCellHash()
	{
		numParticles = 0;
		particleIndices = NULL;
		numHashEntries = 0;
		hashEntries = NULL;
		isHashValid = false;
	}
};


/*!
Structure for a pair of world to shape transformations of colliding rigid body shapes
*/
struct PxsWorld2ShapePair
{
	PxcTransform		w2s;				// Current world to shape transform
	PxcTransform		w2sOld;				// World to shape transform from previous frame
};


#endif	// PXS_ENABLE_FLUIDS

#endif

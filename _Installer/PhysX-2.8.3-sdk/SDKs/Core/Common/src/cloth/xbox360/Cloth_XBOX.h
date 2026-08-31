#ifndef CLOTH_XBOX_H
#define CLOTH_XBOX_H

#ifndef NX_PHYSICS_CLOTH
#error you must include Cloth.h instead
#endif

// Should maintain 16 byte size/alignment of this structure for the SIMD solver.
//  this is opt for solver,  need adjust again when opt collision 
struct ClothParticle {

	NxVec3 newPosition;		// predicted position which the solver solves for
	NxReal invMass;			// inverse mass, zero for kinematic particles

	NxVec3 position;		// actual position		
	NxU32  flags;			// see NxClothVertexFlag in cloth.cpp		

	NxVec3 velocity;		// velocity of the particle (newPosition - position) / timeStep
	const  Shape *shape;	// colliding or attached shape		
	
	NxVec3 attachment;		// if the vertex is attached (see flags), this is the goal position
	NxReal planeD;			// if the vertex collides (see flags), this is the contact plane d

	
	NxVec3 impulse;			// accumulated impulse for two way interaction with rigid bodies
	NxU32 pad0;				// Check SIMD solver before moving this padding(it can get trashed).

	NxVec3 normal;			// if the vertex collides (see flags), this is the contact normal

//	NxU32  parentIndex;		// for supporting welding, partent indices are not defined per particle
							// anymore but per user vertex -> see mParentIndices
	NxU32  pad1;			// replaces parentIndex

			
	NxVec3 externalNormal;	// the normal we hand back to the user for rendering
	NxU32 pad2;

	NxVec3 originalPosition;// original position of the vertex, used in self collision
	NxU32 pad3;
};

struct ClothParticle_xbox_solver {
	NxVec3 newPosition;		// predicted position which the solver solves for	
	NxReal invMass;			// inverse mass, zero for kinematic particles

	
	const  Shape *shape;	// colliding or attached shape	
	NxU32 pad0;	
	NxU32 pad1;
	NxU32  flags;			// see NxClothVertexFlag in cloth.cpp

};

#endif // CLOTH_XBOX_H

/* NVIDIA/SCE CONFIDENTIAL
   NVIDIA PhysX(TM) SDK for PlayStation(R)3 Version PS3_PhysX_283_B2_forSDK330_hlsrc
*/
#ifndef CLOTH_SHARED_H
#define CLOTH_SHARED_H

#define HASH_SIZE 256
#define CELL_FACTOR 10	// must be >= 2


#define SECONDARY_WEIGHT_FACTOR 0.01f		// mass ratio for constraints involving primary and secondary vertices


enum NxClothVertexFlag
{
	// IMPORTANT! -> make sure you also change the same value in ClothMesh.h (for cooking) and ClothPPUTypes.h!!!
	CVF_ATTACHED				= 0x0001,	// vertex is attached to a shape or location
	CVF_KINEMATIC				= 0x0002,	// vertex is not moved by projections (infinite mass)
	CVF_COLLISION				= 0x0004,	// vertex is colliding
	CVF_CRACK_TIP				= 0x0008,	// vertex candidate to be split next
	CVF_SPLIT					= 0x0010,	// vertex is part of an overstretched edge
	CVF_SLEEP					= 0x0020,	// vertex is asleep
	CVF_BORDER					= 0x0040,	// vertex next to a sandwiched vertex (experimental)
	CVF_TEARABLE				= 0x0080,
	CVF_USER_DEFINED			= 0x0080,
	CVF_TEARABLE_ATTACHMENT     = 0x0100,
	CVF_SELF_COLLISION		    = 0x0200,	
	CVF_IGNORE					= 0x0400,
	CVF_DOMINANT	 		    = 0x0800,	// vertex has inv-mass zero during internal constraint projection
	CVF_NON_SPLITTABLE			= 0x1000,	// vertex cannot be split further, because of memory limitations
	CVF_SECONDARY				= 0x2000,	// when projecting a stretching constraint with mixed secondary and primiary vertex, 
											// the primary gets infinite mass (i.e. is not moved). Softbody only
};

// cloth internal constraint flags
enum ClothConstraintFlags
{
 	CLOTH_CONSTRAINT_IGNORE_CLOTH_STRETCHING	= 0x0001,	// specific to cloth
    CLOTH_CONSTRAINT_IGNORE_CLOTH_STRETCHING2	= 0x0002,	// specific to cloth
    CLOTH_CONSTRAINT_IGNORE_CLOTH_BENDING		= 0x0004,	// specific to cloth   
	CLOTH_CONSTRAINT_IGNORE_CLOTH_ALL			= 0x0007,	// specific to cloth
    CLOTH_CONSTRAINT_IGNORE_SOFTBODY_VOLUME		= 0x0001,	// specific to soft bodies
	CLOTH_CONSTRAINT_IGNORE_SOFTBODY_STRETCHING	= 0x0002,	// specific to soft bodies
	CLOTH_CONSTRAINT_IGNORE_SOFTBODY_ALL		= 0x0003,	// specific to soft bodies
};
/*----------------------------------------------------------------------------*/
// self collision data structures
struct HashCell {
	NxU32 numParticles;
	int firstParticle;
};

struct SelfCollisionParticle {

	void copyFrom(NxU32 nr, ClothParticle &p) 
	{ 
		particleNr = nr;
		velocity = p.velocity;	
		position = p.position;			
		originalPosition = p.originalPosition;
		invMass = p.invMass;
		flags = p.flags;
		if (p.flags & CVF_KINEMATIC) invMass = 0.0f;
	}

	void copyTo(ClothParticle &p)
	{
		p.velocity = velocity;
	}

	NxVec3 velocity;	// this is the only quantity that is updated
	NxU32  particleNr;
	//NxU32  pad0;
	NxVec3 position;	// needed for cloth - cloth friction to reconstruct velocities
	NxReal invMass;
	NxVec3 originalPosition;// needed to excule topologically close vertices from colliding
	NxU16  flags;
	//NxVec3 pad1;
	//NxU32  particleNr;
};

/*----------------------------------------------------------------------------*/

struct ClothInfo
{
	NxReal			attachmentTearFactor;			// 4
	NxReal			stretchingStiffness;			// 4
	NxReal			bendingStiffness;				// 4
	NxReal			dt;								// 4

	NxReal			mHalfThickness;					// 4
	NxU32			bendingMethod;					// 4
	NxU32			solverIterations;				// 4
	NxU32			flags;							// 4

	NxVec3			externalAcceleration;			//12
	NxU32			cellClothFlags;					// 4

	NxVec3			sceneGravity;					//12
	NxReal			dampingCoefficient;				// 4

	NxBounds3		mNewBounds;						//24
	NxReal			frictionCoefficient;			// 4
	NxU32			hierarchicalSolverIterations;	// 4

	NxVec3			windAcceleration;				//12
	NxReal			volumeStiffness;				// 4

#ifdef CELL_SPUCLOTH
	NxVec3			mMetalOffset;					//12
#endif
	NxReal			minAdhereVelocity;				// 4

	NxReal hardStretchLimitationFactor;
	NxReal mSelfCollisionThickness;

};

#define Cloth_bmCenterSpring 0
#define Cloth_bmDihedralAngle 1

#endif



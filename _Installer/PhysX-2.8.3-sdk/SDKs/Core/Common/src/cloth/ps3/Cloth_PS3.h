/* NVIDIA/SCE CONFIDENTIAL
   NVIDIA PhysX(TM) SDK for PlayStation(R)3 Version PS3_PhysX_283_B2_forSDK330_hlsrc
*/
#ifndef CLOTH_PS3_H
#define CLOTH_PS3_H

#include "CellUtil.h"

struct ClothParticle {

	NxVec3 position;		// actual position
	NxU32 pad0;				// Check SIMD solver before moving this padding(it can get trashed).

	NxVec3 velocity;		// velocity of the particle (newPosition - position) / timeStep
	NxReal planeD;			// if the vertex collides (see flags), this is the contact plane d

	NxVec3 newPosition;		// predicted position which the solver solves for
	NxU32 pad1;				// inverse mass, zero for kinematic particles
	
	NxVec3 attachment;		// if the vertex is attached (see flags), this is the goal position
	NxU32  flags;			// see NxClothVertexFlag in cloth.cpp

	
	NxVec3 impulse;			// accumulated impulse for two way interaction with rigid bodies
#ifndef CELL_SPUCLOTH
#error
	const  Shape *shape;	// colliding or attached shape
#else
	CELL_MM_PTR(const Shape) shape;
#endif

	NxVec3 normal;			// if the vertex collides (see flags), this is the contact normal

//	NxU32  parentIndex;		// for supporting welding, partent indices are not defined per particle
							// anymore but per user vertex -> see mParentIndices
	NxReal invMass;			// replaces parentIndex


	NxVec3 externalNormal;	// the normal we hand back to the user for rendering

#ifndef CELL_SPUCLOTH
#error
	NxU32 pad2;
#else
	NxReal bodyInvMass;
#endif

	NxVec3 originalPosition;// original position of the vertex, used in self collision
	NxU32 pad3;
};

#include "Cloth.shared.h"

enum CellClothFlags
{
	NX_CCF_ISSLEEPING		= 0x001,
	NX_CCF_ADHERED			= 0x002,
	NX_CCF_SKIPCOLLISION	= 0x004,

	NX_CCF_DEFORMABLE_CLOTH	= 0x010,
	NX_CCF_DEFORMABLE_SOFTB = 0x020,
	
	NX_CCF_ISMETALACTIVE	= 0x100,
	NX_CCF_METALUPDATE		= 0x200
};

struct CellClothAttachment
{
	NxU32	particleId;			// 4
	NxMat34 absPose;			// 48
	NxMat34 previousAbsPose;	// 48
	char	padTo70[12];		// 12
};

struct ClothInfo;
struct ClothHierarchyPointers;
struct ClothClothingPointers;
struct ClothGlobalInternalConstraint;

struct CellClothShape
{
	NxShapeType					type;					//  4					
	NxMat34						globalPose;				// 48	
	NxMat34						oldGlobalPose;			// 48	
	NxBounds3					bounds;					// 24	
	CELL_MM_PTR(const Shape)	shapeEA;				//  4	
	NxF32						dim0,dim1,dim2,dim3;	// 16	
	NxU32						shapeFlags;				//  4	
	NxU32						U32CollisionFlag;		//  4	
	NxU32						inpPublicFlags;			//  4  //!< Combination of ::NxActorFlag flags. Moved here, only used for root shape.
	NxReal						invBodyMass;			//  4	// new for PS3, used for sandwich shapes
};														//  = 160

struct CellClothPointers
{
	CELL_MM_PTR(ClothParticle) particlesEA; // CELL_ALIGN(128,particlesEA); // 
	CELL_MM_PTR(ClothInfo) infoEA; //CELL_ALIGN(128,infoEA);
	NxU32 numParticles; //CELL_ALIGN(128,numParticles);

	CELL_MM_PTR(ClothGlobalInternalConstraint) constraintsEA; // CELL_ALIGN(128,constraintsEA);
	NxU32 numConstraints; // CELL_ALIGN(128,numConstraints);

	CELL_MM_PTR(CellClothAttachment) bodyPosesEA; // CELL_ALIGN(128,bodyPosesEA);
	NxU32 numAttachments; // CELL_ALIGN(128,numAttachments);

	CELL_MM_PTR(CellClothShape) collisionShapesEA; // CELL_ALIGN(128,collisionShapesEA);
	NxU32 numCollisionShapes; // CELL_ALIGN(128,numCollisionShapes);

	CELL_MM_PTR(ClothHierarchyPointers) hierarchyEA;
	CELL_MM_PTR(ClothClothingPointers) clothingEA;

	CELL_MM_PTR(int) removedParticleStretchingLinksEA;
	NxU32 numRemovedParticleStretchingLinks;
};

enum
{
	SPU_CLOTH_UPDATE = 0,
	SPU_CLOTH_COLLISION = 1
};

#endif



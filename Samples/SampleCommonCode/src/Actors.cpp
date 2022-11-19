/*----------------------------------------------------------------------------*\
|
|						    NVIDIA PhysX Technology
|
|							    www.nvidia.com
|
\*----------------------------------------------------------------------------*/

//Actor creation methods for sample programs

#include "Actors.h"

#include <stdio.h>
#include "cooking.h"
#include "Stream.h"

extern NxPhysicsSDK*     gPhysicsSDK;
extern NxScene*          gScene;

// GEOMETRIC SHAPE ACTORS: PLANE, BOX, SPHERE, AND CAPSULE
NxActor* CreateGroundPlane(NxReal d)
{
    // Create a plane with default descriptor
    NxPlaneShapeDesc planeDesc;
	planeDesc.d = d;
    NxActorDesc actorDesc;
    actorDesc.shapes.pushBack(&planeDesc);
    return gScene->createActor(actorDesc);
}

NxActor* CreateBox(const NxVec3& pos, const NxVec3& boxDim, const NxReal density)
{
	// Add a single-shape actor to the scene
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	// The actor has one shape, a box
	NxBoxShapeDesc boxDesc;
	boxDesc.dimensions.set(boxDim.x,boxDim.y,boxDim.z);
	boxDesc.localPose.t = NxVec3(0,boxDim.y,0);
	actorDesc.shapes.pushBack(&boxDesc);

	if (density)
	{
		actorDesc.body = &bodyDesc;
		actorDesc.density = density;
	}
	else
	{
		actorDesc.body = NULL;
	}
	actorDesc.globalPose.t = pos;
	return gScene->createActor(actorDesc);	
}

NxActor* CreateBoxOriented(const NxVec3& pos, const NxQuat& orientation, const NxVec3& boxDim, const NxReal density)
{
	// Add a single-shape actor to the scene
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	// The actor has one shape, a box
	NxBoxShapeDesc boxDesc;
	boxDesc.dimensions.set(boxDim.x,boxDim.y,boxDim.z);
	boxDesc.localPose.t = NxVec3(0,boxDim.y,0);
	actorDesc.shapes.pushBack(&boxDesc);

	if (density)
	{
		actorDesc.body = &bodyDesc;
		actorDesc.density = density;
	}
	else
	{
		actorDesc.body = NULL;
	}
	actorDesc.globalPose.t = pos;
	actorDesc.globalPose.M = orientation;
	return gScene->createActor(actorDesc);	
}

NxActor* CreateSphere(const NxVec3& pos, const NxReal radius, const NxReal density)
{
	// Add a single-shape actor to the scene
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	// The actor has one shape, a sphere
	NxSphereShapeDesc sphereDesc;
	sphereDesc.radius = radius;
	sphereDesc.localPose.t = NxVec3(0,radius,0);
	actorDesc.shapes.pushBack(&sphereDesc);

	if (density)
	{
		actorDesc.body = &bodyDesc;
		actorDesc.density = density;
	}
	else
	{
		actorDesc.body = NULL;
	}
	actorDesc.globalPose.t = pos;
	return gScene->createActor(actorDesc);	
}

NxActor* CreateCapsule(const NxVec3& pos, const NxReal height, const NxReal radius, const NxReal density)
{
	// Add a single-shape actor to the scene
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	// The actor has one shape, a capsule
	NxCapsuleShapeDesc capsuleDesc;
	capsuleDesc.height = height;
	capsuleDesc.radius = radius;
	capsuleDesc.localPose.t = NxVec3(0,radius+(NxReal)0.5*height,0);
	actorDesc.shapes.pushBack(&capsuleDesc);

	if (density)
	{
		actorDesc.body = &bodyDesc;
		actorDesc.density = density;
	}
	else
	{
		actorDesc.body = NULL;
	}
	actorDesc.globalPose.t = pos;
	return gScene->createActor(actorDesc);	
}

// TRIANGLE MESH SHAPE ACTORS: HALF-PYRAMID, PYRAMID, DOWN WEDGE
NxActor* CreateHalfPyramid(const NxVec3& pos, const NxVec3& boxDim, const NxReal density)
{
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	// Half-Pyramid
	NxVec3 verts[8] = {	NxVec3(boxDim.x,-boxDim.y,-boxDim.z), NxVec3(-boxDim.x,-boxDim.y,-boxDim.z), NxVec3(-boxDim.x,-boxDim.y,boxDim.z), NxVec3(boxDim.x,-boxDim.y,boxDim.z),
						NxVec3(boxDim.x*(NxReal)0.5,boxDim.y,-boxDim.z*(NxReal)0.5), NxVec3(-boxDim.x*(NxReal)0.5,boxDim.y,-boxDim.z*(NxReal)0.5), NxVec3(-boxDim.x*(NxReal)0.5,boxDim.y,boxDim.z*(NxReal)0.5), NxVec3(boxDim.x*(NxReal)0.5,boxDim.y,boxDim.z*(NxReal)0.5) };

	// Create descriptor for triangle mesh
	NxConvexMeshDesc convexDesc;
	convexDesc.numVertices			= 8;
	convexDesc.pointStrideBytes		= sizeof(NxVec3);
	convexDesc.points				= verts;
	convexDesc.flags				= NX_CF_COMPUTE_CONVEX;

	// The actor has one shape, a triangle mesh
	NxConvexShapeDesc convexShapeDesc;
//	convexShapeDesc.meshData = gPhysicsSDK->createConvexMesh(convexDesc);
	InitCooking();
	UserStream upstream = UserStream("c:\\tmp.bin", false);
	UserStream downstream = UserStream("c:\\tmp.bin", true);
	bool status = CookConvexMesh(convexDesc, upstream);
	convexShapeDesc.meshData = gPhysicsSDK->createConvexMesh(downstream);

	convexShapeDesc.localPose.t = NxVec3(0,boxDim.y,0);
	actorDesc.shapes.pushBack(&convexShapeDesc);

	if (density)
	{
		actorDesc.body = &bodyDesc;
		actorDesc.density = density;
	}
	else
	{
		actorDesc.body = NULL;
	}
	actorDesc.globalPose.t = pos;
	return gScene->createActor(actorDesc);
}

NxActor* CreatePyramid(const NxVec3& pos, const NxVec3& boxDim, const NxReal density)
{
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	// Pyramid
	NxVec3 verts[5] = {	NxVec3(boxDim.x,-boxDim.y,-boxDim.z), NxVec3(-boxDim.x,-boxDim.y,-boxDim.z), NxVec3(-boxDim.x,-boxDim.y,boxDim.z), NxVec3(boxDim.x,-boxDim.y,boxDim.z),
						NxVec3(0,boxDim.y,0) };

	// Create descriptor for triangle mesh
	NxConvexMeshDesc convexDesc;
	convexDesc.numVertices			= 5;
	convexDesc.pointStrideBytes		= sizeof(NxVec3);
	convexDesc.points				= verts;
	convexDesc.flags				= NX_CF_COMPUTE_CONVEX;

	// The actor has one shape, a triangle mesh
	NxConvexShapeDesc convexShapeDesc;
//	convexShapeDesc.meshData = gPhysicsSDK->createConvexMesh(convexDesc);
	InitCooking();
	UserStream upstream = UserStream("c:\\tmp.bin", false);
	UserStream downstream = UserStream("c:\\tmp.bin", true);
	bool status = CookConvexMesh(convexDesc, upstream);
	convexShapeDesc.meshData = gPhysicsSDK->createConvexMesh(downstream);

	convexShapeDesc.localPose.t = NxVec3(0,boxDim.y,0);
	actorDesc.shapes.pushBack(&convexShapeDesc);

	if (density)
	{
		actorDesc.body = &bodyDesc;
		actorDesc.density = density;
	}
	else
	{
		actorDesc.body = NULL;
	}
	actorDesc.globalPose.t = pos;
	return gScene->createActor(actorDesc);
}

NxActor* CreateDownWedge(const NxVec3& pos, const NxVec3& boxDim, const NxReal density)
{
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	// Down wedge
	NxVec3 verts[8] = {	NxVec3(boxDim.x,boxDim.y,-boxDim.z), NxVec3(boxDim.x,boxDim.y,boxDim.z), NxVec3(boxDim.x,-boxDim.y,0),
						NxVec3(-boxDim.x,boxDim.y,-boxDim.z), NxVec3(-boxDim.x,boxDim.y,boxDim.z), NxVec3(-boxDim.x,-boxDim.y,0) };

	// Create descriptor for triangle mesh
	NxConvexMeshDesc convexDesc;
	convexDesc.numVertices			= 6;
	convexDesc.pointStrideBytes		= sizeof(NxVec3);
	convexDesc.points				= verts;
	convexDesc.flags				= NX_CF_COMPUTE_CONVEX;

	// The actor has one shape, a triangle mesh
	NxConvexShapeDesc convexShapeDesc;
//	convexShapeDesc.meshData = gPhysicsSDK->createConvexMesh(convexDesc);
	InitCooking();
	UserStream upstream = UserStream("c:\\tmp.bin", false);
	UserStream downstream = UserStream("c:\\tmp.bin", true);
	bool status = CookConvexMesh(convexDesc, upstream);
	convexShapeDesc.meshData = gPhysicsSDK->createConvexMesh(downstream);

	convexShapeDesc.localPose.t = NxVec3(0,boxDim.y,0);
	actorDesc.shapes.pushBack(&convexShapeDesc);

	if (density)
	{
		actorDesc.body = &bodyDesc;
		actorDesc.density = density;
	}
	else
	{
		actorDesc.body = NULL;
	}
	actorDesc.globalPose.t = pos;
	return gScene->createActor(actorDesc);
}

NxActor* CreateConvexObjectComputeHull(const NxVec3& pos, const NxVec3& boxDim, const NxReal density)
{
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	// Compute hull
	NxVec3 verts[8] = 
	{	
		NxVec3(-boxDim.x,-boxDim.y,-boxDim.z), 
		NxVec3(boxDim.x,-boxDim.y,-boxDim.z), 
		NxVec3(-boxDim.x,boxDim.y,-boxDim.z), 
		NxVec3(boxDim.x,boxDim.y,-boxDim.z),
		NxVec3(-boxDim.x,-boxDim.y,boxDim.z), 
		NxVec3(boxDim.x,-boxDim.y,boxDim.z), 
		NxVec3(-boxDim.x,boxDim.y,boxDim.z), 
		NxVec3(boxDim.x,boxDim.y,boxDim.z) 
	};

	// Create descriptor for triangle mesh
	NxConvexMeshDesc convexDesc;
	convexDesc.numVertices			= 8;
	convexDesc.pointStrideBytes		= sizeof(NxVec3);
	convexDesc.points				= verts;
	convexDesc.flags				= NX_CF_COMPUTE_CONVEX;

	// The actor has one shape, a triangle mesh
	NxConvexShapeDesc convexShapeDesc;
//	convexShapeDesc.meshData = gPhysicsSDK->createConvexMesh(convexDesc);
	InitCooking();
	UserStream upstream = UserStream("c:\\tmp.bin", false);
	UserStream downstream = UserStream("c:\\tmp.bin", true);
	bool status = CookConvexMesh(convexDesc, upstream);
	convexShapeDesc.meshData = gPhysicsSDK->createConvexMesh(downstream);

	convexShapeDesc.localPose.t = NxVec3(0,boxDim.y,0);
	actorDesc.shapes.pushBack(&convexShapeDesc);

	if (density)
	{
		actorDesc.body = &bodyDesc;
		actorDesc.density = density;
	}
	else
	{
		actorDesc.body = NULL;
	}
	actorDesc.globalPose.t = pos;
	return gScene->createActor(actorDesc);
}

NxActor* CreateConvexObjectSupplyHull(const NxVec3& pos, const NxVec3& boxDim, const NxReal density)
{
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	// Supply hull
	NxVec3 verts[8] = 
	{	
		NxVec3(-boxDim.x,-boxDim.y,-boxDim.z), 
		NxVec3(boxDim.x,-boxDim.y,-boxDim.z), 
		NxVec3(-boxDim.x,boxDim.y,-boxDim.z), 
		NxVec3(boxDim.x,boxDim.y,-boxDim.z),
		NxVec3(-boxDim.x,-boxDim.y,boxDim.z), 
		NxVec3(boxDim.x,-boxDim.y,boxDim.z), 
		NxVec3(-boxDim.x,boxDim.y,boxDim.z), 
		NxVec3(boxDim.x,boxDim.y,boxDim.z) 
	};

	NxU32 indices[12*3] =
	{          
		1,2,3,        
		0,2,1,    
		5,7,6,    
		4,5,6,    
		5,4,1,    
		1,4,0,    
		1,3,5,    
		3,7,5,    
		3,2,7,    
		2,6,7,    
		2,0,6,    
		4,6,0
	};

	// Create descriptor for triangle mesh
	NxConvexMeshDesc convexDesc;
	convexDesc.numVertices			= 8;
	convexDesc.pointStrideBytes		= sizeof(NxVec3);
	convexDesc.points				= verts;
	convexDesc.numTriangles			= 12;
	convexDesc.triangles			= indices;
	convexDesc.triangleStrideBytes	= 3 * sizeof(NxU32);

	// The actor has one shape, a triangle mesh
	NxConvexShapeDesc convexShapeDesc;
//	convexShapeDesc.meshData = gPhysicsSDK->createConvexMesh(convexDesc);
	InitCooking();
	UserStream upstream = UserStream("c:\\tmp.bin", false);
	UserStream downstream = UserStream("c:\\tmp.bin", true);
	bool status = CookConvexMesh(convexDesc, upstream);
	convexShapeDesc.meshData = gPhysicsSDK->createConvexMesh(downstream);

	convexShapeDesc.localPose.t = NxVec3(0,boxDim.y,0);
	actorDesc.shapes.pushBack(&convexShapeDesc);

	if (density)
	{
		actorDesc.body = &bodyDesc;
		actorDesc.density = density;
	}
	else
	{
		actorDesc.body = NULL;
	}
	actorDesc.globalPose.t = pos;
	return gScene->createActor(actorDesc);
}

NxActor* CreateConcaveObject(const NxVec3& pos, const NxVec3& boxDim, const NxReal density)
{
	NxBodyDesc bodyDesc;
	NxActorDesc actorDesc;

	// Supply hull
	NxVec3 verts[16] = 
	{	
		NxVec3(-boxDim.x,-boxDim.y,-boxDim.z), 
		NxVec3(boxDim.x,-boxDim.y,-boxDim.z), 
		NxVec3(-boxDim.x,boxDim.y,-boxDim.z), 
		NxVec3(boxDim.x,boxDim.y,-boxDim.z),

		NxVec3(-boxDim.x,-boxDim.y,boxDim.z), 
		NxVec3(boxDim.x,-boxDim.y,boxDim.z), 
		NxVec3(-boxDim.x,boxDim.y,boxDim.z), 
		NxVec3(boxDim.x,boxDim.y,boxDim.z),
		
		NxVec3(-boxDim.x,-boxDim.y,3*boxDim.z), 
		NxVec3(boxDim.x,-boxDim.y,3*boxDim.z), 
		NxVec3(-boxDim.x,boxDim.y,3*boxDim.z), 
		NxVec3(boxDim.x,boxDim.y,3*boxDim.z),
		
		NxVec3(3*boxDim.x,-boxDim.y,3*boxDim.z), 
		NxVec3(3*boxDim.x,-boxDim.y,boxDim.z), 
		NxVec3(3*boxDim.x,boxDim.y,3*boxDim.z), 
		NxVec3(3*boxDim.x,boxDim.y,boxDim.z) 
	};

	// Triangles is 12*3 - 8
	NxU32 indices[(12*3 - 8)*3] =
	{ 
		// BOX 1
		1,2,3,        
		0,2,1,  
		5,4,1,    
		1,4,0,    
		1,3,5,    
		3,7,5,    
		3,2,7,    
		2,6,7,    
		2,0,6,    
		4,6,0,
		// BOX 2  
		8,9,10,
		11,10,9,
		7,6,11,
		6,10,11,
		5,8,4,
		5,9,8,
		10,6,4,
		4,8,10,
		// BOX 3
		5,7,13,
		13,7,15,
		12,14,11,
		12,11,9,
		13,12,9,
		5,13,9,
		13,15,12,
		15,14,12,
		15,7,14,
		7,11,14,
	};

	// Create descriptor for triangle mesh
	NxTriangleMeshDesc concaveDesc;
	concaveDesc.numVertices			= 16;
	concaveDesc.pointStrideBytes	= sizeof(NxVec3);
	concaveDesc.points				= verts;
	concaveDesc.numTriangles		= 12*3 - 8;
	concaveDesc.triangles			= indices;
	concaveDesc.triangleStrideBytes	= 3 * sizeof(NxU32);
	concaveDesc.flags				= 0;

	// The actor has one shape, a triangle mesh
	NxTriangleMeshShapeDesc concaveShapeDesc;

	InitCooking();
	UserStream upstream = UserStream("c:\\tmp.bin", false);
	UserStream downstream = UserStream("c:\\tmp.bin", true);
	bool status = CookTriangleMesh(concaveDesc, upstream);
	concaveShapeDesc.meshData = gPhysicsSDK->createTriangleMesh(downstream);
	//
	// Please note about the created Triangle Mesh, user needs to release it when no one uses it to save memory. It can be detected
	// by API "meshData->getReferenceCount() == 0". And, the release API is "gPhysicsSDK->releaseTriangleMesh(*meshData);"
	//

	concaveShapeDesc.localPose.t = NxVec3(0,boxDim.y,0);
	actorDesc.shapes.pushBack(&concaveShapeDesc);

	if (density)
	{
		actorDesc.body = &bodyDesc;
		actorDesc.density = density;
	}
	else
	{
		actorDesc.body = NULL;
	}
	actorDesc.globalPose.t = pos;
	return gScene->createActor(actorDesc);
}

NxActor* CreateBunny(const NxVec3& pos, NxTriangleMesh* triangleMesh, const NxReal density)
{
	NxBodyDesc bodyDesc;
	NxActorDesc actorDesc;

	//create triangle mesh instance
	NxTriangleMeshShapeDesc bunnyShapeDesc;
	bunnyShapeDesc.meshData	= triangleMesh;
	bunnyShapeDesc.localPose.t = NxVec3(0,3,0);

	actorDesc.shapes.pushBack(&bunnyShapeDesc);
	actorDesc.body			= &bodyDesc;
	actorDesc.density		= density;
	actorDesc.globalPose.t  = pos;
	return gScene->createActor(actorDesc);
}

NxActor** CreateStack(const NxVec3& pos, const NxVec3& stackDim, const NxVec3& boxDim, NxReal density)
{
	NxActor** stack = new NxActor*[(int)(8*stackDim.x*stackDim.y*stackDim.z)]; 

	NxVec3 offset = NxVec3(boxDim.x,0,boxDim.z) + pos;

	for (int i = -(int)stackDim.x; i < (int)stackDim.x; i++)
		for (int j = 0; j < (int)stackDim.y; j++)
			for (int k = -(int)stackDim.z; k < (int)stackDim.z; k++)
			{
				NxVec3 boxPos = NxVec3(i*boxDim.x*2,j*boxDim.y*2,k*boxDim.z*2);
				*stack++ = CreateBox(boxPos + offset, boxDim, density);
			}

	return stack;
}


NxActor** CreateTower(const NxVec3& pos, const int heightBoxes, const NxVec3& boxDim, NxReal density)
{
	NxActor** tower = new NxActor*[heightBoxes]; 
	
	float spacing = 0.01f;
	
	NxVec3 boxPos = pos;
	//boxPos.y = pos.y + boxDim.y;
	
	for (int i = 0; i < heightBoxes; i++)
	{
	      tower[i] = CreateBox(boxPos, boxDim, density);
		  boxPos.y += 2.0f * boxDim.y + spacing;
	}
	
  return tower;	 
}

NxQuat AnglesToQuat(const NxVec3& angles)
{
	NxQuat quat;
	NxVec3 a;
	NxReal cr, cp, cy, sr, sp, sy, cpcy, spsy;

	a.x = (NxPi/(NxReal)360.0) * angles.x;    // Pitch
	a.y = (NxPi/(NxReal)360.0) * angles.y;    // Yaw
	a.z = (NxPi/(NxReal)360.0) * angles.z;    // Roll

	cy = NxMath::cos(a.z);
	cp = NxMath::cos(a.y);
	cr = NxMath::cos(a.x);

	sy = NxMath::sin(a.z);
	sp = NxMath::sin(a.y);
	sr = NxMath::sin(a.x);

	cpcy = cp * cy;
	spsy = sp * sy;
	quat.w = cr * cpcy + sr * spsy;		
	quat.x = sr * cpcy - cr * spsy;		
	quat.y = cr * sp * cy + sr * cp * sy;	
	quat.z = cr * cp * sy - sr * sp * cy;

	return quat;
}

NxActor* CreateBoxGear(const NxVec3& pos, const NxReal minRadius, const NxReal maxRadius, const NxReal height, const NxU32 numTeeth, const NxReal density)
{
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;
	bodyDesc.solverIterationCount = 30;  // Fast-moving body, higher solver iteration count

	NxReal perimeter = 2*NxPi*minRadius;
	NxReal toothWidth = (perimeter / (numTeeth*2))*(NxReal)0.75;
	NxReal toothHeight = height;
	NxReal toothDepth = maxRadius - minRadius;
	NxReal a,c,s;
	NxU32 i;

	NxVec3 *verts = new NxVec3[numTeeth*2];

	for (i = 0; i < numTeeth; i++) {
	    a = 360*(i/(NxReal)numTeeth);
	    c = NxMath::cos(a*(NxPi/180))*minRadius;
	    s = NxMath::sin(a*(NxPi/180))*minRadius;
		verts[2*i] = NxVec3(s, -height/2, c);
		verts[2*i+1] = NxVec3(s, height/2, c);
	}

	// Create descriptor for gear center triangle mesh
	NxConvexMeshDesc centerDesc;
	centerDesc.numVertices			= numTeeth*2;
	centerDesc.pointStrideBytes		= sizeof(NxVec3);
	centerDesc.points				= verts;
	centerDesc.flags				= NX_CF_COMPUTE_CONVEX;

	NxConvexShapeDesc centerShapeDesc;
//	centerShapeDesc.meshData = gPhysicsSDK->createConvexMesh(centerDesc);
	InitCooking();
	UserStream upstream = UserStream("c:\\tmp.bin", false);
	UserStream downstream = UserStream("c:\\tmp.bin", true);
	bool status = CookConvexMesh(centerDesc, upstream);
	centerShapeDesc.meshData = gPhysicsSDK->createConvexMesh(downstream);

	centerShapeDesc.localPose.t = NxVec3(0,0,0);
	actorDesc.shapes.pushBack(&centerShapeDesc);

	NxBoxShapeDesc *boxDesc = new NxBoxShapeDesc[numTeeth];

	for (i = 0; i < numTeeth; i++) {
		a = 360*(i/(NxReal)numTeeth);
	    c = NxMath::cos(a*(NxPi/180))*(minRadius+toothDepth/2);
	    s = NxMath::sin(a*(NxPi/180))*(minRadius+toothDepth/2);

		boxDesc[i].dimensions.set(NxVec3(toothWidth/2,toothHeight/2,toothDepth/2));

		NxQuat q = AnglesToQuat(NxVec3(0,a,0));
		boxDesc[i].localPose.M.fromQuat(q);
		boxDesc[i].localPose.t = NxVec3(s,0,c);

		actorDesc.shapes.pushBack(&boxDesc[i]);
	}

	if (density)
	{
		actorDesc.body = &bodyDesc;
		actorDesc.density = density;
	}
	else
	{
		actorDesc.body = NULL;
	}
	actorDesc.globalPose.t = pos;
	return gScene->createActor(actorDesc);
}

NxActor* CreateWheel(const NxVec3& pos, const NxReal minRadius, const NxReal maxRadius, const NxReal height, const NxU32 numTeeth, const NxReal density)
{
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	NxReal perimeter = 2*NxPi*minRadius;
	NxReal toothWidth = (perimeter / (numTeeth*2))*(NxReal)0.75;
	NxReal toothHeight = height;
	NxReal toothDepth = maxRadius - minRadius;
	NxReal a,c,s;
	NxU32 i;

	// WHEEL CENTER
	NxVec3 *verts = new NxVec3[numTeeth*2];

	for (i = 0; i < numTeeth; i++) {
	    a = 360*(i/(NxReal)numTeeth);
	    c = NxMath::cos(a*(NxPi/180))*minRadius;
	    s = NxMath::sin(a*(NxPi/180))*minRadius;
		verts[2*i] = NxVec3(s, -height/2, c);
		verts[2*i+1] = NxVec3(s, height/2, c);
	}

	// Create descriptor for gear center triangle mesh
	NxConvexMeshDesc centerDesc;
	centerDesc.numVertices			= numTeeth*2;
	centerDesc.pointStrideBytes		= sizeof(NxVec3);
	centerDesc.points				= verts;
	centerDesc.flags				= NX_CF_COMPUTE_CONVEX;

	NxConvexShapeDesc centerShapeDesc;
//	centerShapeDesc.meshData = gPhysicsSDK->createConvexMesh(centerDesc);
	InitCooking();
	UserStream upstream = UserStream("c:\\tmp.bin", false);
	UserStream downstream = UserStream("c:\\tmp.bin", true);
	bool status = CookConvexMesh(centerDesc, upstream);
	centerShapeDesc.meshData = gPhysicsSDK->createConvexMesh(downstream);

	centerShapeDesc.localPose.t = NxVec3(0,0,0);
	actorDesc.shapes.pushBack(&centerShapeDesc);

    // WHEEL LEFT EDGE
	for (i = 0; i < numTeeth; i++) {
	    a = 360*(i/(NxReal)numTeeth);
	    c = NxMath::cos(a*(NxPi/(NxReal)180))*maxRadius;
	    s = NxMath::sin(a*(NxPi/(NxReal)180))*maxRadius;
		verts[2*i] = NxVec3(s, -(NxReal)0.1, c);
		verts[2*i+1] = NxVec3(s, (NxReal)0.1, c);
	}

	// Create descriptor for gear left edge triangle mesh
	NxConvexMeshDesc leftEdgeDesc;
	leftEdgeDesc.numVertices		= numTeeth*2;
	leftEdgeDesc.pointStrideBytes	= sizeof(NxVec3);
	leftEdgeDesc.points				= verts;
	leftEdgeDesc.flags				= NX_CF_COMPUTE_CONVEX;

	NxConvexShapeDesc leftEdgeShapeDesc;
//	leftEdgeShapeDesc.meshData = gPhysicsSDK->createConvexMesh(leftEdgeDesc);
	InitCooking();
	UserStream upstream2 = UserStream("c:\\tmp.bin", false);
	UserStream downstream2 = UserStream("c:\\tmp.bin", true);
	status = CookConvexMesh(leftEdgeDesc, upstream2);
	leftEdgeShapeDesc.meshData = gPhysicsSDK->createConvexMesh(downstream2);

	leftEdgeShapeDesc.localPose.t = NxVec3(0,-height/2-(NxReal)0.1,0);
	actorDesc.shapes.pushBack(&leftEdgeShapeDesc);

    // WHEEL RIGHT EDGE
	for (i = 0; i < numTeeth; i++) {
	    a = 360*(i/(NxReal)numTeeth);
	    c = NxMath::cos(a*(NxPi/180))*maxRadius;
	    s = NxMath::sin(a*(NxPi/180))*maxRadius;
		verts[2*i] = NxVec3(s, -(NxReal)0.1, c);
		verts[2*i+1] = NxVec3(s, (NxReal)0.1, c);
	}

	// Create descriptor for gear left edge triangle mesh
	NxConvexMeshDesc rightEdgeDesc;
	rightEdgeDesc.numVertices		= numTeeth*2;
	rightEdgeDesc.pointStrideBytes	= sizeof(NxVec3);
	rightEdgeDesc.points			= verts;
	rightEdgeDesc.flags				= NX_CF_COMPUTE_CONVEX;

	NxConvexShapeDesc rightEdgeShapeDesc;
//	rightEdgeShapeDesc.meshData = gPhysicsSDK->createConvexMesh(rightEdgeDesc);
	InitCooking();
	UserStream upstream3 = UserStream("c:\\tmp.bin", false);
	UserStream downstream3 = UserStream("c:\\tmp.bin", true);
	status = CookConvexMesh(rightEdgeDesc, upstream3);
	rightEdgeShapeDesc.meshData = gPhysicsSDK->createConvexMesh(downstream3);

	rightEdgeShapeDesc.localPose.t = NxVec3(0,height/2+(NxReal)0.1,0);
	actorDesc.shapes.pushBack(&rightEdgeShapeDesc);

	NxBoxShapeDesc *boxDesc = new NxBoxShapeDesc[numTeeth];

	for (i = 0; i < numTeeth; i++) {
		a = 360*(i/(NxReal)numTeeth);
	    c = NxMath::cos(a*(NxPi/180))*(minRadius+toothDepth/2);
	    s = NxMath::sin(a*(NxPi/180))*(minRadius+toothDepth/2);

		boxDesc[i].dimensions.set(NxVec3(toothWidth/2,toothHeight/2,toothDepth/2));

		NxQuat q = AnglesToQuat(NxVec3(0,a,0));
		boxDesc[i].localPose.M.fromQuat(q);
		boxDesc[i].localPose.t = NxVec3(s,0,c);

		actorDesc.shapes.pushBack(&boxDesc[i]);
	}

	if (density)
	{
		actorDesc.body = &bodyDesc;
		actorDesc.density = density;
	}
	else
	{
		actorDesc.body = NULL;
	}
	actorDesc.globalPose.t = pos;
	return gScene->createActor(actorDesc);
}

NxActor* CreateFrame(const NxVec3& pos, const NxReal density)
{
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	NxBoxShapeDesc boxDesc1;
	boxDesc1.dimensions.set(8,(NxReal)0.5,(NxReal)0.05);
	boxDesc1.localPose.t = NxVec3(0,0,(NxReal)1.7);
	actorDesc.shapes.pushBack(&boxDesc1);

	NxBoxShapeDesc boxDesc2;
	boxDesc2.dimensions.set(8,(NxReal)0.5,(NxReal)0.05);
	boxDesc2.localPose.t = NxVec3(0,0,-(NxReal)1.7);
	actorDesc.shapes.pushBack(&boxDesc2);

	if (density)
	{
		actorDesc.body = &bodyDesc;
		actorDesc.density = density;
	}
	else
	{
		actorDesc.body = NULL;
	}
	actorDesc.globalPose.t = pos;
	return gScene->createActor(actorDesc);	
}

NxActor* CreateStep(const NxVec3& pos, const NxVec3& boxDim, const NxReal density)
{
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	NxBoxShapeDesc boxDesc1;
	boxDesc1.dimensions.set(boxDim.x/2,boxDim.y/2,boxDim.z/2);
	boxDesc1.localPose.t = NxVec3(0,0,0);
	actorDesc.shapes.pushBack(&boxDesc1);

	NxBoxShapeDesc boxDesc2;
	boxDesc2.dimensions.set(boxDim.x/6,boxDim.y/16,(NxReal)0.125);
	boxDesc2.localPose.t = NxVec3(0,0,-(NxReal)0.1);
	actorDesc.shapes.pushBack(&boxDesc2);

	NxBoxShapeDesc boxDesc3;
	boxDesc3.dimensions.set((NxReal)0.1,boxDim.y/2,(NxReal)0.5);
	boxDesc3.localPose.t = NxVec3(boxDim.x/2,0,(NxReal)(0.201-0.25));
	actorDesc.shapes.pushBack(&boxDesc3);

	NxBoxShapeDesc boxDesc4;
	boxDesc4.dimensions.set((NxReal)0.1,boxDim.y/2,(NxReal)0.5);
	boxDesc4.localPose.t = NxVec3(-boxDim.x/2,0,(NxReal)(0.201-0.25));
	actorDesc.shapes.pushBack(&boxDesc4);

	if (density)
	{
		actorDesc.body = &bodyDesc;
		actorDesc.density = density;
	}
	else
	{
		actorDesc.body = NULL;
	}
	actorDesc.globalPose.t = pos;
	return gScene->createActor(actorDesc);	
}

void SetActorGroup(NxActor *actor, NxCollisionGroup group)
{
	NxU32 nbShapes = actor->getNbShapes();
	NxShape * const*shapes = actor->getShapes();

	while (nbShapes--)
	{
		shapes[nbShapes]->setGroup(group);
	}
}

void SetActorMaterial(NxActor *actor, NxMaterialIndex index)
{
	NxU32 nbShapes = actor->getNbShapes();
	NxShape * const*shapes = actor->getShapes();

	while (nbShapes--)
	{
		shapes[nbShapes]->setMaterial(index);
	}
}

NxActor* CreateChassis(const NxVec3& pos, const NxVec3& boxDim, const NxReal density)
{
	// Add a single-shape actor to the scene
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	// Half-Pyramid
	NxVec3 verts[16] = {NxVec3(0,4*boxDim.y,-boxDim.z*(NxReal)0.5), NxVec3(-boxDim.x*(NxReal)0.75,4*boxDim.y,-boxDim.z*(NxReal)0.5), NxVec3(-boxDim.x*(NxReal)0.75,4*boxDim.y,boxDim.z*(NxReal)0.5), NxVec3(0,4*boxDim.y,boxDim.z*(NxReal)0.5),
		                NxVec3(boxDim.x,2*boxDim.y,-boxDim.z), NxVec3(-boxDim.x,2*boxDim.y,-boxDim.z), NxVec3(-boxDim.x,2*boxDim.y,boxDim.z), NxVec3(boxDim.x,2*boxDim.y,boxDim.z),
						NxVec3(boxDim.x,boxDim.y,-boxDim.z), NxVec3(-boxDim.x,boxDim.y,-boxDim.z), NxVec3(-boxDim.x,boxDim.y,boxDim.z), NxVec3(boxDim.x,boxDim.y,boxDim.z),
						NxVec3(boxDim.x*(NxReal)0.75,-boxDim.y,-boxDim.z*(NxReal)0.75), NxVec3(-boxDim.x*(NxReal)0.75,-boxDim.y,-boxDim.z*(NxReal)0.75), NxVec3(-boxDim.x*(NxReal)0.75,-boxDim.y,boxDim.z*(NxReal)0.75), NxVec3(boxDim.x*(NxReal)0.75,-boxDim.y,boxDim.z*(NxReal)0.75)
					   };
	// Create descriptor for triangle mesh
	NxConvexMeshDesc convexDesc;
	convexDesc.numVertices			= 16;
	convexDesc.pointStrideBytes		= sizeof(NxVec3);
	convexDesc.points				= verts;
	convexDesc.flags				= NX_CF_COMPUTE_CONVEX;

	// The actor has one shape, a triangle mesh
	NxConvexShapeDesc convexShapeDesc;
//	convexShapeDesc.meshData = gPhysicsSDK->createConvexMesh(convexDesc);
	InitCooking();
	UserStream upstream = UserStream("c:\\tmp.bin", false);
	UserStream downstream = UserStream("c:\\tmp.bin", true);
	bool status = CookConvexMesh(convexDesc, upstream);
	convexShapeDesc.meshData = gPhysicsSDK->createConvexMesh(downstream);

	convexShapeDesc.localPose.t = NxVec3(0,boxDim.y,0);
	actorDesc.shapes.pushBack(&convexShapeDesc);

	if (density)
	{
		actorDesc.body = &bodyDesc;
		actorDesc.density = density;
	}
	else
	{
		actorDesc.body = NULL;
	}
	actorDesc.globalPose.t = pos;
	return gScene->createActor(actorDesc);	
}


NxActor* CreateTurret(const NxVec3& pos, const NxVec3& boxDim, const NxReal density)
{
	// Add a single-shape actor to the scene
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	// Half-Pyramid
	NxVec3 verts[16] = {
						NxVec3(boxDim.x*(NxReal)0.25,6*boxDim.y,-boxDim.z*(NxReal)0.5), NxVec3(-boxDim.x*(NxReal)0.75,6*boxDim.y,-boxDim.z*(NxReal)0.5), NxVec3(-boxDim.x*(NxReal)0.75,6*boxDim.y,boxDim.z*(NxReal)0.5), NxVec3(boxDim.x*(NxReal)0.25,6*boxDim.y,boxDim.z*(NxReal)0.5),
						NxVec3(boxDim.x*(NxReal)0.5,(NxReal)5.5*boxDim.y,-boxDim.z*(NxReal)0.65), NxVec3(-boxDim.x*(NxReal)0.8,(NxReal)5.5*boxDim.y,-boxDim.z*(NxReal)0.65), NxVec3(-boxDim.x*(NxReal)0.8,(NxReal)5.5*boxDim.y,boxDim.z*(NxReal)0.65), NxVec3(boxDim.x*(NxReal)0.5,(NxReal)5.5*boxDim.y,boxDim.z*(NxReal)0.65),
						NxVec3(boxDim.x*(NxReal)0.5,(NxReal)4.5*boxDim.y,-boxDim.z*(NxReal)0.65), NxVec3(-boxDim.x*(NxReal)0.8,(NxReal)4.5*boxDim.y,-boxDim.z*(NxReal)0.65), NxVec3(-boxDim.x*(NxReal)0.8,(NxReal)4.5*boxDim.y,boxDim.z*(NxReal)0.65), NxVec3(boxDim.x*(NxReal)0.5,(NxReal)4.5*boxDim.y,boxDim.z*(NxReal)0.65),
						NxVec3(boxDim.x*(NxReal)0.25,4*boxDim.y,-boxDim.z*(NxReal)0.5), NxVec3(-boxDim.x*(NxReal)0.75,4*boxDim.y,-boxDim.z*(NxReal)0.5), NxVec3(-boxDim.x*(NxReal)0.75,4*boxDim.y,boxDim.z*(NxReal)0.5), NxVec3(boxDim.x*(NxReal)0.25,4*boxDim.y,boxDim.z*(NxReal)0.5),
					   };
	// Create descriptor for triangle mesh
	NxConvexMeshDesc convexDesc;
	convexDesc.numVertices			= 16;
	convexDesc.pointStrideBytes		= sizeof(NxVec3);
	convexDesc.points				= verts;
	convexDesc.flags				= NX_CF_COMPUTE_CONVEX;

	// The actor has one shape, a triangle mesh
	NxConvexShapeDesc convexShapeDesc;
//	convexShapeDesc.meshData = gPhysicsSDK->createConvexMesh(convexDesc);
	InitCooking();
	UserStream upstream = UserStream("c:\\tmp.bin", false);
	UserStream downstream = UserStream("c:\\tmp.bin", true);
	bool status = CookConvexMesh(convexDesc, upstream);
	convexShapeDesc.meshData = gPhysicsSDK->createConvexMesh(downstream);

	convexShapeDesc.localPose.t = NxVec3(0,boxDim.y,0);
	actorDesc.shapes.pushBack(&convexShapeDesc);

	if (density)
	{
		actorDesc.body = &bodyDesc;
		actorDesc.density = density;
	}
	else
	{
		actorDesc.body = NULL;
	}
	actorDesc.globalPose.t = pos;
	return gScene->createActor(actorDesc);	
}

NxActor* CreateCannon(const NxVec3& pos, const NxVec3& boxDim, const NxReal density)
{
	// Add a single-shape actor to the scene
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	// Half-Pyramid
	NxVec3 verts[8] = {
						NxVec3(boxDim.x*2,(NxReal)5.4*boxDim.y,-boxDim.z*(NxReal)0.1), NxVec3(boxDim.x*(NxReal)0.5,(NxReal)5.4*boxDim.y,-boxDim.z*(NxReal)0.1), NxVec3(boxDim.x*(NxReal)0.5,(NxReal)5.4*boxDim.y,boxDim.z*(NxReal)0.1), NxVec3(boxDim.x*2,(NxReal)5.4*boxDim.y,boxDim.z*(NxReal)0.1),
						NxVec3(boxDim.x*2,(NxReal)4.6*boxDim.y,-boxDim.z*(NxReal)0.1), NxVec3(boxDim.x*(NxReal)0.5,(NxReal)4.6*boxDim.y,-boxDim.z*(NxReal)0.1), NxVec3(boxDim.x*(NxReal)0.5,(NxReal)4.6*boxDim.y,boxDim.z*(NxReal)0.1), NxVec3(boxDim.x*2,(NxReal)4.6*boxDim.y,boxDim.z*(NxReal)0.1),
					   };
	// Create descriptor for triangle mesh
	NxConvexMeshDesc convexDesc;
	convexDesc.numVertices			= 8;
	convexDesc.pointStrideBytes		= sizeof(NxVec3);
	convexDesc.points				= verts;
	convexDesc.flags				= NX_CF_COMPUTE_CONVEX;

	// The actor has one shape, a triangle mesh
	NxConvexShapeDesc convexShapeDesc;
//	convexShapeDesc.meshData = gPhysicsSDK->createConvexMesh(convexDesc);
	InitCooking();
	UserStream upstream = UserStream("c:\\tmp.bin", false);
	UserStream downstream = UserStream("c:\\tmp.bin", true);
	bool status = CookConvexMesh(convexDesc, upstream);
	convexShapeDesc.meshData = gPhysicsSDK->createConvexMesh(downstream);

	convexShapeDesc.localPose.t = NxVec3(0,boxDim.y,0);
	actorDesc.shapes.pushBack(&convexShapeDesc);

	if (density)
	{
		actorDesc.body = &bodyDesc;
		actorDesc.density = density;
	}
	else
	{
		actorDesc.body = NULL;
	}
	actorDesc.globalPose.t = pos;
	return gScene->createActor(actorDesc);	
}


NxActor* CreateBlade(const NxVec3& pos, const NxVec3& boxDim, const NxReal mass)
{
	// Add a single-shape actor to the scene
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	// The actor has one shape, a box
	NxBoxShapeDesc boxDesc;
	boxDesc.dimensions.set(boxDim.x,boxDim.y,boxDim.z);
	boxDesc.localPose.t = NxVec3(0,boxDim.y,0);
	actorDesc.shapes.pushBack(&boxDesc);

	if (mass)
	{
		bodyDesc.mass = mass;
		actorDesc.body = &bodyDesc;
	}
	else
	{
		actorDesc.body = NULL;
	}
	actorDesc.globalPose.t = pos;
	return gScene->createActor(actorDesc);	
}

NxActor* CreateBall(const NxVec3& pos, const NxReal radius, const NxReal mass)
{
	// Add a single-shape actor to the scene
	NxActorDesc actorDesc;
	NxBodyDesc bodyDesc;

	// The actor has one shape, a sphere
	NxSphereShapeDesc sphereDesc;
	sphereDesc.radius = radius;
	sphereDesc.localPose.t = NxVec3(0,radius,0);
	actorDesc.shapes.pushBack(&sphereDesc);

	if (mass)
	{
		bodyDesc.mass = mass;
		actorDesc.body = &bodyDesc;
	}
	else
	{
		actorDesc.body = NULL;
	}
	actorDesc.globalPose.t = pos;
	return gScene->createActor(actorDesc);	
}



#ifndef NX_PHYSICS_NP_UTILLIB
#define NX_PHYSICS_NP_UTILLIB

#include "NxUtilLib.h"
#include "NpExportedUtils.h"
#include "NpInertiaTensor.h"
#include "NpJointDesc.h"
#include "NpIntersection.h"
#include "NpBuildSmoothNormals.h"

	bool NxSweepBoxCapsule(const NxBox& box, const NxCapsule& lss, const NxVec3& dir, float length, float& min_dist, NxVec3& normal);
	bool NxSweepBoxSphere(const NxBox& box, const NxSphere& sphere, const NxVec3& dir, float length, float& min_dist, NxVec3& normal);
	bool NxSweepCapsuleCapsule(const NxCapsule& lss0, const NxCapsule& lss1, const NxVec3& dir, float length, float& min_dist, NxVec3& ip, NxVec3& normal);
	bool NxSweepSphereCapsule(const NxSphere& sphere, const NxCapsule& lss, const NxVec3& dir, float length, float& d, NxVec3& ip, NxVec3& nrm);
	bool NxSweepBoxBox(const NxBox& box0, const NxBox& box1, const NxVec3& dir, float length, NxVec3& ip, NxVec3& normal, float& t);
	bool NxSweepBoxTriangles(NxU32 nb_tris, const NxTriangle* triangles, const NxTriangle* edge_triangles, const NxU32* edge_flags,
							const NxBounds3& box, const NxVec3& dir, float length,
							NxVec3& hit, NxVec3& normal, float& d, NxU32& index, NxU32* cachedIndex);
	bool NxSweepCapsuleTriangles(NxU32 up_direction,
									NxU32 nb_tris,
									const NxTriangle* triangles, const NxU32* edge_flags,
									const NxVec3& center, const float radius, const float height,
									const NxVec3& dir, float length,
									NxVec3& hit, NxVec3& normal, float& d, NxU32& index, NxU32* cachedIndex);
	float NxPointOBBSqrDist(const NxVec3& point, const NxVec3& center, const NxVec3& extents, const NxMat33& rot, NxVec3* params);
	float NxSegmentOBBSqrDist(const NxSegment& segment, const NxVec3& c0, const NxVec3& e0, const NxMat33& r0, float* t, NxVec3* p);

/**
\brief brief This is a wrapper around former SDK static exports.

TODO: This class probably wants splitting... A class with 100s of functions is just evil(but they were all global before)
*/
class NpUtilLib : public NxUtilLib
	{
	public:

		NpUtilLib(){}
		~NpUtilLib(){}


	/*
** From NxExportedUtils.h
*/
/***************************************************************************/

	virtual bool NxBoxContainsPoint(const NxBox& box, const NxVec3& p)
		{
		return ::NpBoxContainsPoint(box,p);
		}

	virtual void NxCreateBox(NxBox& box, const NxBounds3& aabb, const NxMat34& mat)
		{
		::NpCreateBox(box,aabb,mat);
		}
	virtual bool NxComputeBoxPlanes(const NxBox& box, NxPlane* planes)
		{
		return ::NpComputeBoxPlanes(box,planes);
		}
	virtual bool NxComputeBoxPoints(const NxBox& box, NxVec3* pts)
		{
		return ::NpComputeBoxPoints(box,pts);
		}
	virtual bool NxComputeBoxVertexNormals(const NxBox& box, NxVec3* pts)
		{
		return ::NpComputeBoxVertexNormals(box,pts);
		}
	virtual const NxU32* NxGetBoxEdges()
		{
		return ::NpGetBoxEdges();
		}
	virtual const NxI32* NxGetBoxEdgesAxes()
		{
		return ::NpGetBoxEdgesAxes();
		}
	virtual const NxU32* NxGetBoxTriangles()
		{
		return ::NpGetBoxTriangles();
		}
	virtual const NxVec3* NxGetBoxLocalEdgeNormals()
		{
		return ::NpGetBoxLocalEdgeNormals();
		}
	virtual void NxComputeBoxWorldEdgeNormal(const NxBox& box, NxU32 edge_index, NxVec3& world_normal)
		{
		::NpComputeBoxWorldEdgeNormal(box,edge_index,world_normal);
		}
	virtual void NxComputeCapsuleAroundBox(const NxBox& box, NxCapsule& capsule)
		{
		::NpComputeCapsuleAroundBox(box,capsule);
		}
	virtual bool NxIsBoxAInsideBoxB(const NxBox& a, const NxBox& b)
		{
		return ::NpIsBoxAInsideBoxB(a,b);
		}
	virtual const NxU32* NxGetBoxQuads()
		{
		return ::NpGetBoxQuads();
		}
	virtual const NxU32* NxBoxVertexToQuad(NxU32 vertexIndex)
		{
		return ::NpBoxVertexToQuad(vertexIndex);
		}
	virtual void NxComputeBoxAroundCapsule(const NxCapsule& capsule, NxBox& box)
		{
		::NpComputeBoxAroundCapsule(capsule,box);
		}
	virtual void NxSetFPUPrecision24()
		{
		::NpSetFPUPrecision24();
		}
	virtual void NxSetFPUPrecision53()
		{
		::NpSetFPUPrecision53();
		}
	virtual void NxSetFPUPrecision64()
		{
		::NpSetFPUPrecision64();
		}
	virtual void NxSetFPURoundingChop()
		{
		::NpSetFPURoundingChop();
		}
	virtual void NxSetFPURoundingUp()
		{
		::NpSetFPURoundingUp();
		}
	virtual void NxSetFPURoundingDown()
		{
		::NpSetFPURoundingDown();
		}
	virtual void NxSetFPURoundingNear()
		{
		::NpSetFPURoundingNear();
		}
	virtual void NxSetFPUExceptions(bool b)
		{
		::NpSetFPUExceptions(b);
		}
	virtual int NxIntChop(const NxF32& f)
		{
		return ::NpIntChop(f);
		}
	virtual int NxIntFloor(const NxF32& f)
		{
		return ::NpIntFloor(f);
		}
	virtual int NxIntCeil(const NxF32& f)
		{
		return ::NpIntCeil(f);
		}
	virtual NxF32 NxComputeDistanceSquared(const NxRay& ray, const NxVec3& point, NxF32* t)
		{
		return ::NpComputeDistanceSquared(ray,point,t);
		}
	virtual NxF32 NxComputeSquareDistance(const NxSegment& seg, const NxVec3& point, NxF32* t)
		{
		return ::NpComputeSquareDistance(seg,point,t);
		}
	virtual NxBSphereMethod NxComputeSphere(NxSphere& sphere, unsigned nb_verts, const NxVec3* verts)
		{
		return ::NpComputeSphere(sphere,nb_verts,verts);
		}
	virtual bool NxFastComputeSphere(NxSphere& sphere, unsigned nb_verts, const NxVec3* verts)
		{
		return ::NpFastComputeSphere(sphere,nb_verts,verts);
		}
	virtual void NxMergeSpheres(NxSphere& merged, const NxSphere& sphere0, const NxSphere& sphere1)
		{
		::NpMergeSpheres(merged,sphere0,sphere1);
		}
	virtual void NxNormalToTangents(const NxVec3 & n, NxVec3 & t1, NxVec3 & t2)
		{
		::NpNormalToTangents(n,t1,t2);
		}
	virtual bool NxDiagonalizeInertiaTensor(const NxMat33 & denseInertia, NxVec3 & diagonalInertia, NxMat33 & rotation)
		{
		return ::NpDiagonalizeInertiaTensor(denseInertia,diagonalInertia,rotation);
		}
	virtual void NxFindRotationMatrix(const NxVec3 & x, const NxVec3 & b, NxMat33 & M)
		{
		::NpFindRotationMatrix(x,b,M);
		}
	virtual void NxComputeBounds(NxVec3& min, NxVec3& max, NxU32 nbVerts, const NxVec3* verts)
		{
		::NpComputeBounds(min,max,nbVerts,verts);
		}
	virtual NxU32 NxCrc32(const void* buffer, NxU32 nbBytes)
		{
		return ::NpCrc32(buffer,nbBytes);
		}
/*
** From NxInertiaTensor.h
*/
/***************************************************************************/

	virtual NxReal NxComputeSphereMass			(NxReal radius, NxReal density)
		{
		return ::NpComputeSphereMass(radius,density);
		}
	virtual NxReal NxComputeSphereDensity		(NxReal radius, NxReal mass)
		{
		return ::NpComputeSphereDensity(radius,mass);
		}

	virtual NxReal NxComputeBoxMass			(const NxVec3& extents, NxReal density)
		{
		return ::NpComputeBoxMass(extents,density);
		}
	virtual NxReal NxComputeBoxDensity			(const NxVec3& extents, NxReal mass)
		{
		return ::NpComputeBoxDensity(extents,mass);
		}


	virtual NxReal NxComputeEllipsoidMass		(const NxVec3& extents, NxReal density)
		{
		return ::NpComputeEllipsoidMass(extents, density);
		}
	virtual NxReal NxComputeEllipsoidDensity	(const NxVec3& extents, NxReal mass)
		{
		return ::NpComputeEllipsoidDensity(extents,mass);
		}

	virtual NxReal NxComputeCylinderMass		(NxReal radius, NxReal length, NxReal density)
		{
		return ::NpComputeCylinderMass(radius,length,density);
		}
	virtual NxReal NxComputeCylinderDensity	(NxReal radius, NxReal length, NxReal mass)
		{
		return ::NpComputeCylinderDensity(radius,length,mass);
		}

	virtual NxReal NxComputeConeMass			(NxReal radius, NxReal length, NxReal density)
		{
		return ::NpComputeConeMass(radius,length,density);
		}
	virtual NxReal NxComputeConeDensity		(NxReal radius, NxReal length, NxReal mass)
		{
		return ::NpComputeConeDensity(radius,length,mass);
		}

	virtual void NxComputeBoxInertiaTensor	(NxVec3& diagInertia, NxReal mass, NxReal xlength, NxReal ylength, NxReal zlength)
		{
		::NpComputeBoxInertiaTensor(diagInertia,mass,xlength,ylength,zlength);
		}
	virtual void NxComputeSphereInertiaTensor(NxVec3& diagInertia, NxReal mass, NxReal radius, bool hollow)
		{
		::NpComputeSphereInertiaTensor(diagInertia,mass,radius,hollow);
		}

/*
** From NxJointDesc.h
*/
/***************************************************/

	virtual void NxJointDesc_SetGlobalAnchor(NxJointDesc & dis, const NxVec3 & wsAnchor)
		{
		::NpJointDesc_SetGlobalAnchor(dis,wsAnchor);
		}

	virtual void NxJointDesc_SetGlobalAxis(NxJointDesc & dis, const NxVec3 & wsAxis)
		{
		::NpJointDesc_SetGlobalAxis(dis,wsAxis);
		}

/*
** From NxIntersectionBoxBox.h
*/
/******************************************************************/

	virtual bool NxBoxBoxIntersect(	const NxVec3& extents0, const NxVec3& center0, const NxMat33& rotation0,
														const NxVec3& extents1, const NxVec3& center1, const NxMat33& rotation1,
														bool fullTest)
		{
		return ::NpBoxBoxIntersect(extents0,center0,rotation0,extents1,center1,rotation1,fullTest);
		}


	virtual bool NxTriBoxIntersect(const NxVec3 & vertex0, const NxVec3 & vertex1, const NxVec3 & vertex2, const NxVec3 & center, const NxVec3& extents)
		{
		return ::NpTriBoxIntersect(vertex0,vertex1,vertex2,center,extents);
		}

	virtual NxSepAxis NxSeparatingAxis(	const NxVec3& extents0, const NxVec3& center0, const NxMat33& rotation0,
															const NxVec3& extents1, const NxVec3& center1, const NxMat33& rotation1,
															bool fullTest=true)
		{
		return ::NpSeparatingAxis(extents0,center0,rotation0,extents1,center1,rotation1,fullTest);
		}

	/*
	** From NxIntersectionRayPlane.h
	*/
/*************************************************************************/

	virtual	void NxSegmentPlaneIntersect(const NxVec3& v1, const NxVec3& v2, 
		const NxPlane& plane, NxReal& dist, NxVec3& pointOnPlane)
		{

		return ::NpSegmentPlaneIntersect(v1,v2,plane,dist,pointOnPlane);
		}

	virtual	bool NxRayPlaneIntersect(const NxRay& ray, const NxPlane& plane, 
		NxReal& dist, NxVec3& pointOnPlane)
		{
		return ::NpRayPlaneIntersect(ray,plane,dist,pointOnPlane);
		}

/*
** From NxIntersectionRaySphere.h
*/
/**************************************************************************/

	virtual bool NxRaySphereIntersect(const NxVec3& origin, const NxVec3& dir, NxReal length, const NxVec3& center, NxReal radius, NxReal& hit_time, NxVec3& hit_pos)
		{
		return ::NpRaySphereIntersect(origin, dir, length, center, radius, hit_time, hit_pos);
		}

	/*
** From NxIntersectionSegmentBox
*/
/**************************************************************************/

	virtual bool NxSegmentBoxIntersect(const NxVec3& p1, const NxVec3& p2,
		const NxVec3& bbox_min,const NxVec3& bbox_max, NxVec3& intercept)
		{
		return ::NpSegmentBoxIntersect(p1,p2,bbox_min,bbox_max,intercept);
		}

	virtual bool NxRayAABBIntersect(const NxVec3& min, const NxVec3& max, 
		const NxVec3& origin, const NxVec3& dir, NxVec3& coord)
		{
		return ::NpRayAABBIntersect(min,max,origin,dir,coord);
		}

	virtual NxU32 NxRayAABBIntersect2(const NxVec3& min, const NxVec3& max, 
		const NxVec3& origin, const NxVec3& dir, NxVec3& coord, NxReal & t)
		{
		return ::NpRayAABBIntersect2(min,max,origin,dir,coord,t);
		}

	virtual bool NxSegmentOBBIntersect(const NxVec3& p0, const NxVec3& p1, 
		const NxVec3& center, const NxVec3& extents, const NxMat33& rot)
		{
		return ::NpSegmentOBBIntersect(p0,p1,center,extents,rot);
		}

	virtual bool NxSegmentAABBIntersect(const NxVec3& p0, const NxVec3& p1, 
		const NxVec3& min, const NxVec3& max)
		{
		return ::NpSegmentAABBIntersect(p1,p1,min,max);
		}

	virtual bool NxRayOBBIntersect(const NxRay& ray, const NxVec3& center, 
		const NxVec3& extents, const NxMat33& rot)
		{
		return ::NpRayOBBIntersect(ray,center,extents,rot);
		}

/*
** From NxIntersectionSegmentCapsule.h
*/
/*************************************************************************/

	virtual  NxU32 NxRayCapsuleIntersect(const NxVec3& origin, const NxVec3& dir, 
		const NxCapsule& capsule, NxReal t[2])
		{
		return ::NpRayCapsuleIntersect(origin,dir,capsule,t);
		}
/*
** From NxRayTryIntersect.h
*/
/********************************************************************************/

	virtual bool NxRayTriIntersect(const NxVec3& orig, const NxVec3& dir, const NxVec3& vert0, const NxVec3& vert1, const NxVec3& vert2, float& t, float& u, float& v, bool cull)
		{
		return ::NpRayTriIntersect(orig,dir,vert0,vert1,vert2,t,u,v,cull);
		}
/*
** From NxIntersectionSweptSpheres.h
*/
/***************************************************************************/

	virtual bool NxSweptSpheresIntersect(	const NxSphere& sphere0, const NxVec3& velocity0,
																const NxSphere& sphere1, const NxVec3& velocity1)
		{
		return ::NpSweptSpheresIntersect(sphere0, velocity0,sphere1,velocity1);
		}
/*
** From NxBuildSmoothNormals.h
*/
		virtual bool NxBuildSmoothNormals(
		NxU32 nbTris,
		NxU32 nbVerts,
		const NxVec3* verts,
		const NxU32* dFaces,
		const NxU16* wFaces,
		NxVec3* normals,
		bool flip=false
		)
			{
			return ::NpBuildSmoothNormals(nbTris,nbVerts,verts,dFaces,wFaces,normals,flip);
			}

	virtual bool NxSweepBoxCapsule(const NxBox& box, const NxCapsule& lss, const NxVec3& dir, float length, float& min_dist, NxVec3& normal)
	{
		return ::NxSweepBoxCapsule(box, lss, dir, length, min_dist, normal);
	}
	virtual bool NxSweepBoxSphere(const NxBox& box, const NxSphere& sphere, const NxVec3& dir, float length, float& min_dist, NxVec3& normal)
	{
		return ::NxSweepBoxSphere(box, sphere, dir, length, min_dist, normal);
	}
	virtual bool NxSweepCapsuleCapsule(const NxCapsule& lss0, const NxCapsule& lss1, const NxVec3& dir, float length, float& min_dist, NxVec3& ip, NxVec3& normal)
	{
		return ::NxSweepCapsuleCapsule(lss0, lss1, dir, length, min_dist, ip, normal);
	}
	virtual bool NxSweepSphereCapsule(const NxSphere& sphere, const NxCapsule& lss, const NxVec3& dir, float length, float& d, NxVec3& ip, NxVec3& nrm)
	{
		return ::NxSweepSphereCapsule(sphere, lss, dir, length, d, ip, nrm);
	}
	virtual bool NxSweepBoxBox(const NxBox& box0, const NxBox& box1, const NxVec3& dir, float length, NxVec3& ip, NxVec3& normal, float& t)
	{
		return ::NxSweepBoxBox(box0, box1, dir, length, ip, normal, t);
	}
	virtual bool NxSweepBoxTriangles(NxU32 nb_tris, const NxTriangle* triangles, const NxTriangle* edge_triangles, const NxU32* edge_flags,
									const NxBounds3& box,
									const NxVec3& dir, float length,
									NxVec3& hit, NxVec3& normal, float& d, NxU32& index, NxU32* cachedIndex=NULL)
	{
		return 	::NxSweepBoxTriangles(nb_tris, triangles, edge_triangles, edge_flags, box, dir, length, hit, normal, d, index, cachedIndex);
	}
	virtual bool NxSweepCapsuleTriangles(NxU32 up_direction,
									NxU32 nb_tris,
									const NxTriangle* triangles, const NxU32* edge_flags,
									const NxVec3& center, const float radius, const float height,
									const NxVec3& dir, float length,
									NxVec3& hit, NxVec3& normal, float& d, NxU32& index, NxU32* cachedIndex=NULL)
	{
		return ::NxSweepCapsuleTriangles(up_direction, nb_tris, triangles, edge_flags,
									center, radius, height,
									dir, length,
									hit, normal, d, index, cachedIndex);
	}
	virtual float NxPointOBBSqrDist(const NxVec3& point, const NxVec3& center, const NxVec3& extents, const NxMat33& rot, NxVec3* params)
	{
		return ::NxPointOBBSqrDist(point, center, extents, rot, params);
	}
	virtual float NxSegmentOBBSqrDist(const NxSegment& segment, const NxVec3& c0, const NxVec3& e0, const NxMat33& r0, float* t, NxVec3* p)
	{
		return ::NxSegmentOBBSqrDist(segment, c0, e0, r0, t, p);
	}
	};
#endif

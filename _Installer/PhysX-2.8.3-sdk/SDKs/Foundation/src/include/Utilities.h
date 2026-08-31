#ifndef NX_FOUNDATION_UTILITIES
#define NX_FOUNDATION_UTILITIES
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/
#include "Nxf.h"
#include "NxSphere.h"
class NxPlane;
class NxVec3;
class NxRay;
class NxCapsule;
class NxBox;
class NxSegment;
class NxMat33;
class NxMat34;
struct NxIntegrals;
class NxSimpleTriangleMesh;
class NxBounds3;

//prototypes of stuff that should be moved to foundation, but are still in physics/src!
void segmentPlaneIntersect(const NxVec3& v1, const NxVec3& v2, const NxPlane& plane, NxReal& d, NxVec3& pointOnPlane);
bool rayPlaneIntersect(const NxRay& line, const NxPlane& plane, NxReal& distance_along_line, NxVec3& point_on_plane);
NxU32 rayCapsuleIntersect(const NxVec3& origin, const NxVec3& dir, const NxCapsule& capsule, NxReal s[2]);
bool boxBoxIntersect(const NxVec3& e0, const NxVec3& c0, const NxMat33& r0, const NxVec3& e1, const NxVec3& c1, const NxMat33& r1, bool full_test);
bool raySphereIntersect(const NxVec3& origin, const NxVec3& dir, NxReal length, const NxVec3& center, NxReal radius, NxReal& hit_time, NxVec3& hit_pos);
bool buildSmoothNormals(NxU32 nbTris, NxU32 nbVerts,	const NxVec3* verts,	const NxU32* dFaces, const NxU16* wFaces,	NxVec3* normals,	bool flip);
bool triBoxIntersect(const NxVec3 & vertex0, const NxVec3 & vertex1, const NxVec3 & vertex2, const NxVec3 & center, const NxVec3& extents);

bool segmentBoxIntersect(const NxVec3 &p1, const NxVec3 &p2,const NxVec3 &bbox_min,const NxVec3 &bbox_max, NxVec3 & intercept);
bool segmentOBBIntersect(const NxVec3& p0, const NxVec3& p1, const NxVec3& center, const NxVec3& extents, const NxMat33& rot);
bool rayOBBIntersect(const NxRay& ray, const NxVec3& center, const NxVec3& extents, const NxMat33& rot);
bool segmentAABBIntersect(const NxVec3& p0, const NxVec3& p1, const NxVec3& min, const NxVec3& max);
bool rayAABBIntersect(const NxVec3& min, const NxVec3& max, const NxVec3& origin, const NxVec3& _dir, NxVec3& coord);
NxU32 rayAABBIntersect2(const NxVec3& min, const NxVec3& max, const NxVec3& origin, const NxVec3& _dir, NxVec3& coord, NxReal & t);
void transformInertiaTensor(const NxVec3 &invD, const NxMat33 & M, NxMat33 &mIInv);
bool RayPolygon(NxU32 nb_verts, const NxVec3* verts, const NxU8* indices, const NxRay& ray, float& t);

namespace NxFoundation
	{

//prototypes of utils for functs in foundation
bool computeVolumeIntegrals(const NxSimpleTriangleMesh& mesh, NxReal density, NxIntegrals& integrals);
void createBox(NxBox& box, const NxBounds3& aabb, const NxMat34& mat);
const NxI32* getBoxEdgesAxes();
const NxU32* getBoxTriangles();
const NxVec3* getBoxLocalEdgeNormals();
int intChop(const NxF32& f);
int intFloor(const NxF32& f);
int intCeil(const NxF32& f);
void computeBoxWorldEdgeNormal(const NxBox& box, NxU32 edge_index, NxVec3& world_normal);
void computeCapsuleAroundBox(const NxBox& box, NxCapsule& capsule);
bool isBoxAInsideBoxB(const NxBox& a, const NxBox& b);
const NxU32* getBoxQuads();
const NxU32* boxVertexToQuad(NxU32 vertexIndex);
void computeBoxAroundCapsule(const NxCapsule& capsule, NxBox& box);
NxU32 crc32(const void* buffer, NxU32 nbBytes);
bool computeBoxPlanes(const NxBox& box, NxPlane* planes);
NxBSphereMethod computeSphere(NxSphere& sphere, unsigned nb_verts, const NxVec3* verts);
bool fastComputeSphere(NxSphere& sphere, unsigned nb_verts, const NxVec3* verts);
void computeBounds(NxVec3& min, NxVec3& max, NxU32 nbVerts, const NxVec3* verts);
NxF32 computeDistanceSquared(const NxRay& ray, const NxVec3& point, NxF32* t);
bool diagonalizeInertiaTensor(const NxMat33 & denseInertia, NxVec3 & diagonalInertia, NxMat33 & rotation);
bool computeBoxVertexNormals(const NxBox& box, NxVec3* pts);
bool boxContainsPoint(const NxBox& box, const NxVec3& p);
void mergeSpheres(NxSphere& merged, const NxSphere& sphere0, const NxSphere& sphere1);
bool computeBoxPoints(const NxBox& box, NxVec3* pts);
void findRotationMatrix(const NxVec3 & from, const NxVec3 & to, NxMat33 & mtx);
NxF32 computeSquareDistance(const NxSegment& seg, const NxVec3& point, NxF32* t);
void normalToTangents(const NxVec3 & n, NxVec3 & t1, NxVec3 & t2);
bool computeBoxPoints(const NxBox& box, NxVec3* pts);
const NxU32* getBoxEdges();
void setFPUPrecision24();
void setFPUPrecision53();
void setFPUPrecision64();

void setFPURoundingChop();
void setFPURoundingUp();
void setFPURoundingDown();
void setFPURoundingNear();
void setFPUExceptions(bool exOn);


//!	Fills a buffer with a given dword.
//!	\param		addr	[in] buffer address
//!	\param		nb		[in] number of dwords to write
//!	\param		value	[in] the dword value
//!	\warning	writes nb*4 bytes !
NX_INLINE void storeDwords(NxU32* dest, NxU32 nb, NxU32 value)
	{
	// The asm code below **SHOULD** be equivalent to one of those C versions
	// or the other if your compiler is good: (checked on VC++ 6.0)
	//
	//	1) while(nb--)	*dest++ = value;
	//
	//	2) for(udword i=0;i<nb;i++)	dest[i] = value;
	//
#if defined(WIN32) && !defined(_WIN64) && !defined(_M_ARM)
	_asm push eax
		_asm push ecx
		_asm push edi
		_asm mov edi, dest
		_asm mov ecx, nb
		_asm mov eax, value
		_asm rep stosd
		_asm pop edi
		_asm pop ecx
		_asm pop eax
#else
	while(nb--)	*dest++ = value;
#endif
	}
	}
#endif

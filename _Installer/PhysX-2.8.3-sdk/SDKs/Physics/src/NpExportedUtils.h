#ifndef __NP_EXPORTED_UTILS__
#define __NP_EXPORTED_UTILS__

/*----------------------------------------------------------------------------*\
|
|						Public Interface to NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/
/** \addtogroup physics
  @{
*/

#include "Nxp.h"
#include "NxFoundation.h"

struct NxIntegrals;

bool NpBoxContainsPoint(const NxBox& box, const NxVec3& p);
void NpCreateBox(NxBox& box, const NxBounds3& aabb, const NxMat34& mat);
bool NpComputeBoxPlanes(const NxBox& box, NxPlane* planes);
bool NpComputeBoxPoints(const NxBox& box, NxVec3* pts);
bool NpComputeBoxVertexNormals(const NxBox& box, NxVec3* pts);
const NxU32* NpGetBoxEdges();
const NxI32* NpGetBoxEdgesAxes();
const NxU32* NpGetBoxTriangles();
const NxVec3* NpGetBoxLocalEdgeNormals();
void NpComputeBoxWorldEdgeNormal(const NxBox& box, NxU32 edge_index, NxVec3& world_normal);
void NpComputeCapsuleAroundBox(const NxBox& box, NxCapsule& capsule);
bool NpIsBoxAInsideBoxB(const NxBox& a, const NxBox& b);
const NxU32* NpGetBoxQuads();
const NxU32* NpBoxVertexToQuad(NxU32 vertexIndex);
void NpComputeBoxAroundCapsule(const NxCapsule& capsule, NxBox& box);
void NpSetFPUPrecision24();
void NpSetFPUPrecision53();
void NpSetFPUPrecision64();
void NpSetFPURoundingChop();
void NpSetFPURoundingUp();
void NpSetFPURoundingDown();
void NpSetFPURoundingNear();
void NpSetFPUExceptions(bool b);
int NpIntChop(const NxF32& f);
int NpIntFloor(const NxF32& f);
int NpIntCeil(const NxF32& f);
NxF32 NpComputeDistanceSquared(const NxRay& ray, const NxVec3& point, NxF32* t);
NxF32 NpComputeSquareDistance(const NxSegment& seg, const NxVec3& point, NxF32* t);
NxBSphereMethod NpComputeSphere(NxSphere& sphere, unsigned nb_verts, const NxVec3* verts);
bool NpFastComputeSphere(NxSphere& sphere, unsigned nb_verts, const NxVec3* verts);
void NpMergeSpheres(NxSphere& merged, const NxSphere& sphere0, const NxSphere& sphere1);
void NpNormalToTangents(const NxVec3 & n, NxVec3 & t1, NxVec3 & t2);
bool NpDiagonalizeInertiaTensor(const NxMat33 & denseInertia, NxVec3 & diagonalInertia, NxMat33 & rotation);
void NpFindRotationMatrix(const NxVec3 & x, const NxVec3 & b, NxMat33 & M);
void NpComputeBounds(NxVec3& min, NxVec3& max, NxU32 nbVerts, const NxVec3* verts);
NxU32 NpCrc32(const void* buffer, NxU32 nbBytes);

#endif

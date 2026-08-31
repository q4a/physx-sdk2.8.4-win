// Generic conversions from NovodeX to ICE (mainly for Opcode)

#include "Opcode.h"
#include "NxRay.h"
#include "NxSphere.h"
#include "NxBox.h"
#include "NxPlane.h"
#include "NxCapsule.h"
#include "NxBounds3.h"
#include "NxTriangle.h"


NX_INLINE void NXToICE(IceMaths::Matrix3x3& ice33, const NxMat33& nx33)
	{
	nx33.getColumnMajor(ice33.m);
	}

NX_INLINE void NXToICE(IceMaths::OBB& iceBox, const NxVec3& dim, const NxVec3* pos, const NxMat33* rot)
	{
	// Extents
	dim.get(&iceBox.mExtents.x);

	// Center
	if(pos)	pos->get(&iceBox.mCenter.x);
	else	iceBox.mCenter.Zero();

	// Rotation
	if(rot)	rot->getColumnMajor(iceBox.mRot.m);
	else	iceBox.mRot.Identity();
	}

NX_INLINE void NXToICE(IceMaths::OBB& iceBox, const NxBox& nxBox)
	{
	NXToICE(iceBox, nxBox.extents, &nxBox.center, &nxBox.rot);
	}

NX_INLINE void NXToICE(IceMaths::Matrix4x4& iceMat44, const NxMat34& nxMat34)
	{
	nxMat34.M.getColumnMajorStride4(iceMat44.m);
	iceMat44.SetTrans(nxMat34.t.x, nxMat34.t.y, nxMat34.t.z);
	iceMat44.m[0][3] = iceMat44.m[1][3] = iceMat44.m[2][3] = 0.0f;
	iceMat44.m[3][3] = 1.0f;
	}

NX_COMPILE_TIME_ASSERT(sizeof(IceMaths::Point)==sizeof(NxVec3));
NX_INLINE const IceMaths::Point& NXToICE(const NxVec3& nxVec3)			{ return (const IceMaths::Point&)nxVec3;		}
NX_INLINE IceMaths::Point& NXToICE(NxVec3& nxVec3)						{ return (IceMaths::Point&)nxVec3;				}
NX_INLINE const IceMaths::Point* NXToICE(const NxVec3* nxVec3)			{ return (const IceMaths::Point*)nxVec3;		}
NX_INLINE IceMaths::Point* NXToICE(NxVec3* nxVec3)						{ return (IceMaths::Point*)nxVec3;				}

NX_COMPILE_TIME_ASSERT(sizeof(IceMaths::Ray)==sizeof(NxRay));
#if !defined(__flash__) && !defined(__webgl__)
NX_COMPILE_TIME_ASSERT(OFFSET_OF(NxRay, orig)==0);
NX_COMPILE_TIME_ASSERT(OFFSET_OF(NxRay, dir)==12);
#endif
NX_INLINE const IceMaths::Ray& NXToICE(const NxRay& nxRay)				{ return (const IceMaths::Ray&)nxRay;			}

NX_COMPILE_TIME_ASSERT(sizeof(IceMaths::Sphere)==sizeof(NxSphere));
#if !defined(__flash__) && !defined(__webgl__)
NX_COMPILE_TIME_ASSERT(OFFSET_OF(NxSphere, center)==0);
NX_COMPILE_TIME_ASSERT(OFFSET_OF(NxSphere, radius)==12);
#endif
NX_INLINE const IceMaths::Sphere& NXToICE(const NxSphere& nxSphere)		{ return (const IceMaths::Sphere&)nxSphere;		}

NX_COMPILE_TIME_ASSERT(sizeof(IceMaths::Segment)==sizeof(NxSegment));
NX_INLINE const IceMaths::Segment& NXToICE(const NxSegment& nxSegment)	{ return (const IceMaths::Segment&)nxSegment;	}

NX_COMPILE_TIME_ASSERT(sizeof(IceMaths::LSS)==sizeof(NxCapsule));
NX_INLINE const IceMaths::LSS& NXToICE(const NxCapsule& nxCapsule)		{ return (const IceMaths::LSS&)nxCapsule;		}

NX_COMPILE_TIME_ASSERT(sizeof(IceMaths::AABB)==sizeof(NxBounds3));
#if !defined(__flash__) && !defined(__webgl__)
NX_COMPILE_TIME_ASSERT(OFFSET_OF(NxBounds3, min)==0);
NX_COMPILE_TIME_ASSERT(OFFSET_OF(NxBounds3, max)==12);
#endif
NX_INLINE const IceMaths::AABB& NXToICE(const NxBounds3& nxAABB)		{ return (const IceMaths::AABB&)nxAABB;			}

NX_COMPILE_TIME_ASSERT(sizeof(IceMaths::Plane)==sizeof(NxPlane));
#if !defined(__flash__) && !defined(__webgl__)
NX_COMPILE_TIME_ASSERT(OFFSET_OF(NxPlane, normal)==0);
NX_COMPILE_TIME_ASSERT(OFFSET_OF(NxPlane, d)==12);
#endif
NX_INLINE const IceMaths::Plane& NXToICE(const NxPlane& nxPlane)		{ return (const IceMaths::Plane&)nxPlane;		}

NX_COMPILE_TIME_ASSERT(sizeof(IceMaths::Triangle)==sizeof(NxTriangle));
NX_INLINE const IceMaths::Triangle& NXToICE(const NxTriangle& nxTri)	{ return (const IceMaths::Triangle&)nxTri;		}
NX_INLINE const IceMaths::Triangle* NXToICE(const NxTriangle* nxTri)	{ return (const IceMaths::Triangle*)nxTri;		}

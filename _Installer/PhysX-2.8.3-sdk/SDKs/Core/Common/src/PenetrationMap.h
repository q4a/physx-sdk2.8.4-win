#ifndef NX_PENETRATION_MAP
#define NX_PENETRATION_MAP
/*----------------------------------------------------------------------------*\
|
|							NVIDIA PhysX Technology
|
|							     www.nvidia.com
|
\*----------------------------------------------------------------------------*/

//#define USE_MORTON_PMAP

#include "Physics.h"
#include "Allocateable.h"
#include "NxBounds3.h"
#include "Opcode.h"

class NxUserOutputStream;

//namespace NxCollision {

class NvTriangleMesh;
class TriangleMesh;

	// PMAP version 4
	#define PMAP_VERSION 4

	class PMapSample : public NxFoundation::NxAllocateable
	{
	public:
		NX_INLINE PMapSample()
		{
			mClosestFace = 0xffffffff;
		}

		NX_INLINE	NX_BOOL	IsInside()					const	{ return !(mClosestFace & NX_SIGN_BITMASK);	}
		NX_INLINE	NX_BOOL	IsOutside()					const	{ return mClosestFace & NX_SIGN_BITMASK;	}
		NX_INLINE	void	SetOutside()						{ mClosestFace |= NX_SIGN_BITMASK;			}

		NX_INLINE	void	SetClosestFace(NxU32 face)			{ mClosestFace = face;						}
		NX_INLINE	NxU32	GetClosestFace()			const	{ return mClosestFace & 0x3fffffff;			}
//		NX_INLINE	NxU32	GetClosestFace()			const	{ return mClosestFace & 0x1fffffff;			}

		NX_INLINE	NxU32	GetData()					const	{ return mClosestFace;						}
		NX_INLINE	void	SetData(NxU32 data)					{ mClosestFace = data;						}

		NX_INLINE	NX_BOOL	IsEmptyCell()				const	{ return mClosestFace & 0x40000000;			}
//		NX_INLINE	NX_BOOL	IsUniformCell()				const	{ return mClosestFace & 0x20000000;			}

		private:
		NxU32	mClosestFace;
	};

	class PenetrationMap : public NxFoundation::NxAllocateable
	{
		public:
		// Constructor / Destructor
										PenetrationMap();
										PenetrationMap(const PenetrationMap& pmap);
		virtual							~PenetrationMap();

					bool				Create(NvTriangleMesh* mesh, NxU32 density, const char* name, CustomArray* buffer=null, bool load=false, NxUserOutputStream* outputStream=NULL);

		NX_INLINE	const NxBounds3&	GetLocalBox()			const	{ return mLocalBox;			}
		NX_INLINE	NxU32				GetDensity()			const	{ return mDensity;			}
		NX_INLINE	NxU32				GetNbSamples()			const	{ return mNbSamples;		}
		NX_INLINE	const PMapSample*	GetSamples()			const	{ return mPMap;				}

					NX_BOOL				ComputeClosestSample(NxVec3& sample, const NxVec3& local_pt, const NxVec3* culler=NULL, const NxMat33* cull_transform=NULL)	const;
		NX_INLINE	NX_BOOL				ComputeFilteredSample(NxVec3& sample, const NxVec3& local_pt, const NxVec3* culler=NULL, const NxMat33* cull_transform=NULL)	const
										{
											// Check the point is at least inside the object's AABB
											if(!mLocalBox.contain(local_pt))	return NX_FALSE;

											// Compute (i,j,k) coordinates inside the AABB, to derive a cell index
											// TODO: PT: stupid _ftols here.....
											NxU32 i = NxU32((local_pt.x - mLocalBox.min[0]) * mCoeffs.x);
											NxU32 j = NxU32((local_pt.y - mLocalBox.min[1]) * mCoeffs.y);
											NxU32 k = NxU32((local_pt.z - mLocalBox.min[2]) * mCoeffs.z);
#ifdef USE_MORTON_PMAP
											NxU32 tag = mMortonLUT[i] + (mMortonLUT[j]<<1) + (mMortonLUT[k]<<2);
#else
											NxU32 tag = i+j*mDensity+k*mDensity2;
#endif
											// Early exit if we land in an empty cell
	//										if(mPMap[tag].GetData() & 0x40000000)	return FALSE;
											if(mPMap[tag].IsEmptyCell())	return NX_FALSE;

											// Else compute filtered sample
											return ComputeFilteredSample_(i, j, k, sample, local_pt, culler, cull_transform);
										}

		NX_INLINE	NX_BOOL				ComputeTaggedFilteredSample(NxU32& tag, NxVec3& sample, const NxVec3& local_pt, const NxVec3* culler=NULL, const NxMat33* cull_transform=NULL)	const
										{
											// Check the point is at least inside the object's AABB
											if(!mLocalBox.contain(local_pt))	return NX_FALSE;

											// Compute (i,j,k) coordinates inside the AABB, to derive a cell index
											// TODO: PT: stupid _ftols here.....
											NxU32 i = NxU32((local_pt.x - mLocalBox.min[0]) * mCoeffs.x);
											NxU32 j = NxU32((local_pt.y - mLocalBox.min[1]) * mCoeffs.y);
											NxU32 k = NxU32((local_pt.z - mLocalBox.min[2]) * mCoeffs.z);

											// Early exit if we land in an empty cell
#ifdef USE_MORTON_PMAP
											tag = mMortonLUT[i] + (mMortonLUT[j]<<1) + (mMortonLUT[k]<<2);
#else
											tag = i+j*mDensity+k*mDensity2;
#endif

	//										if(mPMap[tag].GetData() & 0x40000000)	return FALSE;
											if(mPMap[tag].IsEmptyCell())	return NX_FALSE;

											// Else compute filtered sample
											return ComputeFilteredSample_(i, j, k, sample, local_pt, culler, cull_transform);
										}

		// V1:
		//		0 <= ijk < Density
		// <=>	0 <= ijk <= Density-1
		// <=>	0 <= ijk*mOneOverDensityMinusOne <= 1
		// <=>	-0.5 <= (ijk*mOneOverDensityMinusOne)-0.5 <= 0.5
		// <=>	-Extents <= ((ijk*mOneOverDensityMinusOne)-0.5) * EdgeLengths <= +Extents
		// <=>	Center-Extents <= Center+((ijk*mOneOverDensityMinusOne)-0.5) * EdgeLengths <= Center+Extents
		// <=>	Min <= Center+((ijk*mOneOverDensityMinusOne)-0.5) * EdgeLengths <= Max
		//
		// V2:
		// <=>	0 <= ijk*mOneOverCoeffs <= EdgeLengths = 2 * Extents
		// <=>	-Extents <= ijk*mOneOverCoeffs - Extents <= +Extents
		// ...
		NX_INLINE	void				ComputeSamplingPoint(NxVec3& p, NxU32 i, NxU32 j, NxU32 k)			const
										{
	// V1
	//										p.x =  mCenter.x + ((float(i)*mOneOverDensityMinusOne)-0.5f) * mEdgeLengths.x;
	//										p.y =  mCenter.y + ((float(j)*mOneOverDensityMinusOne)-0.5f) * mEdgeLengths.y;
	//										p.z =  mCenter.z + ((float(k)*mOneOverDensityMinusOne)-0.5f) * mEdgeLengths.z;
	// V2
											p.x =  mCenter.x + ((float(i)*mOneOverCoeffs.x) - mExtents.x);
											p.y =  mCenter.y + ((float(j)*mOneOverCoeffs.y) - mExtents.y);
											p.z =  mCenter.z + ((float(k)*mOneOverCoeffs.z) - mExtents.z);
										}
		private:
#ifdef USE_MORTON_PMAP
				NxU32*					mMortonLUT;
				void					InitMortonLUT();
#endif
				NxBounds3				mLocalBox;
				NxVec3					mCenter;
				NxVec3					mExtents;
				NxVec3					mEdgeLengths;
				NxVec3					mCoeffs;
				NxVec3					mOneOverCoeffs;
				NxU32					mDensity;
				NxU32					mDensity2;
				float					mDensityMinusOne;
				float					mOneOverDensityMinusOne;
				NxU32					mNbSamples;
				PMapSample*				mPMap;
				TriangleMesh*			mOwner;
				bool					Init(NxU32 density, const NxBounds3& local_box);
				NxU32					ComputeSurfaceCells(const Opcode::Model* model);
		public:
				bool					Save(const char* filename, CustomArray* dest=null);
		private:
				bool					Load(const char* filename, CustomArray* src=null);
				bool					PrecomputeOutsideCells();
				NX_BOOL					ComputeFilteredSample_(NxU32 i, NxU32 j, NxU32 k, NxVec3& sample, const NxVec3& local_pt, const NxVec3* culler, const NxMat33* cull_transform) const;
				NxReal					Test();
	};
//}

#endif

#include "NxApex.h"
#ifndef __NX_FROM_PX_H__
#define __NX_FROM_PX_H__
/*
 * Copyright 2009-2010 NVIDIA Corporation.  All rights reserved.
 *
 * NOTICE TO USER:
 *
 * This source code is subject to NVIDIA ownership rights under U.S. and
 * international Copyright laws.  Users and possessors of this source code
 * are hereby granted a nonexclusive, royalty-free license to use this code
 * in individual and commercial software.
 *
 * NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE
 * CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR
 * IMPLIED WARRANTY OF ANY KIND.  NVIDIA DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOURCE CODE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.
 * IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOURCE CODE.
 *
 * U.S. Government End Users.   This source code is a "commercial item" as
 * that term is defined at  48 C.F.R. 2.101 (OCT 1995), consisting  of
 * "commercial computer  software"  and "commercial computer software
 * documentation" as such terms are  used in 48 C.F.R. 12.212 (SEPT 1995)
 * and is provided to the U.S. Government only as a commercial end item.
 * Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through
 * 227.7202-4 (JUNE 1995), all U.S. Government End Users acquire the
 * source code with only those rights set forth herein.
 *
 * Any use of this source code in individual and commercial software must
 * include, in the user documentation and internal comments to the code,
 * the above Disclaimer and U.S. Government End Users Notice.
 */

/*!
\file
\brief Conversion utilities between PhysX 2.8 and 3.0 data types
*/

#include "NxVec3.h"
#include "NxMat33.h"
#include "NxMat34.h"
#include "NxStream.h"
#include "NxBounds3.h"

#include "PxVec3.h"
#include "PxQuat.h"
#include "PxMat33Legacy.h"
#include "PxMat34Legacy.h"
#include "PxFileBuf.h"

namespace physx {
namespace apex {

PX_PUSH_PACK_DEFAULT

PX_INLINE void PxFromNxVec3( physx::PxVec3& out, const NxVec3& in ) { out.set( in.x, in.y, in.z ); }
PX_INLINE void NxFromPxVec3( NxVec3& out, const physx::PxVec3& in ) { out.set( in.x, in.y, in.z ); }
PX_INLINE void PxFromNxQuat( physx::PxQuat& out, const NxQuat& in ) { out = physx::PxQuat( in.x, in.y, in.z, in.w ); }
PX_INLINE void NxFromPxQuat( NxQuat& out, const physx::PxQuat& in ) { out.setXYZW( in.x, in.y, in.z, in.w ); }
PX_INLINE void PxVec3FromArray( physx::PxVec3& out, const physx::PxF32 arr[3] ) { out.set( arr[0], arr[1], arr[2] ); }
PX_INLINE void PxVec3FromArray( physx::PxVec3& out, const physx::PxF64 arr[3] ) { out.set( (physx::PxF32)arr[0], (physx::PxF32)arr[1], (physx::PxF32)arr[2] ); }
PX_INLINE void PxFromNxMat33( physx::PxMat33Legacy& out, const NxMat33& in ) { physx::PxF32 buf[9]; in.getRowMajor(buf); out.setRowMajor(buf); }
PX_INLINE void NxFromPxMat33( NxMat33& out, const physx::PxMat33Legacy& in ) { physx::PxF32 buf[9]; in.getRowMajor(buf); out.setRowMajor(buf); }
PX_INLINE void PxFromNxMat34( physx::PxMat34Legacy& out, const NxMat34& in ) { PxFromNxMat33(out.M, in.M); PxFromNxVec3(out.t, in.t); }
PX_INLINE void NxFromPxMat34( NxMat34& out, const physx::PxMat34Legacy& in ) { NxFromPxMat33(out.M, in.M); NxFromPxVec3(out.t, in.t); }
PX_INLINE void PxFromNxBounds3( physx::PxBounds3& out, const NxBounds3& in ) { PxFromNxVec3(out.minimum, in.min); PxFromNxVec3(out.maximum, in.max); }
PX_INLINE void NxFromPxBounds3( NxBounds3& out, const physx::PxBounds3& in ) { NxFromPxVec3(out.min, in.minimum); NxFromPxVec3(out.max, in.maximum); }
PX_INLINE void ArrayFromPxQuat( physx::PxF32 arr[4], const physx::PxQuat& q ) { arr[0]=q.x; arr[1]=q.y; arr[2]=q.z; arr[3]=q.w; }
PX_INLINE void PxQuatFromArray( physx::PxQuat& out, const physx::PxF32 arr[4] ) { out.x=arr[0]; out.y=arr[1]; out.z=arr[2]; out.w=arr[3]; }

PX_INLINE NxVec3& NxFromPxVec3Fast(physx::PxVec3& in) { return reinterpret_cast<NxVec3&>(in); }
PX_INLINE const NxVec3& NxFromPxVec3Fast(const physx::PxVec3& in) { return reinterpret_cast<const NxVec3&>(in); }
PX_INLINE physx::PxVec3& PxFromNxVec3Fast(NxVec3& in) { return reinterpret_cast<physx::PxVec3&>(in); }
PX_INLINE const physx::PxVec3& PxFromNxVec3Fast(const NxVec3& in) { return reinterpret_cast<const physx::PxVec3&>(in); }
PX_INLINE NxQuat& NxFromPxQuatFast(physx::PxQuat& in) { return reinterpret_cast<NxQuat&>(in); }
PX_INLINE const NxQuat& NxFromPxQuatFast(const physx::PxQuat& in) { return reinterpret_cast<const NxQuat&>(in); }
PX_INLINE physx::PxQuat& PxFromNxQuatFast(NxQuat& in) { return reinterpret_cast<physx::PxQuat&>(in); }
PX_INLINE const physx::PxQuat& PxFromNxQuatFast(const NxQuat& in) { return reinterpret_cast<const physx::PxQuat&>(in); }

#define PXFROMNXVEC3(nxv) physx::PxVec3((nxv).x, (nxv).y, (nxv).z)
#define NXFROMPXVEC3(pxv) NxVec3((pxv).x, (pxv).y, (pxv).z)


/**
\brief A wrapper class that provides an NxStream API for a physx::PxFileBuf
*/
class NxFromPxStream : public NxStream
{
private:
	physx::PxFileBuf& mPx;
	NxFromPxStream& operator=( const NxFromPxStream& ) { return *this; }

public:
	NxFromPxStream( physx::PxFileBuf& px ) : mPx( px ) {}
	physx::PxU8			readByte()								const { return mPx.readByte(); }
	physx::PxU16			readWord()								const { return mPx.readWord(); }
	physx::PxU32			readDword()								const { return mPx.readDword(); }
	physx::PxF32			readFloat()								const { return mPx.readFloat(); }
	physx::PxF64			readDouble()							const { return mPx.readDouble(); }
	void			readBuffer(void* buffer, physx::PxU32 size)	const { mPx.read(buffer, size); }

	NxStream&		storeByte(physx::PxU8 b)						{ mPx.storeByte(b); return *this; }
	NxStream&		storeWord(physx::PxU16 w)						{ mPx.storeWord(w); return *this; }
	NxStream&		storeDword(physx::PxU32 d)						{ mPx.storeDword(d); return *this; }
	NxStream&		storeFloat(physx::PxF32 f)						{ mPx.storeFloat(f); return *this; }
	NxStream&		storeDouble(physx::PxF64 f)					{ mPx.storeDouble(f); return *this; }
	NxStream&		storeBuffer(const void* buffer, physx::PxU32 size)	{ mPx.write(buffer, size); return *this; }
};

//! \brief Replacement for NxBounds3::transform()
PX_INLINE void PxBounds3boundsOfOBB(physx::PxBounds3& b, const physx::PxMat33Legacy& orientation, const physx::PxVec3& translation, const physx::PxVec3& halfDims)
{
	physx::PxF32 dimx = halfDims[0];
	physx::PxF32 dimy = halfDims[1];
	physx::PxF32 dimz = halfDims[2];

	physx::PxF32 x = abs(orientation(0,0) * dimx) + abs(orientation(0,1) * dimy) + abs(orientation(0,2) * dimz);
	physx::PxF32 y = abs(orientation(1,0) * dimx) + abs(orientation(1,1) * dimy) + abs(orientation(1,2) * dimz);
	physx::PxF32 z = abs(orientation(2,0) * dimx) + abs(orientation(2,1) * dimy) + abs(orientation(2,2) * dimz);

	physx::PxVec3 min(-x + translation[0], -y + translation[1], -z + translation[2]);
	physx::PxVec3 max(x + translation[0], y + translation[1], z + translation[2]);
	b.set(min, max);
}

//! \brief Replacement for NxBounds3::transform()
PX_INLINE void PxBounds3Transform(physx::PxBounds3& b, const physx::PxMat33Legacy& orientation, const physx::PxVec3& translation)
{
	// convert to center and extents form
	physx::PxVec3 center, extents;
	b.getCenter(center);
	b.getExtents(extents);

	center = orientation * center + translation;
	PxBounds3boundsOfOBB(b, orientation, center, extents);
}

//! \brief Replacement for NxBounds3::transform()
PX_INLINE void PxBounds3boundsOfOBB(physx::PxBounds3& b, const physx::PxMat44& tm, const physx::PxVec3& center, const physx::PxVec3& halfDims)
{
	physx::PxF32 dimx = halfDims[0];
	physx::PxF32 dimy = halfDims[1];
	physx::PxF32 dimz = halfDims[2];

	physx::PxF32 x = abs(tm(0,0) * dimx) + abs(tm(0,1) * dimy) + abs(tm(0,2) * dimz);
	physx::PxF32 y = abs(tm(1,0) * dimx) + abs(tm(1,1) * dimy) + abs(tm(1,2) * dimz);
	physx::PxF32 z = abs(tm(2,0) * dimx) + abs(tm(2,1) * dimy) + abs(tm(2,2) * dimz);

	physx::PxVec3 min(-x + center[0], -y + center[1], -z + center[2]);
	physx::PxVec3 max(x + center[0], y + center[1], z + center[2]);
	b.set(min, max);
}

//! \brief Replacement for NxBounds3::transform()
PX_INLINE void PxBounds3Transform(physx::PxBounds3& b, const physx::PxMat44& tm)
{
	// convert to center and extents form
	physx::PxVec3 center, extents;
	b.getCenter(center);
	b.getExtents(extents);

	center = tm.transform( center );
	PxBounds3boundsOfOBB(b, tm, center, extents);
}

//! \brief replacement for NxVec3::equals
PX_INLINE bool PxVec3equals(const physx::PxVec3 & a, const physx::PxVec3 & v, physx::PxF32 epsilon)
{
	return 
		physx::PxEquals(a.x, v.x, epsilon) &&
		physx::PxEquals(a.y, v.y, epsilon) &&
		physx::PxEquals(a.z, v.z, epsilon);
}

PX_POP_PACK

}} // end namespace physx::apex

#endif // __NX_FROM_PX_H__

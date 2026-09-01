#include "NxApex.h"
#ifndef __NX_APEX_STREAM_H__
#define __NX_APEX_STREAM_H__
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
\brief NxApexStream and serialization helpers
*/

namespace physx 
{

	namespace apex 
	{

PX_PUSH_PACK_DEFAULT

/**
\brief flags for stream opening modes
*/
struct NxApexStreamFlags
{
	enum Enum
	{
		OPEN_FOR_READ = (1<<0),  //!< Open stream file for read
		OPEN_FOR_WRITE = (1<<1), //!< Open stream file for write
	};
};

/**
\brief A file oriented physx::PxFileBuf
*/
class NxApexStream : public physx::PxFileBuf, public NxApexInterface
{
public:
	virtual bool isOpen() const = 0;
};

PX_POP_PACK


// Public, useful operators for serializing nonversioned data follow.
PX_INLINE physx::PxFileBuf& operator >> ( physx::PxFileBuf& stream, bool& b )  { b = (0 != stream.readByte()); return stream; }
PX_INLINE physx::PxFileBuf& operator >> ( physx::PxFileBuf& stream, physx::PxI8& b )  { b = stream.readByte(); return stream; }
PX_INLINE physx::PxFileBuf& operator >> ( physx::PxFileBuf& stream, physx::PxI16& w ) { w = stream.readWord(); return stream; }
PX_INLINE physx::PxFileBuf& operator >> ( physx::PxFileBuf& stream, physx::PxI32& d ) { d = stream.readDword(); return stream; }
PX_INLINE physx::PxFileBuf& operator >> ( physx::PxFileBuf& stream, physx::PxU8& b )  { b = stream.readByte(); return stream; }
PX_INLINE physx::PxFileBuf& operator >> ( physx::PxFileBuf& stream, physx::PxU16& w ) { w = stream.readWord(); return stream; }
PX_INLINE physx::PxFileBuf& operator >> ( physx::PxFileBuf& stream, physx::PxU32& d ) { d = stream.readDword(); return stream; }
PX_INLINE physx::PxFileBuf& operator >> ( physx::PxFileBuf& stream, physx::PxF32& f ) { f = stream.readFloat(); return stream; }
PX_INLINE physx::PxFileBuf& operator >> ( physx::PxFileBuf& stream, physx::PxF64& f ) { f = stream.readDouble(); return stream; }
PX_INLINE physx::PxFileBuf& operator >> ( physx::PxFileBuf& stream, physx::PxVec3& v ) { stream >> v.x >> v.y >> v.z; return stream;}
PX_INLINE physx::PxFileBuf& operator >> ( physx::PxFileBuf& stream, physx::PxBounds3& b ) { stream >> b.minimum >> b.maximum; return stream;}
PX_INLINE physx::PxFileBuf& operator >> ( physx::PxFileBuf& stream, physx::PxMat33Legacy& m ) { stream >> m(0,0) >> m(0,1) >> m(0,2) >> m(1,0) >> m(1,1) >> m(1,2) >> m(2,0) >> m(2,1) >> m(2,2); return stream;}
PX_INLINE physx::PxFileBuf& operator >> ( physx::PxFileBuf& stream, physx::PxMat34Legacy& tm ) { stream >> tm.M >> tm.t; return stream;}
PX_INLINE physx::PxFileBuf& operator >> ( physx::PxFileBuf& stream, physx::PxQuat& q ) { stream >> q.x >> q.y >> q.z >> q.w; return stream;}
PX_INLINE physx::PxFileBuf& operator >> ( physx::PxFileBuf& stream, NxGroupsMask& gm ) { stream >> gm.bits0 >> gm.bits1 >> gm.bits2 >> gm.bits3; return stream;}

PX_INLINE physx::PxFileBuf& operator >> ( physx::PxFileBuf& stream, NxRange<physx::PxF32>& r ) { stream >> r.minimum >> r.maximum; return stream;}
PX_INLINE physx::PxFileBuf& operator >> ( physx::PxFileBuf& stream, NxRange<physx::PxU32>& r ) { stream >> r.minimum >> r.maximum; return stream;}
PX_INLINE physx::PxFileBuf& operator >> ( physx::PxFileBuf& stream, NxRange<physx::PxVec3>& r ) { stream >> r.minimum >> r.maximum; return stream;}
PX_INLINE physx::PxFileBuf& operator >> ( physx::PxFileBuf& stream, NxRenderCullMode::Enum& w ) { stream.read(&w, sizeof(w)); return stream; }

// The opposite of the above operators--takes data and writes it to a stream.
PX_INLINE physx::PxFileBuf& operator << ( physx::PxFileBuf& stream, const bool b )  { stream.storeByte( b ? (physx::PxU8)1 : (physx::PxU8)0 ); return stream;}
PX_INLINE physx::PxFileBuf& operator << ( physx::PxFileBuf& stream, const physx::PxI8 b )  { stream.storeByte( b ); return stream;}
PX_INLINE physx::PxFileBuf& operator << ( physx::PxFileBuf& stream, const physx::PxI16 w ) { stream.storeWord( w ); return stream;}
PX_INLINE physx::PxFileBuf& operator << ( physx::PxFileBuf& stream, const physx::PxI32 d ) { stream.storeDword( d ); return stream;}
PX_INLINE physx::PxFileBuf& operator << ( physx::PxFileBuf& stream, const physx::PxU8 b )  { stream.storeByte( b ); return stream;}
PX_INLINE physx::PxFileBuf& operator << ( physx::PxFileBuf& stream, const physx::PxU16 w ) { stream.storeWord( w ); return stream;}
PX_INLINE physx::PxFileBuf& operator << ( physx::PxFileBuf& stream, const physx::PxU32 d ) { stream.storeDword( d ); return stream;}
PX_INLINE physx::PxFileBuf& operator << ( physx::PxFileBuf& stream, const physx::PxF32 f ) { stream.storeFloat( f ); return stream;}
PX_INLINE physx::PxFileBuf& operator << ( physx::PxFileBuf& stream, const physx::PxF64 f ) { stream.storeDouble( f ); return stream;}
PX_INLINE physx::PxFileBuf& operator << ( physx::PxFileBuf& stream, const physx::PxVec3& v ) { stream << v.x << v.y << v.z; return stream;}
PX_INLINE physx::PxFileBuf& operator << ( physx::PxFileBuf& stream, const physx::PxBounds3& b ) { stream << b.minimum << b.maximum; return stream;}
PX_INLINE physx::PxFileBuf& operator << ( physx::PxFileBuf& stream, const NxGroupsMask gm ) { stream << gm.bits0 << gm.bits1 << gm.bits2 << gm.bits3; return stream;}
PX_INLINE physx::PxFileBuf& operator << ( physx::PxFileBuf& stream, const physx::PxMat33Legacy& m ) { stream << m(0,0) << m(0,1) << m(0,2) << m(1,0) << m(1,1) << m(1,2) << m(2,0) << m(2,1) << m(2,2); return stream;}
PX_INLINE physx::PxFileBuf& operator << ( physx::PxFileBuf& stream, const physx::PxMat34Legacy& tm ) { stream << tm.M << tm.t; return stream;}
PX_INLINE physx::PxFileBuf& operator << ( physx::PxFileBuf& stream, const physx::PxQuat& q ) { stream << q.x << q.y << q.z << q.w; return stream;}

PX_INLINE physx::PxFileBuf& operator << ( physx::PxFileBuf& stream, const NxRange<physx::PxF32> r ) { stream << r.minimum << r.maximum; return stream;}
PX_INLINE physx::PxFileBuf& operator << ( physx::PxFileBuf& stream, const NxRange<physx::PxU32> r ) { stream << r.minimum << r.maximum; return stream;}
PX_INLINE physx::PxFileBuf& operator << ( physx::PxFileBuf& stream, const NxRange<physx::PxVec3> r ) { stream << r.minimum << r.maximum; return stream;}

} // end of namespace apex
} // end namespace physx

#endif // __NX_APEX_STREAM_H__
